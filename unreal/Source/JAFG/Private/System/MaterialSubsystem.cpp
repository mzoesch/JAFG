// Copyright 2024 mzoesch. All rights reserved.

#include "System/MaterialSubsystem.h"

#include "Engine/Texture2DArray.h"
#include "System/JAFGInstance.h"
#include "World/Voxel/VoxelSubsystem.h"

void UMaterialSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<UVoxelSubsystem>();
    Super::Initialize(Collection);

    UE_LOG(LogTemp, Log, TEXT("UMaterialSubsystem::Initialize: Initializing Material Subsystem."))

    this->InitializeMaterials();
    
    return;
}

void UMaterialSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
}

void UMaterialSubsystem::InitializeMaterials(void)
{
    if (this->GetWorld() == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("UMaterialSubsystem::InitializeMaterials: World is nullptr."))
        return;
    }
    
    const UJAFGInstance* JAFGInstance = Cast<UJAFGInstance>(GetWorld()->GetGameInstance());
    if (JAFGInstance == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("UMaterialSubsystem::InitializeMaterials: Could not cast UJAFGInstance."))
        return;
    }

    this->MDynamicOpaque = UMaterialInstanceDynamic::Create(JAFGInstance->MOpaque, this);
    this->MDynamicFullBlendOpaque = UMaterialInstanceDynamic::Create(JAFGInstance->MFullBlendOpaque, this);
    this->MDynamicFloraBlendOpaque = UMaterialInstanceDynamic::Create(JAFGInstance->MFloraBlendOpaque, this);
    
    UTexture2DArray* TextureArray = UTexture2DArray::CreateTransient(UMaterialSubsystem::TextureArrayWidthHorizontal, UMaterialSubsystem::TextureArrayWidthVertical, 1, EPixelFormat::PF_R8G8B8A8);
    TextureArray->Filter = TextureFilter::TF_Nearest;
    TextureArray->SRGB = true;
    TextureArray->CompressionSettings = TextureCompressionSettings::TC_Default;

    UVoxelSubsystem* VoxelSubsystem = JAFGInstance->GetSubsystem<UVoxelSubsystem>();
    if (!VoxelSubsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("UMaterialSubsystem::InitializeMaterials: Could not get UVoxelSubsystem."))
        return;
    }

    int TextureIndex = 0;
    constexpr int StoneVoxelIdx = 2; /* two core voxels */ 
    constexpr int DirtVoxelIdx = 3;
    constexpr int GrassVoxelIdx = 4;

    TextureArray->SourceTextures.Add(JAFGInstance->Stone);
    VoxelSubsystem->VoxelMasks[StoneVoxelIdx].AddTextureIndex(ENormalLookup::Default, TextureIndex++);

    TextureArray->SourceTextures.Add(JAFGInstance->Dirt);
    VoxelSubsystem->VoxelMasks[DirtVoxelIdx].AddTextureIndex(ENormalLookup::Default, TextureIndex++);

    TextureArray->SourceTextures.Add(JAFGInstance->GrassTop);
    VoxelSubsystem->VoxelMasks[GrassVoxelIdx].AddTextureIndex(ENormalLookup::Top, TextureIndex++);
    TextureArray->SourceTextures.Add(JAFGInstance->Grass);
    VoxelSubsystem->VoxelMasks[GrassVoxelIdx].AddTextureIndex(ENormalLookup::Default, TextureIndex++);

    // https://forums.unrealengine.com/t/cant-make-a-a-texture-array-at-runtime/157889/2

    TextureArray->UpdateResource();
    TextureArray->UpdateSourceFromSourceTextures();

    this->MDynamicOpaque->SetTextureParameterValue("TexArr", TextureArray);
    this->MDynamicFullBlendOpaque->SetTextureParameterValue("TexArr", TextureArray);
    this->MDynamicFloraBlendOpaque->SetTextureParameterValue("TexArr", TextureArray);

    UE_LOG(LogTemp, Log, TEXT("UMaterialSubsystem::InitializeMaterials: Texture array size: %d"), TextureArray->SourceTextures.Num());

    return;
}
