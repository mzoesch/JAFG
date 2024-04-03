// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"

#include "HyperlaneTransmitterInfo.generated.h"

DECLARE_DELEGATE(FTCPTransmitterEventSignature)
DECLARE_DELEGATE_OneParam(FTCPTransmitterClientEventSignature, const FString& /* address */)

/**
 * A client connection to the transmitter.
 */
struct FTCPTransmitterClient
{
    FSocket* Socket = nullptr;
    FString Address = L"";

    bool operator==(const FTCPTransmitterClient& Other) const
    {
        return this->Address == Other.Address && this->Socket == Other.Socket;
    }
};

UCLASS(NotBlueprintable)
class JAFG_API AHyperlaneTransmitterInfo : public AInfo
{
    GENERATED_BODY()

public:

    explicit AHyperlaneTransmitterInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

    TMap<FString, TSharedPtr<FTCPTransmitterClient>> Clients;

    FThreadSafeBool bShouldListen;

    FTCPTransmitterEventSignature OnListenBegin;
    FTCPTransmitterEventSignature OnListenEnd;
    FTCPTransmitterClientEventSignature OnClientConnected;
    FTCPTransmitterClientEventSignature OnClientDisconnected;

    bool bShouldPingCheck = true;
    float PingCheckInterval = 3.0f;

    FString DisconnectAllClientsMessage = L"<DISCONNECT-ALL>";
    void DisconnectAllClients(void) { this->DisconnectClient(DisconnectAllClientsMessage); }
    void DisconnectClient(const FString& ClientAddress);

    FString PingMessage = L"<PING>";
    TArray<uint8> PingData;

    uint16 Port = 0;
    FString SocketName = L"";

    int32 BufferMaxSizeInBytes = 0;

    /**
     * The listen socket.
     */
    FSocket* Socket = nullptr;

    TFuture<void> ServerFinishedFuture;

    static TFuture<void> RunLambdaOnBackGroundThread(TFunction< void()> InFunction)
    {
        return Async(EAsyncExecution::Thread, InFunction);
    }

    static TFuture<void> RunLambdaOnGameThread(TFunction< void()> InFunction)
    {
        return Async(EAsyncExecution::TaskGraphMainThread, InFunction);
    }
};
