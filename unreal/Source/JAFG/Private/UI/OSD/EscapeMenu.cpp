// Copyright 2024 mzoesch. All rights reserved.

#include "UI/OSD/EscapeMenu.h"

#include "Player/WorldPlayerController.h"

UEscapeMenu::UEscapeMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UEscapeMenu::NativeConstruct(void)
{
    Super::NativeConstruct();

    AWorldPlayerController* WorldPlayerController = Cast<AWorldPlayerController>(this->GetOwningPlayer());

    if (WorldPlayerController == nullptr)
    {
#if WITH_EDITOR
        LOG_ERROR(LogCommonSlate, "Owning Player controller is not of type AWorldPlayerController but HUD decided to spawn this widget anyway.")
#else /* WITH_EDITOR */
        LOG_FATAL(LogCommonSlate, "Owning Player controller is invalid.")
#endif /* !WITH_EDITOR */
    }

    this->EscapeMenuVisibilityChangedDelegateHandle = WorldPlayerController->SubscribeToEscapeMenuVisibilityChanged(ADD_SLATE_VIS_DELG(UEscapeMenu::OnEscapeMenuVisibilityChanged));

    return;
}

void UEscapeMenu::NativeDestruct(void)
{
    Super::NativeDestruct();

    AWorldPlayerController* WorldPlayerController = Cast<AWorldPlayerController>(this->GetOwningPlayer());

    if (WorldPlayerController == nullptr)
    {
        return;
    }

    if (WorldPlayerController->UnSubscribeToEscapeMenuVisibilityChanged(this->EscapeMenuVisibilityChangedDelegateHandle) == false)
    {
        LOG_ERROR(LogCommonSlate, "Failed to unsubscribe from Escape Menu Visibility Changed event.")
    }

    return;
}

void UEscapeMenu::OnEscapeMenuVisibilityChanged(const bool bVisible)
{
    if (bVisible)
    {
        this->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        this->SetVisibility(ESlateVisibility::Collapsed);
    }

    return;
}
