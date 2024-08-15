// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "TabBar/JAFGTabBarPanel.h"

#include "EncyclopediaTabBarPanel_Accumulated.generated.h"

JAFG_VOID

class UTileView;

UCLASS(Abstract, Blueprintable)
class JAFG_API UEncyclopediaTabBarPanel_Accumulated : public UJAFGTabBarPanel
{
    GENERATED_BODY()

public:

    explicit UEncyclopediaTabBarPanel_Accumulated(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UTileView> TileView_Accumulated;
};
