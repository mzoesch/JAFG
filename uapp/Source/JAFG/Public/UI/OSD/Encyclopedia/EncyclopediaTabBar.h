// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "TabBar/JAFGTabBar.h"

#include "EncyclopediaTabBar.generated.h"

JAFG_VOID

UCLASS(Abstract, Blueprintable)
class JAFG_API UEncyclopediaTabBar : public UJAFGTabBar
{
    GENERATED_BODY()

public:

    explicit UEncyclopediaTabBar(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UJAFGTabBarBase> AccumulatedPanelWidgetClass;

    // UJAFGTabBar implementation
    virtual auto RegisterAllTabs(void) -> void override;
    // ~UJAFGTabBar implementation
};
