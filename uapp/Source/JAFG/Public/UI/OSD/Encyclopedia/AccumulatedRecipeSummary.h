// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "JAFGUserWidget.h"

#include "AccumulatedRecipeSummary.generated.h"

JAFG_VOID

class UJAFGButton;
class URecipeContainerSlot;
class UCanvasPanel;
class UOverlay;
class UTileView;
class UJAFGTextBlock;

struct FAccumulatedRecipeSummaryPassData : public FWidgetPassData
{
    explicit FAccumulatedRecipeSummaryPassData(const FAccumulated& InTargetAccumulated)
        : TargetAccumulated(InTargetAccumulated)
    {
        return;
    }

    FAccumulated TargetAccumulated;
};

/**
 * Summary of one accumulated and possible recipes for it.
 * Has the ability to travel back in time.
 */
UCLASS(Abstract, Blueprintable)
class JAFG_API UAccumulatedRecipeSummary : public UJAFGUserWidget, public TSharedFromThis<UAccumulatedRecipeSummary>
{
    GENERATED_BODY()

public:

    explicit UAccumulatedRecipeSummary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

public:

    // UJAFGUserWidget implementation
    virtual void PassDataToWidget(const FWidgetPassData& UncastedData) override;
    // ~UJAFGUserWidget implementation

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<URecipeContainerSlot> RecipeContainerSlotClass;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGTextBlock> TextBlock_Name;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGTextBlock> TextBlock_CurrentRecipeCounter;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UOverlay> Overlay_CurrentRecipeWrapper;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UCanvasPanel> CanvasPanel_CurrentRecipe;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<URecipeContainerSlot> Slot_ProductSlot;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGButton> Button_PreviousRecipe;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGButton> Button_Close;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGButton> Button_LastRecipe;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGButton> Button_NextRecipe;

    void BuildCurrentRecipe(void);

private:

    void SmartClearOverlayOfCurrentRecipe(void) const;
    void UpdateButtonEnabledStates(const int32 TotalRecipes) const;

    void AddRecipeToStack(const FAccumulated& Accumulated);

    UFUNCTION()
    void OnButtonPreviousRecipeClicked( /* void */ );
    UFUNCTION()
    void OnButtonCloseClicked( /* void */ );
    UFUNCTION()
    void OnButtonLastRecipeClicked( /* void */ );
    UFUNCTION()
    void OnButtonNextRecipeClicked( /* void */ );

    int32 CurrentTargetRecipeCursor { };
    FAccumulated TargetAccumulated;

    struct FRecipeStackEntry
    {
        FAccumulated Accumulated;
        int32 Cursor;
    };
    TArray<FRecipeStackEntry> RecipeHistoryStack;
};
