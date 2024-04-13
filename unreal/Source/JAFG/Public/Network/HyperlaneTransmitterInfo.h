// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "GameFramework/Info.h"
#include "World/WorldPlayerController.h"

#include "HyperlaneTransmitterInfo.generated.h"

JAFG_VOID

class FHyperlaneWorker;

DECLARE_DELEGATE(FTCPTransmitterEventSignature)
DECLARE_DELEGATE_OneParam(FTCPTransmitterClientEventSignature, const FString& /* Address */)
DECLARE_DELEGATE_TwoParams(FTCPTransmitterSocketEventSignature, const FString& /* Address */, const uint16& /* Port */)

/**
 * A client connection to the transmitter.
 */
struct FTCPTransmitterClient
{
    FDateTime Timestamp;
    FSocket* Socket  = nullptr;
    FString  Address = L"";

    bool operator==(const FTCPTransmitterClient& Other) const
    {
        return this->Address == Other.Address && this->Socket == Other.Socket;
    }
};

namespace TransmittableData
{

struct FChunkInitializationData
{
    FIntVector ChunkKey;
    TArray<int32> Voxels;

    void SerializeToBytes(TArray<uint8>& OutBytes);

    static TransmittableData::FChunkInitializationData DeserializeFromBytes(const TArray<uint8>& InBytes);
};

}

// TODO Make subsystem
UCLASS(NotBlueprintable)
class JAFG_API AHyperlaneTransmitterInfo : public AInfo
{
    GENERATED_BODY()

    friend FHyperlaneWorker;

public:

    explicit AHyperlaneTransmitterInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

    void SendChunkInitializationData(const AWorldPlayerController* Target, TransmittableData::FChunkInitializationData& Data);

private:

    /**
     * If false the main transmitter loop will stop. And safely disconnect all clients.
     */
    FThreadSafeBool bShouldListen = false;

    inline static constexpr uint8 UTF8Terminator = 0x00;
    /** In seconds. */
    inline static constexpr float DisconnectUnvalidatedClientsTimeout { 5.0f };
    /**
     * All current unvalidated clients.
     * Will be disconnected if they don't send the validation message and moved to the
     * AHyperlaneTransmitterInfo#Clients map if they do.
     */
    TMap<FString, TSharedPtr<FTCPTransmitterClient>> UnvalidatedClients;
    /**
     * All current connected clients.
     */
    TMap<FString, TSharedPtr<FTCPTransmitterClient>> Clients;
    bool IsHyperlaneWorkerValid(const FString& HyperlaneIdentifier, AWorldPlayerController*& OutPlayerController) const;

    FTCPTransmitterSocketEventSignature OnListenBeginDelegate;
    FTCPTransmitterSocketEventSignature OnListenBeginFailureDelegate;
    FTCPTransmitterEventSignature OnListenEndDelegate;
    FTCPTransmitterClientEventSignature OnClientConnectedDelegate;
    FTCPTransmitterClientEventSignature OnClientDisconnectedDelegate;

    void OnListenBeginDelegateHandler(const FString& InAddress, const uint16& InPort);
    void OnListenBeginFailureDelegateHandler(const FString& InAddress, const uint16& InPort);
    void OnListenEndDelegateHandler(void);
    void OnClientConnectedDelegateHandler(const FString& InAddress);
    void OnClientDisconnectedDelegateHandler(const FString& Address);

    FThreadSafeBool bShouldPingCheck = false;
    float PingCheckInterval = 0.0f;
    inline static const FString PingMessage = L"<PING>";
    /* Initialized with the ping message. But as bytes to be easily emitted by the transmitter. */
    TArray<uint8> PingData;

    const FString DisconnectAllClientsMessage = TEXT("<DISCONNECT-ALL>");
    FORCEINLINE auto DisconnectAllClients(void) -> void { this->DisconnectSingleClient(DisconnectAllClientsMessage); }
    /**
     * @param ClientAddress Give AHyperlaneTransmitterInfo#DisconnectAllClientsMessage
     *                      to disconnect all clients at once.
     */
    void DisconnectSingleClient(const FString& ClientAddress);

    uint16 Port = 0x0;
    FString SocketName = L"";

    int32 BufferMaxSizeInBytes = 0x0;

    FSocket* Socket = nullptr;

    int32 MaxBacklog = 0x0;

    TFuture<void> ServerEndFuture;
    void CreateServerEndFuture(void);

    void Emit(const TArray<uint8>& InBytes, const FString& InClientAddress);
};
