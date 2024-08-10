// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ExternalGameInstanceSubsystem.h"

#include "RecipeSubsystem.generated.h"

UCLASS(NotBlueprintable)
class JAFGEXTERNALCORE_API URecipeSubsystem : public UExternalGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    URecipeSubsystem();

    // Subsystem implementation
    virtual auto Initialize(FSubsystemCollectionBase& Collection) -> void override;
    virtual auto Deinitialize(void) -> void override;
    // ~Subsystem implementation


};
