// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "JAFGUserWidget.h"
#include "TabBar/JAFGTabBarPanel.h"

#include "GamePluginOverviewPanelWidget.generated.h"

JAFG_VOID

class UJAFGButton;
class UJAFGBorder;
class UJAFGTextBlock;
class UJAFGScrollBox;
class UGamePluginOverviewPanelWidget;

struct FPluginPassData : public FWidgetPassData
{
    FPluginPassData(void) = delete;
    explicit FPluginPassData(const UGamePluginOverviewPanelWidget* Owner, const TSharedRef<IPlugin>& Plugin)
    : Owner(Owner), Plugin(Plugin.Get())
    {
        return;
    }

    const UGamePluginOverviewPanelWidget* Owner = nullptr;
    /* This is not a shared ref as it should be? Is this a problem? Let's hope not. */
    IPlugin& Plugin;
};

UCLASS(Abstract, Blueprintable)
class UGamePluginEntryWidget : public UJAFGUserWidget
{
    GENERATED_BODY()

public:

    explicit UGamePluginEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

    virtual void PassDataToWidget(const FWidgetPassData& UncastedData) override;

    UFUNCTION(BlueprintCallable)
    void NativeRefreshWidget( /* void */ );
    UFUNCTION(BlueprintImplementableEvent)
    void RefreshWidget( /* void */ );

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true", OptionalWidget = "true"))
    TObjectPtr<UJAFGBorder> Border_PluginEntry;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGTextBlock> TextBlock_PluginName;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGTextBlock> TextBlock_PluginDescription;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGButton> Button_TogglePlugin;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGBorder> Border_RestartInfoWrapper;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGTextBlock> TextBlock_RestartInfo;


    UFUNCTION(BlueprintPure)
    bool ShowNonFriendlyName( /* void */ ) const;


    UFUNCTION(BlueprintPure)
    bool IsJAFGGamePluginEnabled( /* void */ ) const;
    UFUNCTION(BlueprintPure)
    bool IsJAFGGamePluginDisabled( /* void */ ) const;
    UFUNCTION(BlueprintPure)
    bool IsJAFGGamePluginDivertedFromDefault( /* void */ ) const;
    UFUNCTION(BlueprintPure)
    bool WillJAFGGamePluginBeEnabled( /* void */ ) const;
    UFUNCTION(BlueprintPure)
    bool WillJAFGGamePluginBeDisabled( /* void */ ) const;

    UFUNCTION(BlueprintPure)
    bool IsPluginEnabled( /* void */ ) const;
    UFUNCTION(BlueprintPure)
    bool CanPluginEverBeToggled( /* void */ ) const;
    UFUNCTION(BlueprintPure)
    FString GetReasonForPluginUnabilityForToggle( /* void */ ) const;

protected:

    UFUNCTION()
    void OnTogglePluginClicked( /* void */ );
    bool ShouldTogglePluginButtonBeEnabled(void) const;

private:

    /* This warning makes no sense if the parent object is destroyed, we will also be killed. */
    // ReSharper disable once CppUE4ProbableMemoryIssuesWithUObject
    const UGamePluginOverviewPanelWidget* Owner = nullptr;
    IPlugin* Plugin = nullptr;
};

UCLASS(Abstract, Blueprintable)
class JAFG_API UGamePluginOverviewPanelWidget : public UJAFGTabBarPanel
{
    GENERATED_BODY()

public:

    explicit UGamePluginOverviewPanelWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

    /** Call this from a list entry to refresh buttons, etc. */
    void OnPluginChanged(void) const;

    UFUNCTION(BlueprintCallable)
    void OnNativePluginFilterChanged( /* void */ );
    UFUNCTION(BlueprintCallable)
    void NativeRefreshRenderedPlugins( /* void */);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UJAFGUserWidget> GamePluginEntryWidgetClass;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGScrollBox> ScrollBox_GamePlugins;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGButton> Button_Apply;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGButton> Button_ApplyAndRestart;

    UPROPERTY(BlueprintReadWrite)
    bool bShowNonFriendlyName = true;

    /*
     * Most important boolean flags to filter the plugins at the top, the more below we go, the more specific the
     * filtering gets.
     * Top level flags will always be more respected than the lower level flags. If an upper flag forbids a specific
     * plugin from showing, but a lower filter suggests that it should not be hidden, the plugin will be hidden.
     */

    UPROPERTY(BlueprintReadWrite)
    bool bShowOnlyServerPlugins   = false;
    UPROPERTY(BlueprintReadWrite)
    bool bShowOnlyClientPlugins   = false;
    UPROPERTY(BlueprintReadWrite)
    bool bShowOnlyOptionalPlugins = false;

    UPROPERTY(BlueprintReadWrite)
    bool bShowOnlyEnabledGamePlugins  = false;
    UPROPERTY(BlueprintReadWrite)
    bool bShowOnlyDisabledGamePlugins = false;

    UPROPERTY(BlueprintReadWrite)
    bool bShowProjectPlugins    = false;
    UPROPERTY(BlueprintReadWrite)
    bool bShowEnginePlugins     = false;
    UPROPERTY(BlueprintReadWrite)
    bool bShowDisabledPlugins   = false;
    UPROPERTY(BlueprintReadWrite)
    bool bShowNonRuntimePlugins = false;

protected:

    UFUNCTION()
    void OnApplyClicked( /* void */ );
    UFUNCTION()
    void OnApplyAndRestartClicked( /* void */ );
    bool ShouldApplyButtonsBeEnabled(void) const;

    void ApplySettings(void) const;
    void RestartGame(void) const;

    void RefreshRenderedPlugins(void) const;
    /** If skipped, the plugin will be hidden to the user. */
    bool SkipPlugin(const TSharedRef<IPlugin>& Plugin) const;

    void RefreshButtonStates(void) const;
};
