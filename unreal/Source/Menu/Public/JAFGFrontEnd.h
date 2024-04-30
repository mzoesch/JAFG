// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Components/WidgetSwitcher.h"
#include "FrontEnd/JAFGWidget.h"

#include "JAFGFrontEnd.generated.h"

class UJAFGSettingScreen;
JAFG_VOID

class UCreditsFrontEnd;
class UEditorFrontEnd;
class UNewSessionFrontEnd;
class UJoinSessionFrontEnd;
class UWidgetSwitcher;
class UTextBlock;

UENUM(BlueprintType)
namespace EMenuFrontEndTab
{

enum Type
{
    Invalid = 0,
    NewSession,
    JoinSession,
    OpenEditor,
    Options,
    Credits,
    Quit
};

}

UCLASS(Abstract, Blueprintable)
class MENU_API UJAFGFrontEnd : public UJAFGWidget
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UNewSessionFrontEnd> NewSessionFrontEndClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UJoinSessionFrontEnd> JoinSessionFrontEndClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UEditorFrontEnd> EditorFrontEndClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UJAFGSettingScreen> SettingsFrontEndClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UCreditsFrontEnd> CreditsFrontEndClass;

    //////////////////////////////////////////////////////////////////////////
    // Bindings
    //////////////////////////////////////////////////////////////////////////

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UWidgetSwitcher> WS_Menu;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UTextBlock> Watermark;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UTextBlock> BuildConfiguration;

    //////////////////////////////////////////////////////////////////////////
    // Pure Properties
    //////////////////////////////////////////////////////////////////////////

    UFUNCTION(BlueprintPure, Category = "Menu", meta = (AllowPrivateAccess = "true"))
    int32 GetActiveMenuTab( /* void */ ) const { return this->WS_Menu->GetActiveWidgetIndex(); }

protected:

    virtual auto NativeConstruct(void) -> void override;
            auto ConstructWidgetSwitcher(void) const -> void;

    UFUNCTION(BlueprintCallable, Category = "Menu", meta = (AllowPrivateAccess = "true"))
    void OpenMenuTab(const EMenuFrontEndTab::Type MenuTab) const;

    /* Event only called if widget is visible. Not on hidden tab occurrence changes. */
    auto OnNewSessionClicked(void) const -> void;
    /* Event only called if widget is visible. Not on hidden tab occurrence changes. */
    auto OnJoinSessionClicked(void) const -> void;
    auto OnQuitClicked(void) const -> void;
    auto OnQuitClickedDelegate(const bool bAccepted) const -> void;

private:

    void ConstructBuildConfiguration(void) const;
};
