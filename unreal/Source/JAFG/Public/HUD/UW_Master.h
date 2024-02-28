// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "UW_Master.generated.h"

/** The class that all HUD widgets must inherit from. */
UCLASS(Abstract)
class JAFG_API UW_Master : public UUserWidget
{
    GENERATED_BODY()

public:
    
    FORCEINLINE virtual void SetVisibility(const ESlateVisibility InVisibility) override { this->SetVisibility(InVisibility, false); }
    /**
     * @param bVisibilityIsCustom An extra safety parameter. Always call the SetVisibility method declared in the
     *                            Unreal UWidget class. This method should only be used if there is a special custom
     *                            widget that needs to be crated and it uses a non standard visibility.
     */
    void SetVisibility(const ESlateVisibility InVisibility, const bool bVisibilityIsCustom);
};
