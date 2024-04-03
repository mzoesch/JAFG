// Copyright 2024 mzoesch. All rights reserved.

#include "Network/HyperlaneTransmitterInfo.h"

#include "Common/TcpSocketBuilder.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "Layers/LayersSubsystem.h"
#include "Network/NetworkStatics.h"

AHyperlaneTransmitterInfo::AHyperlaneTransmitterInfo(const FObjectInitializer& ObjectInitializer) : Super(
    ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->bReplicates = false;
    this->bNetLoadOnClient = false;

    return;
}

void AHyperlaneTransmitterInfo::BeginPlay(void)
{
    Super::BeginPlay();

    if (UNetworkStatics::IsSafeClient(this))
    {
        UE_LOG(LogTemp, Fatal,
               TEXT("AHyperlaneTransmitterInfo::BeginPlay: Net loaded on a client. This is disallowed."))
        return;
    }

    if (UNetworkStatics::IsSafeStandalone(this))
    {
        UE_LOG(LogTemp, Warning,
               TEXT("AHyperlaneTransmitterInfo::BeginPlay: Net loaded on a standalone. Discarding initializtation."))
        return;
    }

    UE_LOG(LogTemp, Warning,
           TEXT("AHyperlaneTransmitterInfo::BeginPlay: Net loaded on a server. Initializing HyperlaneTransmitterInfo."))

    this->Clients.Empty();

    OnListenBegin.BindLambda([]() { UE_LOG(LogTemp, Warning, TEXT("OnListenBegin: fired.")) });
    OnListenEnd.BindLambda([]() { UE_LOG(LogTemp, Warning, TEXT("OnListenEnd: fired.")) });
    OnClientConnected.BindLambda([](const FString& Address)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnClientConnected: fired with %s."), *Address)
    });
    OnClientDisconnected.BindLambda([](const FString& Address)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnClientDisconnected: fired with %s."), *Address)
    });

    PingData.Append((uint8*)TCHAR_TO_UTF8(*PingMessage), PingMessage.Len());

    FIPv4Address Address;
    FIPv4Address::Parse(TEXT("0.0.0.0"), Address);

    this->Port = 8080;
    this->SocketName = FString(TEXT("HyperlaneTransmitterInfo"));
    /* 2 MB */
    this->BufferMaxSizeInBytes = 2 * 1024 * 1024;

    this->bShouldListen = true;

    FIPv4Endpoint Endpoint(Address, this->Port);

    this->Socket = FTcpSocketBuilder(*this->SocketName)
                   /* .AsNonBlocking() */
                   .AsReusable()
                   .BoundToEndpoint(Endpoint)
                   .Listening(this->BufferMaxSizeInBytes);

    this->Socket->SetReceiveBufferSize(this->BufferMaxSizeInBytes, this->BufferMaxSizeInBytes);
    this->Socket->SetSendBufferSize(this->BufferMaxSizeInBytes, this->BufferMaxSizeInBytes);

    if (this->Socket->Listen(8))
    {
        UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::BeginPlay: Listening on %s:%d."), *Address.ToString(),
               this->Port)
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AHyperlaneTransmitterInfo::BeginPlay: Failed to listen on %s:%d."),
               *Address.ToString(), this->Port)
        return;
    }

    ServerFinishedFuture = RunLambdaOnBackGroundThread([&]()
    {
        uint32 BufferSize = 0;
        TArray<uint8> ReceiveBuffer;
        TArray<TSharedPtr<FTCPTransmitterClient>> ClientsDisconnected;
        ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

        FDateTime LastPing = FDateTime::Now();

        while (bShouldListen)
        {
            bool bHasPendingConnection = false;
            Socket->HasPendingConnection(bHasPendingConnection);
            if (bHasPendingConnection)
            {
                TSharedPtr<FInternetAddr> InternetAddr = SocketSubsystem->CreateInternetAddr();
                FSocket* NewSocket = Socket->Accept(*InternetAddr, TEXT("HyperlaneTransmitterInfo-Clients"));

                const FString AddressString = InternetAddr->ToString(true);

                TSharedPtr<FTCPTransmitterClient> NewClientItem = MakeShareable(new FTCPTransmitterClient());
                NewClientItem->Socket = NewSocket;
                NewClientItem->Address = AddressString;

                if (Clients.FindRef(AddressString) != nullptr)
                {
                    UE_LOG(LogTemp, Fatal, TEXT("AHyperlaneTransmitterInfo::BeginPlay: Client %s already exists. Cannot add."), *AddressString)
                    return;
                }

                Clients.Add(AddressString, NewClientItem);

                AsyncTask(ENamedThreads::GameThread, [&]()
                {
                    OnClientConnected.Execute(AddressString);
                });
            }

            FDateTime Now = FDateTime::Now();
            for (auto ClientPair : Clients)
            {
                TSharedPtr<FTCPTransmitterClient> Client = ClientPair.Value;

                //Did we disconnect? Note that this almost never changed from connected due to engine bug, instead it will be caught when trying to send data
                ESocketConnectionState ConnectionState = SCS_NotConnected;
                if (Client->Socket != nullptr)
                {
                    ConnectionState = Client->Socket->GetConnectionState();
                }

                if (ConnectionState != SCS_Connected)
                {
                    ClientsDisconnected.Add(Client);
                    continue;
                }

                if (Client->Socket->HasPendingData(BufferSize))
                {
                    ReceiveBuffer.SetNumUninitialized(BufferSize);
                    int32 Read = 0;

                    Client->Socket->Recv(ReceiveBuffer.GetData(), ReceiveBuffer.Num(), Read);

                    UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::BeginPlay: Received %d bytes from %s."), Read, *Client->Address)
                }

                if (bShouldPingCheck)
                {
                    float TimeSinceLastPing = (Now- LastPing).GetTotalSeconds();

                    if (TimeSinceLastPing > PingCheckInterval)
                    {
                        LastPing = Now;
                        int32 BytesSent = 0;
                        /*
                         * We probably should send a new line?
                         */
                        bool Sent = Client->Socket->Send(PingData.GetData(), PingData.Num(), BytesSent);
                        if (Sent)
                        {
                            // UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::BeginPlay: Sent ping to %s."), *Client->Address)
                        }
                        else
                        {
                            UE_LOG(LogTemp, Error, TEXT("AHyperlaneTransmitterInfo::BeginPlay: Failed to send ping to %s."), *Client->Address)
                            if (Client->Socket->Close())
                            {
                                UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::BeginPlay: Closed socket for %s."), *Client->Address)
                            }
                            else
                            {
                                UE_LOG(LogTemp, Error, TEXT("AHyperlaneTransmitterInfo::BeginPlay: Closed socket for %s with errors."), *Client->Address)
                            }
                            ClientsDisconnected.Add(Client);
                        }
                    }
                }
            }

            if (ClientsDisconnected.Num() > 0)
            {
                for (TSharedPtr<FTCPTransmitterClient> ClientToRemove : ClientsDisconnected)
                {
                    const FString AddrFromDeadClient = ClientToRemove->Address;
                    Clients.Remove(AddrFromDeadClient);
                    AsyncTask(ENamedThreads::GameThread, [&, AddrFromDeadClient]()
                    {
                        OnClientDisconnected.Execute(AddrFromDeadClient);
                    });
                }

                ClientsDisconnected.Empty();
            }


            FPlatformProcess::Sleep(0.03f);
        }

        this->DisconnectAllClients();
        // for (const auto ClientPair : Clients)
        // {
        //     if (ClientPair.Value->Socket == nullptr)
        //     {
        //         UE_LOG(LogTemp, Error,
        //                TEXT("AHyperlaneTransmitterInfo::BeginPlay: Client %s has a null socket. Cannot disconnect."),
        //                *ClientPair.Value->Address)
        //         continue;
        //     }
        //
        //     ClientPair.Value->Socket->Close();
        // }
        // Clients.Empty();

        // Server Lifetime is over.

        AsyncTask(ENamedThreads::GameThread, [&]()
        {
            Clients.Empty();
            OnListenEnd.Execute();
        });

        return;
    });

    return;
}

void AHyperlaneTransmitterInfo::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (Socket != nullptr)
    {
        bShouldListen = false;
        UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::EndPlay: HyperlaneTransmitterInfo waiting for future."))
        ServerFinishedFuture.Wait();
        UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::EndPlay: HyperlaneTransmitterInfo future finished."))

        Socket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
        Socket = nullptr;
    }

    return;
}

inline void AHyperlaneTransmitterInfo::DisconnectClient(const FString& ClientAddress)
{
    UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::DisconnectClient: Disconnecting %s."), *ClientAddress)

    bool bDisconnectAll = ClientAddress == DisconnectAllClientsMessage;

    if (bDisconnectAll)
    {
        UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::DisconnectClient: Disconnecting all clients. Current connected clients: %d."), Clients.Num())
        for (auto ClientPair : Clients)
        {
            const TSharedPtr<FTCPTransmitterClient> Client = ClientPair.Value;
            if (Client->Socket != nullptr)
            {
                Client->Socket->Close();
            }
            OnClientDisconnected.Execute(ClientAddress);
        }
        Clients.Empty();

        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("AHyperlaneTransmitterInfo::DisconnectClient: Disconnecting %s."), *ClientAddress)
    if (const TSharedPtr<FTCPTransmitterClient> Client = Clients[ClientAddress]; Client.IsValid())
    {
        if (Client->Socket != nullptr)
        {
            Client->Socket->Close();
        }
        Clients.Remove(ClientAddress);
        OnClientDisconnected.Execute(ClientAddress);
        return;
    }

    UE_LOG(LogTemp, Error, TEXT("AHyperlaneTransmitterInfo::DisconnectClient: Client %s is invalid."), *ClientAddress)

    return;
}
