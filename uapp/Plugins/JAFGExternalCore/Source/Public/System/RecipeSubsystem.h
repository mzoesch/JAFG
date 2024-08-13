// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "JAFGExternalCoreIncludes.h"
#include "ExternalGameInstanceSubsystem.h"
#include "WorldCore/JAFGWorldSubsystems.h"

#include "RecipeSubsystem.generated.h"

class IPlugin;
class URecipeSubsystem;

/**
 * For the Delivery fields, the following JavaScript Object Notation (JSON) is valid.
 * Null values are discarded if the prescription is shapeless.
 * {
 *   // Single accumulated delivery
 *   "Delivery": "Name"
 *
 *   // Multiple accumulated delivery as shapeless crafting
 *   "Delivery": ["Name1", "Name2"]
 *
 *   // Multiple accumulated delivery as shaped crafting
 *   "Delivery":
 *   {
 *     "Contents": ["Name1", null, "Name2"],
 *     "Width": 2
 *   }
 * }
 *
 *
 * For the Product field the following JavaScript Object Notation (JSON) is valid.
 * {
 *   // Single accumulated product
 *   "Product": "Name"
 *
 *   // Single accumulated product with amount
 *   "Product":
 *   {
 *     "Content": "Name",
 *     "Amount": 1
 *   }
 * }
 *
 *
 * For the naming conventions the typical JAFG Accumulated naming conventions are used.
 * See VoxelSubsystem.h for more information.
 */
namespace RecipeJsonTranslation
{

inline static const FString Delivery         = TEXT("Delivery");
inline static const FString DeliveryContents = TEXT("Contents");
inline static const FString DeliveryWidth    = TEXT("Width");

inline static const FString Product          = TEXT("Product");
inline static const FString ProductContent   = TEXT("Content");
inline static const FString ProductAmount    = TEXT("Amount");

}

namespace ERecipeType
{

enum Type : uint8
{
    Invalid = 0,
    ShapedCrafting,
    ShapelessCrafting,
};

}

FORCEINLINE auto LexToString(const ERecipeType::Type InType) -> FString
{
    switch (InType)
    {
    case ERecipeType::ShapedCrafting:
    {
        return TEXT("ShapedCrafting");
    }
    case ERecipeType::ShapelessCrafting:
    {
        return TEXT("ShapelessCrafting");
    }
    default:
    {
        return TEXT("Invalid");
    }
    }
}

#define PRIVATE_SHAPELESS_WIDTH -1

// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
typedef int8  recipe_shape_width;
// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
typedef uint8 sender_shape_width;

/**
 * The delivery that comes from a container in-game and then gets compared to the FRecipeDelivery
 * for potential products.
 */
struct FSenderDeliver
{
    //////////////////////////////////////////////////////////////////////////
    // Constructors
    FORCEINLINE FSenderDeliver(void) = delete;

    FORCEINLINE explicit FSenderDeliver(const sender_shape_width InWidth)
        : Contents(), Width(InWidth) { return; }

    FORCEINLINE explicit FSenderDeliver(const FAccumulated& InContent)
        : Contents({ InContent }), Width(1) { return; }

    FORCEINLINE explicit FSenderDeliver(const TArray<FAccumulated>& InContent, const sender_shape_width InWidth)
        : Contents(InContent), Width(InWidth) { return; }

    FORCEINLINE explicit FSenderDeliver(const sender_shape_width InWidth, const TArray<FAccumulated>& InContent)
        : Contents(InContent), Width(InWidth) { return; }

    // ~Constructors
    //////////////////////////////////////////////////////////////////////////

    TArray<FAccumulated> Contents;
    sender_shape_width   Width;

    /** If the delivery has any content inside contents, that are not null. */
    bool IsEmpty(void) const;
};

/** The delivery that is known from a recipe. */
struct FRecipeDelivery
{
    //////////////////////////////////////////////////////////////////////////
    // Constructors
    FORCEINLINE explicit FRecipeDelivery(void)
        : Contents(), Width(PRIVATE_SHAPELESS_WIDTH) { return; }

    FORCEINLINE explicit FRecipeDelivery(const recipe_shape_width InWidth)
        : Contents(), Width(InWidth) { return; }

    FORCEINLINE explicit FRecipeDelivery(const FAccumulated& InContent)
        : Contents({ InContent }), Width(PRIVATE_SHAPELESS_WIDTH) { return; }

    FORCEINLINE explicit FRecipeDelivery(const FAccumulated& InContent, const recipe_shape_width InWidth)
        : Contents({ InContent }), Width(InWidth) { return; }

    FORCEINLINE explicit FRecipeDelivery(const TArray<FAccumulated>& InContents)
        : Contents(InContents), Width(PRIVATE_SHAPELESS_WIDTH) { return; }

    FORCEINLINE explicit FRecipeDelivery(const TArray<FAccumulated>& InContents, const recipe_shape_width InWidth)
        : Contents(InContents), Width(InWidth) { return; }
    // ~Constructors
    //////////////////////////////////////////////////////////////////////////

    TArray<FAccumulated> Contents;
    recipe_shape_width   Width;

    /** @return True, if Accumulated is null or if delivery contents has InAccumulated. */
    FORCEINLINE auto Contains(const FAccumulated& InAccumulated) const -> bool
    {
        return InAccumulated.IsNull() || this->Contents.Contains(InAccumulated);
    }
};

struct FRecipeProduct
{
    /*
     * Note, always use this struct when referring to the product of a recipe.
     * Today this might seem like a big overhead, but in some time this will save a lot of refactoring.
     * There is a very high chance that the product will be extended in the future.
     * E.g., the product might receive byproducts additional to the main product.
     */

    FAccumulated Product;

    /** Does not compare amount. */
    FORCEINLINE        auto operator==(const FRecipeProduct& O) const -> bool { return this->Product == O.Product; }
    /** Does not compare amount. */
    FORCEINLINE        auto operator!=(const FRecipeProduct& O) const -> bool { return !(*this == O); }
    /** Does not compare amount. */
    FORCEINLINE static auto Equals(const FRecipeProduct& A, const FRecipeProduct& B) -> bool { return A == B; }
    FORCEINLINE        auto IsNull(void) const -> bool { return this->Product.IsNull(); }
    FORCEINLINE        auto ToString(void) const -> FString { return FString::Printf(TEXT("%s"), *this->Product.ToString()); }
};

struct FRecipe
{
    //////////////////////////////////////////////////////////////////////////
    // Constructors
    FORCEINLINE explicit FRecipe(void)
        : Namespace(), Name(), Delivery(), Product() { return; }

    FORCEINLINE explicit FRecipe(const FString& InNamespace, const FString& InName)
        : Namespace(InNamespace), Name(InName), Delivery(), Product() { return; }
    // ~Constructors
    //////////////////////////////////////////////////////////////////////////

    FString         Namespace;
    FString         Name;
    FRecipeDelivery Delivery;
    FRecipeProduct  Product;

    FORCEINLINE auto GetType(void) const -> ERecipeType::Type
    {
        return this->Delivery.Width == PRIVATE_SHAPELESS_WIDTH
            ? ERecipeType::ShapelessCrafting
            : ERecipeType::ShapedCrafting;
    }

    FORCEINLINE auto IsShapeless(void) const -> bool
    {
        return this->GetType() == ERecipeType::ShapelessCrafting;
    }

    FORCEINLINE auto IsShaped(void) const -> bool
    {
        return this->GetType() == ERecipeType::ShapedCrafting;
    }

    /** @return True, if Accumulated is null or if delivery contents has InAccumulated. */
    FORCEINLINE auto DeliveryContains(const FAccumulated& InAccumulated) const -> bool
    {
        return this->Delivery.Contains(InAccumulated);
    }

    FORCEINLINE auto ToString(void) const -> FString
    {
        return FString::Printf(TEXT("{%s::%s,%s}"), *this->Namespace, *this->Name, *this->Product.ToString());
    }
};

namespace RecipeDelivery
{

static const FRecipeDelivery Null           = FRecipeDelivery();
static constexpr recipe_shape_width ShapelessWidth = PRIVATE_SHAPELESS_WIDTH;

}

namespace RecipeProduct
{

static const FRecipeProduct Null = FRecipeProduct(Accumulated::Null);

}

/**
 * Apart from initialization, only this subsystem should be used to access recipes.
 * The URecipeSubsystem holds all recipes that are available in the game, but a world has the power
 * to override these recipes.
 */
UCLASS(NotBlueprintable)
class JAFGEXTERNALCORE_API UWorldRecipeSubsystem : public UJAFGWorldSubsystem
{
    GENERATED_BODY()

public:

    virtual void OnWorldBeginPlay(UWorld& InWorld) override;

    /** The recipes from the base game with the modifications of the current opened world. */
    auto GetRecipes(void) -> const TArray<FRecipe>& { return this->CachedActiveRecipes; }

    auto GetRecipeSubsystem(const UWorld& InWorld) -> URecipeSubsystem*;
    auto GetRecipeSubsystem(void) const -> URecipeSubsystem*;

    /** @return True, if found. */
    auto GetRecipe(const FSenderDeliver& InSenderDelivery, FRecipe& OutRecipe) const -> bool;
    /** @return True, if found. */
    auto GetProduct(const FSenderDeliver& InSenderDelivery, FRecipeProduct& OutProduct) const -> bool;

protected:

    bool IsRecipeValidForDelivery(const FRecipe& InRecipe, const FSenderDeliver& InSenderDelivery) const;
    bool IsRecipeValidForDelivery_Shapeless(const FRecipe& InRecipe, const FSenderDeliver& InSenderDelivery, const bool bAllowNullInRecipe = false) const;
    bool IsRecipeValidForDelivery_Shaped(const FRecipe& InRecipe, const FSenderDeliver& InSenderDelivery) const;

    TArray<FRecipe> WorldRecipes;
    TArray<FRecipe> CachedActiveRecipes;
};

/**
 * Holds the recipes that are available from the base game. May be modified by any world. See UWorldRecipeSubsystem.
 */
UCLASS(Abstract, NotBlueprintable)
class JAFGEXTERNALCORE_API URecipeSubsystem : public UExternalGameInstanceSubsystem
{
    GENERATED_BODY()

    friend class UWorldRecipeSubsystem;

public:

    URecipeSubsystem();

    // Subsystem implementation
    virtual auto Initialize(FSubsystemCollectionBase& Collection) -> void override;
    virtual auto Deinitialize(void) -> void override;
    // ~Subsystem implementation

    /** The recipes that are available from the base game. */
    FORCEINLINE auto GetRecipes(void) const -> const TArray<FRecipe>& { return this->Recipes; }

protected:

    void ParseRecipe(const IPlugin* InPlugin, const FString& RecipeName);
    void ParseRecipe(const FString& RecipeNamespace, const FString& RecipeName, const TSharedPtr<FJsonObject>& Obj);

    /** @return True, if everything was parsed successfully. */
    bool ParseRecipeDelivery(const TSharedPtr<FJsonObject>& Obj, FRecipeDelivery& OutDelivery, FString& OutError) const;
    /** @return True, if everything was parsed successfully. */
    bool ParseRecipeProduct(const TSharedPtr<FJsonObject>& Obj, FRecipeProduct& OutProduct, FString& OutError) const;

    /**
     * Combines multiple accumulates into one to enable faster and easier search
     * while checking for matching recipes.
     */
    void SimplifyDelivery_Shapeless(FRecipeDelivery& InOutDelivery) const;

    virtual void ReloadLocalRecipes(void) PURE_VIRTUAL(URecipeSubsystem::ReLoadLocalRecipes)

    TArray<FRecipe> Recipes;
};

#undef DELIVERY_PRIVATE_SHAPELESS_WIDTH
