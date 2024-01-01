// © 2023 mzoesch. All rights reserved.

#include "Assets/GenPrevAssets.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/KismetRenderingLibrary.h"

#include "Assets/General.h"
#include "Lib/Cuboid.h"

AGenPrevAssets::AGenPrevAssets()
{
	this->PrimaryActorTick.bCanEverTick = true;

	this->SceneComponent = this->CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	this->SetRootComponent(this->SceneComponent);

	this->CaptureComponent = this->CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Camera"));
	this->CaptureComponent->SetupAttachment(this->SceneComponent);
	this->CaptureComponent->bCaptureEveryFrame = false;
	this->CaptureComponent->bCaptureOnMovement = false;
	this->CaptureComponent->bAlwaysPersistRenderingState = true;
	this->CaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	// this->CaptureComponent->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	
	return;
}

void AGenPrevAssets::BeginPlay()
{
	Super::BeginPlay();
	// if (this->bIsGenerated == false)
    	// {
    	ACuboid* Cuboid = this->GetWorld()->SpawnActor<ACuboid>(
    	ACuboid::StaticClass(),
    	FTransform(FRotator(180, 45, 0), FVector(500, 0, -200), FVector::OneVector),
    	FActorSpawnParameters()
    	);
    	Cuboid->GenerateMesh(EVoxel::Grass);
    
    	UE_LOG(LogTemp, Warning, TEXT("Generated MESH"))
    		// return;
    	// }
    
    	// if (this->bIsRendered == false && this->waitCount >= 100)
    	// {
    	this->CaptureComponent->TextureTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, 1024, 1024, ETextureRenderTargetFormat::RTF_RGBA8, FLinearColor::Black, false);
    	this->CaptureComponent->CaptureScene();
    	UKismetRenderingLibrary::ExportRenderTarget(this, this->CaptureComponent->TextureTarget, FGeneral::GeneratedAssetsDirectory, TEXT("Render.png"));
    	this->bIsRendered = true;
    	waitCount++;
    	UE_LOG(LogTemp, Warning, TEXT("Rendered TEXTURE"))
    	return;
    	// }
    	//
    	// if (waitCount < 2000)
    	// {
    	// 	this->waitCount++;
    	// 	return;
    	// }
	return;
}

void AGenPrevAssets::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	
}
