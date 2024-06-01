// Copyright 2024 mzoesch. All rights reserved.

#include "HyperlaneComponent.h"

#include "CommonNetworkStatics.h"
#include "JAFGLogDefs.h"
#include "HyperlaneTransmitterSubsystem.h"
#include "HyperlaneWorker.h"
#include "WorldCore/JAFGWorldSubsystems.h"

UHyperlaneComponent::UHyperlaneComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryComponentTick.bCanEverTick = false;
}

void UHyperlaneComponent::BeginPlay(void)
{
    Super::BeginPlay();

    if (WorldStatics::IsInDevWorld(this))
    {
        /* We are in the development world. The chunk generation subsystems are not active here. */
        this->Deactivate();
        return;
    }

    if (UNetStatics::IsSafeServer(this))
    {
        this->HyperlaneIdentifier = FGuid::NewGuid().ToString();
        LOG_DISPLAY(LogHyperlane, "Created new Hyperlane Identifier for [%s]: [%s].", *this->GetOwner()->GetName(), *this->HyperlaneIdentifier)
    }

    /*
     * Must never exist on a character or some other AActor because of replication.
     * We only want this component to exist on the server and the client controlling it.
     */
    check( Cast<APlayerController>(this->GetOwner()) )

    if (UNetStatics::IsSafeClient(this))
    {
        this->RequestHyperlaneIdentifier_ServerRPC();
    }

    return;
}

void UHyperlaneComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (this->HyperlaneWorker != nullptr)
    {
        delete this->HyperlaneWorker;
        this->HyperlaneWorker = nullptr;
    }

    return;
}

bool UHyperlaneComponent::IsConnectedAndReady(void) const
{
    return this->HyperlaneWorker->IsConnectedAndReady();
}

void UHyperlaneComponent::SetHyperlaneWorkerAddress(const FClientAddress& InAddress)
{
    if (UNetStatics::IsServer(this) == false)
    {
        LOG_FATAL(LogHyperlane, "Called not on a server. This is disallowed.")
        return;
    }

    if (InAddress.IsEmpty())
    {
        LOG_FATAL(LogHyperlane, "Hyperlane Worker Address is empty.")
        return;
    }

    if (this->HyperlaneWorkerAddress.IsEmpty() == false)
    {
        LOG_FATAL(LogHyperlane, "Hyperlane Worker Address has already been set. Has: [%s]. Got: [%s].", *this->HyperlaneWorkerAddress, *InAddress)
        return;
    }

    this->HyperlaneWorkerAddress = InAddress;

    return;
}

FClientAddress UHyperlaneComponent::GetHyperlaneWorkerAddress() const
{
    if (UNetStatics::IsServer(this) == false)
    {
        LOG_FATAL(LogHyperlane, "Called not on a server. This is disallowed.")
        return L"";
    }

    return this->HyperlaneWorkerAddress;
}

void UHyperlaneComponent::SendChunkInitializationDataToClient(const FIntVector& ChunkKey, voxel_t* RawVoxels) const
{
    const UHyperlaneTransmitterSubsystem* Subsystem = this->GetWorld()->GetSubsystem<UHyperlaneTransmitterSubsystem>();
    check( Subsystem )

    TransmittableData::FChunkInitializationData Data = TransmittableData::FChunkInitializationData { ChunkKey, RawVoxels };
    Subsystem->SendChunkInitializationData(this, Data);

    return;
}

bool UHyperlaneComponent::AskServerForVoxelData_ServerRPC_Validate(const FChunkKey& ChunkKey)
{
    return this->MyAskServerForVoxelData_ServerRPC_Validate(ChunkKey);
}

void UHyperlaneComponent::AskServerForVoxelData_ServerRPC_Implementation(const FChunkKey& ChunkKey)
{
    this->MyAskServerForVoxelData_ServerRPC_Implementation(ChunkKey);
}

void UHyperlaneComponent::RequestHyperlaneIdentifier_ServerRPC_Implementation(void)
{
    this->ReplicateHyperlaneIdentifier_ClientRPC(this->HyperlaneIdentifier);
}

void UHyperlaneComponent::ReplicateHyperlaneIdentifier_ClientRPC_Implementation(const FString& Ident)
{
    if (Ident.IsEmpty())
    {
        LOG_FATAL(LogHyperlane, "Hyperlane Identifier is empty.")
        return;
    }

    LOG_VERBOSE(LogHyperlane, "Set Hyperlane Identifier on client to [%s].", *Ident)

    this->HyperlaneIdentifier = Ident;
    this->HyperlaneWorker     = new FHyperlaneWorker(this);

    return;
}
