// Copyright 2024 mzoesch. All rights reserved.

#include "LocalSessionSupervisorSubsystem.h"

#include "Definitions.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystemUtils.h"
#include "WorldCore/RegisteredWorldNames.h"

ULocalSessionSupervisorSubsystem::ULocalSessionSupervisorSubsystem(void) : Super()
{
    return;
}

void ULocalSessionSupervisorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    LOG_VERBOSE(LogLSSSS, "Called.")

    const IOnlineSubsystem* OSSModule = IOnlineSubsystem::Get();

    if (OSSModule == nullptr)
    {
        LOG_FATAL(LogLSSSS, "Failed to get Online Subsystem module.")
        return;
    }

    this->OnlineSessionInterface = OSSModule->GetSessionInterface();

    if (this->OnlineSessionInterface == nullptr || this->OnlineSessionInterface.IsValid() == false)
    {
        LOG_FATAL(LogLSSSS, "Failed to get Online Session Interface.")
        return;
    }

    this->OnlineSessionInterface->OnCreateSessionCompleteDelegates.AddLambda(
        [this] (const FName SessionName, const bool bSuccess)
        {
            this->OnCreateSessionCompleteDelegate(SessionName, bSuccess);
        }
    );
    this->OnlineSessionInterface->OnFindSessionsCompleteDelegates.AddLambda(
        [this] (const bool bSuccess)
        {
            this->OnFindSessionsCompleteDelegate(bSuccess);
        }
    );
    this->OnlineSessionInterface->OnJoinSessionCompleteDelegates.AddLambda(
        [this] (const FName SessionName, const EOnJoinSessionCompleteResult::Type Result)
        {
            this->OnJoinSessionCompleteDelegate(SessionName, Result);
        }
    );

    return;
}

void ULocalSessionSupervisorSubsystem::Deinitialize(void)
{
    Super::Deinitialize();

    LOG_VERBOSE(LogLSSSS, "Called.")

    if (this->OnlineSessionInterface.IsValid() == false)
    {
        LOG_WARNING(LogLSSSS, "Online Session Interface is not valid. Could not correctly deinitialize.")
        return;
    }

    if (this->ActiveSessionSettings.bIsActive)
    {
        this->ForceActiveSessionDestroy();
    }

    this->OnlineSessionInterface->OnCreateSessionCompleteDelegates.Clear();
    this->OnlineSessionInterface->OnFindSessionsCompleteDelegates.Clear();
    this->OnlineSessionInterface->OnJoinSessionCompleteDelegates.Clear();

    return;
}

void ULocalSessionSupervisorSubsystem::HostListenServer(const FString& InSessionName, const int32 InMaxPublicConnections, const bool bInLAN)
{
    LOG_VERBOSE(LogLSSSS, "Called.")

    FString SanitizedSessionName = InSessionName; SanitizedSessionName.ReplaceInline(TEXT(" "), TEXT("-"));
    if (SanitizedSessionName.IsEmpty())
    {
        LOG_FATAL(LogLSSSS, "Session name is empty.")
        return;
    }
    if (SanitizedSessionName.Len() > ULocalSessionSupervisorSubsystem::MaxSessionNameLength)
    {
        LOG_FATAL(LogLSSSS, "Session name is too long. Found length: %d, expected max length: %d.", SanitizedSessionName.Len(), ULocalSessionSupervisorSubsystem::MaxSessionNameLength)
        return;
    }

    FOnlineSessionSettings OnlineSessionSettings = FOnlineSessionSettings();
    OnlineSessionSettings.NumPublicConnections = InMaxPublicConnections;
    OnlineSessionSettings.NumPrivateConnections = 0;
    OnlineSessionSettings.bShouldAdvertise = true;
    OnlineSessionSettings.bAllowJoinInProgress = true;
    OnlineSessionSettings.bIsLANMatch = bInLAN;
    OnlineSessionSettings.bIsDedicated = false;
    OnlineSessionSettings.bUsesStats = false;
    OnlineSessionSettings.bAllowInvites = true;
    OnlineSessionSettings.bUsesPresence = true;
    OnlineSessionSettings.bAllowJoinViaPresence = true;
    OnlineSessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
    OnlineSessionSettings.bAntiCheatProtected = false;
    OnlineSessionSettings.bUseLobbiesIfAvailable = false;
    OnlineSessionSettings.bUseLobbiesVoiceChatIfAvailable = false;

    this->ActiveSessionSettings = FMyOnlineSessionSettings{SanitizedSessionName, OnlineSessionSettings, true};

    if (this->OnlineSessionInterface->CreateSession(
            /* No need to be fancy here, as we must always be in the front end menu when calling this method. */ 0,
            *SanitizedSessionName,
            OnlineSessionSettings
        ) == false
    )
    {
        LOG_FATAL(LogLSSSS, "Failed to create session due to unknown error.")
    }

    return;
}

void ULocalSessionSupervisorSubsystem::OnCreateSessionCompleteDelegate(const FName SessionName, const bool bSuccess)
{
    LOG_DISPLAY(LogLSSSS, "Session [%s] creation %s.", *SessionName.ToString(), bSuccess ? TEXT("succeeded") : TEXT("failed"))

    if (bSuccess == false)
    {
        LOG_FATAL(LogLSSSS, "Session [%s] creation failed, but no further action is implemented.", *SessionName.ToString())
        return;
    }

#if WITH_EDITOR
    /*
     * Safety net if multiple PIE instances are active. As the delegate will be called for all running application
     * instances.
     * Maybe we can find a better way to handle this? Like only adding the delegate handler in the correct instance and
     * not at the start of this subsystem's lifetime.
     */
    if (this->ActiveSessionSettings.bIsActive == false)
    {
        LOG_WARNING(LogLSSSS, "ctive Session Settings is nullptr. Cannot proceed. If this is not in the calling PIE instance, this is can be ignored.")
        return;
    }
#endif /* WITH_EDITOR */

    /* Shamelessly copied from GameplayStatics.h. There may be safer ways to implement this? */

    const     FString     LevelName  = FString::Printf(TEXT("%s?listen"), *RegisteredWorlds::World);
    constexpr ETravelType TravelType = ETravelType::TRAVEL_Absolute;

    FWorldContext* WorldContextObject = GEngine->GetWorldContextFromWorld(this->GetWorld());
    if (WorldContextObject == nullptr)
    {
        LOG_FATAL(LogLSSSS, "World Context Object is invalid.")
        return;
    }

    FURL MyURL = FURL(&WorldContextObject->LastURL, *LevelName, TravelType);

    if (MyURL.IsLocalInternal() == false)
    {
        LOG_FATAL(LogLSSSS, "URL is not local internal. Must be as we are the hosting listen server. Falty URL: %s.", *MyURL.ToString())
        return;
    }

    if (GEngine->MakeSureMapNameIsValid(MyURL.Map) == false)
    {
        LOG_FATAL(LogLSSSS, "Map name is invalid. Falty URL: %s.", *MyURL.ToString())
        return;
    }

    LOG_VERBOSE(LogLSSSS, "Server traveling to %s.", *MyURL.ToString())

    this->GetWorld()->ServerTravel(MyURL.ToString());

    return;
}

void ULocalSessionSupervisorSubsystem::OnFindSessionsCompleteDelegate(const bool bSuccess)
{
    return;
}

void ULocalSessionSupervisorSubsystem::OnJoinSessionCompleteDelegate(const FName SessionName, const EOnJoinSessionCompleteResult::Type Result)
{
    return;
}

bool ULocalSessionSupervisorSubsystem::ForceActiveSessionDestroy(void)
{
    if (this->OnlineSessionInterface->DestroySession(FName(this->ActiveSessionSettings.Name)))
    {
        LOG_DISPLAY(LogLSSSS, "Active Session [%s] destroyed.", *this->ActiveSessionSettings.Name)
        this->ActiveSessionSettings = FMyOnlineSessionSettings();
        return true;
    }

    LOG_ERROR(LogLSSSS, "Failed to destroy session [%s].", *this->ActiveSessionSettings.Name)
    return false;
}
