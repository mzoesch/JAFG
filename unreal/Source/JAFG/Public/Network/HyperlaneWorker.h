// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class UHyperlaneComponent;

DECLARE_DELEGATE(FTCPHyperlaneWorkerEventSignature)
DECLARE_DELEGATE_OneParam(FTCPHyperlaneWorkerMessageSignature, const TArray<uint8>& /* Bytes */)

class JAFG_API FHyperlaneWorker final : public FRunnable
{
    /**
     * Kinda cheeky but we need to access the UWorld somehow.
     */
    UHyperlaneComponent* Owner = nullptr;

public:

    explicit FHyperlaneWorker(UHyperlaneComponent* InOwner);
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

    void OnConnectedDelegateHandler(void);
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

    bool IsConnected(void) const;
    // ~Network members
};
