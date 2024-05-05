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

    inline static constexpr int TextureArrayWidthHorizontal { 16 };
    inline static constexpr int TextureArrayWidthVertical   { 16 };

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Materials", meta = (ToolTip = "Automatically set by the subsystem."))
    TObjectPtr<UMaterialInstanceDynamic> MDynamicOpaque;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Materials", meta = (ToolTip = "Automatically set by the subsystem."))
    TObjectPtr<UMaterialInstanceDynamic> MDynamicFullBlendOpaque;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Materials", meta = (ToolTip = "Automatically set by the subsystem."))
    TObjectPtr<UMaterialInstanceDynamic> MDynamicFloraBlendOpaque;

private:

    void InitializeMaterials(void);
};
