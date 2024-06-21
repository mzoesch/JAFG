// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Slot.h"
#include "Accumulated.h"

#include "Container.generated.h"

DECLARE_DELEGATE(PrivateOnCursorValueChangedSignature)
DECLARE_DELEGATE(PrivateOnContainerChangedSignature)

DECLARE_MULTICAST_DELEGATE(FOnContainerChangedSignature)

UINTERFACE()
class JAFGEXTERNALCORE_API UContainer : public UInterface
{
    GENERATED_BODY()
};

class JAFGEXTERNALCORE_API IContainer
{
    GENERATED_BODY()

public:

    FORCEINLINE virtual auto GetContainerSize(void) const -> int32 { return this->Container.Num(); }
    FORCEINLINE virtual auto GetContainer(void) -> TArray<FSlot>& { return this->Container; }
    FORCEINLINE virtual auto GetContainer(void) const -> const TArray<FSlot>& { return this->Container; }
    FORCEINLINE virtual auto GetContainer(const int32 Index) -> FSlot& { return this->Container[Index]; }
    FORCEINLINE virtual auto GetContainer(const int32 Index) const -> const FSlot& { return this->Container[Index]; }
    FORCEINLINE virtual auto GetContainerValue(const int32 Index) const -> FAccumulated { return this->Container[Index].Content; }
    FORCEINLINE virtual auto GetContainerValueRef(const int32 Index) -> FAccumulated& { return this->Container[Index].Content; }
    FORCEINLINE virtual auto AddToContainer(const FAccumulated& Value) -> bool { return FSlot::AddToFirstSuitableSlot(this->Container, Value); }

    virtual void PushContainerUpdatesToClient(void) = 0;
    virtual void PushContainerUpdatesToServer(void) = 0;

    /** Only locally called. Meaning only there, where a HUD / OSD is displaying information about this container. */
    FOnContainerChangedSignature OnLocalContainerChangedEvent;

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

    /** Has to be bound by the implementer to allow for replication. As it is not allowed in an interface. */
    PrivateOnCursorValueChangedSignature OnCursorValueChangedEvent;
};
