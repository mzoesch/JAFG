// Copyright 2024 mzoesch. All rights reserved.

#include "System/RecipeSubsystem.h"
#include "System/VoxelSubsystem.h"

URecipeSubsystem::URecipeSubsystem(void)
{
    return;
}

void URecipeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<UVoxelSubsystem>();
    Super::Initialize(Collection);

    LOG_VERBOSE(LogRecipeSystem, "Called.")

    return;
}

void URecipeSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
}
