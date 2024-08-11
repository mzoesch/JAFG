// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ExternalModificationSubsystem.h"

#include "JAFGTestModTwoSubsystem.generated.h"

UCLASS(NotBlueprintable)
class JAFGTESTMODTWO_API UJAFGTestModTwoSubsystem : public UExternalModificationSubsystem
{
    GENERATED_BODY()

public:

    // USubsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    // ~USubsystem implementation
};
