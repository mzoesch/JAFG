// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "JAFGSlateSettings.generated.h"

class UJAFGWidget;
class UWarningPopUp;
class UWarningPopUpYesNo;
struct FPropertyChangedEvent;

UCLASS(Config=JAFG, DefaultConfig, meta=(DisplayName="JAFG Slate Settings"))
class JAFGSETTINGS_API UJAFGSlateSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:

    explicit UJAFGSlateSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

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

    /**
     * Must inherit from UEscapeMenu.
     */
    UPROPERTY(Config, EditAnywhere,  BlueprintReadOnly, Category = "Slate")
    TSubclassOf<UJAFGWidget> EscapeMenuWidgetClass;

    UPROPERTY(Config, EditAnywhere,  BlueprintReadOnly, Category = "Slate")
    TSubclassOf<UWarningPopUp> WarningPopUpWidgetClass;

    UPROPERTY(Config, EditAnywhere,  BlueprintReadOnly, Category = "Slate")
    TSubclassOf<UWarningPopUpYesNo> WarningPopUpYesNoWidgetClass;
};
