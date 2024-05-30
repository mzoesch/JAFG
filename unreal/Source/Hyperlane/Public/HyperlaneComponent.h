// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HyperlaneTypes.h"
#include "JAFGTypeDefs.h"

#include "HyperlaneComponent.generated.h"

namespace TransmittableData
{
struct FChunkInitializationData;
}

class FHyperlaneWorker;

UCLASS(Abstract, NotBlueprintable)
class HYPERLANE_API UHyperlaneComponent : public UActorComponent
{
    GENERATED_BODY()

    friend FHyperlaneWorker;

public:

    explicit UHyperlaneComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // UActorComponent implementation
    virtual void BeginPlay(void) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    // ~UActorComponent implementation

    FORCEINLINE auto IsLocalController(void) const -> bool { return Cast<APlayerController>(this->GetOwner())->IsLocalController(); }

    /** Called on the server if a Hyperlane Worker was found that matches this UHyperlaneComponent#HyperlaneIdentifier. */
    auto SetHyperlaneWorkerAddress(const FClientAddress& InAddress) -> void;
    auto GetHyperlaneWorkerAddress(void) const -> FClientAddress;

    FORCEINLINE auto GetHyperlaneIdentifier(void) const -> FString { return this->HyperlaneIdentifier; }

    UFUNCTION(Server, Reliable, WithValidation)
    void AskServerForVoxelData_ServerRPC(const FIntVector& ChunkKey);

protected:

    FORCEINLINE virtual bool MyAskServerForVoxelData_ServerRPC_Validate(const FChunkKey& ChunkKey)
        PURE_VIRTUAL(UHyperlaneComponent::MyAskServerForVoxelData_ServerRPC_Validate, return false;)
    FORCEINLINE virtual void MyAskServerForVoxelData_ServerRPC_Implementation(const FChunkKey& ChunkKey)
        PURE_VIRTUAL(UHyperlaneComponent::MyAskServerForVoxelData_ServerRPC_Implementation)
    virtual void OnChunkInitializationDataReceived(const TransmittableData::FChunkInitializationData& Data)
        PURE_VIRTUAL(UHyperlaneComponent::OnChunkInitializationDataReceived)

    void SendChunkInitializationDataToClient(const FIntVector& ChunkKey, voxel_t* RawVoxels) const;

private:

    /**
     * Server only.
     * The address of the hyperlane worker that this AActor Component is connected to.
     */
    FClientAddress  HyperlaneWorkerAddress = L"";
    /** Generated on server and replicated to the client on Post Login.  */
    FString         HyperlaneIdentifier    = L"";
    UFUNCTION(Client, Reliable)
    void ReplicateHyperlaneIdentifier_ClientRPC(const FString& Ident);
    UFUNCTION(Server, Reliable)
    void RequestHyperlaneIdentifier_ServerRPC();

    /**
     * The corresponding Worker that is associated with this component.
     * Therefore, this variable is only set on the locally controlled player.
     *
     * Note, a listen server character is locally controlled, but we will also never create a worker there. We will
     * just mimic the server's worker.
     */
    FHyperlaneWorker* HyperlaneWorker = nullptr;
};
