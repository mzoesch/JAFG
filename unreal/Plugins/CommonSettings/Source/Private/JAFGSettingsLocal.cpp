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
    check( this->DefaultColors )

    int32 UpdatedValues = 0;

    if (this->PrimaryColor != this->DefaultColors->PrimaryColor)
    {
        this->DefaultColors->PrimaryColor = this->PrimaryColor;
        ++UpdatedValues;
    }

    if (this->PrimaryColorAlpha != this->DefaultColors->PrimaryColorAlpha)
    {
        this->DefaultColors->PrimaryColorAlpha = this->PrimaryColorAlpha;
        ++UpdatedValues;
    }

    if (this->SecondaryColor != this->DefaultColors->SecondaryColor)
    {
        this->DefaultColors->SecondaryColor = this->SecondaryColor;
        ++UpdatedValues;
    }

    if (this->AddedSubMenuColor != this->DefaultColors->AddedSubMenuColor)
    {
        this->DefaultColors->AddedSubMenuColor = this->AddedSubMenuColor;
        ++UpdatedValues;
    }

    if (UpdatedValues <= 0)
    {
        LOG_VERBOSE(LogGameSettings, "No user interface colors were updated.")
        return;
    }

    TArray<UUserWidget*> Widgets;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this->DefaultColors, Widgets, UJAFGUserWidget::StaticClass(), true);
    for (UUserWidget* Widget : Widgets)
    {
        if (const UJAFGUserWidget* JAFGWidget = Cast<UJAFGUserWidget>(Widget); JAFGWidget)
        {
            JAFGWidget->UpdateTreeBrushes();
        }
    }

    LOG_DISPLAY(LogGameSettings, "Updated %d user interface colors on %d widgets.", UpdatedValues, Widgets.Num())

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

FColor UJAFGSettingsLocal::GetPrimaryColorAlpha(void) const
{
    return this->PrimaryColorAlpha;
}

void UJAFGSettingsLocal::SetPrimaryColorAlpha(const FColor InPrimaryColorAlpha)
{
    this->PrimaryColorAlpha = InPrimaryColorAlpha;
}

FColor UJAFGSettingsLocal::GetSecondaryColor(void) const
{
    return this->SecondaryColor;
}

void UJAFGSettingsLocal::SetSecondaryColor(const FColor InSecondaryColor)
{
    this->SecondaryColor = InSecondaryColor;
}

FColor UJAFGSettingsLocal::GetAddedSubMenuColor(void) const
{
    return this->AddedSubMenuColor;
}

void UJAFGSettingsLocal::SetAddedSubMenuColor(const FColor InAddedSubMenuColor)
{
    this->AddedSubMenuColor = InAddedSubMenuColor;
}
