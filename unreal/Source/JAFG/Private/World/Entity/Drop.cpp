// Copyright 2024 mzoesch. All rights reserved.

#include "World/Entity/Drop.h"

#include "Jar/Accumulated.h"
#include "Kismet/GameplayStatics.h"
#include "World/Entity/Cuboid.h"

ADrop::ADrop(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->bReplicates = true;
    this->bNetLoadOnClient = true;

    this->SceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneComponent"));
    this->SetRootComponent(this->SceneComponent);

    this->Cuboid = nullptr;

    return;
}

void ADrop::BeginPlay(void)
{
    Super::BeginPlay();

    this->Cuboid = this->GetWorld()->SpawnActorDeferred<ACuboid>(ACuboid::StaticClass(), FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::OneVector));
    this->Cuboid->AttachToComponent(this->SceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

    this->Cuboid->SetHasCollisionConvexMesh(true);
    this->Cuboid->SetHasPawnCollision(true);
    this->Cuboid->SetAccumulatedIndex(ECommonVoxels::Air + 1);

    UGameplayStatics::FinishSpawningActor(this->Cuboid, FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::OneVector));

    this->Cuboid->RegenerateProceduralMesh();

    return;
}
