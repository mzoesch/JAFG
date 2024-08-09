// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "TabBar/JAFGTabBar.h"

#include "SettingsTabBar.generated.h"

class UGameSettingRegistry;
class USettingsTabBarPanel;

UCLASS(Abstract, Blueprintable)
class COMMONSETTINGS_API USettingsTabBar : public UJAFGTabBar
{
    GENERATED_BODY()

public:

    explicit USettingsTabBar(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<USettingsTabBarPanel> TabBarPanelClass;

    virtual auto GetDefaultSettingsTabDescriptor(void) const -> FTabBarTabDescriptor;

    virtual void RegisterAllTabs(void) override;

    UPROPERTY()
    TObjectPtr<UGameSettingRegistry> OwningRegistry = nullptr;
};
