// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/ActorInfos/ContainerReplicatorActor.h"
#include "Container.h"
#include "Player/WorldPlayerController.h"
#include "UI/WorldHUD.h"
#include "WorldCore/WorldCharacter.h"

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
        }

        this->OnContainerChangedDelegate.Broadcast(InReason, InIndex);

        this->PushContainerUpdateToClient(InIndex);

        return true;
    }

    if (InOwner->IsLocalContainerOwner() == false)
    {
        jrelaxedCheckNoEntry()
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
        this->OnContainerChangedDelegate.Broadcast(InReason, InIndex);

        this->PushContainerUpdateToServer(InIndex, InReason);

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
    FAccumulated& ValueRef = this->GetContainerValueRef(InIndex);
    if (FAccumulated::DeepEquals(ValueRef, InContent))
    {
        /* Happens if the server confirms the action that the client has performed and predicted successfully. */
        return false;
    }

    ValueRef = InContent;

    this->OnContainerChangedDelegate.Broadcast(InReason, InIndex);

    return true;
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
    Container->OnPushContainerUpdateToClient().BindLambda([this, Identifier] (const int32 Index) -> void
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

IContainerOwner* UContainerReplicatorComponent::AsContainerOwner(void) const
{
    return Cast<AWorldCharacter>(Cast<APlayerController>(this->GetOwner())->GetPawn())->AsContainerOwner();
}

void UContainerReplicatorComponent::AddContainer(const FJCoordinate& WorldKey, const TArray<FSlot>& Slots)
{
    INoRepContainer* Container = new FUnownedContainer();
    Container->OnContainerChangedDelegate.AddLambda([this] (const ELocalContainerChange::Type InReason, const int32 InIndex) -> void
    {
        if (InReason == ELocalContainerChange::Replicated)
        {
            this->GetWorld()->GetFirstPlayerController()->GetHUD<AWorldHUD>()->MarkCurrentContainerAsDirty();
        }
    });
    Container->OnPushContainerUpdateToServer().BindLambda( [this, WorldKey] (const int32 InIndex, const ELocalContainerChange::Type InReason)
    {
        this->PushClientContainerAction_ServerRPC(WorldKey, InIndex, InReason);
    });
    Container->ResetContainerData(Slots);

    this->Containers.Emplace(WorldKey, Container);

    return;
}
