// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/ActorInfos/ContainerReplicatorActor.h"
#include "Container.h"
#include "UI/WorldHUD.h"

class JAFG_API FUnownedContainer final : public INoRepContainer
{
    virtual auto EasyAddToContainer(const FAccumulated& Value) -> bool override;
    virtual auto EasyChangeContainer(
        const int32 InIndex,
        const accamount_t_signed InAmount,
        const ELocalContainerChange::Type Reason = ELocalContainerChange::Custom
    ) -> bool override;
    virtual auto EasyChangeContainer(
        const int32 InIndex,
        IContainerOwner* InOwner,
        const TFunctionRef<bool(const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner)>& Alternator,
        const ELocalContainerChange::Type InReason
    ) -> bool override;
    virtual auto EasyChangeContainerCl(
        const int32 InIndex,
        IContainerOwner* InOwner,
        const TFunctionRef<bool(const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner)>& Alternator,
        const ELocalContainerChange::Type InReason
    ) -> bool override;
    virtual auto EasyOverrideContainerOnCl(
        const int32 InIndex,
        const FAccumulated& InContent,
        const ELocalContainerChange::Type InReason = ELocalContainerChange::Replicated
    ) -> bool override;

    virtual auto PushContainerUpdatesToServer(void) -> void override;
};

bool FUnownedContainer::EasyAddToContainer(const FAccumulated& Value)
{
    LOG_WARNING(LogTemp, "Not implemented yet.")
    return false;
}

bool FUnownedContainer::EasyChangeContainer(
    const int32 InIndex,
    const accamount_t_signed InAmount,
    const ELocalContainerChange::Type Reason /* = ELocalContainerChange::Custom */
)
{
    LOG_WARNING(LogTemp, "Not implemented yet.")
    return false;
}

bool FUnownedContainer::EasyChangeContainer(
    const int32 InIndex,
    IContainerOwner* InOwner,
    const TFunctionRef<bool(const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner)>& Alternator,
    const ELocalContainerChange::Type InReason
)
{
    if (Alternator(InIndex, this, InOwner))
    {
        if (InOwner->IsLocalContainerOwner())
        {
            InOwner->OnCursorValueChangedDelegate.Broadcast();
            this->OnLocalContainerChangedEvent.Broadcast(InReason, InIndex);
        }

        this->PushContainerUpdatesToClient(InIndex);

        return true;
    }

    if (InOwner->IsLocalContainerOwner() == false)
    {
        LOG_ERROR(LogContainerStuff, "Enclosing block should never be called.")
    }

    return false;
}

bool FUnownedContainer::EasyChangeContainerCl(
    const int32 InIndex,
    IContainerOwner* InOwner,
    const TFunctionRef<bool(const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner)>& Alternator,
    const ELocalContainerChange::Type InReason
)
{
    if (Alternator(InIndex, this, InOwner))
    {
        InOwner->OnCursorValueChangedDelegate.Broadcast();
        this->OnLocalContainerChangedEvent.Broadcast(InReason, InIndex);

        // TODO Push container updates ? or this this the OnLocalContainerChangedEvent

        return true;
    }

    return false;
}

bool FUnownedContainer::EasyOverrideContainerOnCl(
    const int32 InIndex,
    const FAccumulated& InContent,
    const ELocalContainerChange::Type InReason /* = ELocalContainerChange::Replicated */
)
{
    this->GetContainerValueRef(InIndex) = InContent;

    this->OnLocalContainerChangedEvent.Broadcast(InReason, InIndex);

    return true;
}

void FUnownedContainer::PushContainerUpdatesToServer(void)
{
    LOG_WARNING(LogTemp, "Not implemented yet.")
    return;
}

AContainerReplicatorActor::AContainerReplicatorActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = true;
}

void AContainerReplicatorActor::CreateNewContainer(
    const FJCoordinate& Identifier,
    const TFunction<void(IContainer* const Container)>& OnLongExecQueryFinished
) const
{
    INoRepContainer* Container = new FUnownedContainer();
    Container->OnPushUpdatesToClient().BindLambda([this, Identifier] (const int32 Index) -> void
    {
        this->BroadcastUpdateToSubscribedClients(Identifier, Index);
    });
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
    Container->OnLocalContainerChangedEvent.AddLambda([this] (const ELocalContainerChange::Type InReason, const int32 InIndex) -> void
    {
        if (InReason == ELocalContainerChange::Replicated)
        {
            this->GetWorld()->GetFirstPlayerController()->GetHUD<AWorldHUD>()->MarkCurrentContainerAsDirty();
        }
    });
    Container->ResetContainerData(Slots);

    this->Containers.Emplace(WorldKey, Container);

    return;
}
