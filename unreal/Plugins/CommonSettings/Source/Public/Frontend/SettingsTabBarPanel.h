// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SettingsData/GameSetting.h"
#include "TabBar/JAFGTabBarPanel.h"

#include "SettingsTabBarPanel.generated.h"

class UGameSettingListEntry_Color;
class UGameSettingListEntry_KeyIn;
class UGameSettingListEntry_Scalar;
class UGameSettingCollection;
class UJAFGScrollBox;
class UJAFGButton;

struct COMMONSETTINGS_API FSettingsPassData : public FWidgetPassData
{
    explicit FSettingsPassData(TObjectPtr<UGameSetting> InPageSetting) : PageSetting(InPageSetting) { }
    TObjectPtr<UGameSetting> PageSetting;
};

UCLASS(Abstract, Blueprintable)
class COMMONSETTINGS_API USettingsTabBarPanel : public UJAFGTabBarPanel
{
    GENERATED_BODY()

public:

    explicit USettingsTabBarPanel(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void PassDataToWidget(const FWidgetPassData& UncastedData) override;

protected:

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UJAFGScrollBox> SB_SettingsContent;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UJAFGButton> B_Apply;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UJAFGButton> B_Cancel;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UGameSettingListEntry_Scalar> GameSettingListEntry_ScalarWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UGameSettingListEntry_KeyIn> GameSettingListEntry_KeyInWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UGameSettingListEntry_Color> GameSettingListEntry_ColorWidgetClass;

    virtual auto OnNativeMadeVisible(void) -> void override;
    virtual auto OnNativeMadeCollapsed(void) -> void override;

    virtual void CreateSettingsPage(void);
    virtual void CreateCollectionSubPage(const UGameSettingCollection* InCollection);
    virtual void CreateConcreteSetting(UGameSetting* InSetting, UPanelWidget* Parent);

    /** Does not have to be but should be a collection. */
    UPROPERTY()
    TObjectPtr<UGameSetting> PageSetting = nullptr;
};
