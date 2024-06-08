// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Entity/Drop.h"

#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "WorldCore/WorldCharacter.h"

ADrop::ADrop(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = true;

    this->SphereComponent = this->CreateDefaultSubobject<USphereComponent>(TEXT("SpehereComponent"));
    this->SphereComponent->InitSphereRadius(this->CollisionSphereRadius);
    this->SphereComponent->SetupAttachment(this->MeshComponent);
    this->SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    this->SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    this->SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
    this->SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ADrop::OnSphereComponentOverlapBegin);
    this->SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ADrop::OnSphereComponentOverlapEnd);

    this->bReplicates = true;

    return;
}

void ADrop::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ADrop, CurrentAccumulated)

    return;
}

void ADrop::BeginPlay(void)
{
    Super::BeginPlay();

    this->SetReplicateMovement(true);

    this->bBeginPlayed = true;
    this->CreationTime = this->GetWorld()->GetTimeSeconds();

    TArray<FVector> CollisionConvexMesh;
    CollisionConvexMesh.Add(FVector( this->ConvexX,  this->ConvexY,  this->ConvexZ )); /* Forward  Top    Right */
    CollisionConvexMesh.Add(FVector( this->ConvexX,  this->ConvexY, -this->ConvexZ )); /* Forward  Bottom Right */
    CollisionConvexMesh.Add(FVector( this->ConvexX, -this->ConvexY,  this->ConvexZ )); /* Forward  Top    Left  */
    CollisionConvexMesh.Add(FVector( this->ConvexX, -this->ConvexY, -this->ConvexZ )); /* Forward  Bottom Left  */
    CollisionConvexMesh.Add(FVector(-this->ConvexX, -this->ConvexY,  this->ConvexZ )); /* Backward Top    Left  */
    CollisionConvexMesh.Add(FVector(-this->ConvexX, -this->ConvexY, -this->ConvexZ )); /* Backward Bottom Left  */
    CollisionConvexMesh.Add(FVector(-this->ConvexX,  this->ConvexY,  this->ConvexZ )); /* Backward Top    Right */
    CollisionConvexMesh.Add(FVector(-this->ConvexX,  this->ConvexY, -this->ConvexZ )); /* Backward Bottom Right */

    this->MeshComponent->bUseComplexAsSimpleCollision = false;

    this->MeshComponent->AddCollisionConvexMesh(CollisionConvexMesh);
    this->MeshComponent->SetSimulatePhysics(true);
    this->MeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

    if (UNetStatics::IsSafeClient(this))
    {
        this->SphereComponent->OnComponentBeginOverlap.RemoveAll(this);
        this->SphereComponent->OnComponentEndOverlap.RemoveAll(this);
    }

    if (this->GetCurrentAccumulatedIndex() != ECommonVoxels::Null)
    {
        this->GenerateMesh(this->CurrentAccumulated);
    }

    return;
}

void ADrop::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (this->GetActorLocation().Z < ADrop::ForceKillZ)
    {
        LOG_WARNING(
            LogEntitySystem,
            "Dropped accumulated [%s] fell out of the world. Killing.",
            *this->CurrentAccumulated.ToString()
        )
        this->Destroy();
    }

    if (
           UNetStatics::IsSafeClient(this) == false
        && this->CreationTime != 0.0f
        && this->GetWorld()->GetTimeSeconds() - this->CreationTime > ADrop::InvincibleTime + ADrop::EpsilonInvincibleThreshold
    )
    {
        for (AWorldCharacter* Character : this->OverlappingCharacters)
        {
            if (Character->EasyAddToContainer(this->CurrentAccumulated))
            {
                this->Destroy();
                return;
            }

            continue;
        }
    }

    return;
}

void ADrop::GenerateMesh(const FAccumulated& InAccumulated)
{
    this->CurrentAccumulated = InAccumulated;
    Super::GenerateMesh(InAccumulated.AccumulatedIndex);
    return;
}

void ADrop::AddForce(const FVector& InForce) const
{
    this->MeshComponent->AddForce(InForce);
}

void ADrop::OnSphereComponentOverlapBegin(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    int32 OtherBodyIndex,
    const bool bFromSweep,
    const FHitResult& SweepResult
)
{
    if (UNetStatics::IsSafeClient(this))
    {
        LOG_WARNING(LogEntitySystem, "Client should not be able to overlap with this actor.")
        return;
    }

    if (OtherActor->IsA(AWorldCharacter::StaticClass()) == false)
    {
        return;
    }

    AWorldCharacter* OtherCharacter = Cast<AWorldCharacter>(OtherActor); check( OtherCharacter )

    /* We check for zero. Because the overlap event will be called before the Begin Play event. */
    if (this->CreationTime == 0.0f || this->GetWorld()->GetTimeSeconds() - this->CreationTime <= ADrop::InvincibleTime)
    {
        this->OverlappingCharacters.AddUnique(OtherCharacter);
        return;
    }

    if (OtherCharacter->EasyAddToContainer(this->CurrentAccumulated))
    {
        this->Destroy();
    }

    return;
}

void ADrop::OnSphereComponentOverlapEnd(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    int32 OtherBodyIndex
)
{
    if (UNetStatics::IsSafeClient(this))
    {
        LOG_WARNING(LogEntitySystem, "Client should not be able to overlap with this actor.")
        return;
    }

    if (OtherActor->IsA(AWorldCharacter::StaticClass()) == false)
    {
        return;
    }

    if (this->OverlappingCharacters.Contains(Cast<AWorldCharacter>(OtherActor)))
    {
        this->OverlappingCharacters.RemoveSingle(Cast<AWorldCharacter>(OtherActor));
    }

    return;
}

void ADrop::OnRep_CurrentAccumulated(void)
{
    if (this->bBeginPlayed)
    {
        this->GenerateMesh(this->CurrentAccumulated);
    }
}

void ADrop::GenerateMesh(const voxel_t InAccumulated)
{
    LOG_FATAL(LogEntitySystem, "Dissallowed call on derived class.")
}
