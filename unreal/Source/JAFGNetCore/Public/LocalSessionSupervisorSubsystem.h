// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WorldCore/RegisteredWorldNames.h"

#include "LocalSessionSupervisorSubsystem.generated.h"

class IOnlineSessionSearchCallback;
class ULocalSessionSupervisorSubsystem;

typedef TScriptInterface<IOnlineSessionSearchCallback> FSearchCallback;

UINTERFACE()
class JAFGNETCORE_API UOnlineSessionSearchCallback : public UInterface
{
    GENERATED_BODY()
};

class JAFGNETCORE_API IOnlineSessionSearchCallback
{
    GENERATED_BODY()

public:

    UFUNCTION()
    virtual void OnOnlineSessionFoundCompleteDelegate(const bool bSuccess, const ULocalSessionSupervisorSubsystem* Subsystem) = 0;
};

struct FMyOnlineSessionSettings
{
    FString                Name;
    FOnlineSessionSettings Settings;
};

struct FMyOnlineSessionSearch
{
    bool bSearching;
    TSharedRef<FOnlineSessionSearch> Search = MakeShared<FOnlineSessionSearch>();
};

namespace ECurrentLSSSSState
{

enum Type : uint8
{
    Invalid,
    None,
    Hosted,
    Joined,
};

}

UCLASS(NotBlueprintable)
class JAFGNETCORE_API ULocalSessionSupervisorSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    ULocalSessionSupervisorSubsystem();

    inline static constexpr uint8 MaxSessionNameLength = 0xFF;
    inline static constexpr uint8 MaxPublicConnections = 0xF;

    // USubsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize(void) override;
    // ~USubsystem implementation

    void HostListenServer(const FString& InSessionName, const int32 InMaxPublicConnections, const bool bInLAN);
    void FindSafeSessions(const uint32 InMaxSearchResults, const bool bInLANQuery, const FSearchCallback& InCallback);
    void FindSessions(const uint32 InMaxSearchResults, const bool bInLANQuery, const FSearchCallback& InCallback);
    void JoinSession(const int32 InIndex) const;
    void JoinSession(const FOnlineSessionSearchResult& InSearchResult) const;
    void LeaveSession(const ERegisteredWorlds::Type InNewWorld = ERegisteredWorlds::FrontEnd);

    const FMyOnlineSessionSearch& GetSearch(void) const { return this->ActiveSessionSearch; }

protected:

    virtual void OnCreateSessionCompleteDelegate(const FName SessionName, const bool bSuccess);
    virtual void OnFindSessionsCompleteDelegate(const bool bSuccess);
    virtual void OnJoinSessionCompleteDelegate(const FName SessionName, const EOnJoinSessionCompleteResult::Type Result);

    virtual bool ForceActiveSessionDestroy(void);

    virtual void SafeClientTravel(const ERegisteredWorlds::Type InNewWorld);

private:

    bool bActiveSessionExists = false;

    ECurrentLSSSSState::Type CurrentState = ECurrentLSSSSState::Invalid;

    IOnlineSessionPtr OnlineSessionInterface       = nullptr;
    FSearchCallback   ActiveSearchCallback         = nullptr;
    FMyOnlineSessionSettings ActiveSessionSettings = FMyOnlineSessionSettings();
    FMyOnlineSessionSearch   ActiveSessionSearch   = FMyOnlineSessionSearch();
};
