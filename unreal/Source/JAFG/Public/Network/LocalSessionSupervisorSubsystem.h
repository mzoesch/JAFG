// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"

#include "LocalSessionSupervisorSubsystem.generated.h"

class UJAFGInstance;
class ULocalSessionSupervisorSubsystem;

UINTERFACE()
class UOnlineSessionSearchCallback : public UInterface
{
    GENERATED_BODY()
};

class JAFG_API IOnlineSessionSearchCallback
{
    GENERATED_BODY()

public:

    UFUNCTION()
    virtual void OnOnlineSessionFoundCompleteDelegate(const bool bSuccess, const ULocalSessionSupervisorSubsystem* Subsystem) = 0;
};

struct FMyOnlineSessionSettings
{
    FString Name;
    FOnlineSessionSettings Settings;
};

struct FMyOnlineSessionSearch
{
    bool bSearching;

    // ReSharper disable once CppConstValueFunctionReturnType
    const TSharedRef<FOnlineSessionSearch> GetOnlineSessionSearch(void) const { return this->OnlineSessionSearch; }
    TSharedRef<FOnlineSessionSearch> ChangeOnlineSessionSearch(void)
    {
        if (this->bSearching)
        {
            UE_LOG(LogTemp, Fatal, TEXT("FMyOnlineSessionSearch::ChangeOnlineSessionSearch: Tried to change search while other still pending."))
        }
        
        return this->OnlineSessionSearch;
    }
    
private:

    TSharedRef<FOnlineSessionSearch> OnlineSessionSearch = MakeShared<FOnlineSessionSearch>();
};

UCLASS()
class JAFG_API ULocalSessionSupervisorSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:

    static inline constexpr uint8 MaxSessionNameLength = 0x40;
    static inline constexpr uint8 MaxPublicConnections = 0x10;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize(void) override;

    /**
     * @return True, if input was rejected. Will not return false if the session creation failed in some other matter
     *         as it is the responsibility of the asynchronous delegate to handle that.
     */
    bool HostListenServer(const FString& InSessionName, const int InMaxPublicConnections, const bool bInLAN);
    void FindSessionsAndSafeDiscardPrevious(const uint32 MaxSearchResults, const bool bLANQuery, const TScriptInterface<IOnlineSessionSearchCallback>& InCallback);
    void FindSessions(const uint32 MaxSearchResults, const bool bLANQuery, const TScriptInterface<IOnlineSessionSearchCallback>& InCallback);

    bool ForceActiveSessionDestroy(void);

protected:

    ////////////////////////////////////////////////////////////////
    // Delegates
    ////////////////////////////////////////////////////////////////

    virtual void OnCreateSessionCompleteDelegate(const FName SessionName, const bool bSuccess);
    virtual void OnFindSessionsCompleteDelegate(const bool bSuccess);
    
private:

    IOnlineSessionPtr OnlineSessionInterface;
    FMyOnlineSessionSettings* ActiveSessionSettings;
    FMyOnlineSessionSearch ActiveOnlineSessionSearch;

    TScriptInterface<IOnlineSessionSearchCallback> ActiveOnlineSessionSearchCallback;

public:

    /** Never make changes through this method. */
    TSharedRef<FOnlineSessionSearch> GetActiveOnlineSessionSearch(void) const { return this->ActiveOnlineSessionSearch.GetOnlineSessionSearch(); }
};
