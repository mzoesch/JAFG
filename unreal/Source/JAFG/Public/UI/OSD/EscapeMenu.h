// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Concretes/CommonBarWidget.h"

#include "EscapeMenu.generated.h"

JAFG_VOID

UCLASS(Abstract, Blueprintable)
class JAFG_API UEscapeMenu : public UCommonBarWidget
{
    GENERATED_BODY()

public:

    explicit UEscapeMenu(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    virtual void NativeDestruct(void) override;
    // ~UUserWidget implementation

    FDelegateHandle EscapeMenuVisibilityChangedDelegateHandle;
    virtual void OnEscapeMenuVisibilityChanged(const bool bVisible);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UCommonBarEntryWidget> DefaultEntryWidget;

    /**
     * Optional. Will override the default entry widget.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UCommonBarEntryWidget> ResumeWidgetClass;

    /**
     * Optional. Will override the default entry widget.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UCommonBarEntryWidget> AchievementsWidgetClass;

    /**
     * Optional. Will override the default entry widget.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UCommonBarEntryWidget> SettingsWidgetClass;

    /**
     * Optional. Will override the default entry widget.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UCommonBarEntryWidget> SessionOptionsWidgetClass;

    /**
     * Optional. Will override the default entry widget.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UCommonBarEntryWidget> ExitToMainMenuWidgetClass;

    /**
     * Optional. Will override the default entry widget.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UCommonBarEntryWidget> ExitToDesktopWidgetClass;

private:

    void RegisterAllTabs(void);
};
