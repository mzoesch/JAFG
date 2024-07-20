// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameSettingListEntry.h"

#include "GameSettingListEntry_Scalar.generated.h"

class UJAFGEnhancedButton;
class UGameSettingValueScalar;
class UJAFGSlider;
class UJAFGTextBlock;

struct COMMONSETTINGS_API FGameSettingListEntryPassData_Scalar : public FGameSettingListEntryPassData
{
    UGameSettingValueScalar* Scalar;
};

UCLASS(Abstract, Blueprintable)
class COMMONSETTINGS_API UGameSettingListEntry_Scalar : public UGameSettingListEntry
{
    GENERATED_BODY()

public:

    explicit UGameSettingListEntry_Scalar(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

public:

    virtual void PassDataToWidget(const FWidgetPassData& UncastedData) override;

protected:

    // UGameSettingListEntry implementation
    virtual void OnRestoreSettingsToInitial(void) override;
    // ~UGameSettingListEntry implementation

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UJAFGSlider> Slider_SettingValue;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UJAFGTextBlock> Text_SettingValue;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UJAFGEnhancedButton> Button_ResetToDefault;

    UFUNCTION()
    void OnSliderValueChanged(const float Value);

    UFUNCTION(BlueprintImplementableEvent)
    void OnValueChanged(const float Value, const float NormalizedValue);

    UFUNCTION()
    void OnResetToDefaultClicked( /* void */ );
    void UpdateResetToDefaultButton(void) const;

private:

    UPROPERTY()
    TObjectPtr<UGameSettingValueScalar> Setting = nullptr;
};
