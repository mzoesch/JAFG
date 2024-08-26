// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGSettingsLocal.h"
#include "DefaultColorsSubsystem.h"
#include "JAFGLogDefs.h"
#include "JAFGUserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

UJAFGSettingsLocal::UJAFGSettingsLocal(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

UJAFGSettingsLocal* UJAFGSettingsLocal::Get(void)
{
    return GEngine ? CastChecked<UJAFGSettingsLocal>(GEngine->GetGameUserSettings()) : nullptr;
}

void UJAFGSettingsLocal::ApplySettings(const bool bCheckForCommandLineOverrides)
{
    Super::ApplySettings(bCheckForCommandLineOverrides);

    if (this->DefaultColors)
    {
        this->SmartUpdateUserInterfaceColors();
    }

    return;
}

void UJAFGSettingsLocal::SetAndUpdateDefaultColorsSubsystem(UDefaultColorsSubsystem* InDefaultColors)
{
    if (InDefaultColors == nullptr)
    {
        LOG_FATAL(LogGameSettings, "Default colors subsystem is not set.")
        return;
    }

    if (this->DefaultColors == InDefaultColors)
    {
        LOG_WARNING(LogGameSettings, "Default colors subsystem is already set.")
        return;
    }

    this->DefaultColors = InDefaultColors;
    this->SmartUpdateUserInterfaceColors();

    return;
}


void UJAFGSettingsLocal::SmartUpdateUserInterfaceColors(void) const
{
#define SMART_UPDATE_VALUE(Value)                  \
    if (this->Value != this->DefaultColors->Value) \
    {                                              \
        this->DefaultColors->Value = this->Value;  \
        ++UpdatedValues;                           \
    }

    check( this->DefaultColors )

    int32 UpdatedValues = 0;

    SMART_UPDATE_VALUE(PrimaryColor)
    SMART_UPDATE_VALUE(PrimaryColorAlphaMax)
    SMART_UPDATE_VALUE(PrimaryColorAlphaMid)
    SMART_UPDATE_VALUE(PrimaryColorAlphaLess)
    SMART_UPDATE_VALUE(SecondaryColor)
    SMART_UPDATE_VALUE(InGameOSDColor)

    SMART_UPDATE_VALUE(HeavyBlurStrength)
    SMART_UPDATE_VALUE(MediumBlurStrength)
    SMART_UPDATE_VALUE(LightBlurStrength)

    SMART_UPDATE_VALUE(GargantuanHeaderFontSize)
    SMART_UPDATE_VALUE(HeaderFontSize)
    SMART_UPDATE_VALUE(SubHeaderFontSize)
    SMART_UPDATE_VALUE(BodyFontSize)
    SMART_UPDATE_VALUE(SmallFontSize)
    SMART_UPDATE_VALUE(TinyFontSize)

    if (UpdatedValues <= 0)
    {
        LOG_VERBOSE(LogGameSettings, "No user interface colors were updated.")
        return;
    }

    TArray<UUserWidget*> Widgets;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this->DefaultColors, Widgets, UJAFGUserWidget::StaticClass(), false);
    for (UUserWidget* Widget : Widgets)
    {
        if (const UJAFGUserWidget* JAFGWidget = Cast<UJAFGUserWidget>(Widget); JAFGWidget)
        {
            JAFGWidget->UpdateTreeSchemes();
        }
    }

    LOG_DISPLAY(LogGameSettings, "Updated %d user interface colors on %d widgets.", UpdatedValues, Widgets.Num())

    return;
#undef SMART_UPDATE_VALUE
}

float UJAFGSettingsLocal::GetMasterVolume(void) const
{
    return this->JAFGMasterVolume;
}

void UJAFGSettingsLocal::SetMasterVolume(const float InMasterVolume)
{
    this->JAFGMasterVolume = InMasterVolume;
}

float UJAFGSettingsLocal::GetMusicVolume(void) const
{
    return this->MusicVolume;
}

void UJAFGSettingsLocal::SetMusicVolume(const float InMusicVolume)
{
    this->MusicVolume = InMusicVolume;
}

float UJAFGSettingsLocal::GetMiscVolume(void) const
{
    return this->MiscVolume;
}

void UJAFGSettingsLocal::SetMiscVolume(const float InMiscVolume)
{
    this->MiscVolume = InMiscVolume;
}

float UJAFGSettingsLocal::GetVoiceVolume(void) const
{
    return this->VoiceVolume;
}

void UJAFGSettingsLocal::SetVoiceVolume(const float InVoiceVolume)
{
    this->VoiceVolume = InVoiceVolume;
}

const TArray<FLoadedInputAction>& UJAFGSettingsLocal::GetAllLoadedInputActions(void) const
{
    if (this->OwningInputSubsystem == nullptr)
    {
        LOG_FATAL(LogGameSettings, "Owning input subsystem is not set.")
        static const TArray<FLoadedInputAction> Out = TArray<FLoadedInputAction>();
        return Out;
    }

    return this->OwningInputSubsystem->GetAllLoadedInputActions();
}

FColor UJAFGSettingsLocal::GetPrimaryColor(void) const
{
    return this->PrimaryColor;
}

void UJAFGSettingsLocal::SetPrimaryColor(const FColor InPrimaryColor)
{
    this->PrimaryColor = InPrimaryColor;
}

FColor UJAFGSettingsLocal::GetPrimaryColorAlphaMax(void) const
{
    return this->PrimaryColorAlphaMax;
}

void UJAFGSettingsLocal::SetPrimaryColorAlphaMax(const FColor InPrimaryColorAlphaMax)
{
    this->PrimaryColorAlphaMax = InPrimaryColorAlphaMax;
}

FColor UJAFGSettingsLocal::GetPrimaryColorAlphaMid(void) const
{
    return this->PrimaryColorAlphaMid;
}

void UJAFGSettingsLocal::SetPrimaryColorAlphaMid(const FColor InPrimaryColorAlphaMid)
{
    this->PrimaryColorAlphaMid = InPrimaryColorAlphaMid;
}

FColor UJAFGSettingsLocal::GetPrimaryColorAlphaLess(void) const
{
    return this->PrimaryColorAlphaLess;
}

void UJAFGSettingsLocal::SetPrimaryColorAlphaLess(const FColor InPrimaryColorAlphaLess)
{
    this->PrimaryColorAlphaLess = InPrimaryColorAlphaLess;
}

FColor UJAFGSettingsLocal::GetSecondaryColor(void) const
{
    return this->SecondaryColor;
}

void UJAFGSettingsLocal::SetSecondaryColor(const FColor InSecondaryColor)
{
    this->SecondaryColor = InSecondaryColor;
}

FColor UJAFGSettingsLocal::GetInGameOSDColor(void) const
{
    return this->InGameOSDColor;
}

void UJAFGSettingsLocal::SetInGameOSDColor(const FColor InInGameOSDColor)
{
    this->InGameOSDColor = InInGameOSDColor;
}

float UJAFGSettingsLocal::GetHeavyBlurStrength(void) const
{
    return this->HeavyBlurStrength;
}

void UJAFGSettingsLocal::SetHeavyBlurStrength(const float InHeavyBlurStrength)
{
    this->HeavyBlurStrength = InHeavyBlurStrength;
}

float UJAFGSettingsLocal::GetMediumBlurStrength(void) const
{
    return this->MediumBlurStrength;
}

void UJAFGSettingsLocal::SetMediumBlurStrength(const float InMediumBlurStrength)
{
    this->MediumBlurStrength = InMediumBlurStrength;
}

float UJAFGSettingsLocal::GetLightBlurStrength(void) const
{
    return this->LightBlurStrength;
}

void UJAFGSettingsLocal::SetLightBlurStrength(const float InLightBlurStrength)
{
    this->LightBlurStrength = InLightBlurStrength;
}

int32 UJAFGSettingsLocal::GetGargantuanHeaderFontSize(void) const
{
    return this->GargantuanHeaderFontSize;
}

void UJAFGSettingsLocal::SetGargantuanHeaderFontSize(const int32 InGargantuanHeaderFontSize)
{
    this->GargantuanHeaderFontSize = InGargantuanHeaderFontSize;
}

int32 UJAFGSettingsLocal::GetHeaderFontSize(void) const
{
    return this->HeaderFontSize;
}

void UJAFGSettingsLocal::SetHeaderFontSize(const int32 InHeaderFontSize)
{
    this->HeaderFontSize = InHeaderFontSize;
}

int32 UJAFGSettingsLocal::GetSubHeaderFontSize(void) const
{
    return this->SubHeaderFontSize;
}

void UJAFGSettingsLocal::SetSubHeaderFontSize(const int32 InSubHeaderFontSize)
{
    this->SubHeaderFontSize = InSubHeaderFontSize;
}

int32 UJAFGSettingsLocal::GetBodyFontSize(void) const
{
    return this->BodyFontSize;
}

void UJAFGSettingsLocal::SetBodyFontSize(const int32 InBodyFontSize)
{
    this->BodyFontSize = InBodyFontSize;
}

int32 UJAFGSettingsLocal::GetSmallFontSize(void) const
{
    return this->SmallFontSize;
}

void UJAFGSettingsLocal::SetSmallFontSize(const int32 InSmallFontSize)
{
    this->SmallFontSize = InSmallFontSize;
}

int32 UJAFGSettingsLocal::GetTinyFontSize(void) const
{
    return this->TinyFontSize;
}

void UJAFGSettingsLocal::SetTinyFontSize(const int32 InTinyFontSize)
{
    this->TinyFontSize = InTinyFontSize;
}

FColor UJAFGSettingsLocal::GetChatMessageColorPlayer(void) const
{
    return this->ChatMessageColorPlayer;
}

void UJAFGSettingsLocal::SetChatMessageColorPlayer(const FColor& InChatMessageColorPlayer)
{
    this->ChatMessageColorPlayer = InChatMessageColorPlayer;
}

FColor UJAFGSettingsLocal::GetChatMessageColorWhisper(void) const
{
    return this->ChatMessageColorWhisper;
}

void UJAFGSettingsLocal::SetChatMessageColorWhisper(const FColor& InChatMessageColorWhisper)
{
    this->ChatMessageColorWhisper = InChatMessageColorWhisper;
}

FColor UJAFGSettingsLocal::GetChatMessageColorVerbose(void) const
{
    return this->ChatMessageColorVerbose;
}

void UJAFGSettingsLocal::SetChatMessageColorVerbose(const FColor& InChatMessageColorVerbose)
{
    this->ChatMessageColorVerbose = InChatMessageColorVerbose;
}

FColor UJAFGSettingsLocal::GetChatMessageColorInfo(void) const
{
    return this->ChatMessageColorInfo;
}

void UJAFGSettingsLocal::SetChatMessageColorInfo(const FColor& InChatMessageColorInfo)
{
    this->ChatMessageColorInfo = InChatMessageColorInfo;
}

FColor UJAFGSettingsLocal::GetChatMessageColorWarning(void) const
{
    return this->ChatMessageColorWarning;
}

void UJAFGSettingsLocal::SetChatMessageColorWarning(const FColor& InChatMessageColorWarning)
{
    this->ChatMessageColorWarning = InChatMessageColorWarning;
}

FColor UJAFGSettingsLocal::GetChatMessageColorError(void) const
{
    return this->ChatMessageColorError;
}

void UJAFGSettingsLocal::SetChatMessageColorError(const FColor& InChatMessageColorError)
{
    this->ChatMessageColorError = InChatMessageColorError;
}

FColor UJAFGSettingsLocal::GetChatMessageColorSuccess(void) const
{
    return this->ChatMessageColorSuccess;
}

void UJAFGSettingsLocal::SetChatMessageColorSuccess(const FColor& InChatMessageColorSuccess)
{
    this->ChatMessageColorSuccess = InChatMessageColorSuccess;
}

int32 UJAFGSettingsLocal::GetClientRenderDistance(void) const
{
    return this->ClientRenderDistance;
}

void UJAFGSettingsLocal::SetClientRenderDistance(const int32 InClientRenderDistance)
{
    this->ClientRenderDistance = InClientRenderDistance;
}

int32 UJAFGSettingsLocal::GetServerRenderDistance(void) const
{
    return this->ServerRenderDistance;
}

void UJAFGSettingsLocal::SetServerRenderDistance(const int32 InServerRenderDistance)
{
    this->ServerRenderDistance = InServerRenderDistance;
}
