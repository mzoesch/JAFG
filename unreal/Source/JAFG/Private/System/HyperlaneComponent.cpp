// Copyright 2024 mzoesch. All rights reserved.

#include "System/HyperlaneComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Network/NetworkStatics.h"
#include "Network/HyperlaneWorker.h"
#include "World/WorldGeneratorInfo.h"

class AWorldGeneratorInfo;

UHyperlaneComponent::UHyperlaneComponent(const FObjectInitializer& ObjectInitializer)
{
    this->PrimaryComponentTick.bCanEverTick = false;
}

UHyperlaneComponent::~UHyperlaneComponent()
{
    return;
}

void UHyperlaneComponent::BeginPlay(void)
{
    Super::BeginPlay();

    if (UNetworkStatics::IsSafeClient(this) == false)
    {
        UE_LOG(LogTemp, Log, TEXT("HyperlaneComponent::BeginPlay: No replication needed on a non client instance. Discarding hyperlane creation."))
        return;
    }

    this->Worker = new FHyperlaneWorker(this);

    return;
}

void UHyperlaneComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (this->Worker)
    {
        if (this->GetWorld() != nullptr)
        {
            if (UNetworkStatics::IsSafeClient(this) == false)
            {
                UE_LOG(LogTemp, Fatal, TEXT("HyperlaneComponent::~UHyperlaneComponent: Hyperlane found on a non client instance. Somethin fishy is going on."))
            }
        }

        /*
         * The worker will kill itself. No need to worry about stale threads.
         */
        delete this->Worker;

        return;
    }

    if (this->GetWorld() != nullptr)
    {
        if (UNetworkStatics::IsSafeClient(this))
        {
            /*
             * We do not want to have memory leaks. It is better to just crash.
             */
            UE_LOG(LogTemp, Fatal, TEXT("HyperlaneComponent::~UHyperlaneComponent: Could not destry hyperlane."))
        }
    }

    return;
}

void UHyperlaneComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHyperlaneComponent::InitializeChunkWithAuthorityData(FIntVector ChunkKey, TArray<int32> Voxels)
{
    AsyncTask(ENamedThreads::GameThread, [this, ChunkKey, Voxels]()
    {
        AWorldGeneratorInfo* Info = Cast<AWorldGeneratorInfo>(UGameplayStatics::GetActorOfClass(this, AWorldGeneratorInfo::StaticClass()));
        check( Info )
        Info->InitializeChunkWithAuthorityData(ChunkKey, Voxels);

        return;
    });
}
