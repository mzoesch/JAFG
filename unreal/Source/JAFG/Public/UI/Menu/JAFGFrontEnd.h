// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/JAFGCommonWidget.h"

#include "JAFGFrontEnd.generated.h"

class UMenuNewSessionFrontEnd;
class UButton;
class UWidgetSwitcher;
class UTextBlock;

UENUM()
enum class EMenuTab : uint8
{
    Invalid         = 0,
    NewSession      = 1,
    JoinSession     = 2,
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
    
protected:

    virtual void NativeConstruct(void) override;

    UFUNCTION(BlueprintCallable, Category = "Menu", meta = (AllowPrivateAccess = "true"))
    void OpenMenuTab(const EMenuTab MenuTab) const;

    /* Event only called if widget is visible. Not on hidden tab occurrence changes. */
    void OnNewSessionClicked(void) const;
    /* Event only called if widget is visible. Not on hidden tab occurrence changes. */
    void OnJoinSessionClicked(void) const { };
    
private:

    void ConstructBuildConfiguration(void) const;
};
