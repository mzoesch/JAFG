// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Concretes/CommonBarPanelWidget.h"
#include "Concretes/CommonBarWidget.h"

#include "SettingsPanelWidget.generated.h"

JAFG_VOID

UCLASS(Abstract, Blueprintable)
class JAFG_API USettingsPanelWidget : public UCommonBarPanelWidget
{
    GENERATED_BODY()

public:

    explicit USettingsPanelWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};

UCLASS(Abstract, Blueprintable)
class JAFG_API USettingsBarPanelWidget : public UCommonBarWidget
{
    GENERATED_BODY()

public:

    explicit USettingsBarPanelWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UCommonBarEntryWidget> DefaultEntryWidget;

    void RegisterAllTabs(void);
};
