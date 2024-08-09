// Copyright 2024 mzoesch. All rights reserved.

#include "SettingsData/GameSettingValueKeyIn.h"
#include "SettingsData/JAFGInputSubsystem.h"

UGameSettingValueKeyIn::UGameSettingValueKeyIn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UGameSettingValueKeyIn::SetInputData(UJAFGInputSubsystem* InInputSubsystem, const FString& InMappingName)
{
    this->InputSubsystem = InInputSubsystem;
    this->MappingName    = InMappingName;

    return;
}

void UGameSettingValueKeyIn::StoreInitial()
{
}

void UGameSettingValueKeyIn::ResetToDefault()
{
}

void UGameSettingValueKeyIn::RestoreToInitial()
{
}

FText UGameSettingValueKeyIn::GetPrimaryKeyText(void) const
{
    return this->InputSubsystem->GetSafeAction(this->MappingName)->NorthDefaultKeyA.GetDisplayName(false);
}

FText UGameSettingValueKeyIn::GetSecondaryKeyText(void) const
{
    return this->InputSubsystem->GetSafeAction(this->MappingName)->NorthDefaultKeyB.GetDisplayName(false);
}

void UGameSettingValueKeyIn::ChangeBinding(const bool bIsPrimary, FKey NewKey)
{
}
