// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "HUD/UW_Master.h"

#include "Container.generated.h"

class UTileView;

/**
 * A general-purpose container superclass.
 *
 * Containers may inherit from this class to gain access to common container functionality.
 */
UCLASS(Abstract)
class JAFG_API UW_Container : public UW_Master
{
    GENERATED_BODY()

private:

    bool bDirty;
    
public:

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTileView* CharacterInventorySlots;

    /* TODO We may want to add the quick-slots here as well. */
    
public:
    
    /**
     * Will set the bDirty flag to true and enables a refresh of the container on the next tick.
     *
     * TODO ...
     *      This is kinda a sketchy solution and should be redone.
     *      Why is there not bTickEnabled flag for Widgets, like in AActors?
     */
    FORCEINLINE void MarkAsDirty() { bDirty = true; }
    
protected:
    
    virtual void NativeOnInitialized() override;

    /**
     * The NativeTick method used by this container. Must always be called by derived classes if they
     * implement the NativeTick method defined in UUserWidget.
     */
    virtual void NativeTickImpl(const FGeometry& MyGeometry, const float InDeltaTime, bool& bOutDiscardTick);
    
    /**
     * Should be overriden by derived classes to add custom functionality to the container.
     * But the derived class must always call the parent implementation as well.
     */
    virtual void RefreshCharacterInventory();
};
