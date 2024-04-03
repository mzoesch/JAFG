// Copyright 2024 mzoesch. All rights reserved.

#include "Network/HyperlaneTransmitterInfo.h"

#include "Common/TcpSocketBuilder.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "Network/NetworkStatics.h"

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

AHyperlaneTransmitterInfo::AHyperlaneTransmitterInfo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->bReplicates = false;
    this->bNetLoadOnClient = false;

    this->bShouldListen = false;
    this->Clients.Empty();

    OnListenBeginDelegate.Unbind();
    OnListenBeginFailureDelegate.Unbind();
    OnListenEndDelegate.Unbind();
    OnClientConnectedDelegate.Unbind();
    OnClientDisconnectedDelegate.Unbind();

    this->bShouldPingCheck = false;
    this->PingCheckInterval = 0.0f;

    this->PingMessage = L"";
    this->PingData.Empty();

    this->Port = 0x0;
    this->SocketName = FString(TEXT(""));

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

    this->PingMessage = L"<PING>";
    this->PingData.Append( /* (uint8*) */ reinterpret_cast<uint8*>(TCHAR_TO_UTF8(*PingMessage)), PingMessage.Len());

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

        UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::EndPlay: Waiting for server end future."))
        ServerEndFuture.Wait();
        UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::EndPlay: Finished waiting for server end future."))

        Socket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);

        Socket = nullptr;
    }

    return;
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

        for (const TPair<FString, TSharedPtr<FTCPTransmitterClient>>& ClientPair : Clients)
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

        Clients.Empty();

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
        TArray<TSharedPtr<FTCPTransmitterClient>> ClientsDisconnected = TArray<TSharedPtr<FTCPTransmitterClient>>();
        ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

        FDateTime LastPing = FDateTime::Now();

        while (this->bShouldListen)
        {
            bool bHasPendingConnection = false;
            Socket->HasPendingConnection(bHasPendingConnection);
            if (bHasPendingConnection)
            {
                TSharedPtr<FInternetAddr> InternetAddr = SocketSubsystem->CreateInternetAddr();
                FSocket* NewSocket = Socket->Accept(*InternetAddr, TEXT("HyperlaneTransmitterInfo-Clients"));

                const FString AddressString = InternetAddr->ToString(true);

                TSharedPtr<FTCPTransmitterClient> NewClientItem = MakeShareable(new FTCPTransmitterClient());
                NewClientItem->Socket  = NewSocket;
                NewClientItem->Address = AddressString;

                if (Clients.FindRef(AddressString) != nullptr)
                {
                    UE_LOG(LogTemp, Fatal, TEXT("AHyperlaneTransmitterInfo::CreateServerEndFuture: Client %s already exists. Cannot add."), *AddressString)
                    return;
                }

                Clients.Add(AddressString, NewClientItem);

                AsyncTask(ENamedThreads::GameThread, [&, AddressString] (void)
                {
                    OnClientConnectedDelegate.Execute(AddressString);
                });
            }

            const float TimeSinceLastPing = (FDateTime::Now() - LastPing).GetTotalSeconds();
            const bool bPingThisIteration = this->bShouldPingCheck && TimeSinceLastPing > this->PingCheckInterval;
            if (bPingThisIteration)
            {
                LastPing = FDateTime::Now();
            }

            for (const TPair<FString, TSharedPtr<FTCPTransmitterClient>>& ClientPair : Clients)
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

                if (Client->Socket->HasPendingData(BufferSize))
                {
                    ReceiveBuffer.SetNumUninitialized(BufferSize);
                    int32 Read = 0;

                    if (Client->Socket->Recv(ReceiveBuffer.GetData(), ReceiveBuffer.Num(), Read) == false)
                    {
                        UE_LOG(LogTemp, Error, TEXT("AHyperlaneTransmitterInfo::CreateServerEndFuture: Failed to receive data from %s."), *Client->Address)

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

                    UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::CreateServerEndFuture: Received %d bytes from %s."), Read, *Client->Address)
                }

                if (bPingThisIteration)
                {
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
                }

                continue;
            }

            if (ClientsDisconnected.Num() > 0)
            {
                for (const TSharedPtr<FTCPTransmitterClient> ClientToRemove : ClientsDisconnected)
                {
                    const FString AddrFromDeadClient = ClientToRemove->Address;

                    Clients.Remove(AddrFromDeadClient);

                    AsyncTask(ENamedThreads::GameThread, [&, AddrFromDeadClient]()
                    {
                        OnClientDisconnectedDelegate.Execute(AddrFromDeadClient);
                    });
                }

                ClientsDisconnected.Empty();
            }

            /*
             * Wait for a bit to not consume all CPU time.
             * We should take a look here again in the future for a better solution.
             */
            FPlatformProcess::Sleep(0.03f);
        }

        this->DisconnectAllClients();

        /* Servers lifetime is over. */

        AsyncTask(ENamedThreads::GameThread, [&] (void)
        {
            OnListenEndDelegate.Execute();
        });

        return;
    });

    return;
}
