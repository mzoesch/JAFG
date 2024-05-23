// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "SettingsData/GameSettingRegistry.h"

#include "JAFGGameSettingRegistry.generated.h"

class UJAFGGameSettingRegistry;

UCLASS(NotBlueprintable)
class UJAFGGameSettingRegistrySubsystem : public ULocalPlayerSubsystem
{
    GENERATED_BODY()

public:

    UJAFGGameSettingRegistrySubsystem();

    // ULocalPlayerSubsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    // ~ULocalPlayerSubsystem implementation

    UPROPERTY()
    TObjectPtr<UJAFGGameSettingRegistry> LocalRegistry;
};

UCLASS(NotBlueprintable)
class JAFGSETTINGS_API UJAFGGameSettingRegistry : public UGameSettingRegistry
{
    GENERATED_BODY()

public:

    explicit UJAFGGameSettingRegistry(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // UGameSettingRegistry implementation
    virtual void Initialize(ULocalPlayer* InLocalPlayer) override;
    // ~UGameSettingRegistry implementation
};
