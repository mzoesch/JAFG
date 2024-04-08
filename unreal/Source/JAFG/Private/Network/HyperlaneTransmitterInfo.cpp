// Copyright 2024 mzoesch. All rights reserved.

#include "Network/HyperlaneTransmitterInfo.h"

#include "Common/TcpSocketBuilder.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "Network/NetworkStatics.h"
#include "Serialization/BufferArchive.h"
#include "World/WorldPlayerController.h"
#include "World/Chunk/LocalPlayerChunkGeneratorSubsystem.h"

namespace CommonTransmitterStatics
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

void TransmittableData::FChunkInitializationData::SerializeToBytes(TArray<uint8>& OutBytes)
{
    FBufferArchive Ar = FBufferArchive();
    Ar.Seek(0b0);

    Ar << ChunkKey;
    Ar << Voxels;

    OutBytes.Empty();
    OutBytes.Append(Ar.GetData(), Ar.Num());

    Ar.FlushCache();
    Ar.Empty();

    return;
}

TransmittableData::FChunkInitializationData TransmittableData::FChunkInitializationData::DeserializeFromBytes(const TArray<uint8>& InBytes)
{
    TransmittableData::FChunkInitializationData Data;

    FMemoryReader Ar = FMemoryReader(InBytes, true);
    Ar.Seek(0b0);

    Ar << Data.ChunkKey;
    Ar << Data.Voxels;

    Ar.FlushCache();
    if (Ar.Close() == false)
    {
        UE_LOG(LogTemp, Fatal, TEXT("TransmittableData::FChunkInitializationData::DeserializeFromBytes: Failed to close memory reader."))
    }

    return Data;
}

AHyperlaneTransmitterInfo::AHyperlaneTransmitterInfo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->bReplicates = false;
    this->bNetLoadOnClient = false;

    this->bShouldListen = false;
    this->Clients.Empty();

    this->OnListenBeginDelegate.Unbind();
    this->OnListenBeginFailureDelegate.Unbind();
    this->OnListenEndDelegate.Unbind();
    this->OnClientConnectedDelegate.Unbind();
    this->OnClientDisconnectedDelegate.Unbind();

    this->bShouldPingCheck = false;
    this->PingCheckInterval = 0.0f;

    this->PingData.Empty();

    this->Port = 0x0;
    this->SocketName = L"";

    this->BufferMaxSizeInBytes = 0x0;

    this->Socket = nullptr;

    this->MaxBacklog = 0x0;

    this->ServerEndFuture = TFuture<void>();

    return;
}

void AHyperlaneTransmitterInfo::BeginPlay(void)
{
    Super::BeginPlay();

    if (UNetworkStatics::IsSafeClient(this))
    {
        UE_LOG(LogTemp, Fatal, TEXT("AHyperlaneTransmitterInfo::BeginPlay: Net loaded on a client. This is disallowed."))
        return;
    }

    if (UNetworkStatics::IsSafeStandalone(this))
    {
        UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::BeginPlay: Net loaded on a standalone. Discarding initializtation."))
        return;
    }

    /*
     * TODO Only create a hyperlane maybe if we are in a session? A future problem.
     */
    UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::BeginPlay: Net loaded on a server. Initializing."))

    this->bShouldListen = true;
    this->Clients.Empty();

    OnListenBeginDelegate.BindLambda( [&] (const FString& Address, const uint16& Port)
    {
        this->OnListenBeginDelegateHandler(Address, Port);
    });

    OnListenBeginFailureDelegate.BindLambda( [&] (const FString& Address, const uint16& Port)
    {
        this->OnListenBeginFailureDelegateHandler(Address, Port);
    });

    OnListenEndDelegate.BindLambda( [&] (void)
    {
        this->OnListenEndDelegateHandler();
    });

    OnClientConnectedDelegate.BindLambda( [&] (const FString& Address)
    {
        this->OnClientConnectedDelegateHandler(Address);
    });

    OnClientDisconnectedDelegate.BindLambda( [&] (const FString& Address)
    {
        this->OnClientDisconnectedDelegateHandler(Address);
    });

    this->bShouldPingCheck = true;
    this->PingCheckInterval = 5.0f;

    this->PingData.Append( /* (uint8*) */ reinterpret_cast<uint8*>(TCHAR_TO_UTF8(*this->PingMessage)), this->PingMessage.Len());

    FIPv4Address HostAddress;
    FIPv4Address::Parse(TEXT("0.0.0.0"), HostAddress);

    this->Port = 8080;
    this->SocketName = FString(TEXT("HyperlaneTransmitterInfo"));

    /* Roughly 4 MB. */
    this->BufferMaxSizeInBytes = 4 * 1024 * 1024;

    const FIPv4Endpoint Endpoint(HostAddress, this->Port);

    this->Socket = FTcpSocketBuilder(*this->SocketName)
        /* .AsNonBlocking() */
        .AsReusable()
        .BoundToEndpoint(Endpoint)
        .Listening(this->BufferMaxSizeInBytes);

    this->Socket->SetReceiveBufferSize(this->BufferMaxSizeInBytes, this->BufferMaxSizeInBytes);
    this->Socket->SetSendBufferSize(this->BufferMaxSizeInBytes, this->BufferMaxSizeInBytes);

    this->MaxBacklog = 0x8;

    if (this->Socket->Listen(this->MaxBacklog) == false)
    {
        this->OnListenBeginFailureDelegate.Execute(HostAddress.ToString(), this->Port);
        return;
    }

    this->OnListenBeginDelegate.Execute(HostAddress.ToString(), this->Port);

    this->CreateServerEndFuture();

    return;
}

void AHyperlaneTransmitterInfo::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (Socket != nullptr)
    {
        this->bShouldListen = false;
        this->bShouldPingCheck = false;

        UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::EndPlay: Waiting for Server End Future."))
        ServerEndFuture.Wait();
        UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::EndPlay: Finished waiting for Server End Future."))

        Socket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);

        Socket = nullptr;
    }

    return;
}

void AHyperlaneTransmitterInfo::SendChunkInitializationData(const AWorldPlayerController* Target, TransmittableData::FChunkInitializationData& Data)
{
    /*
     * Mocking the Hyperlane as a local controlled player controller will not have a worker for him on the
     * Hyperlane.
     */
    if (Target->IsLocalController())
    {
        check( GEngine )
        check( this->GetWorld() )
        check( GEngine->GetFirstGamePlayer(this->GetWorld()) )
        check( GEngine->GetFirstGamePlayer(this->GetWorld())->GetSubsystem<ULocalPlayerChunkGeneratorSubsystem>() )

        GEngine->GetFirstGamePlayer(this->GetWorld())->GetSubsystem<ULocalPlayerChunkGeneratorSubsystem>()->InitializeChunkWithAuthorityData(Data.ChunkKey, Data.Voxels);

        return;
    }

    TArray<uint8> Bytes = TArray<uint8>();
    Data.SerializeToBytes(Bytes);

    this->Emit(Bytes, Target->GetHyperlaneWorkerAddress());

    return;
}

bool AHyperlaneTransmitterInfo::IsHyperlaneWorkerValid(const FString& HyperlaneIdentifier, AWorldPlayerController*& OutPlayerController) const
{
    OutPlayerController = nullptr;

    check( this->GetWorld() )
    FConstPlayerControllerIterator It = this->GetWorld()->GetPlayerControllerIterator();
    check( It )

    for (; It; ++It)
    {
        if (AWorldPlayerController* PlayerController = Cast<AWorldPlayerController>(*It); PlayerController != nullptr)
        {
            if (PlayerController->GetHyperlaneIdentifier() == HyperlaneIdentifier)
            {
                OutPlayerController = PlayerController;
                return true;
            }

            continue;
        }

        continue;
    }

    return false;
}

void AHyperlaneTransmitterInfo::OnListenBeginDelegateHandler(const FString& InAddress, const uint16& InPort)
{
    UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::OnListenBeginDelegateHandler: Fired with %s:%d."), *InAddress, InPort)
}

void AHyperlaneTransmitterInfo::OnListenBeginFailureDelegateHandler(const FString& InAddress, const uint16& InPort)
{
    UE_LOG(LogTemp, Error, TEXT("AHyperlaneTransmitterInfo::OnListenBeginFailureDelegateHandler: Fired with %s:%d."), *InAddress, InPort)
}

void AHyperlaneTransmitterInfo::OnListenEndDelegateHandler(void)
{
    UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::OnListenEndDelegateHandler: Fired."))
}

void AHyperlaneTransmitterInfo::OnClientConnectedDelegateHandler(const FString& Address)
{
    UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::OnClientConnectedDelegateHandler: Fired with %s."), *Address)
}

void AHyperlaneTransmitterInfo::OnClientDisconnectedDelegateHandler(const FString& Address)
{
    UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::OnClientDisconnectedDelegateHandler: Fired with %s."), *Address)
}

void AHyperlaneTransmitterInfo::DisconnectSingleClient(const FString& ClientAddress)
{
    UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::DisconnectClient: Disconnecting %s."), *ClientAddress)

    if (ClientAddress == DisconnectAllClientsMessage)
    {
        UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::DisconnectClient: Disconnecting all clients. Current client connections: %d."), Clients.Num())

        for (const TPair<FString, TSharedPtr<FTCPTransmitterClient>>& ClientPair : this->Clients)
        {
            if (const TSharedPtr<FTCPTransmitterClient> Client = ClientPair.Value; Client->Socket != nullptr)
            {
                Client->Socket->Close();
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("AHyperlaneTransmitterInfo::DisconnectClient: Client %s is invalid."), *ClientPair.Key)
            }

            OnClientDisconnectedDelegate.Execute(ClientAddress);

            continue;
        }

        this->Clients.Empty();

        return;
    }

    if (const TSharedPtr<FTCPTransmitterClient> Client = Clients[ClientAddress]; Client.IsValid())
    {
        if (Client->Socket != nullptr)
        {
            Client->Socket->Close();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("AHyperlaneTransmitterInfo::DisconnectClient: Client %s is invalid."), *ClientAddress)
        }

        Clients.Remove(ClientAddress);

        OnClientDisconnectedDelegate.Execute(ClientAddress);

        return;
    }

    UE_LOG(LogTemp, Fatal, TEXT("AHyperlaneTransmitterInfo::DisconnectClient: Client %s is invalid."), *ClientAddress)

    return;
}

void AHyperlaneTransmitterInfo::CreateServerEndFuture(void)
{
    this->ServerEndFuture = CommonTransmitterStatics::RunLambdaOnBackGroundThread( [&] (void)
    {
        uint32 BufferSize = 0;
        TArray<uint8> ReceiveBuffer = TArray<uint8>();
        TArray<FString> NowValidatedClients = TArray<FString>();
        TArray<TSharedPtr<FTCPTransmitterClient>> ClientsDisconnected = TArray<TSharedPtr<FTCPTransmitterClient>>();
        ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

        FDateTime LastPing = FDateTime::Now();

        while (this->bShouldListen)
        {
            bool bHasPendingConnection = false;
            this->Socket->HasPendingConnection(bHasPendingConnection);
            if (bHasPendingConnection)
            {
                TSharedPtr<FInternetAddr> InternetAddr = SocketSubsystem->CreateInternetAddr();
                FSocket* NewSocket = this->Socket->Accept(*InternetAddr, TEXT("HyperlaneTransmitterInfo-Clients"));

                const FString AddressString = InternetAddr->ToString(true);

                TSharedPtr<FTCPTransmitterClient> NewClientItem = MakeShareable(new FTCPTransmitterClient());
                NewClientItem->Timestamp = FDateTime::Now();
                NewClientItem->Socket    = NewSocket;
                NewClientItem->Address   = AddressString;

                if (this->Clients.Contains(AddressString) || this->UnvalidatedClients.Contains(AddressString))
                {
                    UE_LOG(LogTemp, Fatal, TEXT("AHyperlaneTransmitterInfo::CreateServerEndFuture: Client %s already exists. Cannot add."), *AddressString)
                    return;
                }

                this->UnvalidatedClients.Add(AddressString, NewClientItem);
            }

            for (const TPair<FString, TSharedPtr<FTCPTransmitterClient>>& ClientPair : this->UnvalidatedClients)
            {
                TSharedPtr<FTCPTransmitterClient> Client = ClientPair.Value;

                if (const float TimeSinceConnected = (FDateTime::Now() - Client->Timestamp).GetTotalSeconds(); TimeSinceConnected > AHyperlaneTransmitterInfo::DisconnectUnvalidatedClientsTimeout)
                {
                    UE_LOG(LogTemp, Error, TEXT("AHyperlaneTransmitterInfo::CreateServerEndFuture: Disconnecting unvalidated client %s."), *Client->Address)

                    /*
                     * Note that we can't call the disconnect method here as it would change the iterator that
                     * we are currently iterating over.
                     */

                    if (Client->Socket->Close())
                    {
                        UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::CreateServerEndFuture: Closed socket for %s."), *Client->Address)
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("AHyperlaneTransmitterInfo::CreateServerEndFuture: Closed socket for %s with errors."), *Client->Address)
                    }

                    ClientsDisconnected.Add(Client);

                    continue;
                }

                /*
                 * Why is this always SCS_Connected even if we are not connected? Is this a bug in the engine?
                 * Either way when we ping the client we will know if he is not connected.
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

                BufferSize = 0;
                ReceiveBuffer.Empty();
                if (Client->Socket->HasPendingData(BufferSize))
                {
                    ReceiveBuffer.SetNumUninitialized(BufferSize);
                    int32 Read = 0;

                    if (Client->Socket->Recv(ReceiveBuffer.GetData(), ReceiveBuffer.Num(), Read))
                    {
                        ReceiveBuffer.Append(&AHyperlaneTransmitterInfo::UTF8Terminator, 1);
                        FString ReceivedData = FString(UTF8_TO_TCHAR(ReceiveBuffer.GetData()));
                        UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::CreateServerEndFuture: Received data from %s: %s. Combining with unreals client connection if valid."), *Client->Address, *ReceivedData)

                        AWorldPlayerController* CorrespondingPlayerController = nullptr;
                        if (this->IsHyperlaneWorkerValid(ReceivedData, CorrespondingPlayerController))
                        {
                            check( CorrespondingPlayerController )

                            if (this->Clients.Contains(Client->Address))
                            {
                                UE_LOG(LogTemp, Fatal, TEXT("AHyperlaneTransmitterInfo::CreateServerEndFuture: Client %s is already validated."), *Client->Address)
                                return;
                            }

                            CorrespondingPlayerController->SetHyperlaneWorkerAddress(Client->Address);
                            this->Clients.Add(Client->Address, Client);

                            /*
                             * Note that we remove the client item here as it would change the iterator that
                             * we are currently iterating over.
                             */

                            NowValidatedClients.Add(Client->Address);

                            continue;
                        }

                        UE_LOG(LogTemp, Error, TEXT("AHyperlaneTransmitterInfo::CreateServerEndFuture: Received invalid data from %s. Disconnecting them."), *Client->Address)

                        /*
                         * Note that we can't call the disconnect method here as it would change the iterator that
                         * we are currently iterating over.
                         */

                        if (Client->Socket->Close())
                        {
                            UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::CreateServerEndFuture: Closed socket for %s."), *Client->Address)
                        }
                        else
                        {
                            UE_LOG(LogTemp, Error, TEXT("AHyperlaneTransmitterInfo::CreateServerEndFuture: Closed socket for %s with errors."), *Client->Address)
                        }

                        ClientsDisconnected.Add(Client);

                        continue;
                    }

                    UE_LOG(LogTemp, Error, TEXT("AHyperlaneTransmitterInfo::CreateServerEndFuture: Failed to receive data from %s. Tried reading %d but only got %d."), *Client->Address, BufferSize, Read)

                    /*
                     * Note that we can't call the disconnect method here as it would change the iterator that
                     * we are currently iterating over.
                     */

                    if (Client->Socket->Close())
                    {
                        UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::CreateServerEndFuture: Closed socket for %s."), *Client->Address)
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("AHyperlaneTransmitterInfo::CreateServerEndFuture: Closed socket for %s with errors."), *Client->Address)
                    }

                    ClientsDisconnected.Add(Client);

                    continue;
                }

                continue;
            }

            if (NowValidatedClients.Num() > 0)
            {
                for (const FString& ClientAddress : NowValidatedClients)
                {
                    this->UnvalidatedClients.Remove(ClientAddress);

                    AsyncTask(ENamedThreads::GameThread, [&, ClientAddress] (void)
                    {
                        this->OnClientConnectedDelegate.Execute(ClientAddress);
                    });
                }

                NowValidatedClients.Empty();
            }

            if (this->bShouldPingCheck && ((FDateTime::Now() - LastPing).GetTotalSeconds() > this->PingCheckInterval))
            {
                LastPing = FDateTime::Now();

                for (const TPair<FString, TSharedPtr<FTCPTransmitterClient>>& ClientPair : this->Clients)
                {
                    TSharedPtr<FTCPTransmitterClient> Client = ClientPair.Value;

                    /*
                     * Why is this always SCS_Connected even if we are not connected? Is this a bug in the engine?
                     * Either way when we ping the client we will know if he is not connected.
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
                    /*
                     * We probably should send a new line?
                     */
                    if (const bool Sent = Client->Socket->Send(PingData.GetData(), PingData.Num(), BytesSent); Sent == false)
                    {
                        UE_LOG(LogTemp, Error, TEXT("AHyperlaneTransmitterInfo::CreateServerEndFuture: Failed to send ping to %s."), *Client->Address)

                        /*
                         * Note that we can't call the disconnect method here as it would change the iterator that
                         * we are currently iterating over.
                         */

                        if (Client->Socket->Close())
                        {
                            UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::CreateServerEndFuture: Closed socket for %s."), *Client->Address)
                        }
                        else
                        {
                            UE_LOG(LogTemp, Error, TEXT("AHyperlaneTransmitterInfo::CreateServerEndFuture: Closed socket for %s with errors."), *Client->Address)
                        }

                        ClientsDisconnected.Add(Client);
                    }

                    continue;
                }
            }

            if (ClientsDisconnected.Num() > 0)
            {
                for (const TSharedPtr<FTCPTransmitterClient> ClientToRemove : ClientsDisconnected)
                {
                    const FString AddrFromDeadClient = ClientToRemove->Address;

                    this->UnvalidatedClients.Remove(AddrFromDeadClient);
                    this->Clients.Remove(AddrFromDeadClient);

                    AsyncTask(ENamedThreads::GameThread, [&, AddrFromDeadClient]()
                    {
                        this->OnClientDisconnectedDelegate.Execute(AddrFromDeadClient);
                    });
                }

                ClientsDisconnected.Empty();
            }

            /*
             * Wait for a bit to not consume all CPU time.
             * We should take a look here again in the future for a better solution.
             */
            FPlatformProcess::Sleep(0.3f);
        }

        this->DisconnectAllClients();

        /* Servers lifetime is over. */

        AsyncTask(ENamedThreads::GameThread, [&] (void)
        {
            this->OnListenEndDelegate.Execute();
        });

        return;
    });

    return;
}

void AHyperlaneTransmitterInfo::Emit(const TArray<uint8>& InBytes, const FString& InClientAddress)
{
    const TSharedPtr<FTCPTransmitterClient> Client = this->Clients.Contains(InClientAddress) ? this->Clients[InClientAddress] : nullptr;

    if (Client == nullptr || Client.IsValid() == false)
    {
        UE_LOG(LogTemp, Fatal, TEXT("AHyperlaneTransmitterInfo::Emit: Client %s is invalid."), *InClientAddress)
        return;
    }

    int32 BytesSent = 0;
    if (Client->Socket->Send(InBytes.GetData(), InBytes.Num(), BytesSent) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("AHyperlaneTransmitterInfo::Emit: Failed to send data to %s."), *InClientAddress)
        this->DisconnectSingleClient(InClientAddress);
        return;
    }

    if (BytesSent != InBytes.Num())
    {
        UE_LOG(LogTemp, Fatal, TEXT("AHyperlaneTransmitterInfo::Emit: Failed to send all data to %s."), *InClientAddress)
        this->DisconnectSingleClient(InClientAddress);
        return;
    }

    return;
}
