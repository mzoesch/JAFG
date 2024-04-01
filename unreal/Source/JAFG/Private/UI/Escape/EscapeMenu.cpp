// Copyright 2024 mzoesch. All rights reserved.

#include "UI/Escape/EscapeMenu.h"

#include "InputActionValue.h"
#include "Components/Button.h"
#include "World/WorldCharacter.h"

#define OWNING_CHARACTER Cast<AWorldCharacter>(this->GetOwningPlayerPawn())

void UEscapeMenu::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->ToggleEscapeMenu(true);

    this->B_Resume->OnClicked.AddDynamic(this, &UEscapeMenu::OnResumeClicked);

    return;
}

/** Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void UEscapeMenu::OnResumeClicked()
{
    OWNING_CHARACTER->OnToggleEscapeMenu(FInputActionValue());
}

void UEscapeMenu::ToggleEscapeMenu(const bool bCollapsed)
{
    /*
     * We might want to do some additional stuff here later.
     */

    if (bCollapsed)
    {
        this->SetVisibility(ESlateVisibility::Collapsed);
    }
    else
    {
        this->SetVisibility(ESlateVisibility::Visible);
    }
}

#undef OWNING_CHARACTER
