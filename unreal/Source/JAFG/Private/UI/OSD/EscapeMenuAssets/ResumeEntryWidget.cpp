// Copyright 2024 mzoesch. All rights reserved.

#include "UI/OSD/EscapeMenuAssets/ResumeEntryWidget.h"

#include "Player/WorldPlayerController.h"

UResumeEntryWidget::UResumeEntryWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UResumeEntryWidget::OnThisTabPressed(void) const
{
    this->Owner->UnfocusAllTabs();

    AWorldPlayerController* WorldPlayerController = Cast<AWorldPlayerController>(this->Owner->GetOwningPlayer());
    check( WorldPlayerController )

    WorldPlayerController->OnToggleEscapeMenu(FInputActionValue());

    return;
}

void UResumeEntryWidget::OnTabPressed(const FString& Identifier)
{
    Super::OnTabPressed(Identifier);
}
