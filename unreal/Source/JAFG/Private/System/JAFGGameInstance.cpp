// Copyright 2024 mzoesch. All rights reserved.

#include "System/JAFGGameInstance.h"

#include "GameFramework/GameUserSettings.h"
#include "Net/Core/PushModel/PushModel.h"

UJAFGGameInstance::UJAFGGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->bShutdownRequested        = false;
    this->bAlreadyForwardedShutdown = false;
    return;
}

void UJAFGGameInstance::Init()
{
    Super::Init();

    if (IS_PUSH_MODEL_ENABLED() == false)
    {
        LOG_FATAL(LogSystem, "Push model is not enabled. This is a requirement for the game instance.")
        return;
    }

    GEngine->GetGameUserSettings()->SetFullscreenMode(EWindowMode::Windowed);
    GEngine->GetGameUserSettings()->SetScreenResolution(FIntPoint(1024, 768));

    return;
}

FDelegateHandle UJAFGGameInstance::SubscribeToShutdownRequest(const FOnShutdownRequestSignature::FDelegate& Delegate)
{
    return this->OnShutdownRequestDelegate.Add(Delegate);
}

FDelegateHandle UJAFGGameInstance::SubscribeToShutdownRequest(
    const FOnShutdownRequestSignature::FDelegate& Delegate,
    const bool bHoldShutdown,
    const FString& Holder /* = FGuid::NewGuid().ToString() */
)
{
    if (bHoldShutdown == false)
    {
        return this->SubscribeToShutdownRequest(Delegate);
    }

    this->ShutdownHolders.Add(Holder);

    return this->OnShutdownRequestDelegate.Add(Delegate);
}

bool UJAFGGameInstance::UnsubscribeFromShutdownRequest(const FDelegateHandle& Handle)
{
    return this->OnShutdownRequestDelegate.Remove(Handle);
}

bool UJAFGGameInstance::UnsubscribeFromShutdownRequest(const FDelegateHandle& Handle, const FString& Holder)
{
    if (this->ShutdownHolders.Contains(Holder) == false)
    {
        ensureAlwaysMsgf(false, TEXT("Holder not found."));
        checkNoEntry()
        return false;
    }

    this->ShutdownHolders.Remove(Holder);

    const bool ReturnCode = this->UnsubscribeFromShutdownRequest(Handle);

    if (this->bShutdownRequested && this->ShutdownHolders.IsEmpty())
    {
        AsyncTask(ENamedThreads::GameThread, [this] (void) { this->ForwardShutdownToEngine(); } );
    }
    else
    {
        if (this->bShutdownRequested)
        {
            LOG_DISPLAY(LogSystem, "Removed hold [%s] but others are still holding the shutdown. Waiting.", *Holder)
        }
    }

    return ReturnCode;
}

void UJAFGGameInstance::RequestControlledShutdown(void)
{
    LOG_DISPLAY(LogSystem, "Shutdown requested.")

    this->OnShutdownRequestDelegate.Broadcast();

    this->bShutdownRequested = true;

    if (this->ShutdownHolders.IsEmpty())
    {
        this->ForwardShutdownToEngine();
    }

    return;
}

void UJAFGGameInstance::ForwardShutdownToEngine(void)
{
    if (this->bAlreadyForwardedShutdown)
    {
        return;
    }

    this->bAlreadyForwardedShutdown = true;

    if (this->ShutdownHolders.IsEmpty() == false)
    {
        ensureAlwaysMsgf(false, TEXT("Shutdown request was forwarded but there are still shutdown holders."));
        checkNoEntry()
    }

    LOG_DISPLAY(LogSystem, "Forwarding shutdown request to engine.")

    this->GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");

    return;
}
