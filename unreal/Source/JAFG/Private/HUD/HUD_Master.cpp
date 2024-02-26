// © 2023 mzoesch. All rights reserved.

#include "HUD/HUD_Master.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "Core/CH_Master.h"
#include "HUD/Assets/AccumulatedCursorPreview.h"
#include "HUD/Container/PlayerInventory.h"
#include "HUD/Container/Hotbar.h"
#include "HUD/Container/Slots/HotbarSlot.h"
#include "HUD/OSD/Crosshair.h"
#include "HUD/OSD/DebugScreen.h"

#define UIL_LOG(Verbosity, Format, ...)     UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)
#define CHARACTER                           Cast<ACH_Master>(this->GetOwningPawn())
#define WORLD_CONTEXT                       this->GetWorld()

AHUD_Master::AHUD_Master()
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->UWCrosshair               = nullptr;
    this->UWHotbar                  = nullptr;

    this->UWPlayerInventory         = nullptr;
    this->AccumulatedCursorPreview  = nullptr;

    this->UWDebugScreen             = nullptr;

    this->VisibleContainers.Empty();
    
    return;
}

void AHUD_Master::BeginPlay()
{
    Super::BeginPlay();

#if WITH_EDITOR
    if (this->GetOwningPawn() == nullptr || this->GetOwningPawn()->GetClass()->IsChildOf(ACH_Master::StaticClass()) == false)
    {
        UIL_LOG(Error, TEXT("AHUD_Master::BeginPlay(): Owning pawn is not a valid type for this HUD. Discarding HUD setup."))
        return;
    }
#else
    if (this->GetOwningPawn() == nullptr || this->GetOwningPawn()->GetClass()->IsChildOf(ACH_Master::StaticClass()) == false)
    {
        UIL_LOG(Fatal, TEXT("AHUD_Master::BeginPlay(): Owning pawn is not a valid type for this HUD. Discarding HUD setup."))
        return;
    }
#endif /* WITH_EDITOR */

    this->VisibleContainers.Empty();
    
    check(this->UWCrosshairClass)
    this->UWCrosshair = CreateWidget<UW_Crosshair>(WORLD_CONTEXT, this->UWCrosshairClass);
    this->UWCrosshair->AddToViewport();

    check(this->UWHotbarClass)
    this->UWHotbar = CreateWidget<UW_Hotbar>(WORLD_CONTEXT, this->UWHotbarClass);
    this->UWHotbar->AddToViewport();

    check(this->UWHotbarSlotClass)
    this->UWHotbar->InitializeHotbar(this->UWHotbarSlotClass, this->UWHotbarSelectorClass);
    
    check(this->UWPlayerInventoryClass)
    check(this->UWAccumulatedCursorPreviewClass)
    /* Added dirty later. */
    
    return;
}

#pragma region OSD

void AHUD_Master::UpdateCrosshair(const ECrosshairState CrosshairState) const
{
    this->UWCrosshair->SetCrosshairState(CrosshairState);
    return;
}

void AHUD_Master::AddCharacterInventoryContainer(FString& OutContainerIdentifier)
{
    this->AddContainer(OutContainerIdentifier, this->UWPlayerInventoryClass);
    this->UWPlayerInventory = Cast<UW_PlayerInventory>(this->VisibleContainers[OutContainerIdentifier]);
    return;
}

#pragma endregion OSD

#pragma region Containers

void AHUD_Master::AddContainer(FString& OuContainerIdentifier, const TSubclassOf<UW_Master> Container)
{
    if (OuContainerIdentifier.IsEmpty() == false)
    {
        UIL_LOG(Fatal, TEXT("AHUD_Master::ToggleContainer: Container identifier is not empty. Value: %s."), *OuContainerIdentifier)
        return;
    }

    if (Container == nullptr)
    {
        UIL_LOG(Fatal, TEXT("AHUD_Master::ToggleContainer: Container class is null."))
        return;
    }

    OuContainerIdentifier = FGuid::NewGuid().ToString();
    UE_LOG(LogTemp, Warning, TEXT("AHUD_Master::ToggleContainer: Generating new identifier. Value: %s."), *OuContainerIdentifier)

    if (this->VisibleContainers.Contains(OuContainerIdentifier))
    {
        UIL_LOG(Fatal, TEXT("AHUD_Master::ToggleContainer: Container identifier already exists. Value: %s."), *OuContainerIdentifier)
        return;
    }

    UW_Master* ContainerPtr = CreateWidget<UW_Master>(WORLD_CONTEXT, Container);
    
    this->VisibleContainers.Add(OuContainerIdentifier, ContainerPtr);

    ContainerPtr->AddToViewport();
    ContainerPtr->SetVisibility(ESlateVisibility::Collapsed);

    return;
}

void AHUD_Master::ToggleContainer(const FString& ContainerIdentifier, const bool bVisible)
{
    if (ContainerIdentifier.IsEmpty())
    {
        UIL_LOG(Fatal, TEXT("AHUD_Master::ToggleContainer: Container identifier is empty."))
        return;
    }

    if (this->VisibleContainers.Contains(ContainerIdentifier) == false)
    {
        UIL_LOG(Fatal, TEXT("AHUD_Master::ToggleContainer: Container identifier does not exist. Value: %s."), *ContainerIdentifier)
        return;
    }
    
    UW_Master* ContainerPtr = this->VisibleContainers[ContainerIdentifier];
    
    ContainerPtr->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

    if (bVisible == false)
    {
        this->SafeDestroyCursorPreview();
        return;
    }
    
    if (UW_Container* TrueContainerPtr = Cast<UW_Container>(ContainerPtr))
    {
        TrueContainerPtr->MarkAsDirty();
    }
    
    return;
}

void AHUD_Master::RemoveContainer(const FString& ContainerIdentifier, bool& bOutSuccess, FString& OutError)
{
    UIL_LOG(Error, TEXT("AHUD_Master::RemoveContainer: Not implemented."))
}

void AHUD_Master::SafeDestroyCursorPreview()
{
    if (this->AccumulatedCursorPreview == nullptr)
    {
        return;
    }

    CHARACTER->ClearCursorHand(false);
    
    this->AccumulatedCursorPreview->RemoveFromParent();
    this->AccumulatedCursorPreview = nullptr;

    return;
}

#pragma endregion Containers

#pragma region Player Inventory

void AHUD_Master::UpdateInventoryAndHotbar()
{
    if (this->AccumulatedCursorPreview != nullptr)
    {
        this->AccumulatedCursorPreview->RemoveFromParent();
        this->AccumulatedCursorPreview = nullptr;
    }

    if (CHARACTER->CursorHand != FAccumulated::NullAccumulated)
    {
        this->AccumulatedCursorPreview = Cast<UW_AccumulatedCursorPreview>(CreateWidget(this->GetWorld(), this->UWAccumulatedCursorPreviewClass));
        this->AccumulatedCursorPreview->AddToViewport();
    }
    
    this->UWHotbar->OnHotbarUpdate();
    this->UWPlayerInventory->MarkAsDirty();

    return;
}

void AHUD_Master::OnInventorySlotSelect()
{
    if (this->AccumulatedCursorPreview != nullptr)
    {
        this->AccumulatedCursorPreview->RemoveFromParent();
        this->AccumulatedCursorPreview = nullptr;
    }
    
    if (CHARACTER->CursorHand == FAccumulated::NullAccumulated)
    {
        this->UWPlayerInventory->MarkAsDirty();
        return;
    }
    
    this->AccumulatedCursorPreview = Cast<UW_AccumulatedCursorPreview>(CreateWidget(this->GetWorld(), this->UWAccumulatedCursorPreviewClass));
    this->AccumulatedCursorPreview->AddToViewport();
    
    this->UWPlayerInventory->MarkAsDirty();

    return;
}

void AHUD_Master::OnQuickSlotSelect() const
{
    this->UWHotbar->OnSlotSelect();
    return;
}

void AHUD_Master::OnHotbarUpdate() const
{
    this->UWHotbar->OnHotbarUpdate();
}

void AHUD_Master::OnInventoryToggle(const bool bOpen)
{
    if (bOpen == false)
    {
        this->UWPlayerInventory->SetVisibility(ESlateVisibility::Hidden);

        if (this->AccumulatedCursorPreview != nullptr)
        {
            this->AccumulatedCursorPreview->RemoveFromParent();
            this->AccumulatedCursorPreview = nullptr;
        }
        
        /* Just a backup if something fishy was going on, so that the inventory state and HUD is all synced up. */
        this->OnHotbarUpdate();
        
        return;
    }

    this->UWPlayerInventory->SetVisibility(ESlateVisibility::Visible);
    this->UWPlayerInventory->MarkAsDirty();
    
    return;
}

void AHUD_Master::OnDebugScreenToggle()
{
    if (this->UWDebugScreen == nullptr)
    {
        check(this->UWDebugScreenClass)
        this->UWDebugScreen = CreateWidget<UW_DebugScreen>(this->GetWorld(), this->UWDebugScreenClass);
        this->UWDebugScreen->AddToViewport();

        return;
    }

    this->UWDebugScreen->RemoveFromParent();
    this->UWDebugScreen = nullptr;

    return;
}

#pragma endregion Player Inventory

#undef UIL_LOG
#undef CHARACTER
#undef WORLD_CONTEXT
