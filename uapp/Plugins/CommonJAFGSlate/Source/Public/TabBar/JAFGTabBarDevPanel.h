// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGTabBarPanel.h"

#include "JAFGTabBarDevPanel.generated.h"

/**
 * Development panel. Use as a placeholder while concrete derived panels are not yet implemented.
 * Not allowed to be used in production.
 */
UCLASS(Abstract, Blueprintable)
class COMMONJAFGSLATE_API UJAFGTabBarDevPanel : public UJAFGTabBarPanel
{
    GENERATED_BODY()

public:

    explicit UJAFGTabBarDevPanel(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void NativeConstruct(void) override;
};
