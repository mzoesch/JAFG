// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "System/RecipeSubsystem.h"

#include "RecipeSubsystemImpl.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API URecipeSubsystemImpl : public UGameRecipeSubsystem
{
    GENERATED_BODY()

public:

    URecipeSubsystemImpl();

protected:

    virtual void ReloadLocalRecipes(void) override;
};
