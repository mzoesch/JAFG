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
    this->SetValue(this->DefaultValue, EGameSettingChangeReason::ResetToDefault);
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

    /*
     * Seeing this. You will see that this is wrong. We save the color in B8G8R8A8 format using the FColor struct.
     * 8 bits meaning: 8^2 = 256 values (for each channel respectively).
     * But the FLinearColor struct uses F32 values for each color channel. To rightfully convert from FLinearColor to
     * FColor, we should multiply each channel by 255 and then cast to uint8.
     * *But* this color is actually not a linear color. But a FColor loaded into a linear struct.
     * Q: Why??
     * A: Because there is a string to FLinearColor conversion function, but not a string to FColor. So we convert to
     *    FLinearColor (but that linear color is invalid, but somehow allowed by the engine (because the correctness
     *    is currently not enforced by it)) and then back to FColor with a falsy conversion.
     *    The soul reason for this is, so we can easily manipulate the color channels directly in the ini files. Using
     *    linear colors is wrong and tells the user that they have so much more color space (which does not exist), as
     *    the JAFG slate plugin uses FColor for the User Interface.
     */
    return FColor(AsLinearColor.R, AsLinearColor.G, AsLinearColor.B, AsLinearColor.A);
}

void UGameSettingValueColor::SetValue(const FColor InValue, const EGameSettingChangeReason::Type Reason /* = EGameSettingChangeReason::Change */)
{
    this->ValueSetter->SetValue(this->OwningPlayer, InValue.ToString());
}
