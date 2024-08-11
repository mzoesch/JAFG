// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ExternalModificationSubsystem.h"

#include "JAFGTestModThreeSubsystem.generated.h"

UCLASS(NotBlueprintable)
class JAFGTESTMODTHREE_API UJAFGTestModThreeSubsystem : public UExternalModificationSubsystem
{
    GENERATED_BODY()

public:

    // USubsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    // ~USubsystem implementation
};
