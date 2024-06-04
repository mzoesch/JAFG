// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "CommonJAFGSlateDeveloperSettings.generated.h"

class UJAFGUserWidget;
class ULoadingScreen;
class UJAFGWarningPopUpYesNo;
class UJAFGWarningPopUp;
class UJAFGTabBarBase;
class UJAFGTabBarButton;

UCLASS(Config=JAFG, DefaultConfig, meta = (DisplayName="JAFG Slate Settings"))
class COMMONJAFGSLATE_API UCommonJAFGSlateDeveloperSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:

    explicit UCommonJAFGSlateDeveloperSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Slate")
    const TSubclassOf<UJAFGTabBarButton> DefaultTabBarButtonWidgetClass;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Slate")
    const TSubclassOf<UJAFGTabBarBase> DefaultTabBarPanelWidgetClass;

    UPROPERTY(Config, EditAnywhere,  BlueprintReadOnly, Category = "PopUp")
    TSubclassOf<UJAFGWarningPopUp> WarningPopUpWidgetClass;

    UPROPERTY(Config, EditAnywhere,  BlueprintReadOnly, Category = "PopUp")
    TSubclassOf<UJAFGWarningPopUpYesNo> WarningPopUpYesNoWidgetClass;

    UPROPERTY(Config, EditAnywhere,  BlueprintReadOnly, Category = "PopUp")
    TSubclassOf<ULoadingScreen> LoadingScreenWidgetClass;

    UPROPERTY(Config, EditAnywhere,  BlueprintReadOnly, Category = "PopUp")
    TSubclassOf<UJAFGUserWidget> ContainerValueCursorWidgetClass;
};
