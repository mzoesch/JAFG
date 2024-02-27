// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "UW_Master.generated.h"

/**
 * The class that all HUD widgets must inherit from.
 *
 * Functionality will be added here later on.
 */
UCLASS()
class JAFG_API UW_Master : public UUserWidget
{
    GENERATED_BODY()

public:
    FString ToString();

    virtual void SetVisibility(const ESlateVisibility InVisibility) override;
};
