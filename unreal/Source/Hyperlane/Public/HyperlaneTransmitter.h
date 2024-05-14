// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HyperlaneComponent.h"
#include "TransmittableData.h"
#include "HyperlaneTypes.h"

class FHyperlaneWorker;
class UHyperlaneTransmitterSubsystem;

DECLARE_DELEGATE(FTCPTransmitterEventSignature)
DECLARE_DELEGATE_OneParam(FTCPTransmitterClientEventSignature, const FClientAddress& /* Address */)
DECLARE_DELEGATE_TwoParams(FTCPTransmitterSocketEventSignature, const FClientAddress& /* Address */, const uint16& /* Port */)

/** Represents a client connection to the transmitter. */
struct FTCPTransmitterClient
{
    /** Where the transmitter recognized this connection request. */
    FDateTime      Timestamp;
    FSocket*       Socket  = nullptr;
    FClientAddress Address = L"";

    FORCEINLINE bool operator==(const FTCPTransmitterClient& Other) const
    {
        return this->Address == Other.Address && this->Socket == Other.Socket;
    }
    FORCEINLINE bool operator==(const FClientAddress& Other) const
    {
        return this->Address == Other;
    }
};

class HYPERLANE_API FHyperlaneTransmitter final
{
    friend FHyperlaneWorker;

public:

    /** Default constructor. Required by the engine. Do not use. */
             FHyperlaneTransmitter(void);
    explicit FHyperlaneTransmitter(UHyperlaneTransmitterSubsystem* InOwningSubsystem);
             ~FHyperlaneTransmitter(void);

    void SendChunkInitializationData(const UHyperlaneComponent* Target, TransmittableData::FChunkInitializationData& Data);

private:

    inline static constexpr uint8 UTF8Terminator { 0x00 };
    inline static constexpr float DisconnectUnvalidatedClientsTimeoutInSeconds { 5.0f };

    TObjectPtr<UHyperlaneTransmitterSubsystem> OwningSubsystem = nullptr;

    /** True, if the game is completely quitting. */
    FThreadSafeBool bStoppedThroughDestructor = false;

    /** If false, the main transmitter loop will stop. And safely disconnect all clients. */
    FThreadSafeBool bShouldListen = true;

    FThreadSafeBool bShouldPingCheck    = true;
    float PingCheckIntervalInSeconds    = 5.0f;
    inline static const FString PingMsg = TEXT("PING");
    TArray<uint8> PingMsgBytes          =
        TArray<uint8>(
            reinterpret_cast<uint8*>(TCHAR_TO_UTF8(*FHyperlaneTransmitter::PingMsg)),
            FHyperlaneTransmitter::PingMsg.Len()
        );

    auto DisconnectAllClients(void) -> void;
    auto DisconnectSingleClient(const FClientAddress& TargetAddr, const bool bRemoveFromMap = true) -> void;

    /**
     * All current unvalidated clients.
     * They will be disconnected if they don't send the validation message and moved to the
     * FHyperlaneTransmitter#Clients map if they do.
     */
    TArray<TSharedPtr<FTCPTransmitterClient>> UnvalidatedClients;
    bool IsHyperlaneWorkerValid(const FHyperlaneIdentifier& HyperlaneIdentifier, UHyperlaneComponent*& OutHyperlaneWorker) const;
    FORCEINLINE bool IsClientUnvalidated(const FClientAddress& InAddress) const
    {
        return this->UnvalidatedClients.ContainsByPredicate( [&InAddress] (const TSharedPtr<FTCPTransmitterClient>& InClient)
        {
            return InClient->Address == InAddress;
        });
    }
    FORCEINLINE void RemoveUnvalidatedClient(const FClientAddress& InAddress)
    {
        if (this->IsClientUnvalidated(InAddress) == false)
        {
            LOG_ERROR(LogHyperlane, "Client [%s] was not unvalidated. Can't remove from unvalidated clients.", *InAddress)
            return;
        }

        int Index = -1;
        for (const TSharedPtr<FTCPTransmitterClient>& UnvalidatedClient : this->UnvalidatedClients)
        {
            ++Index;
            if (UnvalidatedClient->Address == InAddress)
            {
                check( Index != -1 )
                this->UnvalidatedClients.RemoveAt(Index);
                return;
            }
        }
        checkNoEntry()
        return;
    }
    /** All current connected and validated clients. */
    TMap<FClientAddress, TSharedPtr<FTCPTransmitterClient>> Clients;

    /** The port, where this transmitter listens for incoming client connections. */
    uint16  OutPort    = 8080;
    FString SocketName = TEXT("HyperlaneTransmitter");
    inline static constexpr int32 DesiredMaxBufferSizeInBytes { /* Roughly 4 MB. */ 4 * 1024 * 1024 };
    /** If OS is forbidden to set a buffer, this buffer will have another size than FHyperlaneTransmitter#DesiredMaxBufferSizeInBytes. */
    int32 RecvPlatformMaxBufferSizeInBytes = 0x0;
    /** If OS is forbidden to set a buffer, this buffer will have another size than FHyperlaneTransmitter#DesiredMaxBufferSizeInBytes. */
    int32 SendPlatformMaxBufferSizeInBytes = 0x0;
    inline static constexpr int32 MaxBacklog           { 0x8 };

    FSocket* Socket = nullptr;

    TFuture<void> ServerEndFuture = { };
    auto CreateServerEndFuture(void) -> void;

    /**
     * Will emit the given bytes to the given target address. If the emition failed, the client will be disconnected.
     */
    auto Emit(const TArray<uint8>& InBytes, const FClientAddress& TargetAddr) -> void;

    FTCPTransmitterSocketEventSignature OnListenBeginDelegate;
    FTCPTransmitterSocketEventSignature OnListenBeginFailureDelegate;
    FTCPTransmitterEventSignature       OnListenEndDelegate;
    FTCPTransmitterClientEventSignature OnClientConnectedDelegate;
    FTCPTransmitterClientEventSignature OnClientDisconnectedDelegate;
    auto OnListenBeginDelegateHandler(const FClientAddress& InAddress, const uint16& InPort) -> void;
    auto OnListenBeginFailureDelegateHandler(const FClientAddress& InAddress, const uint16& InPort) -> void;
    auto OnListenEndDelegateHandler(void) -> void;
    auto OnClientConnectedDelegateHandler(const FClientAddress& InAddress) -> void;
    auto OnClientDisconnectedDelegateHandler(const FClientAddress& InAddress) -> void;
};
