// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Engine/GameInstance.h"

#include "JAFGGameInstance.generated.h"

JAFG_VOID

DECLARE_MULTICAST_DELEGATE(FOnShutdownRequestSignature);

UCLASS(NotBlueprintable)
class JAFG_API UJAFGGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:

    explicit UJAFGGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    auto SubscribeToShutdownRequest(const FOnShutdownRequestSignature::FDelegate& Delegate) -> FDelegateHandle;
    auto SubscribeToShutdownRequest(
        const FOnShutdownRequestSignature::FDelegate& Delegate,
        const bool bHoldShutdown,
        const FString& Holder = FGuid::NewGuid().ToString()
    ) -> FDelegateHandle;
    /**
     * It is not necessary to call this function after a specific subscriber has been notified that the engine
     * will be shut down in the near future. Only needed if the time to live of the subscriber is shorter than
     * this instance of the game.
     */
    auto UnsubscribeFromShutdownRequest(const FDelegateHandle& Handle) -> bool;
    /**
     * Must be called by the subscribers that are eligible to hold the shutdown request.
     * After they have been notified that the engine will be shut down in the near future, they can hold the shutdown
     * for as long as possible. After their work is done, and they are ready to let the engine shut down, they must
     * call this method to remove their hold.
     */
    auto UnsubscribeFromShutdownRequest(const FDelegateHandle& Handle, const FString& Holder) -> bool;

    auto RequestControlledShutdown(void) -> void;

private:

    bool bShutdownRequested;
    /**
     * Needed for a rare case where all holders can instantly release the shutdown request in the same tick.
     */
    bool bAlreadyForwardedShutdown;

    /**
     * Event called if a shutdown request is made. But before the request is being forwarded to the engine.
     * In short meaning before all destructors are being called.
     */
    FOnShutdownRequestSignature OnShutdownRequestDelegate;

    TArray<FString> ShutdownHolders;

    auto ForwardShutdownToEngine(void) -> void;
};
