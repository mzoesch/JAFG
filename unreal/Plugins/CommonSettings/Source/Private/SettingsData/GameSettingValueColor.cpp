// Copyright 2024 mzoesch. All rights reserved.

#include "SettingsData/GameSettingValueColor.h"

#include "JAFGLogDefs.h"
#include "Kismet/KismetStringLibrary.h"

UGameSettingValueColor::UGameSettingValueColor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UGameSettingValueColor::StoreInitial(void)
{
    Super::StoreInitial();
}

void UGameSettingValueColor::ResetToDefault(void)
{
    Super::ResetToDefault();
}

void UGameSettingValueColor::RestoreToInitial(void)
{
    Super::RestoreToInitial();
}

FColor UGameSettingValueColor::GetValue(void)
{
    const FString OutValue = this->ValueGetter->GetValueAsString(this->OwningPlayer);

    FLinearColor AsLinearColor;
    bool         bValid;

    UKismetStringLibrary::Conv_StringToColor(OutValue, AsLinearColor, bValid);

    if (bValid == false)
    {
        LOG_FATAL(LogGameSettings, "Invalid color value: %s. For setting: %s.", *OutValue, *this->Identifier);
        return FColor::Transparent;
    }

    return AsLinearColor.ToFColor(false);
}

void UGameSettingValueColor::SetValue(FColor InValue, EGameSettingChangeReason::Type Reason)
{
    this->ValueSetter->SetValue(this->OwningPlayer, InValue.ToString());
}
