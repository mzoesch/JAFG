// Copyright 2024 mzoesch. All rights reserved.

#include "UI/OSD/Encyclopedia/AccumulatedRecipeSummary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/JAFGButton.h"
#include "Components/JAFGTextBlock.h"
#include "Components/Overlay.h"
#include "Components/TileView.h"
#include "System/VoxelSubsystem.h"
#include "UI/OSD/Encyclopedia/RecipeContainerSlot.h"

UAccumulatedRecipeSummary::UAccumulatedRecipeSummary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UAccumulatedRecipeSummary::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->Button_PreviousRecipe->OnClicked.AddDynamic(this, &UAccumulatedRecipeSummary::OnButtonPreviousRecipeClicked);
    this->Button_Close->OnClicked.AddDynamic(this, &UAccumulatedRecipeSummary::OnButtonCloseClicked);
    this->Button_LastRecipe->OnClicked.AddDynamic(this, &UAccumulatedRecipeSummary::OnButtonLastRecipeClicked);
    this->Button_NextRecipe->OnClicked.AddDynamic(this, &UAccumulatedRecipeSummary::OnButtonNextRecipeClicked);

    this->Slot_ProductSlot->OnRecipeContainerSlotClicked.BindLambda( [this] (const FAccumulated& Accumulated) -> bool
    {
        if (Accumulated.IsNull())
        {
            return false;
        }

        this->AddRecipeToStack(Accumulated);
        return true;
    });

    return;
}

void UAccumulatedRecipeSummary::PassDataToWidget(const FWidgetPassData& UncastedData)
{
    CAST_PASSED_DATA(FAccumulatedRecipeSummaryPassData)
    {
        this->TargetAccumulated = Data->TargetAccumulated;
    }

    jcheck( this->TargetAccumulated.IsNull() == false && this->TargetAccumulated.IsAir() == false )

    this->CurrentTargetRecipeCursor = 0;
    this->BuildCurrentRecipe();

    return;
}

void UAccumulatedRecipeSummary::BuildCurrentRecipe(void)
{
    const IRecipeSubsystem* RecipeSubsystem = IRecipeSubsystem::Get(this);
    TArray<const FRecipe*>  Recipes         = RecipeSubsystem->GetRecipesForAccumulated(this->TargetAccumulated);

    if (this->CurrentTargetRecipeCursor < 0)
    {
        this->CurrentTargetRecipeCursor = Recipes.Num() - 1;
    }
    else if (this->CurrentTargetRecipeCursor >= Recipes.Num())
    {
        this->CurrentTargetRecipeCursor = 0;
    }

    this->CanvasPanel_CurrentRecipe->ClearChildren();
    this->SmartClearOverlayOfCurrentRecipe();

    this->UpdateButtonEnabledStates(Recipes.Num());

    this->TextBlock_Name->SetText(FText::FromString(this->TargetAccumulated.GetDisplayName(this)));
    this->TextBlock_CurrentRecipeCounter->SetText(FText::FromString(FString::Printf(
        TEXT("%d/%d"),
        this->CurrentTargetRecipeCursor,
        Recipes.Num()
    )));

    if (Recipes.Num() == 0)
    {
        this->CanvasPanel_CurrentRecipe->SetVisibility(ESlateVisibility::Collapsed);
        UJAFGTextBlock* TextBlock = WidgetTree->ConstructWidget<UJAFGTextBlock>(UJAFGTextBlock::StaticClass());
        TextBlock->SetText(FText::FromString(TEXT("No recipe available.")));
        TextBlock->SetColorScheme(EJAFGFontSize::SubHeader);

        this->Overlay_CurrentRecipeWrapper->AddChild(TextBlock);
        this->Slot_ProductSlot->SetVisibility(ESlateVisibility::Collapsed);

        return;
    }

    this->CanvasPanel_CurrentRecipe->SetVisibility(ESlateVisibility::Visible);
    this->Slot_ProductSlot->SetVisibility(ESlateVisibility::Visible);

    check( Recipes.IsValidIndex(this->CurrentTargetRecipeCursor) )
    const FRecipe* Recipe = Recipes[this->CurrentTargetRecipeCursor];

    /* Just copied for faster access. The compiler will compile that out :). */
    const int32 RWidth      = Recipe->IsShapeless() ? 3 : Recipe->Delivery.Width;
    const int32 TargetCount = FMath::Pow(static_cast<float>(RWidth), 2);

    constexpr int32 HSize = 32;
    constexpr int32 VSize = 32;

    /* How much we have added. */
    int32 RCount = 0;

    for (const FAccumulated& RAccumulated : Recipe->Delivery.Contents)
    {
        URecipeContainerSlot* NewSlot = CreateWidget<URecipeContainerSlot>(this->GetWorld(), this->RecipeContainerSlotClass);

        URecipeContainerSlotData* SlotData = NewObject<URecipeContainerSlotData>(this);
        SlotData->Content = RAccumulated;
        SlotData->Owner   = this;
        if (RAccumulated.IsNull() == false)
        {
            NewSlot->OnRecipeContainerSlotClicked.BindLambda( [this] (const FAccumulated& Accumulated) -> bool
            {
                this->PassDataToWidget(FAccumulatedRecipeSummaryPassData(Accumulated));
                return true;
            });
        }
        NewSlot->SetSlotData(SlotData);

        NewSlot->OnRecipeContainerSlotClicked.BindLambda( [this] (const FAccumulated& Accumulated) -> bool
        {
            this->AddRecipeToStack(Accumulated);
            return true;
        });

        UCanvasPanelSlot* CSlot = this->CanvasPanel_CurrentRecipe->AddChildToCanvas(NewSlot);
        CSlot->SetAnchors(FAnchors(0.0f, 0.0f, 0.0f, 0.0f));
        CSlot->SetOffsets(FMargin(HSize * (RCount % RWidth), HSize * static_cast<int>(RCount / RWidth), 0, 0));
        CSlot->SetSize(FVector2D(HSize, VSize));

        ++RCount;

        continue;
    }

    /* Add empty slots, to make it look nice (like a square). */
    for (; RCount < TargetCount; ++RCount)
    {
        URecipeContainerSlot* NewSlot = CreateWidget<URecipeContainerSlot>(this->GetWorld(), this->RecipeContainerSlotClass);

        URecipeContainerSlotData* SlotData = NewObject<URecipeContainerSlotData>(this);
        SlotData->Content = ::Accumulated::Null;
        SlotData->Owner   = this;
        NewSlot->SetSlotData(SlotData);

        UCanvasPanelSlot* CSlot = this->CanvasPanel_CurrentRecipe->AddChildToCanvas(NewSlot);
        CSlot->SetAnchors(FAnchors(0.0f, 0.0f, 0.0f, 0.0f));
        CSlot->SetOffsets(FMargin(HSize * (RCount % RWidth), HSize * static_cast<int>(RCount / RWidth), 0, 0));
        CSlot->SetSize(FVector2D(HSize, VSize));

        continue;
    }

    URecipeContainerSlotData* ProductSlotData = NewObject<URecipeContainerSlotData>(this);
    ProductSlotData->Content = Recipe->Product.Product;
    ProductSlotData->Owner   = this;
    this->Slot_ProductSlot->SetSlotData(ProductSlotData);

    return;
}

void UAccumulatedRecipeSummary::SmartClearOverlayOfCurrentRecipe(void) const
{
    for (UWidget* Child : this->Overlay_CurrentRecipeWrapper->GetAllChildren())
    {
        if (Child->IsA<UJAFGTextBlock>())
        {
            Child->RemoveFromParent();
        }

        continue;
    }

    return;
}

void UAccumulatedRecipeSummary::UpdateButtonEnabledStates(const int32 TotalRecipes) const
{
    this->Button_PreviousRecipe->SetIsEnabled(this->RecipeHistoryStack.IsEmpty() == false);

    this->Button_LastRecipe->SetIsEnabled(TotalRecipes > 1);
    this->Button_NextRecipe->SetIsEnabled(TotalRecipes > 1);

    return;
}

void UAccumulatedRecipeSummary::AddRecipeToStack(const FAccumulated& Accumulated)
{
    if (Accumulated == this->TargetAccumulated)
    {
        return;
    }

    this->RecipeHistoryStack.Push({ this->TargetAccumulated, this->CurrentTargetRecipeCursor });

    this->CurrentTargetRecipeCursor = 0;
    this->TargetAccumulated         = Accumulated;

    this->BuildCurrentRecipe();

    return;
}

void UAccumulatedRecipeSummary::OnButtonPreviousRecipeClicked(void)
{
    jcheck( this->RecipeHistoryStack.IsEmpty() == false )

    auto [Accumulated, StackCursor]      = this->RecipeHistoryStack.Pop();
    this->CurrentTargetRecipeCursor = StackCursor;
    this->TargetAccumulated         = Accumulated;

    this->BuildCurrentRecipe();

    return;
}

void UAccumulatedRecipeSummary::OnButtonCloseClicked(void)
{
    this->RemoveFromParent();
}

void UAccumulatedRecipeSummary::OnButtonLastRecipeClicked(void)
{
    --this->CurrentTargetRecipeCursor;
    this->BuildCurrentRecipe();
    return;
}

void UAccumulatedRecipeSummary::OnButtonNextRecipeClicked(void)
{
    ++this->CurrentTargetRecipeCursor;
    this->BuildCurrentRecipe();
    return;
}
