// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/CH_Master.h"
#include "GameFramework/HUD.h"

#include "HUD_Master.generated.h"

class UW_Container;
struct FInputActionValue;
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
     * Known containers are stored here. The FString is an unique identifier for the container.
     * So the container can be found by its name and be toggled or completely removed.
     */
    static inline TMap<FString, UW_Container*> KnownContainers;
    
public:
    
    virtual void BeginPlay(void) override;

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

public:
    
    void UpdateCrosshair(const ECrosshairState CrosshairState) const;
    
#pragma endregion OSD

#pragma region Containers

private:
    
    FString CurrentOpenContainer;

public:

    FORCEINLINE FString GetCurrentOpenContainer() const { return this->CurrentOpenContainer; }
    FORCEINLINE bool IsContainerOpen() const { return this->CurrentOpenContainer.IsEmpty() == false; }
    
public:
    
    void AddCharacterInventoryContainer(FString& OutContainerIdentifier);
    
    /**
     * Will add a new widget container to the Viewport.
     * The method will mark the visibility of the newly created instance as collapsed.
     */
    void AddContainer(FString& OuContainerIdentifier, const TSubclassOf<UW_Container> Container);

private:
    
    //////////////////////////////////////////////////////////////////////////
    // All toggle container methods should never be called directly
    // but always through the owing player controller.
    // As there are some side effects that must always be handled.
    //////////////////////////////////////////////////////////////////////////
    
    /**
     * Only use this method sparingly. It is better to use RemoveContainer() instead. We do not want to have a
     * large amount of containers in memory.
     * Only high traffic containers should be toggled.
     */
    void ToggleContainer(const FString& ContainerIdentifier, const bool bVisible);
    /** Will always implicitly toggle the container off before removing it from the viewport. */
    void RemoveContainer(const FString& ContainerIdentifier, bool& bOutSuccess, FString& OutError);
    
    friend void APC_Master::TransitToContainerState(const FString& Identifier, const bool bOpen, const bool bDestroy);
    
#pragma endregion Containers
    
#pragma region Character Inventory
    
private:
    
    /**
     * Will also clear the data of the cursor hand in the owing character.
     * Useful when containers are being closed as the cursor preview must be added back to a container.
     */
    void SafeDestroyCursorPreview();
    /** Must always called implicitly by all refresh methods. */
    void RefreshCursorPreview();

public:

    void RefreshCharacterHotbarSelector();
    void RefreshCharacterHotbar();
    void RefreshCharacterInventory();
    /**
     * @param bLazyUpdate If true, the method will only mark the containers as dirty that are visible.
     *                    False should only be used for a forced full refresh of the containers.
     */
    void RefreshCharacterInventoryAndHotbar(const bool bLazyUpdate = true);

#pragma endregion Character Inventory

public:

    void OnDebugScreenToggle();
};
