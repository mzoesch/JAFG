// © 2023 mzoesch. All rights reserved.

#include "World/EntityMaster.h"

#include "Kismet/GameplayStatics.h"

#include "Entity/Drop.h"

AEntityMaster::AEntityMaster()
{
    this->PrimaryActorTick.bCanEverTick = false;
    return;
}

bool AEntityMaster::CreateDrop(const FAccumulated& Accumulated, const FVector& Location, const FVector& Force, const float ForceMultiplier) const
{
    ADrop* Drop = this->GetWorld()->SpawnActorDeferred<ADrop>(ADrop::StaticClass(), FTransform(FRotator::ZeroRotator, Location, FVector::OneVector));
    Drop->SetAccumulated(Accumulated);
    
    UGameplayStatics::FinishSpawningActor(Drop, FTransform(FRotator(0.0f, 0.0f, 180.0f), Location, FVector::OneVector));

    Drop->AddForce(Force.GetSafeNormal() * ForceMultiplier);

    return true;
}

bool AEntityMaster::CreateDrop(const FAccumulated& Accumulated, const FVector& Location, const FVector& Force) const
{
    return this->CreateDrop(Accumulated, Location, Force, FMath::FRandRange(AEntityMaster::MinDefaultForceMultiplier, AEntityMaster::MaxDefaultForceMultiplier));
}

bool AEntityMaster::CreateDrop(const FAccumulated& Accumulated, const FTransform& SourceTransform) const
{
    return this->CreateDrop(
        Accumulated,
        /* TODO We can not just get the 80+ Vector. No, we have to make a ray-cast and check if we can spawn something there. */
        SourceTransform.GetLocation() + SourceTransform.GetRotation().Vector() * 80.0f,
        SourceTransform.GetRotation().Vector()
    );
}
