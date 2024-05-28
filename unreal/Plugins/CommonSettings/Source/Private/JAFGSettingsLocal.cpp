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
            JAFGWidget->UpdateTreeBrushes();
        }
    }

    LOG_DISPLAY(LogGameSettings, "Updated %d user interface colors on %d widgets.", UpdatedValues, Widgets.Num())

#undef SMART_UPDATE_VALUE

    return;
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
