// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "OnlineSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MyOnlineSessionSettings.h"

#include "LocalSessionSupervisorSubsystem.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class SESSIONSUPERVISOR_API ULocalSessionSupervisorSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    inline static constexpr uint8 MaxSessionNameLength = 0x40;
    inline static constexpr uint8 MaxPublicConnections = 0x10;

    // UGameInstanceSubsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize(void) override;
    // ~UGameInstanceSubsystem implementation

    /**
     * @return False, if input was rejected. Will not return false if the session creation failed in some other matter
     *         as it is the responsibility of the asynchronous delegate to handle that.
     */
    bool HostListenServer(const FString& InSessionName, const int InMaxPublicConnections, const bool bInLAN);

protected:

    //////////////////////////////////////////////////////////////////////////
    // Delegates
    //////////////////////////////////////////////////////////////////////////

    virtual void OnCreateSessionCompleteDelegate(const FName SessionName, const bool bSuccess);

private:

    IOnlineSessionPtr         OnlineSessionInterface;
    FMyOnlineSessionSettings* ActiveSessionSettings;
};
