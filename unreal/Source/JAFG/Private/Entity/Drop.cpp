// © 2023 mzoesch. All rights reserved.

#include "Entity/Drop.h"

#include "Kismet/GameplayStatics.h"

#include "Lib/Cuboid.h"
#include "World/WorldVoxel.h"

#define UIL_LOG(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)

ADrop::ADrop()
{
	this->PrimaryActorTick.bCanEverTick = false;

	this->SceneComponent	= CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	this->RootComponent		= this->SceneComponent;

	this->Cuboid			= nullptr;

	return;
}

ADrop::ADrop(const FAccumulated InAccumulated) : ADrop::ADrop()
{
	this->Accumulated = InAccumulated;
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
	this->Cuboid->AccumulatedIndex = this->Accumulated.Accumulated;
	UGameplayStatics::FinishSpawningActor(this->Cuboid, FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1.0f, 1.0f, 1.0f)));

	/* TODO Apply some force. */

	if (Accumulated.Accumulated != EWorldVoxel::WV_Null)
	{
		this->RenderMesh();
		this->Destroy();
		return;
	}

	UIL_LOG(Fatal, TEXT("ADrop::BeginPlay(): No accumulated item is set."));

	return;
}

void ADrop::AddForce(const FVector& Force) const
{
	this->Cuboid->AddForce(Force);
}

void ADrop::RenderMesh() const
{
	this->Cuboid->GenerateMesh(this->Accumulated.Accumulated);
	return;
}

#undef UIL_LOG
