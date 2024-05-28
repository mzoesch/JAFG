// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Frontend/Editors/GameSettingListEntry.h"

#include "GameSettingListEntry_Color.generated.h"

class UJAFGEditableText;
class UBorder;
class UGameSettingValueColor;

struct FGameSettingListEntryPassData_Color : public FGameSettingListEntryPassData
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

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UJAFGEditableText> EditableText_SettingValue;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UBorder> Border_ColorPreview;

    UFUNCTION()
    void OnTextChanged(const FText& Text);

private:

    UPROPERTY()
    TObjectPtr<UGameSettingValueColor> Setting;
};
