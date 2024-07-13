// Copyright 2024 mzoesch. All rights reserved.

#include "LocalSessionSupervisorSubsystem.h"

#include "JAFGLogDefs.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystemUtils.h"
#include "Kismet/GameplayStatics.h"
#include "RegisteredWorldNames.h"

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

    this->CurrentState = ECurrentLSSSSState::None;

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

    if (this->bActiveSessionExists)
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
    OnlineSessionSettings.NumPublicConnections = 20;
    OnlineSessionSettings.NumPrivateConnections = 20;
    OnlineSessionSettings.bShouldAdvertise = true;
    OnlineSessionSettings.bAllowJoinInProgress = true;
    OnlineSessionSettings.bIsLANMatch = false;
    OnlineSessionSettings.bIsDedicated = false;
    OnlineSessionSettings.bUsesStats = false;
    OnlineSessionSettings.bAllowInvites = true;
    OnlineSessionSettings.bUsesPresence = true;
    OnlineSessionSettings.bAllowJoinViaPresence = true;
    OnlineSessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
    OnlineSessionSettings.bAntiCheatProtected = false;
    OnlineSessionSettings.bUseLobbiesIfAvailable = false;
    OnlineSessionSettings.bUseLobbiesVoiceChatIfAvailable = false;

    this->ActiveSessionSettings = FMyOnlineSessionSettings{SanitizedSessionName, OnlineSessionSettings};
    this->bActiveSessionExists  = true;

    if (this->OnlineSessionInterface->CreateSession(
            /* No need to be fancy here, as we must always be in the front end menu when calling this method. */ 0,
            *SanitizedSessionName,
            OnlineSessionSettings
        ) == false
    )
    {
        LOG_FATAL(LogLSSSS, "Failed to create session due to unknown error.")
    }

    LOG_DISPLAY(LogLSSSS, "Session [%s] creation initiated with %d public connections.", *SanitizedSessionName, InMaxPublicConnections)

    return;
}

void ULocalSessionSupervisorSubsystem::FindSafeSessions(const uint32 InMaxSearchResults, const bool bInLANQuery, const FSearchCallback& InCallback)
{
    if (this->ActiveSessionSearch.bSearching)
    {
        return;
    }

    this->FindSessions(InMaxSearchResults, bInLANQuery, InCallback);

    return;
}

void ULocalSessionSupervisorSubsystem::FindSessions(const uint32 InMaxSearchResults, const bool bInLANQuery, const FSearchCallback& InCallback)
{
    if (InCallback.GetObject() == nullptr || InCallback.GetInterface() == nullptr)
    {
        LOG_FATAL(LogLSSSS, "Callback is invalid.")
        return;
    }

    if (this->ActiveSessionSearch.bSearching)
    {
        LOG_FATAL(LogLSSSS, "Disallowed call while searching.")
        return;
    }

    this->ActiveSessionSearch  = FMyOnlineSessionSearch();
    this->ActiveSearchCallback = InCallback;
    this->ActiveSessionSearch.bSearching               = true;
    this->ActiveSessionSearch.Search->MaxSearchResults = 100;
    this->ActiveSessionSearch.Search->bIsLanQuery      = false;

    if (this->OnlineSessionInterface->FindSessions(
        /* No need to be fancy here, as we must always be in the front end menu when calling this method. */ 0,
        this->ActiveSessionSearch.Search)
    )
    {
        LOG_DISPLAY(LogLSSSS, "Successfully initiated session search.")
        return;
    }

    LOG_FATAL(LogLSSSS, "Failed to initiate session search.")

    return;
}

void ULocalSessionSupervisorSubsystem::JoinSession(const int32 InIndex) const
{
    if (this->ActiveSessionSearch.Search->SearchResults.IsValidIndex(InIndex) == false)
    {
        LOG_FATAL(LogLSSSS, "Invalid session index.")
        return;
    }

    this->JoinSession(this->ActiveSessionSearch.Search->SearchResults[InIndex]);

    return;
}

void ULocalSessionSupervisorSubsystem::JoinSession(const FOnlineSessionSearchResult& InSearchResult) const
{
    if (this->OnlineSessionInterface->JoinSession(
        /* No need to be fancy here, as we must always be in the front end menu when calling this method. */ 0,
        FName(InSearchResult.GetSessionIdStr()),
        InSearchResult
        ) == false
    )
    {
        LOG_FATAL(LogLSSSS, "Failed to join session [%s].", *InSearchResult.Session.OwningUserName)
    }

    return;
}

void ULocalSessionSupervisorSubsystem::LeaveSession(const ERegisteredWorlds::Type InNewWorld /* ERegisteredWorlds::FrontEnd */)
{
    if (this->CurrentState == ECurrentLSSSSState::None)
    {
#if WITH_EDITOR
        LOG_WARNING(LogLSSSS, "Current state is None. If PIE started in level [%s] this can be ignored.", *RegisteredWorlds::World)
#else /* WITH_EDITOR */
        LOG_FATAL(LogLSSSS, "Current state is None. Cannot proceed.")
#endif /* !WITH_EDITOR */
    }

    else if (this->CurrentState == ECurrentLSSSSState::Hosted)
    {
        if (this->ForceActiveSessionDestroy() == false)
        {
            LOG_FATAL(LogLSSSS, "Failed to destroy active session.")
            return;
        }
    }

    else if (this->CurrentState == ECurrentLSSSSState::Joined)
    {
        if (this->ForceActiveSessionDestroy() == false)
        {
            LOG_FATAL(LogLSSSS, "Failed to destroy active session.")
            return;
        }
    }

    else
    {
        LOG_FATAL(LogLSSSS, "Current state is invalid. Cannot proceed.")
        return;
    }

    this->CurrentState = ECurrentLSSSSState::None;

    this->SafeClientTravel(InNewWorld);

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

    if (this->CurrentState != ECurrentLSSSSState::None)
    {
        LOG_FATAL(LogLSSSS, "Current state is not None. Cannot proceed.")
        return;
    }

#if WITH_EDITOR
    /*
     * Safety net if multiple PIE instances are active. As the delegate will be called for all running application
     * instances.
     * Maybe we can find a better way to handle this? Like only adding the delegate handler in the correct instance and
     * not at the start of this subsystem's lifetime.
     */
    if (this->bActiveSessionExists == false)
    {
        LOG_WARNING(LogLSSSS, "No active session found but delegate was called. Cannot proceed. If this is not in the calling PIE instance, this is can be ignored.")
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
        LOG_FATAL(LogLSSSS, "URL is not local internal. Must be as we are the hosting listen server. Faulty URL: %s.", *MyURL.ToString())
        return;
    }

    if (GEngine->MakeSureMapNameIsValid(MyURL.Map) == false)
    {
        LOG_FATAL(LogLSSSS, "Map name is invalid. Faulty URL: %s.", *MyURL.ToString())
        return;
    }

    LOG_VERBOSE(LogLSSSS, "Server traveling to %s.", *MyURL.ToString())

    this->CurrentState = ECurrentLSSSSState::Hosted;
    this->GetWorld()->ServerTravel(MyURL.ToString());

    return;
}

void ULocalSessionSupervisorSubsystem::OnFindSessionsCompleteDelegate(const bool bSuccess)
{
    LOG_DISPLAY(LogLSSSS, "Session search %s. With %d results.", bSuccess ? TEXT("succeeded") : TEXT("failed"), this->ActiveSessionSearch.Search->SearchResults.Num())

    this->ActiveSessionSearch.bSearching = false;

    if (this->ActiveSearchCallback.GetObject() == nullptr || this->ActiveSearchCallback.GetInterface() == nullptr)
    {
#if WITH_EDITOR
        /* Same as in OnCreateSessionCompleteDelegate. This is a safety net for multiple PIE instances. */
        LOG_WARNING(LogLSSSS, "Callback is invalid. Cannot proceed. If this is not in the calling PIE instance, this is can be ignored.")
#else
        LOG_FATAL(LogLSSSS, "Callback is invalid. Cannot proceed.")
#endif /* WITH_EDITOR */
        return;
    }

    LOG_WARNING(LogLSSSS, "Callback is valid. Proceeding.")
    this->ActiveSearchCallback->OnOnlineSessionFoundCompleteDelegate(bSuccess, this);

    return;
}

void ULocalSessionSupervisorSubsystem::OnJoinSessionCompleteDelegate(const FName SessionName, const EOnJoinSessionCompleteResult::Type Result)
{
    if (Result != EOnJoinSessionCompleteResult::Success)
    {
        LOG_FATAL(LogLSSSS, "Failed to join session [%s] with result [%d].", *SessionName.ToString(), *LexToString(Result))
        return;
    }

    if (this->CurrentState != ECurrentLSSSSState::None)
    {
        LOG_FATAL(LogLSSSS, "Current state is not None. Cannot proceed.")
        return;
    }

    /* There is no need to do some crazy PIE crap here, as we have to be in a standalone game to exec this method. */

    FString Address = L"";
    if (this->OnlineSessionInterface->GetResolvedConnectString(SessionName, Address) == false)
    {
        LOG_FATAL(LogLSSSS, "Failed to get resolved connect string for session [%s].", *SessionName.ToString())
        return;
    }
    if (Address.IsEmpty())
    {
        LOG_FATAL(LogLSSSS, "Resolved connect string for session [%s] is empty.", *SessionName.ToString())
        return;
    }

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(
        this->GetWorld(),
        /* No need to be fancy here, as we must always be in the front end menu when calling this method. */ 0
    );
    if (PlayerController == nullptr)
    {
        LOG_FATAL(LogLSSSS, "Player Controller is invalid.")
        return;
    }

    LOG_DISPLAY(LogLSSSS, "Successfully joined session [%s]. Everything ready. Traveling to server's level at [%s].", *SessionName.ToString(), *Address)

    this->CurrentState = ECurrentLSSSSState::Joined;
    PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);

    return;
}

bool ULocalSessionSupervisorSubsystem::ForceActiveSessionDestroy(void)
{
    this->bActiveSessionExists = false;

    this->CurrentState = ECurrentLSSSSState::None;

    if (this->OnlineSessionInterface->DestroySession(FName(this->ActiveSessionSettings.Name)))
    {
        LOG_DISPLAY(LogLSSSS, "Active Session [%s] destroyed.", *this->ActiveSessionSettings.Name)
        this->ActiveSessionSettings = FMyOnlineSessionSettings();
        return true;
    }

    LOG_ERROR(LogLSSSS, "Failed to destroy session [%s].", *this->ActiveSessionSettings.Name)
    return false;
}

void ULocalSessionSupervisorSubsystem::SafeClientTravel(const ERegisteredWorlds::Type InNewWorld)
{
    if (this->CurrentState != ECurrentLSSSSState::None)
    {
        LOG_FATAL(LogLSSSS, "Current state is not None. Disallowed call. Found state: %d.", this->CurrentState)
        return;
    }

    /* Shamelessly copied from GameplayStatics.h. There may be safer ways to implement this? */
    const     FString     LevelName  = FString::Printf(TEXT("%s"), *LexToString(InNewWorld));
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
        LOG_FATAL(LogLSSSS, "URL is not local internal. Must be as we are the client. No networking here. Faulty URL: %s.", *MyURL.ToString())
        return;
    }

    if (GEngine->MakeSureMapNameIsValid(MyURL.Map) == false)
    {
        LOG_FATAL(LogLSSSS, "Map name is invalid. Faulty URL: %s.", *MyURL.ToString())
        return;
    }

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this->GetWorld(), 0);
    if (PlayerController == nullptr)
    {
        LOG_FATAL(LogLSSSS, "Player Controller is invalid.")
        return;
    }

    LOG_DISPLAY(LogLSSSS, "Client traveling to [%s].", *LexToString(InNewWorld))

    PlayerController->ClientTravel(MyURL.ToString(), ETravelType::TRAVEL_Absolute);

    return;
}
