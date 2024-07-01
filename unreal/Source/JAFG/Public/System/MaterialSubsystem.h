// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "MaterialSubsystem.generated.h"

class UJAFGMaterialSettings;
class UTextureSubsystem;
class UJAFGGameInstance;
class UVoxelSubsystem;
JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API UMaterialSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    UMaterialSubsystem();

    // Subsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize(void) override;
    // ~Subsystem implementation

    inline static constexpr int TexArrWidthHorizontal { 16 };
    inline static constexpr int TexArrWidthVertical   { 16 };

    /**
     * All dynamic created materials.
     * See ETextureGroup for the indices.
     */
    UPROPERTY()
    TArray<TObjectPtr<UMaterialInstanceDynamic>> MDynamicGroups;

    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> MDynamicDestruction;

    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> MDynamicItem;

private:

    /**
     * Initialize all materials.
     *
     * Auto-detects all textures located in the ContentDir/Assets/Textures directory.
     *
     * All textures must be valid and have the same dimensions (currently UMaterialSubsystem#TexArrWidthHorizontal
     * times UMaterialSubsystem#TexArrWidthVertical). We may want to add additional dimensions in the future.
     * If a texture is not valid, it will *not* be skipped and the application will crash.
     *
     * A texture is valid if:
     *  A) It has the same dimensions as all the other textures.
     *  B) The first characters until the first UTextureSubsystem#TexSectionDividerChar (if not existent until the end,
     *     excluding the file extension (.png)) have to have a corresponding entry in the Voxel Subsystem.
     *  C) Add optionally a Normal vector to the texture name with the following format:
     *     <TextureName><TexSectionDivider><Normal> (See ENormalLookup for valid normals). This texture will now only
     *     be used on the face where its normal is pointing to this direction.
     *     Note, each texture must have a default texture for all faces that are not covered by the normal textures
     *     even if all faces are custom. (A default texture is marked by simply not having a normal vector in
     *     its name.)
     *  D) Add optionally a Blend texture to the texture name with the following format:
     *     <TextureName><TexSectionDivider><Blend> This texture will now be blended with an alpha mask defined in the
     *     ContentDir/Assets/Textures/Alpha directory. The alpha mask must have the same dimensions as the texture and
     *     has to exist.
     *     A blend extension must always come after the normal extension if both are present.
     */
    void InitializeMaterials(void);

    /** @return The names of the textures, not paths. */
    TArray<FString> FindAllImportantTextures(void) const;

    void InitializeDestructionMaterial(void);
    void InitializeItemMaterial(void);

    /**
     * All the blend texture names that where found at application boot-up.
     * Never assume that the indices are the same as the ones in MDynamicGroups.
     */
    TArray<FString> Blends;
    const int32     NoBlend = -1;

    UPROPERTY()
    TObjectPtr<UJAFGGameInstance> GameInstance = nullptr;

    UPROPERTY()
    TObjectPtr<UVoxelSubsystem> VoxelSubsystem = nullptr;

    UPROPERTY()
    TObjectPtr<UTextureSubsystem> TextureSubsystem = nullptr;

    UPROPERTY()
    const UJAFGMaterialSettings* MaterialSettings = nullptr;
};
