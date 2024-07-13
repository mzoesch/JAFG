// Copyright 2024 mzoesch. All rights reserved.

#include "Components/ContainerReplicatorComponentBase.h"
#include "CommonNetworkStatics.h"
#include "Container.h"
#include "JAFGLogDefs.h"
#include "Kismet/GameplayStatics.h"

AContainerReplicatorActorBase::AContainerReplicatorActorBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = true;
    this->bNetLoadOnClient              = false;
    this->bReplicates                   = false;

    return;
}

void AContainerReplicatorActorBase::BeginPlay(void)
{
    Super::BeginPlay();

    this->Containers.Empty();
    this->SetActorTickInterval(1.0);

    return;
}

void AContainerReplicatorActorBase::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

IContainer* AContainerReplicatorActorBase::GetContainer(
    const FJCoordinate& Identifier,
    const TFunction<void(IContainer* const Container)>& OnLongExecQueryFinished
) const
{
    if (IContainer* const * const V = this->Containers.Find(Identifier))
    {
        return *V;
    }

    this->CreateNewContainer(
        Identifier,
        [this, Identifier, OnLongExecQueryFinished] (IContainer* Container)
        {
            if (this->Containers.Contains(Identifier))
            {
                LOG_FATAL(LogContainerStuff, "Container already exists in map.")
                return;
            }

            this->Containers.Emplace(Identifier, Container);

            OnLongExecQueryFinished(Container);

            return;
        }
    );

    return nullptr;
}

// bool AContainerReplicatorActorBase::PerformActionOnContainer(const FJCoordinate& InIdentifier, const int32 InIndex, IContainerOwner* InOwner, const ELocalContainerChange::Type InReason) const
// {
//     bool bChangedData;
//
//     IContainer** Value = this->Containers.Find(InIdentifier);
//     if (Value == nullptr)
//     {
//         LOG_WARNING(LogContainerStuff, "Container for identifier [%s] not found.", *InIdentifier.ToString())
//         return false;
//     }
//
//     switch (InReason)
//     {
//     case ELocalContainerChange::Primary:
//     {
//         bChangedData = (*Value)->GetContainer(InIndex).OnPrimaryClicked(InOwner);
//         break;
//     }
//     case ELocalContainerChange::Secondary:
//     {
//         bChangedData = (*Value)->GetContainer(InIndex).OnSecondaryClicked(InOwner);
//         break;
//     }
//     default:
//     {
//         jcheckNoEntry()
//         return false;
//     }
//     }
//
//     if (bChangedData == false)
//     {
//         return false;
//     }
//
//     if (InOwner->IsLocalContainerOwner())
//     {
//         jcheckNoEntry()
//     }
//
//
//     ... Check if this is correct ... We need to broadcast this to subscribers
//     (*Value)->OnContainerChangedDelegate.Broadcast(InReason, InIndex);
//
//
//     return true;
// }

void AContainerReplicatorActorBase::BroadcastUpdateToSubscribedClients(const FJCoordinate& Identifier, const int32 Index) const
{
    const IContainer* const * const Container = this->Containers.Find(Identifier);
#if !UE_BUILD_SHIPPING
    if (Container == nullptr)
    {
        LOG_FATAL(LogContainerStuff, "Container for identifier [%s] not found.", *Identifier.ToString())
        return;
    }
#endif /* !UE_BUILD_SHIPPING */

    for (const TObjectPtr<UContainerReplicatorComponentBase> Subscriber :  this->SubscribedComponents)
    {
        if (const FJCoordinate* SubbedContent = Subscriber->GetSubscribedContainers().Find(Identifier); SubbedContent)
        {
            Subscriber->UpdateSubbedContainer_ClientRPC(Identifier, Index, (*Container)->GetContainer()[Index].Content);
        }
    }

    return;
}

UContainerReplicatorComponentBase::UContainerReplicatorComponentBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryComponentTick.bCanEverTick = true;
    return;
}

void UContainerReplicatorComponentBase::BeginPlay(void)
{
    Super::BeginPlay();

    this->Containers.Empty();
    this->RequestedContainerQueue.Empty();

    if (UNetStatics::IsServer(this))
    {
        TArray<AActor*> FoundActors = TArray<AActor*>();
        UGameplayStatics::GetAllActorsOfClass(this, AContainerReplicatorActorBase::StaticClass(), FoundActors);

        if (FoundActors.Num() != 1)
        {
            LOG_FATAL(LogContainerStuff, "Found %d ContainerReplicatorActors. There should be exactly 1.", FoundActors.Num())
            return;
        }

        this->ContainerReplicatorActor = MakeWeakObjectPtr(Cast<AContainerReplicatorActorBase>(FoundActors[0]));

        return;
    }

    return;
}

void UContainerReplicatorComponentBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    this->ContainerReplicatorActor.Reset();
    return;
}

void UContainerReplicatorComponentBase::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    FPrivateRequestedContainerQueue Item;
    if (this->RequestedContainerQueue.Dequeue(Item) == false)
    {
        return;
    }

    /* If true, this is being executed on a server-like instance. */
    if (this->ContainerReplicatorActor.IsValid())
    {
        if (IContainer* InstantContainer = this->ContainerReplicatorActor->GetContainer(Item.WorldCoordinate, [Item] (IContainer* Container)
        {
            Item.OnReplicated(Container);
        }))
        {
            Item.OnReplicated(InstantContainer);
        }

        return;
    }

    /* This client is already subscribed to this container. */
    if (this->Containers.Contains(Item.WorldCoordinate))
    {
        Item.OnReplicated(this->Containers[Item.WorldCoordinate]);
        return;
    }

    if (this->CurrentPendingRequestedContainerSet.Contains(Item))
    {
        LOG_FATAL(LogContainerStuff, "Current Pending Requested Container set already contains the item: %s.", *Item.WorldCoordinate.ToString())
        return;
    }

    this->CurrentPendingRequestedContainerSet.Emplace(Item);
    this->RequestContainer_ServerRPC(Item.WorldCoordinate);

    return;
}

void UContainerReplicatorComponentBase::RequestContainerAsync(const FJCoordinate& WorldKey, const TFunction<void(IContainer* Container)>& OnReplicated)
{
    this->RequestedContainerQueue.Enqueue({ WorldKey, OnReplicated });
}

void UContainerReplicatorComponentBase::UnsubscribeContainer(IContainer* Container)
{
#if !UE_BUILD_SHIPPING
    if (UNetStatics::IsSafeClient(this) == false)
    {
        LOG_FATAL(LogContainerStuff, "This function should only be called on a client-like instance.")
        return;
    }

#if WITH_EDITOR
    if (this->SubscribedContainers.Num() > 0)
    {
        LOG_FATAL(LogContainerStuff, "Subbed containers count is counted on the client. Disallowed.")
        return;
    }
#endif /* WITH_EDITOR */
#endif /* !UE_BUILD_SHIPPING */

    const FJCoordinate* FoundContainerKey = this->Containers.FindKey(Container);
    if (FoundContainerKey == nullptr)
    {
        LOG_WARNING(LogContainerStuff, "Container not found in map. Cannot unsubscribe.")
        return;
    }

    const FJCoordinate FoundContainerKeyCopy = *FoundContainerKey;

    this->Containers.Remove(FoundContainerKeyCopy);

    this->UnsubscribeContainer_ServerRPC(FoundContainerKeyCopy);

    return;
}

bool UContainerReplicatorComponentBase::PushClientContainerAction_ServerRPC_Validate(const FIntVector& InWorldKey, const int32 InIndex, const ELocalContainerChange::Type InReason)
{
    if (ELocalContainerChange::IsValidClientAction(InReason) == false)
    {
        LOG_ERROR(LogContainerStuff, "Invalid client action [%s].", *LexToString(InReason))
        return false;
    }

    IContainer** Container = this->ContainerReplicatorActor->FindContainer(InWorldKey);
    if (Container == nullptr)
    {
        LOG_ERROR(LogContainerStuff, "Container for [%s] not found.", *InWorldKey.ToString())
        return false;
    }

    return (*Container)->EasyChangeContainer(
        InIndex,
        this->AsContainerOwner(),
        ELocalContainerChange::ToFunction(InReason),
        InReason
    );
}

void UContainerReplicatorComponentBase::PushClientContainerAction_ServerRPC_Implementation(const FIntVector& InWorldKey, const int32 InIndex, const ELocalContainerChange::Type InReason)
{
    /* Unused method body. This is intentional. See the appropriate validate method. */
}

void UContainerReplicatorComponentBase::UnsubscribeContainer_ServerRPC_Implementation(const FIntVector& WorldKey)
{
    if (this->SubscribedContainers.Contains(WorldKey) == false)
    {
        LOG_WARNING(LogContainerStuff, "Container for [%s] not found in subscribed containers.", *WorldKey.ToString())
    }
    else
    {
        this->SubscribedContainers.Remove(WorldKey);
    }

    if (this->SubscribedContainers.IsEmpty())
    {
        this->ContainerReplicatorActor->SubscribedComponents.Remove(this);
    }

    return;
}

void UContainerReplicatorComponentBase::UpdateSubbedContainer_ClientRPC_Implementation(const FIntVector& WorldKey, const int32 Index, const FAccumulated& Content)
{
    IContainer* const * const Container = this->Containers.Find(WorldKey);
    if (Container == nullptr)
    {
        LOG_WARNING(LogContainerStuff, "Container for [%s] not found. Requesting to unsubscribe.", *WorldKey.ToString())
        this->UnsubscribeContainer_ServerRPC(WorldKey);
        return;
    }

    (*Container)->EasyOverrideContainerOnCl(Index, Content, ELocalContainerChange::Replicated);

    return;
}

void UContainerReplicatorComponentBase::RequestContainer_ServerRPC_Implementation(const FJCoordinate& WorldKey)
{
#if !UE_BUILD_SHIPPING
    if (this->ContainerReplicatorActor.IsValid() == false)
    {
        LOG_FATAL(LogContainerStuff, "ContainerReplicatorActor is not valid.")
        return;
    }
#endif /* !UE_BUILD_SHIPPING */

    this->ContainerReplicatorActor->SubscribedComponents.Add(this);

    if (IContainer* LoadedContainer = this->ContainerReplicatorActor->GetContainer(
        WorldKey,
        [this, WorldKey] (IContainer* Container)
        {
            LOG_WARNING(LogContainerStuff, "Lazy loaded container for [%s].", *WorldKey.ToString())
            this->SubscribedContainers.Add(WorldKey);
            this->PushRequestedContainerData_ClientRPC(WorldKey, Container->GetContainer());
        }
    ); LoadedContainer)
    {
        LOG_WARNING(LogContainerStuff, "Loaded container for [%s].", *WorldKey.ToString())
        this->SubscribedContainers.Add(WorldKey);
        this->PushRequestedContainerData_ClientRPC(WorldKey, LoadedContainer->GetContainer());
    }

    return;
}

void UContainerReplicatorComponentBase::PushRequestedContainerData_ClientRPC_Implementation(const FJCoordinate& WorldKey, const TArray<FSlot>& ContainerSlots)
{
    this->AddContainer(WorldKey, ContainerSlots);

    for (const FPrivateRequestedContainerQueue& PendingContainer : this->CurrentPendingRequestedContainerSet)
    {
        if (PendingContainer.WorldCoordinate != WorldKey)
        {
            continue;
        }

        PendingContainer.OnReplicated(this->Containers[WorldKey]);

        this->CurrentPendingRequestedContainerSet.Remove(PendingContainer);

        return;
    }

    LOG_WARNING(LogContainerStuff, "Recveived container data for [%s] but could not find them in the map.", *WorldKey.ToString())

    return;
}
