// © 2023 mzoesch. All rights reserved.

#include "Entity/Drop.h"

#include "Kismet/GameplayStatics.h"

#include "Lib/Cuboid.h"

ADrop::ADrop()
{
	this->PrimaryActorTick.bCanEverTick = false;

	this->SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	this->RootComponent = this->SceneComponent;

	this->Cuboid = nullptr;
	
	return;
}

ADrop::ADrop(const EItem& I) : ADrop::ADrop()
{
	this->Item = I;
	this->Voxel = EVoxel::Null;

	return;
}

ADrop::ADrop(const EVoxel& V) : ADrop::ADrop()
{
	this->Item = EItem::NullItem;
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
	this->Cuboid->SetAccumulated(FAccumulated(this->Item, this->Voxel));
	UGameplayStatics::FinishSpawningActor(this->Cuboid, FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1.0f, 1.0f, 1.0f)));

	/* TODO Apply some force. */
	
	if (Item != EItem::NullItem)
	{
		this->RenderMesh();
		this->Destroy();
		return;
	}

	if (Voxel != EVoxel::Null)
	{
		this->RenderMesh();
		this->Destroy();
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("ADrop::BeginPlay() - Neither Item nor Voxel is set!"));
	
	return;
}

void ADrop::SetItem(const EItem& I)
{
	this->Item = I;
	this->Voxel = EVoxel::Null;
	
	return;
}

void ADrop::SetVoxel(const EVoxel& V)
{
	this->Item = EItem::NullItem;
	this->Voxel = V;

	return;
}

void ADrop::RenderMesh() const
{
	this->Cuboid->GenerateMesh(FAccumulated(this->Item, this->Voxel));
	return;
}
