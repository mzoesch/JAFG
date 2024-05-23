// Copyright 2024 mzoesch. All rights reserved.

#include "HyperlaneTransmitter.h"

#include "JAFGLogDefs.h"
#include "HyperlaneTransmitterSubsystem.h"
#include "Common/TcpSocketBuilder.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"

namespace TransmitterStatics
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

FHyperlaneTransmitter::FHyperlaneTransmitter(void)
{
}

FHyperlaneTransmitter::FHyperlaneTransmitter(UHyperlaneTransmitterSubsystem* InOwningSubsystem)
{
    LOG_DISPLAY(LogHyperlane, "Starting Hyperlane Transmitter.")

    check( InOwningSubsystem )
    this->OwningSubsystem = InOwningSubsystem;

    this->OnListenBeginDelegate.BindLambda( [this] (const FClientAddress& Address, const uint16& Port) { this->OnListenBeginDelegateHandler(Address, Port); });
    this->OnListenBeginFailureDelegate.BindLambda( [this] (const FClientAddress& Address, const uint16& Port) { this->OnListenBeginFailureDelegateHandler(Address, Port); });
    this->OnListenEndDelegate.BindLambda( [this] (void) { this->OnListenEndDelegateHandler(); });
    this->OnClientConnectedDelegate.BindLambda( [this] (const FClientAddress& Address) { this->OnClientConnectedDelegateHandler(Address); });
    this->OnClientDisconnectedDelegate.BindLambda( [this] (const FClientAddress& Address) { this->OnClientDisconnectedDelegateHandler(Address); });

    this->UnvalidatedClients.Empty();
    this->Clients.Empty();

    FIPv4Address HostAddr; FIPv4Address::Parse(TEXT("0.0.0.0"), HostAddr);

    const FIPv4Endpoint Endpoint(HostAddr, this->OutPort);

    this->Socket = FTcpSocketBuilder(*this->SocketName)
        /* .AsNonBlocking() */
        .AsReusable()
        .BoundToEndpoint(Endpoint)
        .Listening(FHyperlaneTransmitter::DesiredMaxBufferSizeInBytes);

    if (this->Socket->SetReceiveBufferSize(FHyperlaneTransmitter::DesiredMaxBufferSizeInBytes, this->RecvPlatformMaxBufferSizeInBytes) == false)
    {
        LOG_FATAL(LogHyperlane, "Failed to set receive buffer size.")
        return;
    }
    if (this->Socket->SetSendBufferSize(FHyperlaneTransmitter::DesiredMaxBufferSizeInBytes, this->SendPlatformMaxBufferSizeInBytes) == false)
    {
        LOG_FATAL(LogHyperlane, "Failed to set send buffer size.")
        return;
    }
    check( this->RecvPlatformMaxBufferSizeInBytes == FHyperlaneTransmitter::DesiredMaxBufferSizeInBytes )
    check( this->SendPlatformMaxBufferSizeInBytes == FHyperlaneTransmitter::DesiredMaxBufferSizeInBytes )

    if (this->Socket->Listen(this->MaxBacklog) == false)
    {
        this->OnListenBeginFailureDelegate.Execute(HostAddr.ToString(), this->OutPort);
        return;
    }

    this->OnListenBeginDelegate.Execute(HostAddr.ToString(), this->OutPort);

    this->CreateServerEndFuture();

    return;
}

FHyperlaneTransmitter::~FHyperlaneTransmitter(void)
{
    if (this->Socket != nullptr)
    {
        this->bShouldListen    = false;
        this->bShouldPingCheck = false;

        this->bStoppedThroughDestructor = true;

        LOG_DISPLAY(LogHyperlane, "Waiting for Server End Future.")
        this->ServerEndFuture.Wait();
        LOG_DISPLAY(LogHyperlane, "Finished waiting for Server End Future.")

        this->Socket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(this->Socket);

        this->Socket = nullptr;
    }
    else
    {
        LOG_WARNING(LogHyperlane, "Socket was invalid. Could not destroy socket and close connection in a controlled manner.")
    }

    return;
}

void FHyperlaneTransmitter::SendChunkInitializationData(const UHyperlaneComponent* Target, TransmittableData::FChunkInitializationData& Data)
{
    if (Target->IsLocalController())
    {
        checkNoEntry()
        return;
    }

    TArray<uint8> Bytes = TArray<uint8>();
    Data.SerializeToBytes(Bytes);

    this->Emit(Bytes, Target->GetHyperlaneWorkerAddress());

    return;
}

void FHyperlaneTransmitter::DisconnectAllClients(void)
{
    if (this->Clients.Num() == 0)
    {
        LOG_DISPLAY(LogHyperlane, "No clients to disconnect.")
        return;
    }

    LOG_DISPLAY(LogHyperlane, "Current client connections: %d.", this->Clients.Num())

    for (const TPair<FClientAddress, TSharedPtr<FTCPTransmitterClient>>& ClientPair : this->Clients)
    {
       this->DisconnectSingleClient(ClientPair.Key, false);
    }

    this->Clients.Empty();

    return;
}

void FHyperlaneTransmitter::DisconnectSingleClient(const FClientAddress& TargetAddr, const bool bRemoveFromMap /* = true */)
{
    LOG_DISPLAY(LogHyperlane, "Disconnecting %s.", *TargetAddr)

    const TSharedPtr<FTCPTransmitterClient> Client = this->Clients.Contains(TargetAddr) ? this->Clients[TargetAddr] : nullptr;

    if (Client == nullptr || Client.IsValid() == false)
    {
        LOG_FATAL(LogHyperlane, "Client %s is invalid.", *TargetAddr)
        return;
    }

    if (Client->Socket != nullptr)
    {
        Client->Socket->Close();
        Client->Socket = nullptr;
    }
    else
    {
        LOG_ERROR(LogHyperlane, "Client's socket was already invalidated. Faulty client: %s.", *TargetAddr)
    }

    if (bRemoveFromMap)
    {
        this->Clients.Remove(TargetAddr);
    }

    this->OnClientDisconnectedDelegate.Execute(TargetAddr);

    return;
}

bool FHyperlaneTransmitter::IsHyperlaneWorkerValid(const FHyperlaneIdentifier& HyperlaneIdentifier, UHyperlaneComponent*& OutHyperlaneWorker) const
{
    check( this->OwningSubsystem )

    FConstPlayerControllerIterator It = this->OwningSubsystem->GetWorld()->GetPlayerControllerIterator();
    check( It )
    for (; It; ++It)
    {
        const APlayerController* PlayerController = It->Get(); check( PlayerController )
        if (
            UHyperlaneComponent* Comp = Cast<UHyperlaneComponent>(PlayerController->GetComponentByClass(UHyperlaneComponent::StaticClass()));
            Comp
        )
        {
            if (Comp->GetHyperlaneIdentifier() == HyperlaneIdentifier)
            {
                OutHyperlaneWorker = Comp;
                return true;
            }

            continue;
        }

        continue;
    }

    return false;
}

void FHyperlaneTransmitter::CreateServerEndFuture(void)
{
    this->ServerEndFuture = TransmitterStatics::RunLambdaOnBackGroundThread( [this] (void)
    {
        uint32                                    BufferSize          = 0;
        TArray<uint8>                             ReceiveBuffer       = TArray<uint8>();
        TArray<FString>                           NowValidatedClients = TArray<FString>();
        TArray<TSharedPtr<FTCPTransmitterClient>> ClientsDisconnected = TArray<TSharedPtr<FTCPTransmitterClient>>();
        ISocketSubsystem*                         SocketSubsystem     = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

        check( SocketSubsystem )

        FDateTime LastPing = FDateTime::Now();

        while (this->bShouldListen)
        {
            // Searching for pending connections.
            //////////////////////////////////////////////////////////////////////////
            bool bHasPendingConnection = false;
            this->Socket->HasPendingConnection(bHasPendingConnection);
            if (bHasPendingConnection)
            {
                TSharedPtr<FInternetAddr> InternetAddr = SocketSubsystem->CreateInternetAddr();
                FSocket* NewSocket = this->Socket->Accept(*InternetAddr, TEXT("HyperlaneTransmitterInfo-Clients"));

                const FClientAddress AddrStr = InternetAddr->ToString(true);

                TSharedPtr<FTCPTransmitterClient> NewClientItem = MakeShareable(new FTCPTransmitterClient());
                NewClientItem->Timestamp = FDateTime::Now();
                NewClientItem->Socket    = NewSocket;
                NewClientItem->Address   = AddrStr;

                if (this->Clients.Contains(AddrStr))
                {
                    LOG_ERROR(LogHyperlane, "Client %s already exists. Cannot add.", *AddrStr)
                }
                else if (this->IsClientUnvalidated(AddrStr))
                {
                    LOG_ERROR(LogHyperlane, "Client %s is already in unvalidated queue. Cannot add.", *AddrStr)
                }
                else
                {
                    this->UnvalidatedClients.Add(NewClientItem);
                }
            }

            // Disconnect timed out unvalidated clients.
            //////////////////////////////////////////////////////////////////////////
            for (const TSharedPtr<FTCPTransmitterClient> ArrElem : this->UnvalidatedClients)
            {
                if (
                    const float TimeSinceConnected = (FDateTime::Now() - ArrElem->Timestamp).GetTotalSeconds();
                    TimeSinceConnected > FHyperlaneTransmitter::DisconnectUnvalidatedClientsTimeoutInSeconds
                )
                {
                    LOG_ERROR(LogHyperlane, "Disconnecting timed out unvalidated client %s.", *ArrElem->Address)

                    if (ArrElem->Socket->Close())
                    {
                        LOG_DISPLAY(LogHyperlane, "Closed socket for %s.", *ArrElem->Address)
                    }
                    else
                    {
                        LOG_ERROR(LogHyperlane, "Closed socket for %s with unknown errors.", *ArrElem->Address)
                    }

                    ClientsDisconnected.Add(ArrElem);

                    continue;
                }

                /*
                 * Why is this always SCS_Connected even if we are not connected? Is this a bug in the engine?
                 * Either way, when we ping the client, we will know if he is not connected.
                 * Just a safety check here but essentially unnecessary.
                 */
                ESocketConnectionState ConnectionState = ESocketConnectionState::SCS_NotConnected;
                if (ArrElem->Socket != nullptr)
                {
                    ConnectionState = ArrElem->Socket->GetConnectionState();
                }

                if (ConnectionState != ESocketConnectionState::SCS_Connected)
                {
                    ClientsDisconnected.Add(ArrElem);
                    continue;
                }

                BufferSize = 0;
                ReceiveBuffer.Empty();
                if (ArrElem->Socket->HasPendingData(BufferSize))
                {
                    ReceiveBuffer.SetNumUninitialized(BufferSize);
                    int32 Read = 0;

                    if (ArrElem->Socket->Recv(ReceiveBuffer.GetData(), ReceiveBuffer.Num(), Read))
                    {
                        ReceiveBuffer.Append(&FHyperlaneTransmitter::UTF8Terminator, 1);
                        FString ReceivedData = FString(UTF8_TO_TCHAR(ReceiveBuffer.GetData()));
                        LOG_DISPLAY(LogHyperlane, "Received data from %s: %s. Combining with unreals client connection if valid.", *ArrElem->Address, *ReceivedData)

                        if (
                            UHyperlaneComponent* CorrespondingWorker = nullptr;
                            this->IsHyperlaneWorkerValid(ReceivedData, CorrespondingWorker)
                        )
                        {
                            check( CorrespondingWorker )

                            if (this->Clients.Contains(ArrElem->Address))
                            {
                                LOG_FATAL(LogHyperlane, "Client %s is already validated.", *ArrElem->Address)
                                return;
                            }

                            CorrespondingWorker->SetHyperlaneWorkerAddress(ArrElem->Address);
                            this->Clients.Add(ArrElem->Address, ArrElem);

                            NowValidatedClients.Add(ArrElem->Address);

                            continue;
                        }

                        LOG_ERROR(LogHyperlane, "Received invalid data from %s. Disconnecting them.", *ArrElem->Address)

                        if (ArrElem->Socket->Close())
                        {
                            LOG_DISPLAY(LogHyperlane, "Closed socket for %s.", *ArrElem->Address)
                        }
                        else
                        {
                            LOG_ERROR(LogHyperlane, "Closed socket for %s with unknown errors.", *ArrElem->Address)
                        }

                        ClientsDisconnected.Add(ArrElem);

                        continue;
                    }

                    LOG_ERROR(LogHyperlane, "Failed to receive data from %s. Tried reading %d but only got %d.", *ArrElem->Address, BufferSize, Read)

                    if (ArrElem->Socket->Close())
                    {
                        LOG_DISPLAY(LogHyperlane, "Closed socket for %s.", *ArrElem->Address)
                    }
                    else
                    {
                        LOG_ERROR(LogHyperlane, "Closed socket for %s with unknown errors.", *ArrElem->Address)
                    }

                    ClientsDisconnected.Add(ArrElem);

                    continue;
                }

                continue;
            }

            // Add validated clients to the clients map.
            //////////////////////////////////////////////////////////////////////////
            if (NowValidatedClients.Num() > 0)
            {
                for (const FString& ClientAddress : NowValidatedClients)
                {
                    this->RemoveUnvalidatedClient(ClientAddress);

                    TransmitterStatics::RunLambdaOnGameThread( [this, ClientAddress] (void)
                    {
                        this->OnClientConnectedDelegate.Execute(ClientAddress);
                    });
                }

                NowValidatedClients.Empty();
            }

            // Check if validated clients are still connected.
            //////////////////////////////////////////////////////////////////////////
            if (
                   this->bShouldPingCheck
                && (FDateTime::Now() - LastPing).GetTotalSeconds() > this->PingCheckIntervalInSeconds
            )
            {
                LastPing = FDateTime::Now();

                for (const TPair<FString, TSharedPtr<FTCPTransmitterClient>>& ClientPair : this->Clients)
                {
                    TSharedPtr<FTCPTransmitterClient> Client = ClientPair.Value;

                    /*
                     * Why is this always SCS_Connected even if we are not connected? Is this a bug in the engine?
                     * Either way, when we ping the client, we will know if he is not connected.
                     * Just a safety check here but essentially unnecessary.
                     */
                    ESocketConnectionState ConnectionState = ESocketConnectionState::SCS_NotConnected;
                    if (Client->Socket != nullptr)
                    {
                        ConnectionState = Client->Socket->GetConnectionState();
                    }

                    if (ConnectionState != ESocketConnectionState::SCS_Connected)
                    {
                        ClientsDisconnected.Add(Client);
                        continue;
                    }

                    int32 BytesSent = 0;
                    if (const bool Sent = Client->Socket->Send(this->PingAsBytes.GetData(), this->PingAsBytes.Num(), BytesSent); Sent == false)
                    {
                        LOG_ERROR(LogHyperlane, "Failed to send ping to %s.", *Client->Address)

                        if (Client->Socket->Close())
                        {
                            LOG_DISPLAY(LogHyperlane, "Closed socket for %s.", *Client->Address)
                        }
                        else
                        {
                            LOG_ERROR(LogHyperlane, "Closed socket for %s with unknown errors.", *Client->Address)
                        }

                        ClientsDisconnected.Add(Client);
                    }

                    continue;
                }
            }

            // Safely disconnect clients without changing current iterators.
            //////////////////////////////////////////////////////////////////////////
            if (ClientsDisconnected.Num() > 0)
            {
                for (const TSharedPtr<FTCPTransmitterClient> ClientToRemove : ClientsDisconnected)
                {
                    const FClientAddress AddrFromDeadClient = ClientToRemove->Address;

                    if (this->IsClientUnvalidated(AddrFromDeadClient))
                    {
                        this->RemoveUnvalidatedClient(AddrFromDeadClient);
                    }

                    this->Clients.Remove(AddrFromDeadClient);

                    TransmitterStatics::RunLambdaOnGameThread([this, AddrFromDeadClient] (void)
                    {
                        this->OnClientDisconnectedDelegate.Execute(AddrFromDeadClient);
                    });
                }

                ClientsDisconnected.Empty();
            }

            /* Wait for a bit to not consume all CPU time. */
            FPlatformProcess::Sleep(0.3f);

            continue;
        }

        /*
         * Server's lifetime is over.
         * Cleaning up.
         */

        this->DisconnectAllClients();

        /*
         * We cannot broadcast as the game is going to end, and we cannot acquire read access anymore on the
         * game thread.
         */
        if (this->bStoppedThroughDestructor == false)
        {
            TransmitterStatics::RunLambdaOnGameThread( [this] (void) { this->OnListenEndDelegate.Execute(); });
        }

        return;
    });

    return;
}

void FHyperlaneTransmitter::Emit(const TArray<uint8>& InBytes, const FClientAddress& TargetAddr)
{
    const TSharedPtr<FTCPTransmitterClient> Client = this->Clients.Contains(TargetAddr) ? this->Clients[TargetAddr] : nullptr;

    if (Client == nullptr || Client.IsValid() == false)
    {
        LOG_FATAL(LogHyperlane, "Client %s is invalid.", *TargetAddr)
        return;
    }

    int32 BytesSent = 0;
    if (Client->Socket->Send(InBytes.GetData(), InBytes.Num(), BytesSent) == false)
    {
        LOG_ERROR(LogHyperlane, "Failed to send data to %s.", *TargetAddr)
        this->DisconnectSingleClient(TargetAddr);
        return;
    }

    if (BytesSent != InBytes.Num())
    {
        LOG_FATAL(LogHyperlane, "Failed to send all data to %s.", *TargetAddr)
        this->DisconnectSingleClient(TargetAddr);
        return;
    }

    LOG_VERY_VERBOSE(LogHyperlane, "Sent %d bytes to %s.", BytesSent, *TargetAddr)

    return;
}

void FHyperlaneTransmitter::OnListenBeginDelegateHandler(const FClientAddress& InAddress, const uint16& InPort)
{
}

void FHyperlaneTransmitter::OnListenBeginFailureDelegateHandler(const FClientAddress& InAddress, const uint16& InPort)
{
}

void FHyperlaneTransmitter::OnListenEndDelegateHandler()
{
}

void FHyperlaneTransmitter::OnClientConnectedDelegateHandler(const FClientAddress& InAddress)
{
}

void FHyperlaneTransmitter::OnClientDisconnectedDelegateHandler(const FClientAddress& InAddress)
{
}
