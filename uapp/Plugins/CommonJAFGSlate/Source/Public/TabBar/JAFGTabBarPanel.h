// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGTabBarBase.h"

#include "JAFGTabBarPanel.generated.h"

/**
 * The information about the tab.
 * @see UJAFGTabBar.h
 */
UCLASS(Abstract, NotBlueprintable)
class COMMONJAFGSLATE_API UJAFGTabBarPanel : public UJAFGTabBarBase
{
    GENERATED_BODY()

public:

    explicit UJAFGTabBarPanel(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual auto OnNativeMadeVisible(void) -> void override { }
    virtual auto OnNativeMadeCollapsed(void) -> void override { }
};
