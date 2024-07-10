// Copyright 2024 mzoesch. All rights reserved.

#include "Components/ContainerReplicatorComponentBase.h"

#include "CommonNetworkStatics.h"
#include "Container.h"
#include "JAFGLogDefs.h"
#include "Kismet/GameplayStatics.h"

AContainerReplicatorActorBase::AContainerReplicatorActorBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;
    this->bNetLoadOnClient              = false;
    this->bReplicates                   = false;

    return;
}

void AContainerReplicatorActorBase::BeginPlay(void)
{
    Super::BeginPlay();

    this->Containers.Empty();

    return;
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

void UContainerReplicatorComponentBase::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (FPrivateRequestedContainerQueue Item; this->RequestedContainerQueue.Dequeue(Item))
    {
        if (this->CurrentPendingRequestedContainerSet.Contains(Item))
        {
            LOG_FATAL(LogContainerStuff, "Current Pending Requested Container set already contains the item: %s.", *Item.WorldCoordinate.ToString())
            return;
        }

        this->CurrentPendingRequestedContainerSet.Emplace(Item);
        this->RequestContainer_ServerRPC(Item.WorldCoordinate);
    }

    return;
}

void UContainerReplicatorComponentBase::RequestContainerAsync(const FJCoordinate& WorldKey, const TFunction<void(IContainer* Container)>& OnReplicated)
{
    this->RequestedContainerQueue.Enqueue({ WorldKey, OnReplicated });
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

    if (IContainer* LoadedContainer = this->ContainerReplicatorActor->GetContainer(
        WorldKey,
        [this, WorldKey] (IContainer* Container)
        {
            this->RequestedContainerData_ClientRPC(WorldKey, Container->GetContainer());
        }
    ); LoadedContainer)
    {
        this->RequestedContainerData_ClientRPC(WorldKey, LoadedContainer->GetContainer());
    }

    return;
}

void UContainerReplicatorComponentBase::RequestedContainerData_ClientRPC_Implementation(const FJCoordinate& WorldKey, const TArray<FSlot>& ContainerSlots)
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
