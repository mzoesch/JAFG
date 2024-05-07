// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "MaterialSubsystem.generated.h"

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

private:

    /**
     *
     */
    void InitializeMaterials(void);

    /**
     * All the blend texture names that where found at application boot-up.
     * Never assume that the indices are the same as the ones in MDynamicGroups.
     */
    TArray<FString> Blends;
    const int32 NoBlend = -1;
};
