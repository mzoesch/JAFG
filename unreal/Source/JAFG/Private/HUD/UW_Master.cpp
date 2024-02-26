// © 2023 mzoesch. All rights reserved.

#include "HUD/UW_Master.h"

FString UW_Master::ToString()
{
    return "UW_Master";
}

void UW_Master::SetVisibility(ESlateVisibility InVisibility)
{
    Super::SetVisibility(InVisibility);

    UE_LOG(LogTemp, Warning, TEXT("UW_Master::SetVisibility: %s."), InVisibility == ESlateVisibility::Visible ? TEXT("Visible") : InVisibility == ESlateVisibility::Hidden ? TEXT("Hidden") : TEXT("unknown visibility state."))
    
}
