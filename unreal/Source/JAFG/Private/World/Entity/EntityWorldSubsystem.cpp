// Copyright 2024 mzoesch. All rights reserved.

#include "World/Entity/EntityWorldSubsystem.h"

#include "Jar/Accumulated.h"
#include "Kismet/GameplayStatics.h"
#include "Network/NetworkStatics.h"
#include "World/Entity/Drop.h"

UEntityWorldSubsystem::UEntityWorldSubsystem(const FObjectInitializer& ObjectInitializer) /* : Super(ObjectInitializer) */
{
    /* The Object Initializer is not necessary for this class as it is not exposed to Kismet. */
    return;
}

bool UEntityWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    check( Outer )

    const bool bSuperCreate = Super::ShouldCreateSubsystem(Outer);

    LOG_VERBOSE(LogEntitySystem, "Called.")

    if (bSuperCreate == false)
    {
        return false;
    }

    if (Outer->GetWorld()->GetName() != TEXT("L_World"))
    {
        LOG_DISPLAY(LogEntitySystem, "Subsystem will not be created for world %s.", *Outer->GetWorld()->GetName())
        return false;
    }

    if (UNetworkStatics::IsSafeServer(Outer))
    {
        LOG_DISPLAY(LogEntitySystem, "Subsystem will be created.")
        return true;
    }

    LOG_DISPLAY(LogEntitySystem, "Subsystem will not be created on a non server UWorld.")

    return false;
}

void UEntityWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    LOG_VERBOSE(LogEntitySystem, "Called.")
}

void UEntityWorldSubsystem::PostInitialize(void)
{
    Super::PostInitialize();
    LOG_VERBOSE(LogEntitySystem, "Called.")
}

void UEntityWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);
    LOG_VERBOSE(LogEntitySystem, "Called.")
}

void UEntityWorldSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
    LOG_VERBOSE(LogEntitySystem, "Called.")
}

void UEntityWorldSubsystem::CreateDrop(const FAccumulated& Accumulated, const FVector& Location, const FVector& Force, const float ForceMultiplier) const
{
    if (UNetworkStatics::IsSafeServer(this) == false)
    {
        LOG_FATAL(LogEntitySystem, "Not called on a server. Disallowed.")
        return;
    }

    ADrop* Drop = this->GetWorld()->SpawnActorDeferred<ADrop>(ADrop::StaticClass(), FTransform(FRotator::ZeroRotator, Location, FVector::OneVector));

    // Drop->SetAccumulatedIndex(Accumulated.AccumulatedIndex);

    UGameplayStatics::FinishSpawningActor(Drop, FTransform(FRotator(0.0f, 0.0f, 180.0f), Location, FVector::OneVector));

    // Drop->AddForce(Force.GetSafeNormal() * ForceMultiplier);

    return;
}
