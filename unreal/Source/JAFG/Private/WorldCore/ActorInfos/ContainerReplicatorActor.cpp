// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/ActorInfos/ContainerReplicatorActor.h"
#include "Container.h"

class JAFG_API FUnownedContainer final : public INoRepContainer
{
    virtual bool EasyAddToContainer(const FAccumulated& Value) override;
    virtual bool EasyChangeContainer(const int32 Index, const accamount_t_signed Amount) override;

    virtual void PushContainerUpdatesToClient(void) override;
    virtual void PushContainerUpdatesToServer(void) override;
};

bool FUnownedContainer::EasyAddToContainer(const FAccumulated& Value)
{
    return false;
}

bool FUnownedContainer::EasyChangeContainer(const int32 Index, const accamount_t_signed Amount)
{
    return false;
}

void FUnownedContainer::PushContainerUpdatesToClient(void)
{
    return;
}

void FUnownedContainer::PushContainerUpdatesToServer(void)
{
    return;
}

AContainerReplicatorActor::AContainerReplicatorActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;
}

void AContainerReplicatorActor::CreateNewContainer(
    const FJCoordinate& Identifier,
    const TFunction<void(IContainer* const Container)>& OnLongExecQueryFinished
) const
{
    INoRepContainer* Container = new FUnownedContainer();
    Container->ResetContainerData(10);

    OnLongExecQueryFinished(Container);

    return;
}

UContainerReplicatorComponent::UContainerReplicatorComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryComponentTick.bCanEverTick = true;
}

void UContainerReplicatorComponent::AddContainer(const FJCoordinate& WorldKey, const TArray<FSlot>& Slots)
{
    INoRepContainer* Container = new FUnownedContainer();
    Container->ResetContainerData(Slots);

    this->Containers.Emplace(WorldKey, Container);

    return;
}
