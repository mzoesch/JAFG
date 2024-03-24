// Copyright 2024 mzoesch. All rights reserved.

#include "Network/LocalSessionSupervisorSubsystem.h"

#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"

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

    this->ActiveSessionSettings = nullptr;
    
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
    
    FOnlineSessionSettings OnlineSessionSettings;
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
    UE_LOG(LogTemp, Warning, TEXT("ULocalSessionSupervisor::OnCreateSessionCompleteDelegate: Session [%s] created with some name: %s."), *SessionName.ToString(), bSuccess ? TEXT("Success") : TEXT("Failure"))

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
