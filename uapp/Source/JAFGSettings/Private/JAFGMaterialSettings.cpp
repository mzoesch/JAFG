// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGMaterialSettings.h"

UJAFGMaterialSettings::UJAFGMaterialSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->CategoryName = FName(TEXT("JAFG"));
    this->SectionName  = FName(TEXT("CommMaterial"));
}

FName UJAFGMaterialSettings::GetContainerName(void) const
{
    return Super::GetContainerName();
}

FName UJAFGMaterialSettings::GetCategoryName(void) const
{
    return Super::GetCategoryName();
}

FName UJAFGMaterialSettings::GetSectionName(void) const
{
    return Super::GetSectionName();
}

#if WITH_EDITOR

FText UJAFGMaterialSettings::GetSectionText(void) const
{
    return Super::GetSectionText();
}

FText UJAFGMaterialSettings::GetSectionDescription(void) const
{
    return Super::GetSectionDescription();
}

bool UJAFGMaterialSettings::SupportsAutoRegistration(void) const
{
    return Super::SupportsAutoRegistration();
}

void UJAFGMaterialSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
}

TSharedPtr<SWidget> UJAFGMaterialSettings::GetCustomSettingsWidget(void) const
{
    return Super::GetCustomSettingsWidget();
}

#endif /* WITH_EDITOR */
