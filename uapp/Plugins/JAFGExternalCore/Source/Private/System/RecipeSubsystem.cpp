// Copyright 2024 mzoesch. All rights reserved.

#include "System/RecipeSubsystem.h"
#include "Interfaces/IPluginManager.h"
#include "System/PathFinder.h"
#include "System/VoxelSubsystem.h"

bool FSenderDeliver::IsEmpty(void) const
{
    for (const FAccumulated& Accumulated : this->Contents)
    {
        if (Accumulated != ::Accumulated::Null)
        {
            return false;
        }

        continue;
    }

    return true;
}

IRecipeSubsystem* IRecipeSubsystem::Get(const UObject& Context)
{
    if (UWorldRecipeSubsystem* Subsystem = Context.GetWorld()->GetSubsystem<UWorldRecipeSubsystem>(); Subsystem)
    {
        return Subsystem;
    }

    return Context.GetWorld()->GetGameInstance()->GetSubsystem<UGameRecipeSubsystem>();
}

IRecipeSubsystem* IRecipeSubsystem::Get(const UObject* Context)
{
    return IRecipeSubsystem::Get(*Context);
}

TArray<const FRecipe*> IRecipeSubsystem::GetRecipesForAccumulated(const FAccumulated& InAccumulated) const
{
    TArray<const FRecipe*> Out;
    for (const FRecipe& Recipe : this->GetRecipes())
    {
        if (Recipe.ProductContains(InAccumulated))
        {
            Out.Emplace(&Recipe);
        }

        continue;
    }

    return Out;
}

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
     *       Remember to simply recipes with the game instance subsystem of this type.
     */

    LOG_VERBOSE(LogRecipeSystem, "Finished initializing world recipes. Found %d recipes that are now active in this world.", this->CachedActiveRecipes.Num())

    for (const FRecipe& Recipe : this->CachedActiveRecipes)
    {
        LOG_VERBOSE(LogRecipeSystem, "R: %s", *Recipe.ToString())
    }

    return;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
UGameRecipeSubsystem* UWorldRecipeSubsystem::GetRecipeSubsystem(const UWorld& InWorld)
{
    return InWorld.GetGameInstance()->GetSubsystem<UGameRecipeSubsystem>();
}

UGameRecipeSubsystem* UWorldRecipeSubsystem::GetRecipeSubsystem(void) const
{
    return this->GetWorld()->GetGameInstance()->GetSubsystem<UGameRecipeSubsystem>();
}

bool UWorldRecipeSubsystem::GetRecipe(const FSenderDeliver& InSenderDelivery, FRecipe& OutRecipe) const
{
    if (InSenderDelivery.IsEmpty())
    {
        return false;
    }

    /*
     * This is currently O(n) and we maybe should optimize this in the future when we have quite a bit of recipes.
     *  Maybe with some sort of hashing?
     */
    for (const FRecipe& Recipe : this->CachedActiveRecipes)
    {
        if (this->IsRecipeValidForDelivery(Recipe, InSenderDelivery))
        {
            OutRecipe = Recipe;
            return true;
        }

        continue;
    }

    return false;
}

bool UWorldRecipeSubsystem::GetProduct(const FSenderDeliver& InSenderDelivery, FRecipeProduct& OutProduct) const
{
    FRecipe Recipe;
    if (this->GetRecipe(InSenderDelivery, Recipe) == false)
    {
        return false;
    }

    OutProduct = Recipe.Product;

    return true;
}

bool UWorldRecipeSubsystem::IsRecipeValidForDelivery(const FRecipe& InRecipe, const FSenderDeliver& InSenderDelivery) const
{
    return InRecipe.GetType() == ERecipeType::ShapelessRecipe
        ? this->IsRecipeValidForDelivery_Shapeless(InRecipe, InSenderDelivery)
        : this->IsRecipeValidForDelivery_Shaped(InRecipe, InSenderDelivery);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
bool UWorldRecipeSubsystem::IsRecipeValidForDelivery_Shapeless(const FRecipe& InRecipe, const FSenderDeliver& InSenderDelivery, const bool bAllowNullInRecipe /* = false */) const
{
    /*
     * Note:
     *          The amount of FAccumulated is misused to store the amount
     *          of one unique accumulated item in the delivery. As this is
     *          a shapeless recipe, we must only care about the amounts and
     *          not the order of the delivery contents.
     */
    TArray<FAccumulated> DeliveryContentCounter;
    for (const FAccumulated& SenderDeliveryContent : InSenderDelivery.Contents)
    {
        if (SenderDeliveryContent.IsNull())
        {
            continue;
        }

        if (InRecipe.DeliveryContains(SenderDeliveryContent) == false)
        {
            return false;
        }

        if (DeliveryContentCounter.Contains(SenderDeliveryContent))
        {
            ++DeliveryContentCounter[DeliveryContentCounter.Find(SenderDeliveryContent)].Amount;
            continue;
        }

        check( SenderDeliveryContent.Amount == 1 )
        DeliveryContentCounter.Emplace(SenderDeliveryContent);
        check( DeliveryContentCounter.Last().Amount == 1 )

        continue;
    }

    if (DeliveryContentCounter.Num() != InRecipe.Delivery.Contents.Num())
    {
        return false;
    }

    for (const FAccumulated& RecipeDeliveryContent : InRecipe.Delivery.Contents)
    {
        if (RecipeDeliveryContent.IsNull())
        {
            if (bAllowNullInRecipe == false)
            {
                LOG_RELAXED_FATAL(
                    LogRecipeSystem,
                    "Semantic error: Null found in the shapeless recipe [%s::%s].",
                    *InRecipe.Namespace, *InRecipe.Name
                )
                return false;
            }
            continue;
        }

        if (
            /* Find will never search for amounts only compares the accumulated indices. */
            const int32 Index = DeliveryContentCounter.Find(RecipeDeliveryContent);
            Index == INDEX_NONE || DeliveryContentCounter[Index].Amount != RecipeDeliveryContent.Amount
        )
        {
            return false;
        }

        continue;
    }

    return true;
}

bool UWorldRecipeSubsystem::IsRecipeValidForDelivery_Shaped(const FRecipe& InRecipe, const FSenderDeliver& InSenderDelivery) const
{
#define BEGIN_OF_ROW 0
    /*
     * If this is a 3x3 recipe, we of course cannot use this in a 2x2 grid.
     * But we, of course, can use a 2x2 recipe in a 3x3 grid.
     */
    if (InRecipe.Delivery.Width > InSenderDelivery.Width)
    {
        return false;
    }

    /*
     * Check if we have a mismatch from the amounts of each type, basically we treat
     * the recipe as shapeless and check if it would be a valid recipe.
     * This only checks for real accumulated items and not the number of nulls.
     */
    if (this->IsRecipeValidForDelivery_Shapeless(InRecipe, InSenderDelivery, true) == false)
    {
        return false;
    }

    /*
     * The index that we currently check if it matches with the recipe delivery.
     * All indices that are below this value are already checked and are not valid.
     *
     *
     * Example:
     *
     * The hexadecimal numbers in the grids represent the indices,
     * the symbols are a placeholder for arbitrary accumulated items.
     *
     * The Delivery:            The Recipe:
     * +---+---+---+---+        +---+---+     If DeliveryStartIndexCursor is five:
     * |0  |1  |2  |3  |        |0  |1@ |       In this example, all indices below five are already
     * +---+---+---+---+        +---+---+       checked and are invalid.
     * |4  |5  |6@ |7  |        |2@ |3# |       The first valid index is five.
     * +---+---+---+---+        +---+---+
     * |8  |9@ |A# |B  |
     * +---+---+---+---+
     * |C  |D  |E  |F  |
     * +---+---+---+---+
     * Width: 4                 Width: 2
     *
     * Note that if we have any cursor greater than one, we have to add the width of the cursor to the left side
     * as we progress to the next row. We do not care about stuff outside the currently seeking subgrid of the
     * sender delivery grid.
     * The same is true for the recipe grid. If the recipe grid progresses to the next row, we have
     * to add this also to the sender delivery cursor.
     * We have to match both grid sizes as there will be size mismatches.
     */
    int32 DeliveryStartIndexCursor = 0;
    while (DeliveryStartIndexCursor < InSenderDelivery.Contents.Num())
    {
        int32 SenderDeliveryContentCursor = DeliveryStartIndexCursor;
        int32 RecipeContentCursor         = BEGIN_OF_ROW;

        while (true)
        {
            /*
             * We have to check this because if a recipe row reached to an end, we add the indices that
             * we expect to be in the sender delivery content grid cursor. But these can be out of bounds.
             */
            if (SenderDeliveryContentCursor >= InSenderDelivery.Contents.Num())
            {
                goto NextDeliveryIndex;
            }

            if (InSenderDelivery.Contents[SenderDeliveryContentCursor] != InRecipe.Delivery.Contents[RecipeContentCursor])
            {
                goto NextDeliveryIndex;
            }

            /*
             * Both fields are matching. Now we have to carefully go to the next index.
             * Note that we cannot just add one, as we have to keep track of the width of both grids.
             */
            ++SenderDeliveryContentCursor; ++RecipeContentCursor;

            /* We have a winner here, as we reached the end of the recipe without errors :). Yay. */
            if (RecipeContentCursor >= InRecipe.Delivery.Contents.Num())
            {
                break;
            }

            if (RecipeContentCursor % InRecipe.Delivery.Width == BEGIN_OF_ROW)
            {
                if (SenderDeliveryContentCursor % InSenderDelivery.Width == BEGIN_OF_ROW)
                {
                    /* Margin */
                    SenderDeliveryContentCursor += DeliveryStartIndexCursor % InSenderDelivery.Width;
                }
                else
                {
                    const int32 IndicesLeftToNextRow = InSenderDelivery.Width - (SenderDeliveryContentCursor % InSenderDelivery.Width);
                    SenderDeliveryContentCursor += IndicesLeftToNextRow;
                    /* Margin */
                    SenderDeliveryContentCursor += DeliveryStartIndexCursor % InSenderDelivery.Width;
                }

                continue;
            }

            if (SenderDeliveryContentCursor % InSenderDelivery.Width == BEGIN_OF_ROW)
            {
                if (RecipeContentCursor % InRecipe.Delivery.Width != BEGIN_OF_ROW)
                {
                    goto NextDeliveryIndex;
                }

                jcheckNoEntry()
                return false;
            }

            continue;
        }

        /* Mashallah this is the right recipe. */
        if (RecipeContentCursor >= InRecipe.Delivery.Contents.Num())
        {
            return true;
        }

        NextDeliveryIndex:
            ++DeliveryStartIndexCursor;
            continue;
    }

    return false;
#undef BEGIN_OF_ROW
}

UGameRecipeSubsystem::UGameRecipeSubsystem(void)
{
    return;
}

void UGameRecipeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<UVoxelSubsystem>();
    Super::Initialize(Collection);

    LOG_VERBOSE(LogRecipeSystem, "Called.")

    this->ReloadLocalRecipes();

    LOG_VERBOSE(LogRecipeSystem, "Finished loading recipes. Found %d recipes.", this->Recipes.Num())

    return;
}

void UGameRecipeSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
}

void UGameRecipeSubsystem::ParseRecipe(const IPlugin* InPlugin, const FString& RecipeName)
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

void UGameRecipeSubsystem::ParseRecipe(const FString& RecipeNamespace, const FString& RecipeName, const TSharedPtr<FJsonObject>& Obj)
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

    FRecipe Recipe = FRecipe(RecipeNamespace, RecipeName);

    FString Error;
    if (this->ParseRecipeDelivery(Obj, Recipe.Delivery, Error) == false)
    {
        LOG_RELAXED_FATAL(LogRecipeSystem, "Failed to parse delivery for recipe [%s::%s]. Error: %s", *RecipeNamespace, *RecipeName, *Error)
        return;
    }

    if (Recipe.IsShapeless())
    {
        this->SimplifyDelivery_Shapeless(Recipe.Delivery);
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

bool UGameRecipeSubsystem::ParseRecipeDelivery(const TSharedPtr<FJsonObject>& Obj, FRecipeDelivery& OutDelivery, FString& OutError) const
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

bool UGameRecipeSubsystem::ParseRecipeProduct(const TSharedPtr<FJsonObject>& Obj, FRecipeProduct& OutProduct, FString& OutError) const
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
        if (Accumulated.IsNull())
        {
            OutError = FString::Printf(TEXT("Semantic error: Accumulated [%s] not found."), *Obj->GetObjectField(RecipeJsonTranslation::Product)->GetStringField(RecipeJsonTranslation::ProductContent));
            return false;
        }
        OutProduct.Product.AccumulatedIndex = Accumulated.AccumulatedIndex;

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

// ReSharper disable once CppMemberFunctionMayBeStatic
void UGameRecipeSubsystem::SimplifyDelivery_Shapeless(FRecipeDelivery& InOutDelivery) const
{
    TArray<FAccumulated> SimplifiedDelivery;
    for (const FAccumulated& Accumulated : InOutDelivery.Contents)
    {
        if (Accumulated.IsNull())
        {
            jcheck( false && "Null found in a shapeless delivery." )
            continue;
        }

        if (SimplifiedDelivery.Contains(Accumulated))
        {
            ++SimplifiedDelivery[SimplifiedDelivery.Find(Accumulated)].Amount;
            continue;
        }

        check( Accumulated.Amount == 1 )
        SimplifiedDelivery.Emplace(Accumulated);
        check( SimplifiedDelivery.Last().Amount == 1 )

        continue;
    }

    InOutDelivery.Contents = SimplifiedDelivery;

    return;
}
