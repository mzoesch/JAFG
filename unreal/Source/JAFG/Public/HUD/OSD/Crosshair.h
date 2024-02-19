// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "HUD/UW_Master.h"

#include "Crosshair.generated.h"

UENUM(BlueprintType)
enum ECrosshairState : uint8
{
    ECS_None,
    ECS_Default,
    ECS_Interaction,
};

UCLASS()
class JAFG_API UW_Crosshair : public UW_Master
{
    GENERATED_BODY()

private:

    UPROPERTY(BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
    TEnumAsByte<ECrosshairState> CrosshairState;

protected:
    
    UFUNCTION(BlueprintImplementableEvent, Category = "Crosshair")
    void OnCrosshairStateChanged();
    
public:

    void SetCrosshairState(const ECrosshairState NewState);
};
