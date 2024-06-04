// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "ExternalSubsystem.generated.h"

/**
 * If the external core game instance subsystem can be hooked into at initialization,
 * this class should be derived from.
 */
UCLASS(Abstract, NotBlueprintable)
class JAFGEXTERNALCORE_API UExternalSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    UExternalSubsystem();

    // USubsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    // ~USubsystem implementation
};
