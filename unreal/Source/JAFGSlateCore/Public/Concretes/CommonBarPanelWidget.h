// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGWidget.h"

#include "CommonBarPanelWidget.generated.h"

class UCommonBarWidget;

/**
 * Displays the actual information for the Common Bar Widget.
 */
UCLASS(Abstract, Blueprintable)
class JAFGSLATECORE_API UCommonBarPanelWidget : public UJAFGWidget
{
    GENERATED_BODY()

public:

    explicit UCommonBarPanelWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    friend UCommonBarWidget;

    /** Always called before the Kismet implementation. */
    virtual void OnNativeMadeVisible( /* void */ );
    /** Always called before the Kismet implementation. */
    virtual void OnNativeMadeCollapsed( /* void */ );

    /** Always called after the native implementation. */
    UFUNCTION(BlueprintImplementableEvent, Category = "CommonBarPanelWidget")
    void OnMadeVisible( /* void */ );

    /** Always called after the native implementation. */
    UFUNCTION(BlueprintImplementableEvent, Category = "CommonBarPanelWidget")
    void OnMadeCollapsed( /* void */ );
};
