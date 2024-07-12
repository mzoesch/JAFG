// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Slot.h"
#include "Accumulated.h"
#include "CommonNetworkStatics.h"

#include "Container.generated.h"

class UContainerReplicatorComponentBase;

namespace ELocalContainerChange { enum Type : uint8; }

DECLARE_MULTICAST_DELEGATE(OnCursorValueChangedDelegateSignature)
DECLARE_DELEGATE(PrivateOnContainerChangedSignature)

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnContainerChangedSignature, ELocalContainerChange::Type InReason, const int32 InIndex)

DECLARE_DELEGATE_OneParam(FOnPushUpdatesToClientDelegateSignature, const int32 InIndex)

UENUM(NotBlueprintType)
namespace ELocalContainerChange
{

enum Type : uint8
{
    Invalid,
    Replicated,
    Primary,
    Secondary,
    Custom,
};

}

/**
 * A fully working container interface system. Heavily relies on the implementor to handle replication and the
 * definitions of methods and declaration of necessary variables. This is the most basic form of a container
 * interface.
 * @note *Must* be used with a push model implementation.
 */
UINTERFACE()
class JAFGEXTERNALCORE_API UContainer : public UInterface
{
    GENERATED_BODY()
};

class JAFGEXTERNALCORE_API IContainer
{
    GENERATED_BODY()

public:

    /* These methods should directly modify the source data. And never handle UI or replication updates. */

    FORCEINLINE virtual auto IsContainerInitialized(void) const -> bool                 = 0;
    FORCEINLINE virtual auto GetContainerSize(void) const -> int32                      = 0;
    FORCEINLINE virtual auto GetContainer(void) -> TArray<FSlot>&                       = 0;
    FORCEINLINE virtual auto GetContainer(void) const -> const TArray<FSlot>&           = 0;
    FORCEINLINE virtual auto GetContainer(const int32 Index) -> FSlot&                  = 0;
    FORCEINLINE virtual auto GetContainer(const int32 Index) const -> const FSlot&      = 0;
    FORCEINLINE virtual auto GetContainerValue(const int32 Index) const -> FAccumulated = 0;
    FORCEINLINE virtual auto GetContainerValueRef(const int32 Index) -> FAccumulated&   = 0;
    FORCEINLINE virtual auto AddToContainer(const FAccumulated& Value) -> bool          = 0;

    /* These methods should allow for network replication. */

    FORCEINLINE virtual auto EasyAddToContainer(const FAccumulated& Value) -> bool = 0;
    /** @return True if container was changed at the provided index and side effects were called. */
    FORCEINLINE virtual auto EasyChangeContainer(
        const int32 InIndex,
        const accamount_t_signed InAmount,
        const ELocalContainerChange::Type Reason = ELocalContainerChange::Custom
    ) -> bool = 0;
    /**
     * Should generally always call the same side effects as the IContainer::EasyChangeContainer(int32,
     * accamount_t_signed) to not generate confusion.
     * This method is useful if it is not known beforehand if or how the contents of the containers are going to
     * change.
     *
     * @param InIndex    The index to call the Alternator on.
     * @param InOwner    The owner to perform the alternation operation on.
     * @param Alternator The function to change the content at the specified index. Should return true if something
     *                   was changed.
     * @param InReason   The reason for the change.
     * @return True if container was changed at the index.
     */
    FORCEINLINE virtual auto EasyChangeContainer(
        const int32 InIndex,
        IContainerOwner* InOwner,
        const TFunctionRef<bool(const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner)>& Alternator,
        const ELocalContainerChange::Type InReason
    ) -> bool = 0;

    /** Client interpretation of IContainer::EasyChangeContainer. */
    FORCEINLINE virtual auto EasyChangeContainerCl(
        const int32 InIndex,
        IContainerOwner* InOwner,
        const TFunctionRef<bool(const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner)>& Alternator,
        const ELocalContainerChange::Type InReason
    ) -> bool = 0;
    /** @return True if container was changed at the provided index and side effects were called. */
    FORCEINLINE virtual auto EasyOverrideContainerOnCl(
        const int32 InIndex,
        const FAccumulated& InContent,
        const ELocalContainerChange::Type Reason = ELocalContainerChange::Replicated
    ) -> bool = 0;
    FORCEINLINE virtual auto EasyChangeContainerSoftPredict(
        const UObject* Context,
        IContainerOwner* InOwner,
        const int32 InIndex,
        const TFunctionRef<bool(const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner)>& Alternator,
        const ELocalContainerChange::Type InReason
    ) -> bool
    {
        if (UNetStatics::IsSafeClient(Context))
        {
            return this->EasyChangeContainerCl(InIndex, InOwner, Alternator, InReason);
        }

        return this->EasyChangeContainer(InIndex, InOwner, Alternator, InReason);
    };

    FORCEINLINE virtual auto ToString_Container(void) const -> FString = 0;

    /** Only for local "owners" called. Meaning never on a dedicated server. */
    /*
     * TODO We need to make a delegate here with the method that changes something?
     *      Properly not? We need to make a dynamic system though.
     */
    FOnContainerChangedSignature OnLocalContainerChangedEvent;
};

/**
 * A container that does not use the unreal's push model replication subsystem but instead relies on a more
 * manuel approach. E.g.: UActorComponents, AActors (tough should be avoided due to additional costs).
 * Very useful if this container has no real owner and no bounds which could be used for priority replication
 * consideration to clients.
 */
UINTERFACE()
class JAFGEXTERNALCORE_API UNoRepContainer : public UContainer
{
    GENERATED_BODY()
};

class JAFGEXTERNALCORE_API INoRepContainer : public IContainer
{
    GENERATED_BODY()

public:

    FORCEINLINE virtual auto IsContainerInitialized(void) const -> bool override { return this->Container.Num() > 0; }
    FORCEINLINE virtual auto GetContainerSize(void) const -> int32  override { return this->Container.Num(); }
    FORCEINLINE virtual auto GetContainer(void) -> TArray<FSlot>&  override { return this->Container; }
    FORCEINLINE virtual auto GetContainer(void) const -> const TArray<FSlot>&  override { return this->Container; }
    FORCEINLINE virtual auto GetContainer(const int32 Index) -> FSlot&  override { return this->Container[Index]; }
    FORCEINLINE virtual auto GetContainer(const int32 Index) const -> const FSlot&  override { return this->Container[Index]; }
    FORCEINLINE virtual auto GetContainerValue(const int32 Index) const -> FAccumulated  override { return this->Container[Index].Content; }
    FORCEINLINE virtual auto GetContainerValueRef(const int32 Index) -> FAccumulated&  override { return this->Container[Index].Content; }
    FORCEINLINE virtual auto AddToContainer(const FAccumulated& Value) -> bool  override { return FSlot::AddToFirstSuitableSlot(this->Container, Value); }

    FORCEINLINE virtual auto ToString_Container(void) const -> FString override
    {
        FString Result = TEXT("Container{");
        for (const FSlot& Slot : this->Container) { Result += Slot.Content.ToShortString() + TEXT(","); }
        Result += TEXT("}");
        return Result;
    }

    FORCEINLINE virtual auto OnPushUpdatesToClient(void) -> FOnPushUpdatesToClientDelegateSignature&
    {
        return this->OnPushUpdatesToClientDelegate;
    }
    FORCEINLINE virtual void PushContainerUpdatesToClient(const int32 InIndex)
    {
        this->OnPushUpdatesToClientDelegate.Execute(InIndex);
    }

    virtual void PushContainerUpdatesToServer(void) = 0;

    FORCEINLINE virtual auto ResetContainerData(const TArray<FSlot>& NewData) -> void { this->Container = NewData; }
    FORCEINLINE virtual void ResetContainerData(const int32 NewSize) { this->Container.Empty(NewSize); this->Container.SetNum(NewSize); }

protected:

    TArray<FSlot> Container;

    FOnPushUpdatesToClientDelegateSignature OnPushUpdatesToClientDelegate;
};

UINTERFACE()
class JAFGEXTERNALCORE_API UContainerOwner : public UInterface
{
    GENERATED_BODY()
};

class JAFGEXTERNALCORE_API IContainerOwner
{
    GENERATED_BODY()

public:

    FORCEINLINE virtual bool IsLocalContainerOwner(void) = 0;

    FAccumulated CursorValue;

    OnCursorValueChangedDelegateSignature OnCursorValueChangedDelegate;

    virtual auto GetContainerReplicatorComponent(void) const -> UContainerReplicatorComponentBase* = 0;
};
