// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "HUD_Master.generated.h"

class UUW_HotbarSelector;
class UUW_Master;
class UUW_Hotbar;
class UUW_HotbarSlot;

UCLASS()
class JAFG_API AHUD_Master : public AHUD
{
    GENERATED_BODY()

public:

    AHUD_Master();

public:
    
    virtual void BeginPlay() override;
    virtual void Tick(const float DeltaSeconds)	override;

public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    class TSubclassOf<UUW_Master> UWCrosshairClass;
    class UUW_Master* UWCrosshair;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    class TSubclassOf<UUW_Hotbar> UWHotbarClass;
    class UUW_Hotbar* UWHotbar;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    class TSubclassOf<UUW_HotbarSlot> UWHotbarSlotClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    class TSubclassOf<UUW_HotbarSelector> UWHotbarSelectorClass;

public:

    void OnQuickSlotSelect() const;
    
};
