// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Character/CharacterReach.h"
#include "Components/BoxComponent.h"
#include "System/MaterialSubsystem.h"
#include "WorldCore/WorldCharacter.h"

ACharacterReach::ACharacterReach(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->BoxComponent = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("BoxComponent"));
    this->SetRootComponent(this->BoxComponent);
    this->BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    this->BoxComponent->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
    this->BoxComponent->SetBoxExtent(FVector(WorldStatics::SingleVoxelSizeHalf, WorldStatics::SingleVoxelSizeHalf,WorldStatics::SingleVoxelSizeHalf), false);
    this->BoxComponent->SetLineThickness(2.0f);
    this->BoxComponent->ShapeColor = FColor::Black;
    this->BoxComponent->SetHiddenInGame(false);
    this->BoxComponent->SetVisibility(false);

    this->StaticMeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("StaticMeshComponent"));
    this->StaticMeshComponent->SetupAttachment(this->BoxComponent);
    this->StaticMeshComponent->SetWorldScale3D(FVector(1.001f));
    this->StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    this->StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    this->StaticMeshComponent->SetHiddenInGame(false);
    this->StaticMeshComponent->SetVisibility(false);
    this->StaticMeshComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
    this->StaticMeshComponent->SetGenerateOverlapEvents(false);
    this->StaticMeshComponent->SetCastShadow(false);

    return;
}

void ACharacterReach::BeginPlay(void)
{
    Super::BeginPlay();

    if (UNetStatics::IsSafeDedicatedServer(this))
    {
        LOG_FATAL(LogWorldChar, "This actor is not allowed on dedicated servers.")
        return;
    }

    this->DynamicDestructionMaterial = this->GetGameInstance()->GetSubsystem<UMaterialSubsystem>()->MDynamicDestruction;
    jcheck( this->DynamicDestructionMaterial )

    this->StaticMeshComponent->SetStaticMesh(Cast<AWorldCharacter>(GEngine->GetFirstLocalPlayerController(this->GetWorld())->GetPawn())->CharacterReachMesh);
    jcheck( this->StaticMeshComponent->GetStaticMesh() )
    this->StaticMeshComponent->SetMaterial(0, this->DynamicDestructionMaterial);

    return;
}

void ACharacterReach::Update(const bool bVisible) const
{
    this->BoxComponent->SetVisibility(bVisible);
    this->StaticMeshComponent->SetVisibility(bVisible);

    return;
}

void ACharacterReach::Update(const FVector& WorldLocation) const
{
    if (this->BoxComponent->IsVisible() == false)
    {
        this->BoxComponent->SetVisibility(true);
        this->StaticMeshComponent->SetVisibility(true);
        this->DynamicDestructionMaterial->SetScalarParameterValue(TEXT("Progress"), 0.0f);
    }

    this->BoxComponent->SetWorldLocation(WorldStatics::WorldToGrid(WorldLocation) + FVector(50.0f), false);
    this->BoxComponent->SetWorldRotation(FRotator::ZeroRotator);

    return;
}

void ACharacterReach::Update(float Progress) const
{
    if (Progress > 1.0f)
    {
        Progress = 1.0f;
    }
    else if (Progress < 0.0f)
    {
        Progress = 0.0f;
    }

    this->DynamicDestructionMaterial->SetScalarParameterValue(TEXT("Progress"), FMath::Fmod(Progress * 10, 10.0f));

    return;
}
