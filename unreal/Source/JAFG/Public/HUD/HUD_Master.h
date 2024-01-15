// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "HUD_Master.generated.h"

class UUW_HotbarSelector;
class UUW_Master;
class UUW_Hotbar;
class UUW_HotbarSlot;
class UUW_Inventory;
class UUW_DebugScreen;

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

    UPROPERTY()
    class UUW_Master* UWCrosshair;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    class TSubclassOf<UUW_Hotbar> UWHotbarClass;

    UPROPERTY()
    class UUW_Hotbar* UWHotbar;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    class TSubclassOf<UUW_HotbarSlot> UWHotbarSlotClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    class TSubclassOf<UUW_HotbarSelector> UWHotbarSelectorClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    class TSubclassOf<UUW_Inventory> UWInventoryClass;

    UPROPERTY()
    class UUW_Inventory* UWInventory;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    class TSubclassOf<UUW_DebugScreen> UWDebugScreenClass;

    UPROPERTY()
    class UUW_DebugScreen* UWDebugScreen;
    
public:

    void OnQuickSlotSelect() const;
    void OnHotbarUpdate();
    void OnInventoryToggle(const bool bOpen) const;
    void OnDebugScreenToggle();

};
