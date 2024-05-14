// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class UHyperlaneComponent;

DECLARE_DELEGATE(FTCPHyperlaneWorkerEventSignature)
DECLARE_DELEGATE_OneParam(FTCPHyperlaneWorkerMessageSignature, const TArray<uint8>& /* Bytes */)

class HYPERLANE_API FHyperlaneWorker final : public FRunnable
{
public:

    /** Default constructor. Required by the engine. Do not use. */
             FHyperlaneWorker();
    explicit FHyperlaneWorker(UHyperlaneComponent* InOwningComponent);
    virtual  ~FHyperlaneWorker(void) override;

    // FRunnable interface
    virtual auto Init(void) -> bool   override;
    virtual auto Run(void)  -> uint32 override;
    virtual auto Stop(void) -> void   override;
    virtual auto Exit(void) -> void   override;
    // ~FRunnable interface

private:

    // FRunnable members
    FRunnableThread* Thread = nullptr;
    bool bShutdownRequested = false;
    // ~FRunnable members

    TObjectPtr<UHyperlaneComponent> OwningComponent = nullptr;

    inline static constexpr float ExhaustedConnectionTimeInSeconds { 5.0f };

    FString Addr = FString(TEXT("127.0.0.1"));
    int32   Port = 8080;
    inline static constexpr int32 DesiredMaxBufferSizeInBytes { /* Roughly 4 MB. */ 4 * 1024 * 1024 };
    /** If OS is forbidden to set a buffer, this buffer will have another size than FHyperlaneTransmitter#DesiredMaxBufferSizeInBytes. */
    int32 RecvPlatformMaxBufferSizeInBytes = 0x0;
    /** If OS is forbidden to set a buffer, this buffer will have another size than FHyperlaneTransmitter#DesiredMaxBufferSizeInBytes. */
    int32 SendPlatformMaxBufferSizeInBytes = 0x0;

    FThreadSafeBool bShouldReceive           = true;
    FThreadSafeBool bShouldAttemptConnection = true;

    FSocket*                  Socket     = nullptr;
    TSharedPtr<FInternetAddr> RemoteAddr = nullptr;

    TFuture<void> ConnectionEndFuture;
    auto CreateConnectionEndFuture(void) -> void;

    /**
     * Once we are connected to the Hyperlane, we have to verify quickly that we are connected and tell the Hyperlane
     * Transmitter who we are.
     */
    auto SendValidation(void) const -> bool;
    auto Emit(const TArray<uint8>& InBytes) const -> bool;

    auto IsConnected(void) const -> bool;

    FTCPHyperlaneWorkerEventSignature   OnConnectedDelegate;
    FTCPHyperlaneWorkerEventSignature   OnDisconnectedDelegate;
    FTCPHyperlaneWorkerMessageSignature OnBytesReceivedDelegate;
    auto OnConnectedDelegateHandler(void) const -> void;
    auto OnDisconnectedDelegateHandler(void) -> void;
    auto OnBytesReceivedDelegateHandler(const TArray<uint8>& Bytes) const -> void;
};
