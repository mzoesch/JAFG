// Copyright 2024 mzoesch. All rights reserved.

#include "Network/HyperlaneWorker.h"

#include "IPAddressAsyncResolve.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Network/HyperlaneTransmitterInfo.h"
#include "World/Chunk/LocalPlayerChunkGeneratorSubsystem.h"

/*
 * Just for the sake of it.
 * Is UE_LOG thread safe?
 * If not we might want to look into something like that:
 *   AsyncTask(ENamedThreads::GameThread, [] (void) { UE_LOG(LogTemp, Log, TEXT("")); });
 */

#define RETURN_CODE_OK    0x0
#define RETURN_CODE_ERROR 0x1

namespace CommonHyperlaneWorkerStatics
{

static TFuture<void> RunLambdaOnBackGroundThread(TFunction<void(void)> InFunction)
{
    return Async(EAsyncExecution::Thread, InFunction);
}

static TFuture<void> RunLambdaOnGameThread(TFunction<void(void)> InFunction)
{
    return Async(EAsyncExecution::TaskGraphMainThread, InFunction);
}

}

FHyperlaneWorker::FHyperlaneWorker(ULocalPlayerChunkGeneratorSubsystem* InOwner) : Thread(nullptr), bShutdownRequested(false)
{
    this->Owner = InOwner;

    this->Thread = FRunnableThread::Create(this, TEXT("HyperlaneWorker"));

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

    this->bShouldReceiveData = true;
    this->bShouldAttemptConnection = true;

    this->OnConnectedDelegate.BindLambda( [&] (void)
    {
        this->OnConnectedDelegateHandler();
    });

    this->OnDisconnectedDelegate.BindLambda( [&] (void)
    {
        this->OnDisconnectedDelegateHandler();
    });

    this->OnBytesReceivedDelegate.BindLambda( [&] (const TArray<uint8>& Bytes)
    {
        this->OnBytesReceivedDelegateHandler(Bytes);
    });

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

    const FResolveInfo* ResolveInfo = SocketSubsystem->GetHostByName(TCHAR_TO_ANSI(*this->Address));
    while (ResolveInfo->IsComplete() == false)
    {
        FPlatformProcess::Sleep(0.001f);
    }

    if (const int32 Error = ResolveInfo->GetErrorCode(); Error != RETURN_CODE_OK)
    {
        UE_LOG(LogTemp, Error, TEXT("FHyperlaneWorker::Init: DNS resolve error: %d."), Error)
        return false;
    }

    this->RemoteAddress = SocketSubsystem->CreateInternetAddr();

    /* Somewhat wasteful, we could probably use the same address object? */
    this->RemoteAddress->SetRawIp(ResolveInfo->GetResolvedAddress().GetRawIp());
    this->RemoteAddress->SetPort(this->Port);

    this->Socket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("HyperlaneSocket"), false);

    this->Socket->SetSendBufferSize(this->BufferMaxSizeInBytes, this->BufferMaxSizeInBytes);
    this->Socket->SetReceiveBufferSize(this->BufferMaxSizeInBytes, this->BufferMaxSizeInBytes);

    this->CreateConnectionEndFuture();

    return true;
}

uint32 FHyperlaneWorker::Run(void)
{
    this->ConnectionEndFuture.WaitFor(FTimespan::FromSeconds(FHyperlaneWorker::ExhaustedConnectionTimeInSeconds));

    if (this->IsConnected() == false)
    {
        UE_LOG(LogTemp, Error, TEXT("FHyperlaneWorker::Run: Hyperlane Worker failed to connect. Exausting connection attempts."))
        return RETURN_CODE_ERROR;
    }

    while (this->bShutdownRequested == false)
    {
        UE_LOG(LogTemp, Log, TEXT("Hyperlane Worker Running"))
        /*
         * Here do heavy lifting.
         */
        FPlatformProcess::Sleep(2.0f);
    }

    return RETURN_CODE_OK;
}

void FHyperlaneWorker::Stop(void)
{
    this->bShutdownRequested = true;
    UE_LOG(LogTemp, Log, TEXT("FHyperlaneWorker::Stop: Stop requested."))
    return;
}

void FHyperlaneWorker::Exit(void)
{
    /*
     * If the Run method exists on its own. We still want to make
     * sure that all other dependencies of this variable are cleaned up.
     */
    this->bShutdownRequested = true;

    if (this->Socket != nullptr)
    {
        this->bShouldAttemptConnection = false;
        this->bShouldReceiveData = false;

        if (this->ConnectionEndFuture.IsValid())
        {
            UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::Exit: Waiting for Connection End Future."))
            this->ConnectionEndFuture.Wait();
            UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::Exit: Finished waiting for Connection End Future."))
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("FHyperlaneWorker::Exit: Connection End Future is not valid."))
        }

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

        this->OnDisconnectedDelegate.Execute();

        this->Socket = nullptr;
    }

    UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::Exit: Hyperlane Worker Exited."))

    return;
}

void FHyperlaneWorker::OnConnectedDelegateHandler()
{
    UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::OnConnectedDelegateHandler: fired."))
}

void FHyperlaneWorker::OnDisconnectedDelegateHandler()
{
    UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::OnDisconnectedDelegateHandler: fired."))
}

void FHyperlaneWorker::OnBytesReceivedDelegateHandler(const TArray<uint8>& Bytes)
{
    check(this->Owner)
    UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::OnBytesReceivedDelegateHandler: Received %d bytes."), Bytes.Num())

    TransmittableData::FChunkInitializationData Data = TransmittableData::FChunkInitializationData::DeserializeFromBytes(Bytes);

    UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::OnBytesReceivedDelegateHandler: Chunk Key: %s"), *Data.ChunkKey.ToString())

    CommonHyperlaneWorkerStatics::RunLambdaOnGameThread( [&, Data] (void)
    {
        this->Owner->InitializeChunkWithAuthorityData(Data.ChunkKey, Data.Voxels);
    });

    return;
}

void FHyperlaneWorker::CreateConnectionEndFuture()
{
    this->ConnectionEndFuture = CommonHyperlaneWorkerStatics::RunLambdaOnBackGroundThread( [&] (void)
    {
        UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::CreateConnectionEndFuture: Hyperlane Worker connecting on background thread."))

        uint32 BufferSize = 0;
        TArray<uint8> ReceiveBuffer;

        this->bShouldAttemptConnection = true;

        while (this->bShouldAttemptConnection)
        {
            if (this->Socket == nullptr)
            {
                UE_LOG(LogTemp, Fatal, TEXT("FHyperlaneWorker::CreateConnectionEndFuture: Hyperlane Worker failed to connect. Socket is already deinitialized."))
                return;
            }

            if (this->Socket->Connect(*this->RemoteAddress))
            {
                CommonHyperlaneWorkerStatics::RunLambdaOnGameThread( [&] (void)
                {
                    this->OnConnectedDelegate.Execute();
                });

                this->bShouldAttemptConnection = false;

                continue;
            }

            UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::CreateConnectionEndFuture: Hyperlane Worker failed to connect. Retrying in 1 second."))
            FPlatformProcess::Sleep(1.0f);

            continue;
        }

        if (this->IsConnected() == false)
        {
            UE_LOG(LogTemp, Error, TEXT("FHyperlaneWorker::CreateConnectionEndFuture: Hyperlane Worker failed to connect."))
            return;
        }

        this->bShouldReceiveData = true;

        UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::CreateConnectionEndFuture: Hyperlane Worker now receiveing data."))

        while (this->bShouldReceiveData)
        {
            if (this->Socket->HasPendingData(BufferSize))
            {
                ReceiveBuffer.SetNumUninitialized(BufferSize);

                int32 Read = 0;
                this->Socket->Recv(ReceiveBuffer.GetData(), ReceiveBuffer.Num(), Read);

                /*
                 * Kinda sketchy but we probably will never send a message that is so small that it will trigger this
                 * if-statement - as then we should use unreals replication system or remote procedure calls.
                 */
                if (Read == AHyperlaneTransmitterInfo::PingMessage.Len())
                {
                    UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::CreateConnectionEndFuture: Received ping message."))
                }
                else
                {
                    this->OnBytesReceivedDelegate.Execute(ReceiveBuffer);
                }
            }

            /*
             * Is there a better way to do this? This is just consuming CPU time.
             */
            this->Socket->Wait(ESocketWaitConditions::WaitForReadOrWrite, FTimespan(10));

            continue;
        }

        UE_LOG(LogTemp, Warning, TEXT("FHyperlaneWorker::CreateConnectionEndFuture: Hyperlane Worker stopped receiving data."))

        return;
    });

    return;
}

bool FHyperlaneWorker::IsConnected(void) const
{
    return this->Socket != nullptr && this->Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected;
}

#undef RETURN_CODE_OK
#undef RETURN_CODE_ERROR
