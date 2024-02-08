// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "HUD_Master.generated.h"

class UW_AccumulatedCursorPreview;
class UW_HotbarSelector;
class UW_Master;
class UW_Hotbar;
class UW_HotbarSlot;
class UW_PlayerInventory;
class UW_DebugScreen;

UCLASS()
class JAFG_API AHUD_Master : public AHUD
{
    GENERATED_BODY()

public:

    AHUD_Master();

public:
    
    virtual void BeginPlay() override;
    virtual void Tick(const float DeltaSeconds)	override;

#pragma region Sub Classes
    
public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    TSubclassOf<UW_Master> UWCrosshairClass;

    UPROPERTY()
    UW_Master* UWCrosshair;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    TSubclassOf<UW_Hotbar> UWHotbarClass;

    UPROPERTY()
    UW_Hotbar* UWHotbar;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    TSubclassOf<UW_HotbarSlot> UWHotbarSlotClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    TSubclassOf<UW_HotbarSelector> UWHotbarSelectorClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    TSubclassOf<UW_PlayerInventory> UWPlayerInventoryClass;

    UPROPERTY()
    UW_PlayerInventory* UWPlayerInventory;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    TSubclassOf<UW_AccumulatedCursorPreview> UWAccumulatedCursorPreviewClass;

    UPROPERTY()
    UW_AccumulatedCursorPreview* AccumulatedCursorPreview;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    TSubclassOf<UW_DebugScreen> UWDebugScreenClass;

    UPROPERTY()
    UW_DebugScreen* UWDebugScreen;

#pragma endregion Sub Classes
    
#pragma region Player Inventory
    
public:

    void UpdateInventoryAndHotbar();
    
    void OnInventorySlotSelect();
    /** Updates the position of the hotbar selector. */
    void OnQuickSlotSelect() const;
    void OnHotbarUpdate() const;
    void OnInventoryToggle(const bool bOpen);

#pragma endregion Player Inventory

public:

    void OnDebugScreenToggle();
};
