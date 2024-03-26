// Copyright 2024 mzoesch. All rights reserved.

#include "Network/LocalSessionSupervisorSubsystem.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystemUtils.h"

void ULocalSessionSupervisorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    UE_LOG(LogTemp, Log, TEXT("ULocalSessionSupervisor::Initialize: Initializing Local Session Supervisor Subsystem."))
    
    const IOnlineSubsystem* OSSModule = IOnlineSubsystem::Get();
    
    if (OSSModule == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("ULocalSessionSupervisor::Initialize: Online Subsystem Module is invalid."))
        return;
    }

    this->OnlineSessionInterface = OSSModule->GetSessionInterface();

    if (this->OnlineSessionInterface == nullptr || this->OnlineSessionInterface.IsValid() == false)
    {
        UE_LOG(LogTemp, Fatal, TEXT("ULocalSessionSupervisor::Initialize: Online Session Interface is invalid."))
        return;
    }

    this->OnlineSessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &ULocalSessionSupervisorSubsystem::OnCreateSessionCompleteDelegate);
    this->OnlineSessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &ULocalSessionSupervisorSubsystem::OnFindSessionsCompleteDelegate);
    
    this->ActiveSessionSettings = nullptr;
    this->ActiveOnlineSessionSearch = FMyOnlineSessionSearch();
    
    return;
}

void ULocalSessionSupervisorSubsystem::Deinitialize(void)
{
    Super::Deinitialize();

    UE_LOG(LogTemp, Log, TEXT("ULocalSessionSupervisor::Deinitialize: Deinitializing Local Session Supervisor Subsystem."))

    if (this->OnlineSessionInterface.IsValid() == false)
    {
        UE_LOG(LogTemp, Error, TEXT("ULocalSessionSupervisor::Deinitialize: Online Session Interface is invalid."))
        return;
    }

    if (this->ActiveSessionSettings != nullptr)
    {
        this->ForceActiveSessionDestroy();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ULocalSessionSupervisor::Deinitialize: Active Session Settings is nullptr. No session to destory."))
    }

    return;
}

bool ULocalSessionSupervisorSubsystem::HostListenServer(const FString& InSessionName, const int InMaxPublicConnections, const bool bInLAN)
{
    FString SanitizedSessionName = InSessionName; SanitizedSessionName.ReplaceInline(TEXT(" "), TEXT("-"));
    if (SanitizedSessionName.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("ULocalSessionSupervisor::HostListenServer: Session name is empty."))
        return false;
    }
    if (SanitizedSessionName.Len() > ULocalSessionSupervisorSubsystem::MaxSessionNameLength)
    {
        UE_LOG(LogTemp, Error, TEXT("ULocalSessionSupervisor::HostListenServer: Session name is too long."))
        return false;
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

    this->ActiveSessionSettings = new FMyOnlineSessionSettings(SanitizedSessionName, OnlineSessionSettings);
    
    if (this->OnlineSessionInterface->CreateSession(/* No need to be fancy here, as we must always be in the Menu when calling this method. */ 0, *SanitizedSessionName, OnlineSessionSettings))
    {
        UE_LOG(LogTemp, Log, TEXT("ULocalSessionSupervisor::HostListenServer: Successfully created session."))
        return true;
    }

    UE_LOG(LogTemp, Error, TEXT("ULocalSessionSupervisor::HostListenServer: Failed to create session."))

    return false;
}

void ULocalSessionSupervisorSubsystem::FindSessionsAndSafeDiscardPrevious(const uint32 MaxSearchResults, const bool bLANQuery,  TScriptInterface<IOnlineSessionSearchCallback>& InCallback)
{
    if (this->ActiveOnlineSessionSearch.bSearching)
    {
        UE_LOG(LogTemp, Warning, TEXT("ULocalSessionSupervisor::FindSessionsAndSafeDiscardPrevious: Active session search is still pending. Discarding not implemented yet."))
        return;
    }

    this->FindSessions(MaxSearchResults, bLANQuery, InCallback);

    return;
}

void ULocalSessionSupervisorSubsystem::FindSessions(const uint32 MaxSearchResults, const bool bLANQuery, TScriptInterface<IOnlineSessionSearchCallback>& InCallback)
{
    if (InCallback.GetObject() == nullptr || InCallback.GetInterface() == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("ULocalSessionSupervisor::FindSessions: Callback is nullptr."))
        return;
    }

    this->ActiveOnlineSessionSearchCallback = InCallback;

    this->ActiveOnlineSessionSearch.ChangeOnlineSessionSearch()->MaxSearchResults = MaxSearchResults;
    this->ActiveOnlineSessionSearch.ChangeOnlineSessionSearch()->bIsLanQuery = bLANQuery;
    /* Do some further settings here. */

    this->ActiveOnlineSessionSearch.bSearching = true;
    
    if (this->OnlineSessionInterface->FindSessions(/* No need to be fancy here, as we must always be in the Menu when calling this method. */ 0, this->ActiveOnlineSessionSearch.GetOnlineSessionSearch()))
    {
        UE_LOG(LogTemp, Log, TEXT("ULocalSessionSupervisor::FindSession: Successfully initiated session search."))
        return;
    }

    UE_LOG(LogTemp, Error, TEXT("ULocalSessionSupervisor::FindSession: Failed to initiate session search."))

    return;
}

bool ULocalSessionSupervisorSubsystem::ForceActiveSessionDestroy(void)
{
    if (this->OnlineSessionInterface->DestroySession(FName(this->ActiveSessionSettings->Name)))
    {
        UE_LOG(LogTemp, Log, TEXT("ULocalSessionSupervisor::ForceActiveSessionDestroy: Active Session [%s] destroyed."), *this->ActiveSessionSettings->Name)
        delete this->ActiveSessionSettings; this->ActiveSessionSettings = nullptr;
        return true;
    }
    
    UE_LOG(LogTemp, Error, TEXT("ULocalSessionSupervisor::ForceActiveSessionDestroy: Failed to destroy session [%s]."), *this->ActiveSessionSettings->Name)
    return false;
}

void ULocalSessionSupervisorSubsystem::OnCreateSessionCompleteDelegate(const FName SessionName, const bool bSuccess)
{
    UE_LOG(LogTemp, Warning, TEXT("ULocalSessionSupervisor::OnCreateSessionCompleteDelegate: Session [%s] created: %s."), *SessionName.ToString(), bSuccess ? TEXT("Success") : TEXT("Failure"))

    if (bSuccess == false)
    {
        /* Some feedback to the front end is needed here. */
        return;
    }

    if (GEngine == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("ULocalSessionSupervisor::OnCreateSessionCompleteDelegate: GEngine is nullptr."))
        return;
    }

    if (this->GetWorld() == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("ULocalSessionSupervisor::OnCreateSessionCompleteDelegate: World is nullptr."))
        return;
    }

#if WITH_EDITOR
    /*
     * Safety net if multiple PIE instances are active. As the delegate will be called for all running application
     * instances.
     * Maybe we can find a better way to handle this? Like only adding the delegate handler in the correct instance and
     * not at the start of this subsystems lifetime.
     */
    if (this->ActiveSessionSettings == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("ULocalSessionSupervisor::OnCreateSessionCompleteDelegate: Active Session Settings is nullptr. Cannot proceed. If this is not in the calling PIE instance, this is can be ignored."))
        return;
    }
#endif /* WITH_EDITOR */

    /* Shamelessly copied from GameplayStatics.h. There may be safer ways to implement this? */

    const FString LevelName          = "L_World";
    constexpr ETravelType TravelType = ETravelType::TRAVEL_Absolute;

    FWorldContext* WorldContextObject = GEngine->GetWorldContextFromWorld(this->GetWorld());
    
    if (WorldContextObject == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("ULocalSessionSupervisor::OnCreateSessionCompleteDelegate: World Context Object is nullptr."))
        return;
    }
    
    FURL MyURL = FURL(&WorldContextObject->LastURL, *LevelName, TravelType);
    
    if (MyURL.IsLocalInternal() == false)
    {
        UE_LOG(LogTemp, Fatal, TEXT("ULocalSessionSupervisor::OnCreateSessionCompleteDelegate: URL is not local internal. Must be as we are the hosting listen server."))
        return;
    }
    
    if (GEngine->MakeSureMapNameIsValid(MyURL.Map) == false)
    {
        UE_LOG(LogTemp, Fatal, TEXT("ULocalSessionSupervisor::OnCreateSessionCompleteDelegate: Map name is invalid."))
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("ULocalSessionSupervisor::OnCreateSessionCompleteDelegate: Server traveling to %s."), *MyURL.ToString())
    
    this->GetWorld()->ServerTravel(MyURL.ToString());
    
    return;
}

void ULocalSessionSupervisorSubsystem::OnFindSessionsCompleteDelegate(const bool bSuccess)
{
    UE_LOG(LogTemp, Log, TEXT("ULocalSessionSupervisor::OnFindSessionsCompleteDelegate: Session search completed with some result: %s."), bSuccess ? TEXT("Success") : TEXT("Failure"))

    this->ActiveOnlineSessionSearch.bSearching = false;
    
    if (this->ActiveOnlineSessionSearchCallback.GetObject() == nullptr || this->ActiveOnlineSessionSearchCallback.GetInterface() == nullptr)
    {
#if WITH_EDITOR
        /* Same as in OnCreateSessionCompleteDelegate. This is a safety net for multiple PIE instances. */
        UE_LOG(LogTemp, Warning, TEXT("ULocalSessionSupervisor::OnFindSessionsCompleteDelegate: Callback is nullptr. Cannot proceed. If this is not in the calling PIE instance, this is can be ignored."))
#else
        UE_LOG(LogTemp, Fatal, TEXT("ULocalSessionSupervisor::OnFindSessionsCompleteDelegate: Callback is nullptr."))
#endif /* WITH_EDITOR */
        return;
    }

    this->ActiveOnlineSessionSearchCallback->OnOnlineSessionFoundComplete(bSuccess, this);
    
    return;
}
