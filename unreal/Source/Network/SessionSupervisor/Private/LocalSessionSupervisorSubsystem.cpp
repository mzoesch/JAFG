// Copyright 2024 mzoesch. All rights reserved.

#include "LocalSessionSupervisorSubsystem.h"

#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"

void ULocalSessionSupervisorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    LOG_VERBOSE(LogMyNetwork, "Called.")

    const IOnlineSubsystem* OSSModule = IOnlineSubsystem::Get();

    if (OSSModule == nullptr)
    {
        LOG_FATAL(LogMyNetwork, "Failed to get online subsystem.")
        return;
    }

    this->OnlineSessionInterface = OSSModule->GetSessionInterface();

    if (this->OnlineSessionInterface == nullptr || this->OnlineSessionInterface.IsValid() == false)
    {
        LOG_FATAL(LogMyNetwork, "Failed to get session interface.")
        return;
    }

    this->OnlineSessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &ULocalSessionSupervisorSubsystem::OnCreateSessionCompleteDelegate);

    this->ActiveSessionSettings = nullptr;

    return;
}

void ULocalSessionSupervisorSubsystem::Deinitialize(void)
{
    Super::Deinitialize();

    LOG_VERBOSE(LogMyNetwork, "Called.")
}

bool ULocalSessionSupervisorSubsystem::HostListenServer(const FString& InSessionName, const int InMaxPublicConnections, const bool bInLAN)
{
    if (UNetworkStatics::IsSafeStandalone(this) == false)
    {
        LOG_ERROR(LogMyNetwork, "Dissalowed on standalone.")
        return false;
    }

    FString SanitizedSessionName = InSessionName; SanitizedSessionName.ReplaceInline(TEXT(" "), TEXT("-"));
    if (SanitizedSessionName.IsEmpty())
    {
        LOG_ERROR(LogMyNetwork, "Session name is empty.")
        return false;
    }
    if (SanitizedSessionName.Len() > ULocalSessionSupervisorSubsystem::MaxSessionNameLength)
    {
        LOG_ERROR(LogMyNetwork, "Session name is too long.")
        return false;
    }

    FOnlineSessionSettings OnlineSessionSettings           = FOnlineSessionSettings();
    OnlineSessionSettings.NumPublicConnections             = InMaxPublicConnections;
    OnlineSessionSettings.NumPrivateConnections            = 0;
    OnlineSessionSettings.bShouldAdvertise                 = true;
    OnlineSessionSettings.bAllowJoinInProgress             = true;
    OnlineSessionSettings.bIsLANMatch                      = bInLAN;
    OnlineSessionSettings.bIsDedicated                     = false;
    OnlineSessionSettings.bUsesStats                       = false;
    OnlineSessionSettings.bAllowInvites                    = true;
    OnlineSessionSettings.bUsesPresence                    = true;
    OnlineSessionSettings.bAllowJoinViaPresence            = true;
    OnlineSessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
    OnlineSessionSettings.bAntiCheatProtected              = false;
    OnlineSessionSettings.bUseLobbiesIfAvailable           = false;
    OnlineSessionSettings.bUseLobbiesVoiceChatIfAvailable  = false;

    this->ActiveSessionSettings = new FMyOnlineSessionSettings(SanitizedSessionName, OnlineSessionSettings);

    if (
        this->OnlineSessionInterface->CreateSession(
            0, /* No need to be fancy here, as we must always be in the Menu when calling this method. */
            *SanitizedSessionName,
            OnlineSessionSettings
        )
    )
    {
        LOG_VERBOSE(LogMyNetwork, "Session creation request sent.")
    }
    else
    {
        LOG_ERROR(LogMyNetwork, "Failed to send session creation request.")
        return false;
    }

    return true;
}

void ULocalSessionSupervisorSubsystem::OnCreateSessionCompleteDelegate(const FName SessionName, const bool bSuccess)
{
    LOG_DISPLAY(LogMyNetwork, "Session creation complete: %s.", bSuccess ? TEXT("Success") : TEXT("Failure"))

    if (bSuccess == false)
    {
        /* Some feedback to the front end is needed here. */
        return;
    }

#if WITH_EDITOR
    /*
     * Safety net if multiple PIE instances are active. As the delegate will be called for all running application
     * instances.
     * Maybe we can find a better way to handle this? Like only adding the delegate handler in the correct instance and
     * not at the start of this subsystem's lifetime.
     */
    if (this->ActiveSessionSettings == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("ULocalSessionSupervisor::OnCreateSessionCompleteDelegate: Active Session Settings is nullptr. Cannot proceed. If this is not in the calling PIE instance, this is can be ignored."))
        return;
    }
#endif /* WITH_EDITOR */

    /* Shamelessly copied from GameplayStatics.h. There may be safer ways to implement this? */

    const FString LevelName          = "L_World?listen";
    constexpr ETravelType TravelType = ETravelType::TRAVEL_Absolute;

    FWorldContext* WorldContextObject = GEngine->GetWorldContextFromWorld(this->GetWorld());

    if (WorldContextObject == nullptr)
    {
        LOG_FATAL(LogMyNetwork, "World Context Object is nullptr.")
        return;
    }

    FURL MyURL = FURL(&WorldContextObject->LastURL, *LevelName, TravelType);

    if (MyURL.IsLocalInternal() == false)
    {
        LOG_FATAL(LogMyNetwork, "URL is not local internal. Must be as we are the hosting listen server.")
        return;
    }

    if (GEngine->MakeSureMapNameIsValid(MyURL.Map) == false)
    {
        LOG_FATAL(LogMyNetwork, "Level name is invalid.")
        return;
    }

    LOG_VERBOSE(LogMyNetwork, "Server traveling to %s.", *MyURL.ToString())

    this->GetWorld()->ServerTravel(MyURL.ToString());

    return;
}
