// Copyright 2024 mzoesch. All rights reserved.

#include "Network/BackgroundChunkUpdaterComponent.h"

#include "Kismet/GameplayStatics.h"
#include "World/WorldGeneratorInfo.h"

UBackgroundChunkUpdaterComponent::UBackgroundChunkUpdaterComponent(const FObjectInitializer& ObjectInitializer)
{
    PrimaryComponentTick.bCanEverTick = true;
    this->PrimaryComponentTick.bStartWithTickEnabled = true;
    this->PrimaryComponentTick.bAllowTickOnDedicatedServer = false;
    this->PrimaryComponentTick.TickInterval = 1.0f;

    this->ChunkInitializationQueue.Empty();
}

void UBackgroundChunkUpdaterComponent::BeginPlay(void)
{
    Super::BeginPlay();

    this->ChunkInitializationQueue.Empty();

    this->WorldGeneratorInfo = Cast<AWorldGeneratorInfo>(UGameplayStatics::GetActorOfClass(this, AWorldGeneratorInfo::StaticClass()));
    check( WorldGeneratorInfo )

}


void UBackgroundChunkUpdaterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (this->ChunkInitializationQueue.IsEmpty())
    {
        return;
    }

    while (this->ChunkInitializationQueue.IsEmpty() == false)
    {
        FIntVector ChunkKey;
        if (this->ChunkInitializationQueue.Dequeue(ChunkKey) == false)
        {
            UE_LOG(LogTemp, Fatal, TEXT("UBackgroundChunkUpdaterComponent::TickComponent: Could not dequeue chunk key."))
            return;
        }

        UE_LOG(LogTemp, Warning, TEXT("UBackgroundChunkUpdaterComponent::TickComponent: Dequeued chunk key %s."), *ChunkKey.ToString())

        this->AskServerToGenerateChunkForClient_ServerRPC(ChunkKey);
    }
}

void UBackgroundChunkUpdaterComponent::AskServerToGenerateChunkForClient_ServerRPC_Implementation(const FIntVector& ChunkKey)
{
    this->WorldGeneratorInfo->GenerateChunkForClient(ChunkKey, this);
}

