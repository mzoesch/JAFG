// Copyright 2024 mzoesch. All rights reserved.

#include "World/Entity/Drop.h"

#include "Jar/Accumulated.h"

ADrop::ADrop(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->bReplicates = true;
    this->bNetLoadOnClient = true;

    this->bHasCollisionConvexMesh = true;
    this->bHasPawnCollision = true;

    this->CuboidX = ADrop::DefaultDropCuboidX;
    this->CuboidY = ADrop::DefaultDropCuboidY;
    this->CuboidZ = ADrop::DefaultDropCuboidZ;
    this->ConvexX = ADrop::DefaultDropConvexX;
    this->ConvexY = ADrop::DefaultDropConvexY;
    this->ConvexZ = ADrop::DefaultDropConvexZ;

    return;
}

void ADrop::BeginPlay(void)
{
    Super::BeginPlay();

    if (this->AccumulatedIndex == Accumulated::Null.AccumulatedIndex)
    {
        LOG_FATAL(LogEntitySystem, "Accumulated is not set.")
        return;
    }

    this->RegenerateProceduralMesh();

    return;
}

void ADrop::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ADrop::AddForce(const FVector& Force) const
{
    this->AddForceToProceduralMesh(Force);
}

void ADrop::OnSphereComponentOverlapBegin(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    LOG_VERBOSE(LogEntitySystem, "Called.")
}

void ADrop::OnSphereComponentOverlapEnd(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    int32 OtherBodyIndex
)
{
    LOG_VERBOSE(LogEntitySystem, "Called.")
}
