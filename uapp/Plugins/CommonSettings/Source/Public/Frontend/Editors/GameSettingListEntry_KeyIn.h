// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameSettingListEntry.h"

#include "GameSettingListEntry_KeyIn.generated.h"

class UGameSettingValueKeyIn;
class UJAFGEnhancedButton;

struct COMMONSETTINGS_API FGameSettingListEntryPassData_KeyIn : public FGameSettingListEntryPassData
{
    UGameSettingValueKeyIn* KeyIn;
};

UCLASS(Abstract, Blueprintable)
class COMMONSETTINGS_API UGameSettingListEntry_KeyIn : public UGameSettingListEntry
{
    GENERATED_BODY()

public:

    explicit UGameSettingListEntry_KeyIn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

public:

    virtual void PassDataToWidget(const FWidgetPassData& UncastedData) override;

protected:

    void OnPrimaryKeyClicked(void);
    void OnSecondaryKeyClicked(void);
    void OnClearClicked(void);

private:

    void SetAllMutableTexts(void) const;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGEnhancedButton> Button_PrimaryKey;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGEnhancedButton> Button_SecondaryKey;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGEnhancedButton> Button_Clear;

    UPROPERTY()
    TObjectPtr<UGameSettingValueKeyIn> Setting;
};
