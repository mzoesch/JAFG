// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "TabBar/JAFGTabBar.h"

#include "JAFGFrontEnd.generated.h"

class UVerticalBox;
JAFG_VOID

class UScrollBox;
class ULocalSaveEntry;

UCLASS(Abstract, Blueprintable)
class JAFG_API UJAFGFrontEnd : public UJAFGTabBar
{
    GENERATED_BODY()

public:

    explicit UJAFGFrontEnd(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UVerticalBox> VerticalBox_ErrorMessages;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UJAFGTabBarBase> HostSessionPanelWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UJAFGTabBarBase> JoinSessionPanelWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UJAFGTabBarBase> EncyclopediaPanelWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UJAFGTabBarBase> SettingsPanelWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UJAFGTabBarBase> GamePluginOverviewPanelWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UJAFGTabBarButton> QuitGameButtonWidgetClass;

    virtual void RegisterAllTabs(void) override;
};
