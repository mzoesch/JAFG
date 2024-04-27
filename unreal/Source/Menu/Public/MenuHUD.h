// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "FrontEnd/CommonHUD.h"

#include "MenuHUD.generated.h"

JAFG_VOID

class UJAFGFrontEnd;

UCLASS(Abstract, Blueprintable)
class MENU_API AMenuHUD : public ACommonHUD
{
    GENERATED_BODY()

public:

    explicit AMenuHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // AActor implementation
    virtual void BeginPlay(void) override;
    // ~AActor implementation

private:

    TObjectPtr<UJAFGFrontEnd> FrontEndWidget;

    UPROPERTY(EditDefaultsOnly, Category = "Widgets", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UJAFGFrontEnd> FrontEndWidgetClass;
};
