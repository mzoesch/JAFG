// © 2023 mzoesch. All rights reserved.

#include "Entity/Drop.h"

#include "Kismet/GameplayStatics.h"

#include "Lib/Cuboid.h"
#include "World/WorldVoxel.h"

ADrop::ADrop()
{
	this->PrimaryActorTick.bCanEverTick = false;

	this->SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	this->RootComponent = this->SceneComponent;

	this->Cuboid = nullptr;

	return;
}

ADrop::ADrop(const int V) : ADrop::ADrop()
{
	this->Voxel = V;

	return;
}

void ADrop::BeginPlay()
{
	Super::BeginPlay();

	this->Cuboid = this->GetWorld()->SpawnActorDeferred<ACuboid>(ACuboid::StaticClass(), FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1.0f, 1.0f, 1.0f)));
	this->Cuboid->CuboidX = 10;
	this->Cuboid->CuboidY = 10;
	this->Cuboid->CuboidZ = 10;
	this->Cuboid->ConvexX = 10;
	this->Cuboid->ConvexY = 10;
	this->Cuboid->ConvexZ = 10;
	this->Cuboid->SetHasCollisionConvexMesh(true);
	this->Cuboid->SetHasPawnCollision(true);
	/* TODO We do not need to attach, right? We just can find the FTransform and apply it to the new created AActor. */
	this->Cuboid->AttachToComponent(this->SceneComponent, FAttachmentTransformRules::KeepRelativeTransform);
	this->Cuboid->SetAccumulated(FAccumulated(this->Voxel));
	UGameplayStatics::FinishSpawningActor(this->Cuboid, FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1.0f, 1.0f, 1.0f)));

	/* TODO Apply some force. */

	if (Voxel != EWorldVoxel::VoxelNull)
	{
		this->RenderMesh();
		this->Destroy();
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("ADrop::BeginPlay() - Neither Item nor Voxel is set!"));
	
	return;
}

void ADrop::SetVoxel(const int V)
{
	this->Voxel = V;

	return;
}

void ADrop::RenderMesh() const
{
	this->Cuboid->GenerateMesh(FAccumulated(this->Voxel));
	return;
}
