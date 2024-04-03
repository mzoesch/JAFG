// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class UHyperlaneComponent;

DECLARE_DELEGATE(FTCPEventSignature)
DECLARE_MULTICAST_DELEGATE_OneParam(FTCPMessageSignature, const TArray<uint8>& /* bytes */)

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

    FRunnableThread* Thread;
    bool bShutdownRequested = false;



    FString Address = L"";
    int32 Port = 0;
    int32 BufferMaxSizeInBytes = 0;

    FThreadSafeBool bShouldReceiveData;
    FThreadSafeBool bShouldAttemptConnection;

    FSocket* Socket = nullptr;
    TSharedPtr<FInternetAddr> RemoteAddress = nullptr;

    TFuture<void> ConnectionFinishedFuture;

    FTCPEventSignature OnConnected;
    FTCPEventSignature OnDisconnected;

    bool IsConnected(void) const;

    static TFuture<void> RunLambdaOnBackGroundThread(TFunction< void()> InFunction)
    {
        return Async(EAsyncExecution::Thread, InFunction);
    }

    static TFuture<void> RunLambdaOnGameThread(TFunction< void()> InFunction)
    {
        return Async(EAsyncExecution::TaskGraphMainThread, InFunction);
    }
};
