// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "SettingsData/JAFGInputSubsystem.h"

#include "JAFGSettingsLocal.generated.h"

class UDefaultColorsSubsystem;

/** Base class to store all local settings */
UCLASS(NotBlueprintable)
class COMMONSETTINGS_API UJAFGSettingsLocal : public UGameUserSettings
{
    GENERATED_BODY()

public:

    explicit UJAFGSettingsLocal(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    static auto Get(void) -> UJAFGSettingsLocal*;

    virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;

    /*
     * Please note.
     * Always use UFUNCTION() to expose a method to the cached dynamic property paths if it should be accessible
     * through the user interface.
     */

    //////////////////////////////////////////////////////////////////////////
    // Audio

    UFUNCTION()
    float GetMasterVolume( /* void */ ) const;
    UFUNCTION()
    void SetMasterVolume(const float InMasterVolume);

    UFUNCTION()
    float GetMusicVolume( /* void */ ) const;
    UFUNCTION()
    void SetMusicVolume(const float InMusicVolume);

    UFUNCTION()
    float GetMiscVolume( /* void */ ) const;
    UFUNCTION()
    void SetMiscVolume(const float InMiscVolume);

    UFUNCTION()
    float GetVoiceVolume( /* void */ ) const;
    UFUNCTION()
    void SetVoiceVolume(const float InVoiceVolume);

    inline static constexpr float DefaultMasterVolume = 1.00f;
    inline static constexpr float DefaultMusicVolume  = 0.75f;
    inline static constexpr float DefaultMiscVolume   = 0.25f;
    inline static constexpr float DefaultVoiceVolume  = 1.00f;

protected:

    UPROPERTY(Config)
    float JAFGMasterVolume = UJAFGSettingsLocal::DefaultMasterVolume;

    UPROPERTY(Config)
    float MusicVolume      = UJAFGSettingsLocal::DefaultMusicVolume;

    UPROPERTY(Config)
    float MiscVolume       = UJAFGSettingsLocal::DefaultMiscVolume;

    UPROPERTY(Config)
    float VoiceVolume      = UJAFGSettingsLocal::DefaultVoiceVolume;

    // ~Audio
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // Keybindings

public:

    FORCEINLINE auto SetOwningInputSubsystem(UJAFGInputSubsystem* InOwningInputSubsystem) -> void { this->OwningInputSubsystem = InOwningInputSubsystem; }

    auto GetAllLoadedInputActions(void) const -> const TArray<FLoadedInputAction>&;

private:

    UPROPERTY()
    TObjectPtr<UJAFGInputSubsystem> OwningInputSubsystem;

    // ~Keybindings
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // User Interface

public:

    void SetAndUpdateDefaultColorsSubsystem(UDefaultColorsSubsystem* InDefaultColors);
    /** Will smartly avoid unnecessary widget updates, to reduce color schemes update lag. */
    void SmartUpdateUserInterfaceColors(void) const;

    UFUNCTION()
    FColor GetPrimaryColor( /* void */ ) const;
    UFUNCTION()
    void SetPrimaryColor(const FColor InPrimaryColor);

    UFUNCTION()
    FColor GetPrimaryColorAlphaMax( /* void */ ) const;
    UFUNCTION()
    void SetPrimaryColorAlphaMax(const FColor InPrimaryColorAlphaMax);

    UFUNCTION()
    FColor GetPrimaryColorAlphaMid( /* void */ ) const;
    UFUNCTION()
    void SetPrimaryColorAlphaMid(const FColor InPrimaryColorAlphaMid);

    UFUNCTION()
    FColor GetPrimaryColorAlphaLess( /* void */ ) const;
    UFUNCTION()
    void SetPrimaryColorAlphaLess(const FColor InPrimaryColorAlphaLess);

    UFUNCTION()
    FColor GetSecondaryColor( /* void */ ) const;
    UFUNCTION()
    void SetSecondaryColor(const FColor InSecondaryColor);

    UFUNCTION()
    FColor GetInGameOSDColor( /* void */ ) const;
    UFUNCTION()
    void SetInGameOSDColor(const FColor InInGameOSDColor);

    inline static constexpr FColor DefaultPrimaryColor          = FColor(  20,  20,  20, 255 );
    inline static constexpr FColor DefaultPrimaryColorAlphaMax  = FColor(  20,  20,  20, 127 );
    inline static constexpr FColor DefaultPrimaryColorAlphaMid  = FColor(  20,  20,  20, 170 );
    inline static constexpr FColor DefaultPrimaryColorAlphaLess = FColor(  20,  20,  20, 210 );
    inline static constexpr FColor DefaultSecondaryColor        = FColor(  60,  60,  60, 255 );
    inline static constexpr FColor DefaultInGameOSDColor        = FColor( 140, 140, 140, 255 );

    UFUNCTION()
    float GetHeavyBlurStrength( /* void */ ) const;
    UFUNCTION()
    void SetHeavyBlurStrength(const float InHeavyBlurStrength);

    UFUNCTION()
    float GetMediumBlurStrength( /* void */ ) const;
    UFUNCTION()
    void SetMediumBlurStrength(const float InMediumBlurStrength);

    UFUNCTION()
    float GetLightBlurStrength( /* void */ ) const;
    UFUNCTION()
    void SetLightBlurStrength(const float InLightBlurStrength);

    inline static constexpr float DefaultHeavyBlurStrength  = 10.0f;
    inline static constexpr float DefaultMediumBlurStrength =  4.0f;
    inline static constexpr float DefaultLightBlurStrength  =  2.0f;

    UFUNCTION()
    int32 GetGargantuanHeaderFontSize( /* void */ ) const;
    UFUNCTION()
    void SetGargantuanHeaderFontSize(const int32 InGargantuanHeaderFontSize);

    UFUNCTION()
    int32 GetHeaderFontSize( /* void */ ) const;
    UFUNCTION()
    void SetHeaderFontSize(const int32 InHeaderFontSize);

    UFUNCTION()
    int32 GetSubHeaderFontSize( /* void */ ) const;
    UFUNCTION()
    void SetSubHeaderFontSize(const int32 InSubHeaderFontSize);

    UFUNCTION()
    int32 GetBodyFontSize( /* void */ ) const;
    UFUNCTION()
    void SetBodyFontSize(const int32 InBodyFontSize);

    UFUNCTION()
    int32 GetSmallFontSize( /* void */ ) const;
    UFUNCTION()
    void SetSmallFontSize(const int32 InSmallFontSize);

    UFUNCTION()
    int32 GetTinyFontSize( /* void */ ) const;
    UFUNCTION()
    void SetTinyFontSize(const int32 InTinyFontSize);

    inline static constexpr int32 DefaultGargantuanHeaderFontSize = 48;
    inline static constexpr int32 DefaultHeaderFontSize           = 36;
    inline static constexpr int32 DefaultSubHeaderFontSize        = 24;
    inline static constexpr int32 DefaultBodyFontSize             = 16;
    inline static constexpr int32 DefaultSmallFontSize            = 12;
    inline static constexpr int32 DefaultTinyFontSize             =  8;

private:

    UPROPERTY(Config)
    FColor PrimaryColor            = UJAFGSettingsLocal::DefaultPrimaryColor;

    UPROPERTY(Config)
    FColor PrimaryColorAlphaMax    = UJAFGSettingsLocal::DefaultPrimaryColorAlphaMax;

    UPROPERTY(Config)
    FColor PrimaryColorAlphaMid    = UJAFGSettingsLocal::DefaultPrimaryColorAlphaMid;

    UPROPERTY(Config)
    FColor PrimaryColorAlphaLess   = UJAFGSettingsLocal::DefaultPrimaryColorAlphaLess;

    UPROPERTY(Config)
    FColor InGameOSDColor          = UJAFGSettingsLocal::DefaultInGameOSDColor;

    UPROPERTY(Config)
    FColor SecondaryColor          = UJAFGSettingsLocal::DefaultSecondaryColor;

    UPROPERTY(Config)
    float HeavyBlurStrength        = UJAFGSettingsLocal::DefaultHeavyBlurStrength;

    UPROPERTY(Config)
    float MediumBlurStrength       = UJAFGSettingsLocal::DefaultMediumBlurStrength;

    UPROPERTY(Config)
    float LightBlurStrength        = UJAFGSettingsLocal::DefaultLightBlurStrength;

    UPROPERTY(Config)
    int32 GargantuanHeaderFontSize = UJAFGSettingsLocal::DefaultGargantuanHeaderFontSize;

    UPROPERTY(Config)
    int32 HeaderFontSize           = UJAFGSettingsLocal::DefaultHeaderFontSize;

    UPROPERTY(Config)
    int32 SubHeaderFontSize        = UJAFGSettingsLocal::DefaultSubHeaderFontSize;

    UPROPERTY(Config)
    int32 BodyFontSize             = UJAFGSettingsLocal::DefaultBodyFontSize;

    UPROPERTY(Config)
    int32 SmallFontSize            = UJAFGSettingsLocal::DefaultSmallFontSize;

    UPROPERTY(Config)
    int32 TinyFontSize             = UJAFGSettingsLocal::DefaultTinyFontSize;

    /**
     * From the CommonJAFGSlate plugin. All JAFG slate widgets will use this class to get their colors.
     * We not only set the config values but also update this subsystem to reflect the changes.
     * @note I know that this is kinda bad design (two sources of truth), but this is the best way to do it with my
     *       limited knowledge of computer science (We want to avoid circular dependencies).
     */
    UPROPERTY()
    TObjectPtr<UDefaultColorsSubsystem> DefaultColors = nullptr;

    // ~User Interface
    //////////////////////////////////////////////////////////////////////////
};
