// © 2023 mzoesch. All rights reserved.

#include "Assets/GenPrevAssets.h"

#include "ImageUtils.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/KismetRenderingLibrary.h"

#include "Assets/General.h"
#include "Lib/Cuboid.h"
#include "Core/GI_Master.h"
#include "Kismet/GameplayStatics.h"

#define UIL_LOG(Verbosity, Format, ...)     UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)
#define GAME_INSTANCE						Cast<UGI_Master>(this->GetGameInstance())

class UTextureExporterTGA;

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
	this->PrevIndex = GAME_INSTANCE->GetCoreVoxelNum();
	FGeneral::ClearCached2DTextures();
	return;
}

void AGenPrevAssets::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (this->PrevIndex > GAME_INSTANCE->GetVoxelNum() - 1)
	{
		this->SetActorTickEnabled(false);
		// this->FormatGenPrevTextures();

		UIL_LOG(Warning, TEXT("Finished generating prev textures. Other processes may now continue."))

		// UGameplayStatics::OpenLevel(this, "/Game/World/World", false);
		
		return;
	}

	UIL_LOG(Log, TEXT("Generating prev tex for %s (%d)."), *GAME_INSTANCE->GetVoxelName(this->PrevIndex), this->PrevIndex)
	
	ACuboid* Cuboid = this->GetWorld()->SpawnActor<ACuboid>(ACuboid::StaticClass(), this->PrevTransform, FActorSpawnParameters());
	Cuboid->GenerateMesh(this->PrevIndex);

	/* Capture */
	this->CaptureComponent->TextureTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, 16 /* 1024 */, 16 /* 1024 */, ETextureRenderTargetFormat::RTF_RGBA8, FLinearColor::Black, false);
	this->CaptureComponent->TextureTarget->TargetGamma = 1.8f;
	this->CaptureComponent->CaptureScene();

	/* Image Import */
	FImage Image;
	FImageUtils::GetRenderTargetImage(this->CaptureComponent->TextureTarget, Image);
	
	/* Pixel Manipulation */
	UTexture2D*			Tex		= FImageUtils::CreateTexture2DFromImage(Image);
	Tex->MipGenSettings			= TMGS_NoMipmaps;
	FTexture2DMipMap*	Mip		= &Tex->GetPlatformData()->Mips[0];
	const uint32		Width	= Mip->SizeX;
	const uint32		Height	= Mip->SizeY;
	FByteBulkData*      Bulk    = &Mip->BulkData;
	FColor*				Data    = static_cast<FColor*>(Bulk->Lock(EBulkDataLockFlags::LOCK_READ_WRITE));
	
	for (uint32 w = 0; w < Width; ++w) { for (uint32 h = 0; h < Height; ++h)
	{
		if (FColor& Pixel = Data[w + h * Width]; Pixel == FColor::Black)
		{
			// UIL_LOG(Warning, TEXT("AGenPrevAssets::FormatGenPrevTextures: Found black pixel in %s (%d:%d)."), *GAME_INSTANCE->GetVoxelName(this->PrevIndex), w, h)
			Data[w + h * Width] = FColor(255, 0, 0, 255);
		}
	}}

	Bulk->Unlock();
	Tex->UpdateResource();

	// FGeneral::AddTexture2DToCache(FAccumulated(this->PrevIndex), Tex);
	
	/* Export */
	const FString FileName = FString::Printf(TEXT("%s.png"), *GAME_INSTANCE->GetVoxelName(this->PrevIndex));
	const FString OutPath = FGeneral::GeneratedAssetsDirectoryRelative / FileName;

	
	FImageUtils::SaveImageByExtension(*OutPath, Image);

	/* deprecated */
	// UKismetRenderingLibrary::ExportRenderTarget(this, this->CaptureComponent->TextureTarget, FGeneral::GeneratedAssetsDirectoryRelative, FileName);

	Cuboid->Destroy();

	this->PrevIndex++;
	
	return;
}

void AGenPrevAssets::FormatGenPrevTextures()
{
	for (int i = GAME_INSTANCE->GetCoreVoxelNum(); i < GAME_INSTANCE->GetVoxelNum(); ++i)
	{
		UIL_LOG(Log, TEXT("Formatting prev tex for %s [%d/%d]."), *GAME_INSTANCE->GetVoxelName(i), i, GAME_INSTANCE->GetVoxelNum())

		UTexture2D* Tex = FGeneral::LoadTexture2D(FAccumulated(i));

		if (Tex == nullptr)
		{
			UIL_LOG(Error, TEXT("AGenPrevAssets::FormatGenPrevTextures: Could not load texture for %s."), *GAME_INSTANCE->GetVoxelName(i))
			continue;
		}

		// FTexture2DMipMap*	Mip		= &Tex->GetPlatformData()->Mips[0];
		// const uint32		Width	= Mip->SizeX;
		// const uint32		Height	= Mip->SizeY;
		// FByteBulkData*      Bulk    = &Mip->BulkData;
		// FColor*				Data    = static_cast<FColor*>(Bulk->Lock(EBulkDataLockFlags::LOCK_READ_WRITE));
		//
		// for (uint32 w = 0; w < Width; ++w) { for (uint32 h = 0; h < Height; ++h)
		// {
		// 	if (FColor& Pixel = Data[w + h * Width]; Pixel == FColor::Black)
		// 	{
		// 		UIL_LOG(Warning, TEXT("AGenPrevAssets::FormatGenPrevTextures: Found black pixel in %s (%d:%d)."), *GAME_INSTANCE->GetVoxelName(i), w, h)
		// 		Data[w + h * Width] = FColor::White;
		// 	}
		// }}
		//
		// Bulk->Unlock();

		// Tex->UpdateResource();

		// Save the texture to disk.
		
		// const FString FileName = FString::Printf(TEXT("%s.png"), *GAME_INSTANCE->GetVoxelName(i));
		// const FString OutPath = FGeneral::GeneratedAssetsDirectoryRelative / FileName;
		//
		// UIL_LOG(Warning, TEXT("AGenPrevAssets::FormatGenPrevTextures: Overriding %s to %s."), *GAME_INSTANCE->GetVoxelName(i), *OutPath)
		
		// FImage Image;
		// FImageUtils::SaveImageByExtension(*OutPath, Image);


		
		
		
	}
	
	return;
}

#undef UIL_LOG
#undef GI
