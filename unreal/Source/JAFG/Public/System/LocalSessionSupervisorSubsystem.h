// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "LocalSessionSupervisorSubsystem.generated.h"

class UJAFGInstance;

UCLASS()
class JAFG_API ULocalSessionSupervisorSubsystem : public UGameInstanceSubsystem
{
public:
    GENERATED_BODY()
    
public:

    static inline constexpr uint8 MaxSessionNameLength = 0x40;
    static inline constexpr uint8 MaxPublicConnections = 0x10;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    void HostListenServer(void);
    void ForceActiveSessionDestroy(void);

protected:

    virtual void OnCreateSessionComplete(const FName SessionName, const bool bSuccess);

private:

    FName TempName;
    
    IOnlineSessionPtr OnlineSessionInterface;
};
