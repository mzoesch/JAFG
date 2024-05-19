// Copyright 2024 mzoesch. All rights reserved.

#include "HyperlaneWorker.h"

#include "Definitions.h"
#include "HyperlaneComponent.h"
#include "HyperlaneTransmitter.h"
#include "IPAddressAsyncResolve.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

#define RETURN_CODE_OK    0x0
#define RETURN_CODE_ERROR 0x1

namespace WorkerStatics
{

FORCEINLINE static TFuture<void> RunLambdaOnBackGroundThread(TFunction<void(void)> InFunction)
{
    return Async(EAsyncExecution::Thread, InFunction);
}

FORCEINLINE static TFuture<void> RunLambdaOnGameThread(TFunction<void(void)> InFunction)
{
    return Async(EAsyncExecution::TaskGraphMainThread, InFunction);
}

}

FHyperlaneWorker::FHyperlaneWorker(void)
{
}

FHyperlaneWorker::FHyperlaneWorker(UHyperlaneComponent* InOwningComponent)
{
    this->Thread = FRunnableThread::Create(this, TEXT("HyperlaneWorker"));

    this->OwningComponent = InOwningComponent;
    check( this->OwningComponent )

    return;
}

FHyperlaneWorker::~FHyperlaneWorker(void)
{
    if (this->Thread)
    {
        this->Thread->Kill(true);

        delete this->Thread;

        LOG_VERBOSE(LogHyperlane, "Hyperlane Worker destroyed.")

        return;
    }

    LOG_ERROR(LogHyperlane, "Hyperlane Worker tried to kill himself but he was already dead.")

    return;
}

bool FHyperlaneWorker::Init(void)
{
    if (FPlatformProcess::SupportsMultithreading() == false)
    {
        LOG_FATAL(LogHyperlane, "Multithreading is not supported on this platform.")
        return false;
    }

    this->OnConnectedDelegate.BindLambda( [this] (void) { this->OnConnectedDelegateHandler();  });
    this->OnDisconnectedDelegate.BindLambda( [this] (void) { this->OnDisconnectedDelegateHandler(); });
    this->OnBytesReceivedDelegate.BindLambda( [this] (TArray<uint8>& Bytes) { this->OnBytesReceivedDelegateHandler(Bytes); });

    if (this->IsConnected())
    {
        LOG_FATAL(LogHyperlane, "Hyperlane Worker already connected.")
        return false;
    }

    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

    if (SocketSubsystem == nullptr)
    {
        LOG_FATAL(LogHyperlane, "Hyperlane Worker failed to get Socket Subsystem.")
        return false;
    }

    const FResolveInfo* ResolveInfo = SocketSubsystem->GetHostByName(TCHAR_TO_ANSI(*this->Addr));
    while (ResolveInfo->IsComplete() == false)
    {
        /* Kinda sketchy but works. */
        FPlatformProcess::Sleep(0.001f);
    }

    if (const int32 Error = ResolveInfo->GetErrorCode(); Error != RETURN_CODE_OK)
    {
        LOG_FATAL(LogHyperlane, "Hyperlane Worker failed to resolve DNS. Code: %d.", Error)
        return false;
    }

    this->RemoteAddr = SocketSubsystem->CreateInternetAddr();

    /* Somewhat wasteful, we could probably use the same address object? */
    this->RemoteAddr->SetRawIp(ResolveInfo->GetResolvedAddress().GetRawIp());
    this->RemoteAddr->SetPort(this->Port);

    this->Socket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("HyperlaneSocket"), false);

    if (this->Socket->SetSendBufferSize(FHyperlaneWorker::DesiredMaxBufferSizeInBytes, this->SendPlatformMaxBufferSizeInBytes) == false)
    {
        LOG_FATAL(LogHyperlane, "Hyperlane Worker failed to set send buffer size.")
        return false;
    }
    if (this->Socket->SetReceiveBufferSize(FHyperlaneWorker::DesiredMaxBufferSizeInBytes, this->RecvPlatformMaxBufferSizeInBytes) == false)
    {
        LOG_FATAL(LogHyperlane, "Hyperlane Worker failed to set receive buffer size.")
        return false;
    }
    check( this->SendPlatformMaxBufferSizeInBytes == FHyperlaneWorker::DesiredMaxBufferSizeInBytes )
    check( this->RecvPlatformMaxBufferSizeInBytes == FHyperlaneWorker::DesiredMaxBufferSizeInBytes )

    this->CreateConnectionEndFuture();

    return true;
}

uint32 FHyperlaneWorker::Run(void)
{
    this->ConnectionEndFuture.WaitFor(FTimespan::FromSeconds(FHyperlaneWorker::ExhaustedConnectionTimeInSeconds));

    if (this->IsConnected() == false)
    {
        LOG_ERROR(LogHyperlane, "Hyperlane Worker failed to connect. Exausting connection attempts.")
        return RETURN_CODE_ERROR;
    }

    int i = 10;
    while (this->bShutdownRequested == false)
    {
        if (++i > 10)
        {
            i = 0;
            /*
             * We currently have this just to check if everything is rightfully destroyed, and no threads are
             * running stale in the background.
             */
            LOG_VERY_VERBOSE(LogHyperlane, "Hyperlane Worker is running.")
        }

        /*
         * Here do heavy lifting.
         */
        FPlatformProcess::Sleep(2.0f);

        continue;
    }

    return RETURN_CODE_OK;
}

void FHyperlaneWorker::Stop(void)
{
    this->bShutdownRequested = true;
    LOG_DISPLAY(LogHyperlane, "Hyperlane Worker stop requested.")
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
        this->bShouldReceive           = false;

        if (this->ConnectionEndFuture.IsValid())
        {
            LOG_DISPLAY(LogHyperlane, "Waiting for Connection End Future.")
            this->ConnectionEndFuture.Wait();
            LOG_DISPLAY(LogHyperlane, "Finished waiting for Connection End Future.")
        }
        else
        {
            LOG_ERROR(LogHyperlane, "Connection End Future is not valid.")
        }

        if (this->Socket)
        {
            if (this->IsConnected())
            {
                if (this->Socket->Close())
                {
                    LOG_VERBOSE(LogHyperlane, "Hyperlane Worker closed socket without errors.")
                }
                else
                {
                    LOG_ERROR(LogHyperlane, "Hyperlane Worker closed socket with unknown errors.")
                }
            }

            ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(this->Socket);
        }

        this->OnDisconnectedDelegate.Execute();

        this->Socket = nullptr;
    }

    LOG_DISPLAY(LogHyperlane, "Hyperlane Worker exited.")

    return;
}

void FHyperlaneWorker::CreateConnectionEndFuture(void)
{
    this->ConnectionEndFuture = WorkerStatics::RunLambdaOnBackGroundThread( [this] (void)
    {
        LOG_DISPLAY(LogHyperlane, "Hyperlane Worker connecting to [%s:%d] on background thread.", *this->Addr, this->Port)

        uint32        BufferSize    = 0;
        TArray<uint8> ReceiveBuffer = TArray<uint8>();

        while (this->bShouldAttemptConnection)
        {
            if (this->Socket == nullptr)
            {
                LOG_FATAL(LogHyperlane, "Hyperlane Worker failed to connect. Socket is already deinitialized.")
                return;
            }

            if (this->Socket->Connect(*this->RemoteAddr))
            {
                WorkerStatics::RunLambdaOnBackGroundThread( [&] (void)
                {
                    this->OnConnectedDelegate.Execute();
                });

                this->bShouldAttemptConnection = false;

                continue;
            }

            LOG_WARNING(LogHyperlane, "Hyperlane Worker failed to connect. Retrying in 1 second.")
            FPlatformProcess::Sleep(1.0f);

            continue;
        }

        if (this->IsConnected() == false)
        {
            LOG_FATAL(LogHyperlane, "Hyperlane Worker failed to connect.")
            return;
        }

        LOG_DISPLAY(LogHyperlane, "Hyperlane Worker connected to [%s:%d]. Now reciveing data.", *this->Addr, this->Port)

        while (this->bShouldReceive)
        {
            if (this->Socket->HasPendingData(BufferSize))
            {
                ReceiveBuffer.SetNumUninitialized(BufferSize);

                int32 Read = 0;
                this->Socket->Recv(ReceiveBuffer.GetData(), ReceiveBuffer.Num(), Read);
                check( Read == ReceiveBuffer.Num() )
                LOG_VERY_VERBOSE(LogHyperlane, "Hyperlane Worker received %d bytes.", Read)
                this->OnBytesReceivedDelegate.Execute(ReceiveBuffer);
            }

            this->Socket->Wait(ESocketWaitConditions::WaitForReadOrWrite, FTimespan(10));

            continue;
        }

        LOG_VERBOSE(LogHyperlane, "Hyperlane Worker stopped receiving data.")

        return;
    });

    return;
}

bool FHyperlaneWorker::SendValidation(void) const
{
    check( this->OwningComponent )

    const FHyperlaneIdentifier Ident = this->OwningComponent->GetHyperlaneIdentifier();
    if (Ident.IsEmpty())
    {
        LOG_ERROR(LogHyperlane, "Hyperlane Identifier is empty.")
        return false;
    }

    TArray<uint8> Bytes = TArray<uint8>();
    Bytes.Append(reinterpret_cast<uint8*>(TCHAR_TO_UTF8(*Ident)), Ident.Len() );

    if (Bytes.Num() != Ident.Len())
    {
        LOG_ERROR(LogHyperlane, "Failed to convert Hyperlane Identifier [%s] to bytes.", *Ident)
        return false;
    }

    return this->Emit(Bytes);
}

bool FHyperlaneWorker::Emit(const TArray<uint8>& InBytes) const
{
    if (this->IsConnected() == false)
    {
        LOG_FATAL(LogHyperlane, "Hyperlane Worker is not connected.")
        return false;
    }

    int32 BytesSent = 0;
    if (this->Socket->Send(InBytes.GetData(), InBytes.Num(), BytesSent))
    {
        LOG_VERBOSE(LogHyperlane, "Hyperlane Worker sent %d bytes.", BytesSent)
        return true;
    }

    LOG_ERROR(LogHyperlane, "Hyperlane Worker failed to send %d bytes. Actually sent: %d.", InBytes.Num(), BytesSent)

    return false;
}

bool FHyperlaneWorker::IsConnected(void) const
{
    return this->Socket != nullptr && this->Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected;
}

void FHyperlaneWorker::OnConnectedDelegateHandler(void) const
{
    WorkerStatics::RunLambdaOnBackGroundThread( [this] (void)
    {
        int Attempts = 0;
        while (Attempts < 3)
        {
            if (this->SendValidation())
            {
                return;
            }

            LOG_WARNING(LogHyperlane, "Hyperlane Worker failed to send validation message. Retrying in 1 second.")
            FPlatformProcess::Sleep(1.0f);
            ++Attempts;
        }

        LOG_FATAL(LogHyperlane, "Hyperlane Worker failed to send validation message. Exausting attempts after 3 tries.")

        return;
    });

    return;
}

void FHyperlaneWorker::OnDisconnectedDelegateHandler(void)
{
}

void FHyperlaneWorker::OnBytesReceivedDelegateHandler(TArray<uint8>& InBytes) const
{
    /*
     * We call the deserialize methods in a loop. As a single Socket-Recv call can contain multiple messages if it
     * receives more messages than we can currently handle.
     */
    while (true)
    {
        int32 BytesRead = 0;

        switch (TransmittableData::DeserializeType(InBytes))
        {
        case TransmittableData::EDataTransmissionType::Invalid:
        {
            LOG_FATAL(LogHyperlane, "Hyperlane Worker received invalid data.")
            break;
        }
        case TransmittableData::EDataTransmissionType::Ping:
        {
            LOG_VERY_VERBOSE(LogHyperlane, "Received ping message.")
            break;
        }
        case TransmittableData::EDataTransmissionType::ChunkInitializationData:
        {
            TransmittableData::FChunkInitializationData Data = TransmittableData::DeserializeChunkInitializationData(InBytes);
            LOG_VERY_VERBOSE(LogHyperlane, "Hyperlane Worker received chunk data for chunk [%s].", *Data.ChunkKey.ToString())
            AsyncTask(ENamedThreads::GameThread, [this, Data] (void)
            {
                this->OwningComponent->OnChunkInitializationDataReceived(Data);
            });
            BytesRead = TransmittableData::GetTypeSize(TransmittableData::EDataTransmissionType::ChunkInitializationData);
            break;
        }
        default:
        {
            checkNoEntry()
            break;
        }
        }

        if (BytesRead == 0)             { break; }
        if (BytesRead == InBytes.Num()) { break; }
        if (BytesRead > InBytes.Num())
        {
            LOG_FATAL(LogHyperlane, "Hyperlane Worker read more bytes than received.")
            break;
        }

        LOG_VERY_VERBOSE(
            LogHyperlane,
            "Received multiple message in one recv call. Read %d bytes from all %d bytes in buffer.",
            BytesRead, InBytes.Num()
        )

        /*
         * Offsetting the array. But actually we do not?
         * We may want to figure out a way on how to do properly offset the array without changing memory allocations.
         * Currently, this method is not very efficient.
         */
        InBytes.RemoveAt(0, BytesRead);

        continue;
    }


    return;
}

#undef RETURN_CODE_OK
#undef RETURN_CODE_ERROR
