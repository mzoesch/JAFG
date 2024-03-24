// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetSwitcher.h"
#include "UI/JAFGCommonWidget.h"

#include "JAFGFrontEnd.generated.h"

class UMenuJoinSessionFrontEnd;
class UMenuNewSessionFrontEnd;
class UButton;
class UWidgetSwitcher;
class UTextBlock;

UENUM(BlueprintType)
enum class EMenuTab : uint8
{
    Invalid         = 0,
    NewSession      = 1,
    JoinSession     = 2,
    OpenEditor      = 3,
    Options         = 4,
    Credits         = 5,
    Quit            = 6,
};

UCLASS(Abstract, Blueprintable)
class JAFG_API UJAFGFrontEnd : public UJAFGCommonWidget
{
    GENERATED_BODY()

private:

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    UWidgetSwitcher* WS_Menu;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    UTextBlock* Watermark;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    UTextBlock* BuildConfiguration;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    UMenuNewSessionFrontEnd* WB_TabNewSession;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    UMenuJoinSessionFrontEnd* WB_TabJoinSession;

    UFUNCTION(BlueprintPure, Category = "Menu", meta = (AllowPrivateAccess = "true"))
    int32 GetActiveMenuTab() const { return this->WS_Menu->GetActiveWidgetIndex(); }
    
protected:

    virtual void NativeConstruct(void) override;

    UFUNCTION(BlueprintCallable, Category = "Menu", meta = (AllowPrivateAccess = "true"))
    void OpenMenuTab(const EMenuTab MenuTab) const;

    /* Event only called if widget is visible. Not on hidden tab occurrence changes. */
    void OnNewSessionClicked(void) const;
    /* Event only called if widget is visible. Not on hidden tab occurrence changes. */
    void OnJoinSessionClicked(void) const;
    
private:

    void ConstructBuildConfiguration(void) const;
};
