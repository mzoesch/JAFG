// Copyright 2024 mzoesch. All rights reserved.

#include "System/HyperlaneComponent.h"

#include "Network/NetworkStatics.h"
#include "System/HyperlaneWorker.h"

UHyperlaneComponent::UHyperlaneComponent(const FObjectInitializer& ObjectInitializer)
{
    this->PrimaryComponentTick.bCanEverTick = false;
}

UHyperlaneComponent::~UHyperlaneComponent()
{
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

void UHyperlaneComponent::BeginPlay(void)
{
    Super::BeginPlay();

    if (UNetworkStatics::IsSafeClient(this) == false)
    {
        UE_LOG(LogTemp, Log, TEXT("HyperlaneComponent::BeginPlay: No replication needed on a non client instance. Discarding hyperlane creation."))
        return;
    }

    this->Worker = new FHyperlaneWorker();

    return;
}

void UHyperlaneComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
