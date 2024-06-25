// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Slot.h"
#include "Accumulated.h"

#include "Container.generated.h"

namespace ELocalContainerChange { enum Type : uint8; }

DECLARE_MULTICAST_DELEGATE(OnCursorValueChangedDelegateSignature)
DECLARE_DELEGATE(PrivateOnContainerChangedSignature)

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnContainerChangedSignature, ELocalContainerChange::Type InReason, const int32 InIndex)

UENUM(NotBlueprintType)
namespace ELocalContainerChange
{

enum Type : uint8
{
    Invalid,
    Replicated,
    Primary,
    Secondary,
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

    /* These methods should directly modify the source data. And never handle UI or Replication updates. */

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
    FORCEINLINE virtual auto EasyChangeContainer(const int32 Index, const accamount_t_signed Amount) -> bool = 0;

    FORCEINLINE virtual auto ToString_Container(void) const -> FString = 0;

    /** Only for local "owners" called. Meaning never on a dedicated server. */
    FOnContainerChangedSignature OnLocalContainerChangedEvent;
};

UINTERFACE()
class JAFGEXTERNALCORE_API UNoRepContainer : public UContainer
{
    GENERATED_BODY()
};

class JAFGEXTERNALCORE_API INoRepContainer : public IContainer
{
    GENERATED_BODY()

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

    virtual void PushContainerUpdatesToClient(void) = 0;
    virtual void PushContainerUpdatesToServer(void) = 0;

protected:

    TArray<FSlot> Container;
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

    FAccumulated CursorValue;

    OnCursorValueChangedDelegateSignature OnCursorValueChangedDelegate;
};
