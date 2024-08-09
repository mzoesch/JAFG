// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "JAFGMaterialSettings.generated.h"

UCLASS(Config=JAFG, DefaultConfig, meta=(DisplayName="JAFG Material Settings"))
class JAFGSETTINGS_API UJAFGMaterialSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:

    explicit UJAFGMaterialSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // UDeveloperSettings implementation
    virtual auto GetContainerName(void) const -> FName override;
    virtual auto GetCategoryName(void) const -> FName override;
    virtual auto GetSectionName(void) const -> FName override;
#if WITH_EDITOR
    virtual auto GetSectionText(void) const -> FText override;
    virtual auto GetSectionDescription(void) const -> FText override;
    virtual auto SupportsAutoRegistration(void) const -> bool override;
    virtual auto PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) -> void override;
    virtual auto GetCustomSettingsWidget(void) const -> TSharedPtr<SWidget> override;
#endif WITH_EDITOR
    // ~UDeveloperSettings implementation

    UPROPERTY(Config, EditAnywhere,  BlueprintReadOnly, Category = "Material")
    const TSoftObjectPtr<UMaterialInterface> MOpaque;

    UPROPERTY(Config, EditAnywhere,  BlueprintReadOnly, Category = "Material")
    const TSoftObjectPtr<UMaterialInterface> MOpaqueBlend;

    UPROPERTY(Config, EditAnywhere,  BlueprintReadOnly, Category = "Material")
    const TSoftObjectPtr<UMaterialInterface> MDestruction;

    UPROPERTY(Config, EditAnywhere,  BlueprintReadOnly, Category = "Material")
    const TSoftObjectPtr<UMaterialInterface> MItem;
};
