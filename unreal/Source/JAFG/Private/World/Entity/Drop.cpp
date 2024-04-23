// Copyright 2024 mzoesch. All rights reserved.

#include "World/Entity/Drop.h"

#include "Jar/Accumulated.h"
#include "Kismet/GameplayStatics.h"
#include "Network/NetworkStatics.h"
#include "World/WorldCharacter.h"

ADropImpl::ADropImpl(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = true;

    this->OverlappingCharacters.Empty();

    return;
}

void ADropImpl::BeginPlay(void)
{
    Super::BeginPlay();

    if (UNetworkStatics::IsSafeClient(this))
    {
        LOG_DISPLAY(LogEntitySystem, "Replicated to a client. Disabeling the drop.")
        this->SetActorTickEnabled(false);
        return;
    }

    check( this->GetWorld() )

    this->OverlappingCharacters.Empty();

    this->CreationTime = this->GetWorld()->GetTimeSeconds();

    return;
}

void ADropImpl::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (this->CreationTime != 0.0f)
    {
        if (this->GetWorld()->GetTimeSeconds() - this->CreationTime > ADropImpl::InvincibleTime)
        {
            this->bPrioritizeNewCharacters = true;
        }

        if (this->GetWorld()->GetTimeSeconds() - this->CreationTime > ADropImpl::InvincibleTime + ADropImpl::EpsilonInvincibleThreshold)
        {
            this->bPrioritizeNewCharacters = false;
            this->CreationTime = 0.0f;
        }

        return;
    }

    for (AWorldCharacter* OverlappingCharacter : this->OverlappingCharacters)
    {
        if (OverlappingCharacter == nullptr)
        {
            LOG_ERROR(LogEntitySystem, "Overlapping character is nullptr.");
            continue;
        }

        if (OverlappingCharacter->AddToInventory(FAccumulated(this->AccumulatedIndex)))
        {
            this->Destroy();
            return;
        }

        continue;
    }

    return;
}

void ADropImpl::OnWorldCharacterOverlapBegin(AWorldCharacter* OverlappedCharacter)
{
    check( OverlappedCharacter )

    if (this->bPrioritizeNewCharacters)
    {
        this->OverlappingCharacters.Insert(OverlappedCharacter, 0);
    }
    else
    {
        this->OverlappingCharacters.Add(OverlappedCharacter);
    }

    return;
}

void ADropImpl::OnWorldCharacterOverlapEnd(AWorldCharacter* OverlappedCharacter)
{
    if (this->OverlappingCharacters.Remove(OverlappedCharacter) == 0)
    {
        LOG_WARNING(LogEntitySystem, "Could not remove overlapping character: %s. Size: %d.", *OverlappedCharacter->GetName(), this->OverlappingCharacters.Num())
    }

    return;
}

ADrop::ADrop(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->bReplicates = true;
    this->bNetLoadOnClient = true;

    this->SceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneComponent"));
    this->SetRootComponent(this->SceneComponent);

    this->DropImpl = nullptr;

    return;
}

void ADrop::BeginPlay(void)
{
    Super::BeginPlay();

    check( this->GetWorld() )

    if (this->AccumulatedIndex == Accumulated::Null.AccumulatedIndex)
    {
        LOG_FATAL(LogEntitySystem, "No accumulated item is set.");
        return;
    }

    this->DropImpl = this->GetWorld()->SpawnActorDeferred<ADropImpl>(ADropImpl::StaticClass(), FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::OneVector));
    this->DropImpl->AttachToComponent(this->SceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

    this->DropImpl->SetHasCollisionConvexMesh(true);
    this->DropImpl->SetHasPawnCollision(true);
    this->DropImpl->SetAccumulatedIndex(this->AccumulatedIndex);

    this->DropImpl->SetCuboidDimensions(ADrop::DefaultVoxelDropCuboidX, ADrop::DefaultVoxelDropCuboidY, ADrop::DefaultVoxelDropCuboidZ);
    this->DropImpl->SetConvexDimensions(ADrop::DefaultVoxelDropConvexX, ADrop::DefaultVoxelDropConvexY, ADrop::DefaultVoxelDropConvexZ);

    this->CreateDropOverlapBeginDelegate();
    this->CreateDropOverlapEndDelegate();

    UGameplayStatics::FinishSpawningActor(this->DropImpl, FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::OneVector));

    this->DropImpl->RegenerateProceduralMesh();

    if (this->ForceAfterDrop != FVector::ZeroVector)
    {
        this->DropImpl->AddForceToProceduralMesh(this->ForceAfterDrop);
    }

    this->Destroy();

    return;
}

void ADrop::CreateDropOverlapBeginDelegate(void) const
{
    ADropImpl* LambdaDropImpl = this->DropImpl;

    this->DropImpl->OnCuboidBeginOverlapEvent = FOnCuboidBeginOverlapEventSignature::CreateLambda( [LambdaDropImpl] (
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        const int32 OtherBodyIndex,
        const bool bFromSweep,
        const FHitResult& SweepResult
    )
    {
        check( OtherActor )
        check( LambdaDropImpl )

        if (OtherActor->IsA(AWorldCharacter::StaticClass()) == false)
        {
            LOG_ERROR(LogEntitySystem, "Triggered by an actor that is not a world character: %s.", *OtherActor->GetName());
            return;
        }

        check( Cast<AWorldCharacter>(OtherActor) )

        LambdaDropImpl->OnWorldCharacterOverlapBegin(Cast<AWorldCharacter>(OtherActor));

        return;
    });

    return;
}

void ADrop::CreateDropOverlapEndDelegate(void) const
{
    ADropImpl* LambdaDropImpl = this->DropImpl;

    this->DropImpl->OnCuboidEndOverlapEvent = FOnCuboidEndOverlapEventSignature::CreateLambda( [LambdaDropImpl] (
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        const int32 OtherBodyIndex
    )
    {
        check( OtherActor )
        check( LambdaDropImpl )

        if (OtherActor->IsA(AWorldCharacter::StaticClass()) == false)
        {
            LOG_ERROR(LogEntitySystem, "Triggered by an actor that is not a world character: %s.", *OtherActor->GetName());
            return;
        }

        check( Cast<AWorldCharacter>(OtherActor) )

        LambdaDropImpl->OnWorldCharacterOverlapEnd(Cast<AWorldCharacter>(OtherActor));

        return;
    });

    return;
}
