// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Entity/EntitySubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "WorldCore/Entity/Drop.h"

UEntitySubsystem::UEntitySubsystem() : Super()
{
    return;
}

void UEntitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

bool UEntitySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return UNetStatics::IsSafeServer(Outer);
}

void UEntitySubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);
}

void UEntitySubsystem::CreateDrop(const FAccumulated& InAccumulated, const FVector& InLocation, const FVector& InForce, const float ForceMultiplier)
{
    ADrop* Drop = this->GetWorld()->SpawnActorDeferred<ADrop>(ADrop::StaticClass(), FTransform(FRotator::ZeroRotator, InLocation, FVector::OneVector));
    Drop->SetAccumulatedForDeferredMeshGeneration(InAccumulated);
    UGameplayStatics::FinishSpawningActor(Drop, FTransform(FRotator::ZeroRotator, InLocation, FVector::OneVector));

    Drop->AddForce(InForce.GetSafeNormal() * ForceMultiplier);

    return;
}

void UEntitySubsystem::CreateDrop(const FAccumulated& InAccumulated, const FVector& InLocation, const FVector& InForce)
{
    this->CreateDrop(InAccumulated, InLocation, InForce, FMath::FRandRange(UEntitySubsystem::MinRandomForceMultiplier, UEntitySubsystem::MaxRandomForceMultiplier));
}

void UEntitySubsystem::CreateDrop(const FAccumulated& InAccumulated, const FTransform& InSourceTransform)
{
    this->CreateDrop(
        InAccumulated,
        /* TODO We can not just get the 80+ Vector. No, we have to make a ray-cast and check if we can spawn something there. */
        InSourceTransform.GetLocation() + InSourceTransform.GetRotation().Vector() * 80.0f,
        InSourceTransform.GetRotation().Vector()
    );
}

FVector UEntitySubsystem::GetRandomForceVector(const float MinRangeXY, const float MaxRangeXY, const float MinRangeZ, const float MaxRangeZ)
{
    return FVector(FMath::FRandRange(MinRangeXY, MaxRangeXY), FMath::FRandRange(MinRangeXY, MaxRangeXY), FMath::FRandRange(MinRangeZ, MaxRangeZ)).GetSafeNormal();
}

FVector UEntitySubsystem::GetRandomForceVector(void)
{
    return UEntitySubsystem::GetRandomForceVector(-1.0f, 1.0f, -1.0f, 1.0f);
}

FVector UEntitySubsystem::GetRandomUpwardForceVector(void)
{
    return UEntitySubsystem::GetRandomForceVector(-1.0f, 1.0f, 0.0f, 1.0f);
}

FVector2d UEntitySubsystem::GetRandomForceVector2D(void)
{
    return FVector2d(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f)).GetSafeNormal();
}
