// © 2023 mzoesch. All rights reserved.

#include "HUD/UW_Master.h"

#define UIL_LOG(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)

FString UW_Master::ToString()
{
    return "UW_Master";
}

void UW_Master::SetVisibility(const ESlateVisibility InVisibility, const bool bVisibilityIsCustom)
{
    if (InVisibility == this->GetVisibility())
    {
        UIL_LOG(Warning, TEXT("UW_Master::SetVisibility: New Visibility is the same as the current visibility. Discarding SetVisibility request."))
        return;
    }
    
    if (InVisibility == ESlateVisibility::Visible || InVisibility == ESlateVisibility::Collapsed)
    {
        UIL_LOG(Log, TEXT("UW_Master::SetVisibility: New Visibility is %s."), InVisibility == ESlateVisibility::Visible ? TEXT("Visible") : TEXT("Collapsed"))
    }
    else
    {
        if (bVisibilityIsCustom == false)
        {
            UIL_LOG(Warning, TEXT("UW_Master::SetVisibility: Detected strange visibilty request for an User Widget Master subclass. New visibilty: %d."), static_cast<int>(InVisibility))
        }
    }

    Super::SetVisibility(InVisibility);

    return;
}

#undef UIL_LOG
