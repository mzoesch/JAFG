// Copyright 2024 mzoesch. All rights reserved.

#include "System/JAFGGameInstance.h"
#include "GamePluginSettings.h"
#include "JAFGExternalCore.h"
#include "ModificationSupervisorSubsystem.h"
#include "Commands/JAFGCoreChatCommands.h"
#include "GameFramework/GameUserSettings.h"
#include "Net/Core/PushModel/PushModel.h"
#include "System/PreInternalInitializationSubsystem.h"

UJAFGGameInstance::UJAFGGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->bShutdownRequested        = false;
    this->bAlreadyForwardedShutdown = false;
    return;
}

void UJAFGGameInstance::Init(void)
{
#if DO_CHECK_ASSERTIONS
    LOG_FATAL(LogSystem, "Assertions checks are enabled. This is disallowed when not unit testing.")
#endif /* DO_CHECK_ASSERTIONS */

#if UE_BUILD_SHIPPING
    check( false && "Checks are enabled in shipping builds. Was this intensional?" )
#endif /* UE_BUILD_SHIPPING */

    if (IS_PUSH_MODEL_ENABLED() == false)
    {
        LOG_FATAL(LogSystem, "Push model is not enabled. This is a requirement.")
        return;
    }

    GetMutableDefault<UGamePluginSettings>()->OnGameBootUp();

    FJAFGExternalCoreModule::Get().OnInitInternalSubsystem.BindLambda( [] (FSubsystemCollectionBase& Collection)
    {
        LOG_VERBOSE(LogModSubsystem, "Received broadcast to initialize pre internal subsystem.")
        Collection.InitializeDependency<UPreInternalInitializationSubsystem>();

#if !UE_BUILD_SHIPPING
        FJAFGExternalCoreModule::Get().bInternalSubsystemInitialized = true;
#endif /* !UE_BUILD_SHIPPING */

        FJAFGExternalCoreModule::Get().OnInitInternalSubsystem.Unbind();

        return;
    });

    Super::Init();

    LOG_DISPLAY(LogSystem, "Found %d mods to load.", UModificationSupervisorSubsystem::ModSubsystems.Num())

    GEngine->GetGameUserSettings()->SetFullscreenMode(EWindowMode::Windowed);
    GEngine->GetGameUserSettings()->SetScreenResolution(FIntPoint(1440, 900));
    GEngine->GetGameUserSettings()->ApplySettings(false);

    return;
}

void UJAFGGameInstance::OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld)
{
    Super::OnWorldChanged(OldWorld, NewWorld);

    if (NewWorld)
    {
        AsyncTask(ENamedThreads::GameThread, [this] (void)
        {
            if (this->GetWorld())
            {
                if (UShippedWorldChatCommandRegistry* Registry = this->GetWorld()->GetSubsystem<UShippedWorldChatCommandRegistry>(); Registry)
                {
                    ChatCommands::RegisterJAFGCoreChatCommands(Registry);
                }
            }
        });
    }

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
    LOG_DISPLAY(LogSystem, "Shutdown requested.%s", this->bShutdownIsRestart ? TEXT(" Will restart.") : TEXT(""))

    this->OnShutdownRequestDelegate.Broadcast();

    this->bShutdownRequested = true;

    if (this->ShutdownHolders.IsEmpty())
    {
        this->ForwardShutdownToEngine();
    }

    return;
}

void UJAFGGameInstance::RequestControlledRestart(void)
{
    this->bShutdownIsRestart = true;
    this->RequestControlledShutdown();

    return;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
bool UJAFGGameInstance::DoesPlatformSupportRestart(void) const
{
#if PLATFORM_WINDOWS
    return false;
#else /* PLATFORM_WINDOWS */
    #error "DoesPlatformSupportRestart is not implemented for this platform."
    return false;
#endif /* !PLATFORM_WINDOWS */
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


    if (this->DoesPlatformSupportRestart() && this->bShutdownIsRestart)
    {
        FPlatformMisc::RestartApplication();
        return;
    }

    LOG_DISPLAY(LogSystem, "Forwarding shutdown request to engine.")

    this->GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");

    return;
}
