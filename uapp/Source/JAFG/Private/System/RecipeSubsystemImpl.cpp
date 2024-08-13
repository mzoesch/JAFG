// Copyright 2024 mzoesch. All rights reserved.

#include "System/RecipeSubsystemImpl.h"
#include "Interfaces/IPluginManager.h"
#include "System/PathFinder.h"
#include "System/PluginValidationSubsystem.h"
#include "Dom/JsonObject.h"

URecipeSubsystemImpl::URecipeSubsystemImpl(void) : Super()
{
    return;
}

void URecipeSubsystemImpl::ReloadLocalRecipes(void)
{
    this->Recipes.Empty(this->Recipes.Num());

    for (
        TArray<IPlugin*> PluginsToConsider =
             this->GetGameInstance()->GetSubsystem<UPluginValidationSubsystem>()->GetEnabledGamePlugins();
        const IPlugin* Plugin : PluginsToConsider
    )
    {
        check( Plugin )

        TArray<FString> RecipeNames = PathFinder::GetFilesInDirectory(Plugin, EPathType::Recipes, TEXT("json"), true);
        if (RecipeNames.Num() == 0)
        {
            LOG_VERBOSE(LogRecipeSystem, "No recipes found in plugin %s. Skipping", *Plugin->GetName())
            continue;
        }

        const int LoadedRecipes = this->Recipes.Num();

        for (const FString& RecipeName : RecipeNames)
        {
            this->ParseRecipe(Plugin, RecipeName);
        }

        LOG_DISPLAY(LogRecipeSystem, "Loaded %d recipes from plugin %s", this->Recipes.Num() - LoadedRecipes, *Plugin->GetName())

        continue;
    }

    return;
}
