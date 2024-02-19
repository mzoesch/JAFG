// © 2023 mzoesch. All rights reserved.

#include "Entity/CharacterReach.h"

#include "Components/BoxComponent.h"

#include "World/ChunkWorld.h"
#include "World/JCoordinate.h"

ACharacterReach::ACharacterReach()
{
    this->Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
    this->RootComponent = this->Box;
    this->Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    this->Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    this->Box->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f), false);
    this->Box->SetLineThickness(2.0f);
    this->Box->ShapeColor = FColor::Black;
    this->Box->SetHiddenInGame(false);
    this->Box->SetVisibility(false);

    this->Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    this->Mesh->SetWorldScale3D(FVector(1.001f, 1.001f, 1.001f));
    this->Mesh->SetupAttachment(this->Box);
    this->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    this->Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    this->Mesh->SetHiddenInGame(false);
    this->Mesh->SetVisibility(false);
    this->Mesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
    this->Mesh->SetGenerateOverlapEvents(false);
    this->Mesh->SetCastShadow(false);

    return;
}

void ACharacterReach::BeginPlay(void)
{
    Super::BeginPlay();

    check( this->CharacterReachStaticMesh )
    check( this->CharacterReachMaterial   )
    
    this->Mesh->SetStaticMesh(this->CharacterReachStaticMesh);
    this->MDynamicCharacterReachMaterial = UMaterialInstanceDynamic::Create(this->CharacterReachMaterial, this);
    this->Mesh->SetMaterial(0, this->MDynamicCharacterReachMaterial);
    return;
}

void ACharacterReach::OnUpdate(const bool bVisible) const
{
    this->Box->SetVisibility(bVisible);
    this->Mesh->SetVisibility(bVisible);
    return;
}

void ACharacterReach::OnUpdate(const FVector& WorldLocation) const
{
    if (this->Box->IsVisible() == false)
    {
        this->Box->SetVisibility(true);
        this->Mesh->SetVisibility(true);
        this->MDynamicCharacterReachMaterial->SetScalarParameterValue(TEXT("TestParam"), 0.0f);
    }

    this->Box->SetWorldLocation(FVector(AChunkWorld::WorldToWorldVoxelPosition(WorldLocation) * UJCoordinate::J_TO_U_SCALE) + FVector(50.0f, 50.0f, 50.0f), false);
    this->Box->SetWorldRotation(FRotator::ZeroRotator);
    
    return;
}

void ACharacterReach::UpdateMaterial(const float Progress) const
{
    this->MDynamicCharacterReachMaterial->SetScalarParameterValue(TEXT("TestParam"), FMath::Fmod(Progress * 10, 10.0f));
    return;
}
