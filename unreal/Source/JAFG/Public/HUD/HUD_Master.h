// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "HUD_Master.generated.h"

enum ECrosshairState : uint8;
class UW_Crosshair;
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

private:

    /**
     * Visible containers are stored here. The FString is an unique identifier for the container.
     * So the container can be found by its name and be toggled or completely removed.
     */
    static inline TMap<FString, UW_Master*> VisibleContainers;
    
public:
    
    virtual void BeginPlay() override;

#pragma region Sub Classes
    
public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    TSubclassOf<UW_Crosshair> UWCrosshairClass;

    UPROPERTY()
    UW_Crosshair* UWCrosshair;

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

#pragma region OSD

void UpdateCrosshair(const ECrosshairState CrosshairState) const;
    
#pragma endregion OSD

#pragma region Containers

    void AddCharacterInventoryContainer(FString& OutContainerIdentifier);
    
    /**
     * Will add a new widget container to the Viewport.
     * The method will mark the visibility of the newly created instance as collapsed.
     */
    void AddContainer(FString& OuContainerIdentifier, const TSubclassOf<UW_Master> Container);
    /**
     * Only use this method sparingly. It is better to use RemoveContainer() instead. We do not want to have a
     * large amount of containers in memory.
     * Only high traffic containers should be toggled.
     */
    void ToggleContainer(const FString& ContainerIdentifier, const bool bVisible);
    /** Will always implicitly toggle the container off before removing it from the viewport. */
    void RemoveContainer(const FString& ContainerIdentifier, bool& bOutSuccess, FString& OutError);

    void SafeDestroyCursorPreview();
    
#pragma endregion Containers
    
#pragma region Player Inventory
    
public:

    // TODO REDO ALL THIS METHODS
    
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
