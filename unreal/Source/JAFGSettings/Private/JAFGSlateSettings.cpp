// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGSlateSettings.h"

UJAFGSlateSettings::UJAFGSlateSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->CategoryName = FName(TEXT("JAFG"));
    this->SectionName  = FName(TEXT("CommonSlate"));

    return;
}

FName UJAFGSlateSettings::GetContainerName(void) const
{
    return Super::GetContainerName();
}

FName UJAFGSlateSettings::GetCategoryName(void) const
{
    return Super::GetCategoryName();
}

FName UJAFGSlateSettings::GetSectionName(void) const
{
    return Super::GetSectionName();
}

#if WITH_EDITOR

FText UJAFGSlateSettings::GetSectionText(void) const
{
    return Super::GetSectionText();
}

FText UJAFGSlateSettings::GetSectionDescription(void) const
{
    return Super::GetSectionDescription();
}

bool UJAFGSlateSettings::SupportsAutoRegistration(void) const
{
    return Super::SupportsAutoRegistration();
}

void UJAFGSlateSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
}

TSharedPtr<SWidget> UJAFGSlateSettings::GetCustomSettingsWidget(void) const
{
    return Super::GetCustomSettingsWidget();
}

#endif /* WITH_EDITOR */
