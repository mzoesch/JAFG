// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ExternalCoreInternalSettings.generated.h"

UCLASS(Config=JAFG, DefaultConfig, meta=(DisplayName="External Core Internal Settings"))
class JAFGEXTERNALCORE_API UExternalCoreInternalSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:

    explicit UExternalCoreInternalSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    /**
     * A subsystem that is initialized prior all internal subsystems.
     * Hook yourself in here to change the initialization behavior of internal subsystems.
     */
    UPROPERTY(Config, EditAnywhere,  BlueprintReadOnly, Category = "Material")
    TSubclassOf<UGameInstanceSubsystem> PreInternalInitializationSubsystem;
};
