// Copyright 2024 mzoesch. All rights reserved.

#include "System/HyperlaneWorker.h"

#include "IPAddressAsyncResolve.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

/*
 * Just for the sake of it.
 * Is UE_LOG thread safe?
 * If not we might want to look into something like that:
 *   AsyncTask(ENamedThreads::GameThread, []() {	UE_LOG(LogTemp, Log, TEXT("")); });
 */

/*
 * Websockets do not work. Because we cannot craete them
 * We should go back to FSocket and use TCP.
 *
 * @see https://github.com/getnamo/TCP-Unreal/tree/master
 */

FHyperlaneWorker::FHyperlaneWorker(UHyperlaneComponent* InOwner) : Thread(nullptr), bShutdownRequested(false)
{
    this->Owner = InOwner;

    this->Thread = FRunnableThread::Create(this, TEXT("HyperlaneWorker"));
    this->bShutdownRequested = false;

    return;
}

FHyperlaneWorker::~FHyperlaneWorker(void)
{
    if (this->Thread)
    {
        this->Thread->Kill(true);

        delete this->Thread;

        UE_LOG(LogTemp, Log, TEXT("FHyperlaneWorker::~FHyperlaneWorker: Hyperlane Worker destroyed."))

        return;
    }

    UE_LOG(LogTemp, Error, TEXT("FHyperlaneWorker::~FHyperlaneWorker: Hyperlane Worker tried to kill himself but he was already dead."))

    return;
}

bool FHyperlaneWorker::Init(void)
{
    if (FPlatformProcess::SupportsMultithreading() == false)
    {
        UE_LOG(LogTemp, Fatal, TEXT("FHyperlaneWorker::Init: Multithreading is not supported on this platform."))
        return false;
    }

    this->Address = FString(TEXT("127.0.0.1"));
    this->Port = 8080;
    /* Roughly 2MB. */
    this->BufferMaxSizeInBytes = 2 * 1024 * 1024;

    this->OnConnected.BindLambda([]() { UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::Init: OnConnected fired.")); });
    this->OnDisconnected.BindLambda([]() { UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::Init: OnDisconnected fired.")); });

    if (this->IsConnected())
    {
        UE_LOG(LogTemp, Error, TEXT("FHyperlaneWorker::Init: Hyperlane Worker already connected."))
        return false;
    }

    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

    if (SocketSubsystem == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("FHyperlaneWorker::Init: SocketSubsystem is null."))
        return false;
    }

    auto ResolveInfo = SocketSubsystem->GetHostByName(TCHAR_TO_ANSI(*this->Address));
    while (!ResolveInfo->IsComplete());

    auto Error = ResolveInfo->GetErrorCode();
    if (Error != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("FHyperlaneWorker::Init: DNS resolve error: %d."), Error)
        return false;
    }

    this->RemoteAddress = SocketSubsystem->CreateInternetAddr();

    /* somewhat wasteful, we could probably use the same address object? */
    this->RemoteAddress->SetRawIp(ResolveInfo->GetResolvedAddress().GetRawIp());
    this->RemoteAddress->SetPort(this->Port);

    this->Socket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("HyperlaneSocket"), false);

    this->Socket->SetSendBufferSize(this->BufferMaxSizeInBytes, this->BufferMaxSizeInBytes);
    this->Socket->SetReceiveBufferSize(this->BufferMaxSizeInBytes, this->BufferMaxSizeInBytes);

    this->ConnectionFinishedFuture = RunLambdaOnBackGroundThread([&]()
    {
        UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::Init: Hyperlane Worker connecting on background thread."))

        double LastConnectionCheck = FPlatformTime::Seconds();

        uint32 BufferSize = 0;
        TArray<uint8> ReceiveBuffer;
        this->bShouldAttemptConnection = true;

        while (bShouldAttemptConnection)
        {
            if (Socket == nullptr)
            {
                UE_LOG(LogTemp, Error, TEXT("FHyperlaneWorker::Init: Hyperlane Worker failed to connect. Socket is already deinitialized."))
                return;
            }

            if (Socket->Connect(*RemoteAddress))
            {
                RunLambdaOnGameThread([&]()
                {
                    OnConnected.Execute();
                });
                bShouldAttemptConnection = false;
                continue;
            }

            UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::Init: Hyperlane Worker failed to connect."))
            FPlatformProcess::Sleep(1.0f);
        }

        if (this->IsConnected() == false)
        {
            UE_LOG(LogTemp, Error, TEXT("FHyperlaneWorker::Init: Hyperlane Worker failed to connect."))
            return;
        }

        bShouldReceiveData = true;

        UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::Init: Hyperlane Worker now receiveing data."))

        while (bShouldReceiveData)
        {
            if (Socket->HasPendingData(BufferSize))
            {
                ReceiveBuffer.SetNumUninitialized(BufferSize);

                int32 Read = 0;
                Socket->Recv(ReceiveBuffer.GetData(), ReceiveBuffer.Num(), Read);

                UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::Init: Hyperlane Worker received %d bytes."), Read)

                // OnReceivedBytes.Broadcast(ReceiveBuffer);
            }

            /*
             * Is there a better way to do this? This is just consuming CPU time.
             */
            Socket->Wait(ESocketWaitConditions::WaitForReadOrWrite, FTimespan(10));
        }

        UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::Init: Hyperlane Worker stopped receiving data."))

        return;
    });

    return true;
}

uint32 FHyperlaneWorker::Run(void)
{
    this->ConnectionFinishedFuture.WaitFor(FTimespan::FromSeconds(5.0f));

    if (this->IsConnected() == false)
    {
        UE_LOG(LogTemp, Error, TEXT("FHyperlaneWorker::Run: Hyperlane Worker failed to connect. Exausting connection attempts."))
        return 0x1;
    }

    while (bShutdownRequested == false)
    {
        UE_LOG(LogTemp, Log, TEXT("Hyperlane Worker Running"))
        FPlatformProcess::Sleep(2.0f);
    }

    return 0x0;
}

void FHyperlaneWorker::Stop(void)
{
    this->bShutdownRequested = true;
    UE_LOG(LogTemp, Log, TEXT("Hyperlane Worker Stop requested"))
    return;
}

void FHyperlaneWorker::Exit(void)
{
    /* if the run method exists on its own. */
    this->bShutdownRequested = true;

    if (this->Socket != nullptr)
    {
        bShouldAttemptConnection = false;
        bShouldReceiveData = false;

        UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::Exit: Hyperlane Worker waiting for future."))
        ConnectionFinishedFuture.Wait();
        UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::Exit: Hyperlane Worker future finished."))

        if (this->Socket)
        {
            if (this->IsConnected())
            {
                if (this->Socket->Close())
                {
                    UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::Exit: Hyperlane Worker closed socket without errors."))
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("FHyperlaneWorker::Exit: Hyperlane Worker closed socket with errors."))
                }
            }

            ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(this->Socket);
        }

        OnDisconnected.Execute();
    }

    this->Socket = nullptr;

    UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::Exit: Hyperlane Worker Exited."))
}

bool FHyperlaneWorker::IsConnected(void) const
{
    return (this->Socket != nullptr && (this->Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected));
}
