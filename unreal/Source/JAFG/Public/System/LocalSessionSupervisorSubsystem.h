// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"

#include "LocalSessionSupervisorSubsystem.generated.h"

class UJAFGInstance;

struct FMyOnlineSessionSettings
{
    FString Name;
    FOnlineSessionSettings Settings;
};

UCLASS()
class JAFG_API ULocalSessionSupervisorSubsystem : public UGameInstanceSubsystem
{
public:
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
    bool ForceActiveSessionDestroy(void);

protected:

    ////////////////////////////////////////////////////////////////
    // Delegates
    ////////////////////////////////////////////////////////////////

    virtual void OnCreateSessionCompleteDelegate(const FName SessionName, const bool bSuccess);

private:

    IOnlineSessionPtr OnlineSessionInterface;
    FMyOnlineSessionSettings* ActiveSessionSettings;
};
