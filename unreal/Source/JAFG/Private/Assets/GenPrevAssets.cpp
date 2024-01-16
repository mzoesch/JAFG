// © 2023 mzoesch. All rights reserved.

#include "Assets/GenPrevAssets.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/KismetRenderingLibrary.h"

#include "Assets/General.h"
#include "Lib/Cuboid.h"
#include "Core/GI_Master.h"

#define GI CastChecked<UGI_Master>(this->GetGameInstance())

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

	this->PrevIndex = 0;
	
	return;
}

void AGenPrevAssets::BeginPlay()
{
	Super::BeginPlay();

	this->PrevIndex = GI->GetCoreVoxelNum();
	
	return;
}

void AGenPrevAssets::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (this->PrevIndex > GI->GetVoxelNum() - 1)
	{
		this->SetActorTickEnabled(false);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Generating preview for %s."), *GI->GetVoxelName(this->PrevIndex))
	
	ACuboid* Cuboid = this->GetWorld()->SpawnActor<ACuboid>(ACuboid::StaticClass(), this->PrevTransform, FActorSpawnParameters());
	Cuboid->GenerateMesh(this->PrevIndex);

	this->CaptureComponent->TextureTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, 1024, 1024, ETextureRenderTargetFormat::RTF_RGBA8, FLinearColor::Black, false);
	this->CaptureComponent->TextureTarget->TargetGamma = 1.8f;
	this->CaptureComponent->CaptureScene();
	const FString FileName = FString::Printf(TEXT("%s.png"), *GI->GetVoxelName(this->PrevIndex));
	UKismetRenderingLibrary::ExportRenderTarget(this, this->CaptureComponent->TextureTarget, FGeneral::GeneratedAssetsDirectoryRelative, FileName);

	Cuboid->Destroy();

	this->PrevIndex++;
	
	return;
}

#undef GI
