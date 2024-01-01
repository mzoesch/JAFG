// © 2023 mzoesch. All rights reserved.

#include "Assets/GenPrevAssets.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/KismetRenderingLibrary.h"

#include "Assets/General.h"
#include "Lib/Cuboid.h"

AGenPrevAssets::AGenPrevAssets()
{
	this->PrimaryActorTick.bCanEverTick						= true;

	this->SceneComponent									= this->CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	this->CaptureComponent									= this->CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Camera"));
	this->CaptureComponent->bCaptureEveryFrame				= false;
	this->CaptureComponent->bCaptureOnMovement				= false;
	this->CaptureComponent->bAlwaysPersistRenderingState	= true;
	this->CaptureComponent->FOVAngle						= 30.0f;
	this->CaptureComponent->CaptureSource					= ESceneCaptureSource::SCS_FinalColorLDR;
	/* TODO We may want to use this later to save on memory and performance. But this works for now. */
	/*this->CaptureComponent->PrimitiveRenderMode			= ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList; */
	this->CaptureComponent->PrimitiveRenderMode				= ESceneCapturePrimitiveRenderMode::PRM_LegacySceneCapture;

	this->SetRootComponent(this->SceneComponent);
	this->CaptureComponent->SetupAttachment(this->SceneComponent);
	
	return;
}

void AGenPrevAssets::BeginPlay()
{
	Super::BeginPlay();

	ACuboid* Cuboid = this->GetWorld()->SpawnActor<ACuboid>(ACuboid::StaticClass(), this->PrevTransform, FActorSpawnParameters());
    Cuboid->GenerateMesh(2);

    this->CaptureComponent->TextureTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, 1024, 1024, ETextureRenderTargetFormat::RTF_RGBA8, FLinearColor::Black, false);
	this->CaptureComponent->TextureTarget->TargetGamma = 1.8f;
    this->CaptureComponent->CaptureScene();
    UKismetRenderingLibrary::ExportRenderTarget(this, this->CaptureComponent->TextureTarget, FGeneral::GeneratedAssetsDirectoryRelative, TEXT("Render.png"));

	Cuboid->Destroy();
	
	return;
}

void AGenPrevAssets::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	return;
}
