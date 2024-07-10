// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGTypeDefs.h"
#include "Slot.h"
#include "Components/ActorComponent.h"

#include "ContainerReplicatorComponentBase.generated.h"

class IContainer;
class UContainerReplicatorComponentBase;

struct FPrivateRequestedContainerQueue
{
    FJCoordinate                           WorldCoordinate;
    TFunction<void(IContainer* Container)> OnReplicated;

    FORCEINLINE auto operator==(const FPrivateRequestedContainerQueue& O) const -> bool
    {
        return this->WorldCoordinate == O.WorldCoordinate;
    }

    friend FORCEINLINE auto GetTypeHash(const FPrivateRequestedContainerQueue& O) -> uint32
    {
        return GetTypeHash(O.WorldCoordinate);
    }
};

/**
 * Server only AActor (no client netload) that holds all containers that are not owned by any controller.
 * Not meant to be accessed directly but through the abstraction layer provided by UContainerReplicatorComponent.
 *
 * This class holds all (memory) ownerships of the containers. Never assume a long lifetime of a container.
 */
UCLASS(Abstract, NotBlueprintable)
class JAFGEXTERNALCORE_API AContainerReplicatorActorBase : public AInfo
{
    GENERATED_BODY()

public:

    explicit AContainerReplicatorActorBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void BeginPlay(void) override;

    /**
     * @param Identifier              The identifier of the container.
     * @param OnLongExecQueryFinished Callback that is called when this "maybe" long-term query is finished.
     * @return If the container was directly found, nullptr otherwise.
     *
     * @note If the container exists and is returned, the callback delegate will not be called. Only if a long query
     *       is needed (e.g., a database query).
     */
    auto GetContainer(
        const FJCoordinate& Identifier,
        const TFunction<void(IContainer* const Container)>& OnLongExecQueryFinished
    ) const -> IContainer*;

protected:

    virtual auto CreateNewContainer(
        const FJCoordinate& Identifier,
        const TFunction<void(IContainer* const Container)>& OnLongExecQueryFinished
    ) const -> void PURE_VIRTUAL(AContainerReplicatorActorBase::CreateNewContainer)

private:

    mutable TMap<FJCoordinate, IContainer*> Containers;
};

/** Handles replication of containers that are not owned by any controller. */
UCLASS(NotBlueprintable)
class JAFGEXTERNALCORE_API UContainerReplicatorComponentBase : public UActorComponent
{
    GENERATED_BODY()

public:

    explicit UContainerReplicatorComponentBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

public:

    virtual void TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void RequestContainerAsync(const FJCoordinate& WorldKey, const TFunction<void(IContainer* Container)>& OnReplicated);

private:

    UFUNCTION(Server, Reliable)
    void RequestContainer_ServerRPC(const FIntVector /* FJCoordinate */ & WorldKey);

    UFUNCTION(Client, Reliable)
    void RequestedContainerData_ClientRPC(const FIntVector /* FJCoordinate */ & WorldKey, const TArray<FSlot>& ContainerSlots);

protected:

    virtual void AddContainer(const FJCoordinate& WorldKey, const TArray<FSlot>& Slots) PURE_VIRTUAL(UContainerReplicatorComponentBase::AddContainer)

    /** Meaningless on a server. */
    TMap<FJCoordinate, IContainer*> Containers;

    /** Meaningless on a server. */
    TSet<FPrivateRequestedContainerQueue> CurrentPendingRequestedContainerSet;

    /** Meaningless on a server. */
    TQueue<FPrivateRequestedContainerQueue> RequestedContainerQueue;

    UPROPERTY()
    TWeakObjectPtr<AContainerReplicatorActorBase> ContainerReplicatorActor;
};
