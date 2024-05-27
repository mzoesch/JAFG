// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameSettingListEntry.h"

#include "GameSettingListEntry_Scalar.generated.h"

class UGameSettingValueScalar;
class UJAFGSlider;
class UJAFGTextBlock;

struct FGameSettingListEntryPassData_Scalar : public FGameSettingListEntryPassData
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

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UJAFGSlider> Slider_SettingValue;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UJAFGTextBlock> Text_SettingValue;

protected:

    UFUNCTION()
    void OnSliderValueChanged(const float Value);

    UFUNCTION(BlueprintImplementableEvent)
    void OnValueChanged(const float Value);

    UFUNCTION(BlueprintImplementableEvent)
    void OnDefaultValueChanged(const float DefaultValue);

private:

    UPROPERTY()
    TObjectPtr<UGameSettingValueScalar> Setting;
};
