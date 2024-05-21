// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "LocalSessionSupervisorSubsystem.generated.h"

struct FMyOnlineSessionSettings
{
    FString Name;
    FOnlineSessionSettings Settings;
    bool bIsActive = false;
};

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

protected:

    virtual void OnCreateSessionCompleteDelegate(const FName SessionName, const bool bSuccess);
    virtual void OnFindSessionsCompleteDelegate(const bool bSuccess);
    virtual void OnJoinSessionCompleteDelegate(const FName SessionName, const EOnJoinSessionCompleteResult::Type Result);

    virtual bool ForceActiveSessionDestroy(void);

private:

    IOnlineSessionPtr OnlineSessionInterface = nullptr;

    FMyOnlineSessionSettings ActiveSessionSettings = FMyOnlineSessionSettings();
};
