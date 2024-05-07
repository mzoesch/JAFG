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

    // UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Materials", meta = (ToolTip = "Automatically set by the subsystem."))
    // TObjectPtr<UMaterialInstanceDynamic> MDynamicOpaque;
    //
    // UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Materials", meta = (ToolTip = "Automatically set by the subsystem."))
    // TObjectPtr<UMaterialInstanceDynamic> MDynamicFullBlendOpaque;
    //
    // UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Materials", meta = (ToolTip = "Automatically set by the subsystem."))
    // TObjectPtr<UMaterialInstanceDynamic> MDynamicFloraBlendOpaque;

    // The first one must always be the opaque material.
    TArray<TObjectPtr<UMaterialInstanceDynamic>> MDynamicGroups;

private:

    void InitializeAlphaMasks(void);
    void InitializeMaterials(void);

    /**
     * All the blend texture names that where found at application boot-up.
     * The indices are not the same as in MDynamicGroups, but +1 because we have to add the opaque material.
     */
    TArray<FString> Blends;
    const int32 NoBlend = -1;
};
