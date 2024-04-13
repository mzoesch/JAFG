// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"

JAFG_VOID

class ULocalPlayerChunkGeneratorSubsystem;

DECLARE_DELEGATE(FTCPHyperlaneWorkerEventSignature)
DECLARE_DELEGATE_OneParam(FTCPHyperlaneWorkerMessageSignature, const TArray<uint8>& /* Bytes */)

namespace Hyperlane
{

inline static constexpr double UnverifiedHyperlaneConnectionExhaustionTimeInSeconds { 5.0 };

}

class JAFG_API FHyperlaneWorker final : public FRunnable
{
    /**
     * Kinda cheeky but we need to access the UWorld somehow.
     */
    ULocalPlayerChunkGeneratorSubsystem* Owner = nullptr;

public:

    explicit FHyperlaneWorker(ULocalPlayerChunkGeneratorSubsystem* InOwner);
    virtual ~FHyperlaneWorker(void) override;

    // FRunnable interface
    virtual auto Init(void) -> bool   override;
    virtual auto Run(void)  -> uint32 override;
    virtual auto Stop(void) -> void   override;
    virtual auto Exit(void) -> void   override;
    // ~FRunnable interface

private:

    // FRunnable members
    FRunnableThread* Thread;
    bool bShutdownRequested = false;
    // ~FRunnable members

    // Network members
    inline static constexpr float ExhaustedConnectionTimeInSeconds { 5.0f };

    FTCPHyperlaneWorkerEventSignature OnConnectedDelegate;
    FTCPHyperlaneWorkerEventSignature OnDisconnectedDelegate;
    FTCPHyperlaneWorkerMessageSignature OnBytesReceivedDelegate;

    void OnConnectedDelegateHandler(void) const;
    void OnDisconnectedDelegateHandler(void);
    void OnBytesReceivedDelegateHandler(const TArray<uint8>& Bytes);

    FString Address = FString(TEXT("127.0.0.1"));
    int32 Port = 8080;
    /** Roughly 4 MB. */
    int32 BufferMaxSizeInBytes = 4 * 1024 * 1024;

    FThreadSafeBool bShouldReceiveData = true;
    FThreadSafeBool bShouldAttemptConnection = true;

    FSocket* Socket = nullptr;
    TSharedPtr<FInternetAddr> RemoteAddress = nullptr;

    TFuture<void> ConnectionEndFuture;
    void CreateConnectionEndFuture(void);

    /**
     * Once we are connected to the Hyperlane, we have to verify quickly that we are connected and tell the Hyperlane
     * Transmitter who we are.
     */
    bool SendValidation(void) const;
    bool Emit(const TArray<uint8>& Bytes) const;

    bool IsConnected(void) const;
    // ~Network members
};
