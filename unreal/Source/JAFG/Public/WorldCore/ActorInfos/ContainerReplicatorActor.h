// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Components/ContainerReplicatorComponentBase.h"

#include "ContainerReplicatorActor.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API AContainerReplicatorActor : public AContainerReplicatorActorBase
{
    GENERATED_BODY()

public:

    explicit AContainerReplicatorActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual auto CreateNewContainer(
        const FJCoordinate& Identifier,
        const TFunction<void(IContainer* const Container)>& OnLongExecQueryFinished
    ) const -> void override;
};

UCLASS(NotBlueprintable)
class JAFG_API UContainerReplicatorComponent : public UContainerReplicatorComponentBase
{
    GENERATED_BODY()

public:

    explicit UContainerReplicatorComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual auto AsContainerOwner(void) const -> IContainerOwner* override;

protected:

    // UContainerReplicatorComponentBase implementation
    virtual void AddContainer(const FJCoordinate& WorldKey, const TArray<FSlot>& Slots) override;
    //  ~UContainerReplicatorComponentBase implementation
};
