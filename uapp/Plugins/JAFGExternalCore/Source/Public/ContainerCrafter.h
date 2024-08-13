// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Container.h"
#include "System/RecipeSubsystem.h"

#include "ContainerCrafter.generated.h"

/**
 * A fully working container interface system that allows for crafting. Heavily relies on the implementor to handle
 * replication and the definitions of methods and declaration of necessary variables. This is the most basic form
 * of a container interface.
 * @note *Must* be used with a push model implementation.
 */
UINTERFACE()
class JAFGEXTERNALCORE_API UContainerCrafter : public UInterface
{
    GENERATED_BODY()
};

class JAFGEXTERNALCORE_API IContainerCrafter : public IContainer
{
    GENERATED_BODY()

public:

    //////////////////////////////////////////////////////////////////////////
    // These methods should directly modify the source data. And never handle UI or replication updates.
    //////////////////////////////////////////////////////////////////////////

    FORCEINLINE virtual auto IsContainerCrafterInitialized(void) const -> bool                 = 0;
    FORCEINLINE virtual auto GetContainerCrafterSize(void) const -> int32                      = 0;
    FORCEINLINE virtual auto GetContainerCrafter(void) -> TArray<FSlot>&                       = 0;
    FORCEINLINE virtual auto GetContainerCrafter(void) const -> const TArray<FSlot>&           = 0;
    FORCEINLINE virtual auto GetContainerCrafter(const int32 Index) -> FSlot&                  = 0;
    FORCEINLINE virtual auto GetContainerCrafter(const int32 Index) const -> const FSlot&      = 0;
    FORCEINLINE virtual auto GetContainerCrafterValue(const int32 Index) const -> FAccumulated = 0;
    FORCEINLINE virtual auto GetContainerCrafterValueRef(const int32 Index) -> FAccumulated&   = 0;
    FORCEINLINE virtual auto AddToContainerCrafter(const FAccumulated& Value) -> bool          = 0;

    FORCEINLINE virtual auto GetContainerCrafterContents(void) const -> const TArray<FAccumulated>                             = 0;
    FORCEINLINE virtual auto GetContainerCrafterContents(const accamount_t AmountOverride) const -> const TArray<FAccumulated> = 0;
    FORCEINLINE virtual auto GetContainerCrafterWidth(void) const -> sender_shape_width                                        = 0;
    FORCEINLINE virtual auto GetContainerCrafterAsDelivery(void) const -> FSenderDeliver                                       = 0;
    FORCEINLINE virtual auto GetContainerCrafterProduct(void) const -> FRecipeProduct                                          = 0;

    //////////////////////////////////////////////////////////////////////////
    // These methods should allow for network replication.
    //////////////////////////////////////////////////////////////////////////

    FORCEINLINE virtual auto EasyAddToContainerCrafter(const FAccumulated& Value) -> bool = 0;
    FORCEINLINE virtual auto EasyChangeContainerCrafter(
        const int32 InIndex,
        const accamount_t_signed InAmount,
        const ELocalContainerChange::Type Reason = ELocalContainerChange::Custom
    ) -> bool = 0;
    FORCEINLINE virtual auto EasyChangeContainerCrafter(
        const int32 InIndex,
        IContainerOwner* InOwner,
        const TFunctionRef<bool(const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner)>& Alternator,
        const ELocalContainerChange::Type InReason
    ) -> bool = 0;
    FORCEINLINE virtual auto EasyChangeContainerCrafterCl(
        const int32 InIndex,
        IContainerOwner* InOwner,
        const TFunctionRef<bool(const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner)>& Alternator,
        const ELocalContainerChange::Type InReason
    ) -> bool = 0;
    FORCEINLINE virtual auto EasyOverrideContainerCrafterOnCl(
        const int32 InIndex,
        const FAccumulated& InContent,
        const ELocalContainerChange::Type InReason
    ) -> bool = 0;
    FORCEINLINE virtual auto EasyChangeContainerCrafterSoftPredict(
        const UObject* Context,
        IContainerOwner* InOwner,
        const int32 InIndex,
        const TFunctionRef<bool(const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner)>& Alternator,
        const ELocalContainerChange::Type InReason
    ) -> bool
    {
        if (UNetStatics::IsSafeClient(Context))
        {
            return this->EasyChangeContainerCrafterCl(InIndex, InOwner, Alternator, InReason);
        }

        return this->EasyChangeContainerCrafter(InIndex, InOwner, Alternator, InReason);
    }

    virtual auto ToString_ContainerCrafter(void) const -> FString = 0;

private:

    //////////////////////////////////////////////////////////////////////////
    // Parent redirects.
    //////////////////////////////////////////////////////////////////////////

    FORCEINLINE virtual auto IsContainerInitialized(void) const -> bool override final { return this->IsContainerCrafterInitialized(); }
    FORCEINLINE virtual auto GetContainerSize(void) const -> int32 override final { return this->GetContainerCrafterSize(); }
    FORCEINLINE virtual auto GetContainer(void) -> TArray<FSlot>& override final { return this->GetContainerCrafter(); }
    FORCEINLINE virtual auto GetContainer(void) const -> const TArray<FSlot>& override final { return this->GetContainerCrafter(); }
    FORCEINLINE virtual auto GetContainer(const int32 Index) -> FSlot& override final { return this->GetContainerCrafter(Index); }
    FORCEINLINE virtual auto GetContainer(const int32 Index) const -> const FSlot& override final { return this->GetContainerCrafter(Index); }
    FORCEINLINE virtual auto GetContainerValue(const int32 Index) const -> FAccumulated override final { return this->GetContainerCrafterValue(Index); }
    FORCEINLINE virtual auto GetContainerValueRef(const int32 Index) -> FAccumulated& override final { return this->GetContainerCrafterValueRef(Index); }
    FORCEINLINE virtual auto AddToContainer(const FAccumulated& Value) -> bool override final { return this->AddToContainerCrafter(Value); }

    FORCEINLINE virtual auto EasyAddToContainer(const FAccumulated& Value) -> bool override final
    {
        return this->EasyAddToContainerCrafter(Value);
    }
    FORCEINLINE virtual auto EasyChangeContainer(
        const int32 InIndex,
        const accamount_t_signed InAmount,
        const ELocalContainerChange::Type InReason
    ) -> bool override final
    {
        return this->EasyChangeContainerCrafter(InIndex, InAmount, InReason);
    }
    FORCEINLINE virtual auto EasyChangeContainer(
        const int32 InIndex,
        IContainerOwner* InOwner,
        const TFunctionRef<bool(const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner)>& Alternator,
        const ELocalContainerChange::Type InReason
    ) -> bool override final
    {
        return this->EasyChangeContainerCrafter(InIndex, InOwner, Alternator, InReason);
    }

    FORCEINLINE virtual auto EasyChangeContainerCl(
        const int32 InIndex,
        IContainerOwner* InOwner,
        const TFunctionRef<bool(const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner)>& Alternator,
        const ELocalContainerChange::Type InReason
    ) -> bool override final
    {
        return this->EasyChangeContainerCrafterCl(InIndex, InOwner, Alternator, InReason);
    }
    FORCEINLINE virtual auto EasyOverrideContainerOnCl(
        const int32 InIndex,
        const FAccumulated& InContent,
        const ELocalContainerChange::Type InReason
    ) -> bool override final
    {
        return this->EasyOverrideContainerCrafterOnCl(InIndex, InContent, InReason);
    }
    FORCEINLINE virtual auto EasyChangeContainerSoftPredict(
        const UObject* Context,
        IContainerOwner* InOwner,
        const int32 InIndex,
        const TFunctionRef<bool(const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner)>& Alternator,
        const ELocalContainerChange::Type InReason
    ) -> bool override final
    {
        return this->EasyChangeContainerCrafterSoftPredict(Context, InOwner, InIndex, Alternator, InReason);
    }

    virtual auto ToString_Container(void) const -> FString override final { return this->ToString_ContainerCrafter(); }
};
