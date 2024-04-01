// Copyright 2024 mzoesch. All rights reserved.

#include "Network/BackgroundChunkUpdaterComponent.h"

#include "Network/NetworkStatics.h"
#include "Player/JAFGPlayerController.h"

UBackgroundChunkUpdaterComponent::UBackgroundChunkUpdaterComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryComponentTick.bCanEverTick = false;
}

void UBackgroundChunkUpdaterComponent::FillDataFromAuthorityAsync(ACommonChunk* TargetChunk)
{
    if (UNetworkStatics::IsSafeServer(this))
    {
        UE_LOG(LogTemp, Error, TEXT("UBackgroundChunkUpdaterComponent::FillDataFromAuthorityAsync: Called on a server instance. This is disallowed."))
        return;
    }

    if (TargetChunk == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UBackgroundChunkUpdaterComponent::FillDataFromAuthorityAsync: TargetChunk is nullptr."))
        return;
    }
    
    this->FillDataFromAuthority_ServerRPC(this->GetWorld()->GetFirstPlayerController(), TargetChunk);

    return;
}

void UBackgroundChunkUpdaterComponent::FillDataFromAuthority_ServerRPC_Implementation(APlayerController* TargetPlayerController, ACommonChunk* TargetChunk)
{
    UE_LOG(LogTemp, Warning, TEXT("UBackgroundChunkUpdaterComponent::FillDataFromAuthority_ServerRPC_Implementation: Called."))

    if (AJAFGPlayerController* Target = Cast<AJAFGPlayerController>(TargetPlayerController))
    {
        Target->SetInitialChunkDataForClientAsync(TargetChunk);
        return;
    }

    UE_LOG(LogTemp, Error, TEXT("UBackgroundChunkUpdaterComponent::FillDataFromAuthority_ServerRPC_Implementation: TargetPlayerController is not of type AJAFGPlayerController."))
    
    return;
}

bool UBackgroundChunkUpdaterComponent::FillDataFromAuthority_ServerRPC_Validate(APlayerController* TargetPlayerController, ACommonChunk* TargetChunk)
{
    if (TargetPlayerController == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UBackgroundChunkUpdaterComponent::FillDataFromAuthority_ServerRPC_Validate: TargetPlayerController is nullptr."))
        return false;
    }
    
    if (TargetChunk == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UBackgroundChunkUpdaterComponent::FillDataFromAuthority_ServerRPC_Validate: TargetChunk is nullptr."))
        return false;
    }

    return true;
}
