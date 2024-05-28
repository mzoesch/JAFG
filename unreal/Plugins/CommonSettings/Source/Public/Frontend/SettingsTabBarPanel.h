// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SettingsData/GameSetting.h"
#include "TabBar/JAFGTabBarPanel.h"

#include "SettingsTabBarPanel.generated.h"

class UJAFGButton;
class UJAFGScrollBox;
class UGameSettingCollection;
class UGameSettingListEntry_Color;
class UGameSettingListEntry_KeyIn;
class UGameSettingListEntry_Scalar;

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

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

public:

    virtual void PassDataToWidget(const FWidgetPassData& UncastedData) override;

    /** Call this method if a setting in this collection has changed and is now waiting to be applied. */
    void OnApplyableSettingChanged(void);

    /**
     * Call this method with the specific child identifier to disallow applying changed settings.
     * Useful with the current setting is not valid due to bad user input.
     */
    void DisallowApply(const FString& ChildIdentifier);
    /**
     * Release your allowance lock here. If all settings are valid again, the apply-button will be enabled with
     * made changes.
     */
    void ReleaseDisallowApply(const FString& ChildIdentifier);

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

    UFUNCTION()
    void OnApplyClicked( /* void */ );
    UFUNCTION()
    void OnCancelClicked( /* void */ );

    /** Does not have to be but should be a collection. */
    UPROPERTY()
    TObjectPtr<UGameSetting> PageSetting = nullptr;

private:

    TSet<FString> ChildrenThatDisallowApply;
    bool bHasSettingChanged = false;
    void UpdateApplyButtonState(void) const;
};
