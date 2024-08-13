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

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnContainerChangedDelegateSignature, const ELocalContainerChange::Type InReason, const int32 InIndex)

DECLARE_DELEGATE_OneParam(FOnPushContainerUpdateToClientDelegateSignature, const int32 InIndex)
DECLARE_DELEGATE_TwoParams(FOnPushContainerUpdateToServerDelegateSignature, const int32 InIndex, const ELocalContainerChange::Type InReason)

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

namespace ELocalContainerChange
{

/**
 * @return True if this action can be performed by a client connection. All other actions / change reasons can only
 *         be executed by the authority.
 */
FORCEINLINE auto IsValidClientAction(const ELocalContainerChange::Type InType) -> bool
{
    return InType == ELocalContainerChange::Primary || InType == ELocalContainerChange::Secondary;
}

}

FORCEINLINE auto LexToString(const ELocalContainerChange::Type InType) -> FString
{
    switch (InType)
    {
        case ELocalContainerChange::Invalid:
        {
            return TEXT("Invalid");
        }
        case ELocalContainerChange::Replicated:
        {
            return TEXT("Replicated");
        }
        case ELocalContainerChange::Primary:
        {
            return TEXT("Primary");
        }
        case ELocalContainerChange::Secondary:
        {
            return TEXT("Secondary");
        }
        case ELocalContainerChange::Custom:
        {
            return TEXT("Custom");
        }
        default:
        {
            jrelaxedCheckNoEntry()
            return TEXT("Unknown");
        }
    }
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

    //////////////////////////////////////////////////////////////////////////
    // These methods should directly modify the source data. And never handle UI or replication updates.
    //////////////////////////////////////////////////////////////////////////

    FORCEINLINE virtual auto IsContainerInitialized(void) const -> bool                 = 0;
    FORCEINLINE virtual auto GetContainerSize(void) const -> int32                      = 0;
    FORCEINLINE virtual auto GetContainer(void) -> TArray<FSlot>&                       = 0;
    FORCEINLINE virtual auto GetContainer(void) const -> const TArray<FSlot>&           = 0;
    FORCEINLINE virtual auto GetContainer(const int32 Index) -> FSlot&                  = 0;
    FORCEINLINE virtual auto GetContainer(const int32 Index) const -> const FSlot&      = 0;
    FORCEINLINE virtual auto GetContainerValue(const int32 Index) const -> FAccumulated = 0;
    FORCEINLINE virtual auto GetContainerValueRef(const int32 Index) -> FAccumulated&   = 0;
    FORCEINLINE virtual auto AddToContainer(const FAccumulated& Value) -> bool          = 0;

    //////////////////////////////////////////////////////////////////////////
    // These methods should allow for network replication.
    //////////////////////////////////////////////////////////////////////////

    FORCEINLINE virtual auto EasyAddToContainer(const FAccumulated& Value) -> bool = 0;
    /** @return True if container was changed at the provided index and side effects were called. */
    FORCEINLINE virtual auto EasyChangeContainer(
        const int32 InIndex,
        const accamount_t_signed InAmount,
        const ELocalContainerChange::Type InReason = ELocalContainerChange::Custom
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
        const ELocalContainerChange::Type InReason = ELocalContainerChange::Replicated
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
    }

    FORCEINLINE virtual auto ToString_Container(void) const -> FString = 0;

    FOnContainerChangedDelegateSignature OnContainerChangedDelegate;
};

namespace ELocalContainerChange
{

FORCEINLINE auto ToFunction(const ELocalContainerChange::Type InType)
    -> TFunction<bool(
        const int32 InLambdaIndex,
        IContainer* InLambdaTarget,
        IContainerOwner* InLambdaOwner
    )>
{
    switch (InType)
    {
    case ELocalContainerChange::Primary:
    {
        return [] (const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner)
        {
            return InLambdaTarget->GetContainer(InLambdaIndex).OnPrimaryClicked(InLambdaOwner);
        };
    }
    default:
    {
        LOG_WARNING(LogContainerStuff, "No function for change type %s. Returning empty lambda.", *LexToString(InType))
        return [] (const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner) -> bool
        {
            return false;
        };
    }
    }
}

}

/**
 * A container that does not use the unreal's push model replication subsystem but instead relies on a more
 * manuel approach. E.g.: UActorComponents, AActors (tough should be avoided due to additional costs).
 * Very useful if this container has no real owner and no bounds that could be used for priority replication
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

    FORCEINLINE auto OnPushContainerUpdateToClient(void) -> FOnPushContainerUpdateToClientDelegateSignature&
    {
        return this->OnPushContainerUpdateToClientDelegate;
    }
    // ReSharper disable once CppMemberFunctionMayBeConst
    FORCEINLINE auto PushContainerUpdateToClient(const int32 InIndex) -> void
    {
        this->OnPushContainerUpdateToClientDelegate.Execute(InIndex);
    }

    FORCEINLINE auto OnPushContainerUpdateToServer(void) -> FOnPushContainerUpdateToServerDelegateSignature&
    {
        return this->OnPushContainerUpdateToServerDelegate;
    }
    // ReSharper disable once CppMemberFunctionMayBeConst
    FORCEINLINE auto PushContainerUpdateToServer(const int32 InIndex, const ELocalContainerChange::Type InReason) -> void
    {
        this->OnPushContainerUpdateToServerDelegate.Execute(InIndex, InReason);
    }

    FORCEINLINE virtual auto ResetContainerData(const TArray<FSlot>& NewData) -> void { this->Container = NewData; }
    FORCEINLINE virtual void ResetContainerData(const int32 NewSize) { this->Container.Empty(NewSize); this->Container.SetNum(NewSize); }

protected:

    TArray<FSlot> Container;

    FOnPushContainerUpdateToClientDelegateSignature OnPushContainerUpdateToClientDelegate;
    FOnPushContainerUpdateToServerDelegateSignature OnPushContainerUpdateToServerDelegate;
};

/**
 * Represents a player that can interact with a container.
 */
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
