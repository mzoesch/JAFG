// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Frontend/Editors/GameSettingListEntry.h"

#include "GameSettingListEntry_Color.generated.h"

class UJAFGEnhancedButton;
class UJAFGEditableText;
class UBorder;
class UGameSettingValueColor;

struct COMMONSETTINGS_API FGameSettingListEntryPassData_Color : public FGameSettingListEntryPassData
{
    UGameSettingValueColor* Color;
};

UCLASS(Abstract, Blueprintable)
class COMMONSETTINGS_API UGameSettingListEntry_Color : public UGameSettingListEntry
{
    GENERATED_BODY()

public:

    explicit UGameSettingListEntry_Color(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

public:

    virtual void PassDataToWidget(const FWidgetPassData& UncastedData) override;

protected:

          FColor StandardContainerColor;
    const FColor DangerContainerColor = FColor(127, 0, 0, 127);

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UBorder> Border_Container;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UJAFGEditableText> EditableText_SettingValue;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UBorder> Border_ColorPreview;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UJAFGEnhancedButton> Button_ResetToDefault;

    UFUNCTION()
    void OnTextChanged(const FText& Text);
    static auto IsInTextAViableColor(const FText& Text) -> bool;
    auto SetEditableText(const FColor& Color) const -> void;

    UFUNCTION()
    void OnResetToDefaultClicked( /* void */ ) const;

    void UpdateResetToDefaultButton(void) const;

private:

    UPROPERTY()
    TObjectPtr<UGameSettingValueColor> Setting;
};
