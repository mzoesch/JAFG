// Copyright 2024 mzoesch. All rights reserved.

#include "System/RecipeSubsystem.h"
#include "Interfaces/IPluginManager.h"
#include "System/PathFinder.h"
#include "System/VoxelSubsystem.h"

void UWorldRecipeSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    LOG_VERBOSE(LogRecipeSystem, "Called. Initializing base game recipes if not already done and applying them to the world with their rules.")

    if (this->GetRecipeSubsystem(InWorld)->GetRecipes().IsEmpty())
    {
        /* Note that the array may still be empty if no recipes are loaded. */
        this->GetRecipeSubsystem(InWorld)->ReloadLocalRecipes();
    }

    this->CachedActiveRecipes = this->GetRecipeSubsystem(InWorld)->GetRecipes();

    /*
     * TODO: Here then load world-specific recipes and apply them to the cached active recipes.
     */

    LOG_VERBOSE(LogRecipeSystem, "Finished initializing world recipes. Found %d recipes that are now active in this world.", this->CachedActiveRecipes.Num())

    return;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
URecipeSubsystem* UWorldRecipeSubsystem::GetRecipeSubsystem(const UWorld& InWorld)
{
    return InWorld.GetGameInstance()->GetSubsystem<URecipeSubsystem>();
}

URecipeSubsystem* UWorldRecipeSubsystem::GetRecipeSubsystem(void) const
{
    return this->GetWorld()->GetGameInstance()->GetSubsystem<URecipeSubsystem>();
}

URecipeSubsystem::URecipeSubsystem(void)
{
    return;
}

void URecipeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<UVoxelSubsystem>();
    Super::Initialize(Collection);

    LOG_VERBOSE(LogRecipeSystem, "Called.")

    this->ReloadLocalRecipes();

    LOG_VERBOSE(LogRecipeSystem, "Finished loading recipes. Found %d recipes.", this->Recipes.Num())

    return;
}

void URecipeSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
}

void URecipeSubsystem::ParseRecipe(const IPlugin* InPlugin, const FString& RecipeName)
{
    const TSharedPtr<FJsonObject> Obj = PathFinder::LoadJSONFromDisk(InPlugin, EPathType::Recipes, RecipeName);
    if (Obj.IsValid() == false)
    {
        LOG_RELAXED_FATAL(LogRecipeSystem, "Failed to load recipe %s.", *RecipeName)
        return;
    }

    this->ParseRecipe(InPlugin->GetName(), RecipeName, Obj);

    return;
}

void URecipeSubsystem::ParseRecipe(const FString& RecipeNamespace, const FString& RecipeName, const TSharedPtr<FJsonObject>& Obj)
{
    if (this->Recipes.ContainsByPredicate( [RecipeNamespace, RecipeName] (const FRecipe& Recipe) -> bool
    {
        return Recipe.Namespace == RecipeNamespace && Recipe.Name == RecipeName;
    }))
    {
        LOG_RELAXED_FATAL(LogRecipeSystem, "Semantic error: Recipe [%s::%s] already exists.", *RecipeNamespace, *RecipeName)
        return;
    }

    if (Obj->HasField(RecipeJsonTranslation::Delivery) == false)
    {
        LOG_RELAXED_FATAL(LogRecipeSystem, "Syntax error: Recipe [%s::%s] does not have a delivery field.", *RecipeNamespace, *RecipeName)
        return;
    }

    if (Obj->HasField(RecipeJsonTranslation::Product) == false)
    {
        LOG_RELAXED_FATAL(LogRecipeSystem, "Syntax error: Recipe [%s::%s] does not have a product field.", *RecipeNamespace, *RecipeName)
        return;
    }

    FRecipe Recipe;
    Recipe.Namespace = RecipeNamespace;
    Recipe.Name      = RecipeName;

    FString Error;
    if (this->ParseRecipeDelivery(Obj, Recipe.Delivery, Error) == false)
    {
        LOG_RELAXED_FATAL(LogRecipeSystem, "Failed to parse delivery for recipe [%s::%s]. Error: %s", *RecipeNamespace, *RecipeName, *Error)
        return;
    }

    Error.Empty();
    if (this->ParseRecipeProduct(Obj, Recipe.Product, Error) == false)
    {
        LOG_RELAXED_FATAL(LogRecipeSystem, "Failed to parse product for recipe [%s::%s]. Error: %s", *RecipeNamespace, *RecipeName, *Error)
        return;
    }

    this->Recipes.Emplace(Recipe);

    return;
}

/**
 * Assumes an array with the following format:
 *     ["AccumulatedName", "AccumulatedNamespace::AccumulatedName", "AccumulatedName"].
 */
bool ParseDeliveryArray(const UObject& Context, const TArray<TSharedPtr<FJsonValue>>& Array, const bool bCanContainNull, FRecipeDelivery& OutDelivery, FString& OutError)
{
    const UVoxelSubsystem* VoxelSubsystem = Context.GetWorld()->GetGameInstance()->GetSubsystem<UVoxelSubsystem>();

    for (const TSharedPtr<FJsonValue>& Value : Array)
    {
        if (Value->IsNull())
        {
            if (bCanContainNull)
            {
                OutDelivery.Contents.Emplace(Accumulated::Null);
                continue;
            }

            OutError = FString::Printf(TEXT("Semantic error: Delivery contents cannot contain null."));
            continue;
        }

        FAccumulated Accumulated;
        if (FString PotentialNamespace, PotentialName; VoxelSubsystem->SplitAccumulatedName(Value->AsString(), PotentialNamespace, PotentialName))
        {
            Accumulated = FAccumulated(VoxelSubsystem->GetAccumulatedIndex(PotentialNamespace, PotentialName));
        }
        else
        {
            Accumulated = FAccumulated(VoxelSubsystem->GetAccumulatedIndex(Value->AsString()));
        }
        if (Accumulated == Accumulated::Null)
        {
            OutError = FString::Printf(TEXT("Semantic error: Accumulated [%s] not found."), *Value->AsString());
            return false;
        }

        OutDelivery.Contents.Emplace(Accumulated);

        continue;
    }

    if (OutDelivery.Contents.IsEmpty())
    {
        OutError = FString::Printf(TEXT("Semantic error: Delivery contents are empty."));
        return false;
    }

    return true;
}

bool URecipeSubsystem::ParseRecipeDelivery(const TSharedPtr<FJsonObject>& Obj, FRecipeDelivery& OutDelivery, FString& OutError) const
{
    const UVoxelSubsystem* VoxelSubsystem = this->GetGameInstance()->GetSubsystem<UVoxelSubsystem>();
    check( VoxelSubsystem )

    /* Assuming: { "Delivery": { "Contents": ["Name", "Name"], <"Width": NUMERIC> } }. */
    if (Obj->HasTypedField<EJson::Object>(RecipeJsonTranslation::Delivery))
    {
        if (Obj->GetObjectField(RecipeJsonTranslation::Delivery)->HasTypedField<EJson::Array>(RecipeJsonTranslation::DeliveryContents) == false)
        {
            OutError = FString::Printf(TEXT("Semantic error: Delivery object field does not have a contents field."));
            return false;
        }

        if (
            ParseDeliveryArray(
                *this,
                Obj->GetObjectField(RecipeJsonTranslation::Delivery)->GetArrayField(RecipeJsonTranslation::DeliveryContents),
                true,
                OutDelivery,
                OutError
            ) == false
        )
        {
            return false;
        }

        if (Obj->GetObjectField(RecipeJsonTranslation::Delivery)->HasTypedField<EJson::Number>(RecipeJsonTranslation::DeliveryWidth))
        {
            OutDelivery.Width = Obj->GetObjectField(RecipeJsonTranslation::Delivery)->GetIntegerField(RecipeJsonTranslation::DeliveryWidth);
        }
    }

    /* Assuming: { "Delivery": ["Name", "Name"] }. */
    else if (Obj->HasTypedField<EJson::Array>(RecipeJsonTranslation::Delivery))
    {
        if (
            ParseDeliveryArray(
                *this,
                Obj->GetArrayField(RecipeJsonTranslation::Delivery),
                false,
                OutDelivery,
                OutError
            ) == false
        )
        {
            return false;
        }
    }

    /* Assuming { "Delivery": "Name" } */
    else if (Obj->HasTypedField<EJson::String>(RecipeJsonTranslation::Delivery))
    {
        FAccumulated Accumulated;
        if (FString PotentialNamespace, PotentialName; VoxelSubsystem->SplitAccumulatedName(Obj->GetStringField(RecipeJsonTranslation::Delivery), PotentialNamespace, PotentialName))
        {
            Accumulated = FAccumulated(VoxelSubsystem->GetAccumulatedIndex(PotentialNamespace, PotentialName));
        }
        else
        {
            Accumulated = FAccumulated(VoxelSubsystem->GetAccumulatedIndex(Obj->GetStringField(RecipeJsonTranslation::Delivery)));
        }
        if (Accumulated == Accumulated::Null)
        {
            OutError = FString::Printf(TEXT("Semantic error: Accumulated [%s] not found."), *Obj->GetStringField(RecipeJsonTranslation::Delivery));
            return false;
        }

        OutDelivery.Contents.Emplace(Accumulated);
    }

    else
    {
        OutError = FString::Printf(TEXT("Syntax error: Delivery field is not an object, array, or string."));
        return false;
    }

    return true;
}

bool URecipeSubsystem::ParseRecipeProduct(const TSharedPtr<FJsonObject>& Obj, FRecipeProduct& OutProduct, FString& OutError) const
{
    const UVoxelSubsystem* VoxelSubsystem = this->GetGameInstance()->GetSubsystem<UVoxelSubsystem>();
    check( VoxelSubsystem )

    /* Assuming: { "Product": { "Content": "Name", <"Amount": NUMERIC> } }. */
    if (Obj->HasTypedField<EJson::Object>(RecipeJsonTranslation::Product))
    {
        if (Obj->GetObjectField(RecipeJsonTranslation::Product)->HasTypedField<EJson::String>(RecipeJsonTranslation::ProductContent) == false)
        {
            OutError = FString::Printf(TEXT("Semantic error: Product object field does not have a content field."));
            return false;
        }

        FAccumulated Accumulated;
        if (FString PotentialNamespace, PotentialName; this->GetGameInstance()->GetSubsystem<UVoxelSubsystem>()->SplitAccumulatedName(Obj->GetObjectField(RecipeJsonTranslation::Product)->GetStringField(RecipeJsonTranslation::ProductContent), PotentialNamespace, PotentialName))
        {
            Accumulated = FAccumulated(VoxelSubsystem->GetAccumulatedIndex(PotentialNamespace, PotentialName));
        }
        else
        {
            Accumulated = FAccumulated(VoxelSubsystem->GetAccumulatedIndex(Obj->GetObjectField(RecipeJsonTranslation::Product)->GetStringField(RecipeJsonTranslation::ProductContent)));
        }
        if (Accumulated == Accumulated::Null)
        {
            OutError = FString::Printf(TEXT("Semantic error: Accumulated [%s] not found."), *Obj->GetObjectField(RecipeJsonTranslation::Product)->GetStringField(RecipeJsonTranslation::ProductContent));
            return false;
        }

        if (Obj->GetObjectField(RecipeJsonTranslation::Product)->HasTypedField<EJson::Number>(RecipeJsonTranslation::ProductAmount))
        {
            OutProduct.Product.Amount = Obj->GetObjectField(RecipeJsonTranslation::Product)->GetIntegerField(RecipeJsonTranslation::ProductAmount);
        }
        if (OutProduct.Product.Amount <= 0)
        {
            OutError = FString::Printf(TEXT("Semantic error: Product amount must be greater than zero."));
            return false;
        }
    }

    /* Assuming { "Product": "Name" } */
    else if (Obj->HasTypedField<EJson::String>(RecipeJsonTranslation::Product))
    {
        FAccumulated Accumulated;
        if (FString PotentialNamespace, PotentialName; VoxelSubsystem->SplitAccumulatedName(Obj->GetStringField(RecipeJsonTranslation::Product), PotentialNamespace, PotentialName))
        {
            Accumulated = FAccumulated(VoxelSubsystem->GetAccumulatedIndex(PotentialNamespace, PotentialName));
        }
        else
        {
            Accumulated = FAccumulated(VoxelSubsystem->GetAccumulatedIndex(Obj->GetStringField(RecipeJsonTranslation::Product)));
        }
        if (Accumulated == Accumulated::Null)
        {
            OutError = FString::Printf(TEXT("Semantic error: Accumulated [%s] not found."), *Obj->GetStringField(RecipeJsonTranslation::Product));
            return false;
        }

        OutProduct.Product = Accumulated;

        jcheck(
            /*
             * Implicitly asserted,
             * as the constructor should increase the amount by one if not accumulated null.
             */
            OutProduct.Product.Amount == 1
        )
    }

    else
    {
        OutError = FString::Printf(TEXT("Syntax error: Product field is not an object or string."));
        return false;
    }

    return true;
}
