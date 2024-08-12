// Copyright 2024 mzoesch. All rights reserved.

#include "System/PluginValidationSubsystem.h"
#include "GamePluginSettings.h"
#include "GeneralProjectSettings.h"
#include "Interfaces/IPluginManager.h"

enum class EVersionType : uint8
{
    NotFound,
    Exists,
    Any,
};

struct FHelperVersion
{
    explicit FHelperVersion(const FString& Version, const bool bCheckForValidity = true)
    {
        TArray<FString> Parts; Version.ParseIntoArray(Parts, TEXT("."), true);

        if (Parts.Num() >= 1)
        {
            if (Parts[0].Compare("X") == 0)
            {
                MajorType = EVersionType::Any;
            }
            else
            {
                jcheck( Parts[0].IsNumeric() )
                Major     = FCString::Atoi(*Parts[0]);
                MajorType = EVersionType::Exists;
            }
        }

        if (Parts.Num() >= 2)
        {
            if (Parts[1].Compare("X") == 0)
            {
                MinorType = EVersionType::Any;
            }
            else
            {
                jcheck( Parts[1].IsNumeric() )
                Minor     = FCString::Atoi(*Parts[1]);
                MinorType = EVersionType::Exists;
            }
        }

        if (Parts.Num() >= 3)
        {
            if (Parts[2].Compare("X") == 0)
            {
                PatchType = EVersionType::Any;
            }
            else
            {
                jcheck( Parts[2].IsNumeric() )
                Patch     = FCString::Atoi(*Parts[2]);
                PatchType = EVersionType::Exists;
            }
        }

        if (Parts.Num() >= 4)
        {
            if (Parts[3].Compare("X") == 0)
            {
                BuildType = EVersionType::Any;
            }
            else
            {
                jcheck( Parts[3].IsNumeric() )
                Build     = FCString::Atoi(*Parts[3]);
                BuildType = EVersionType::Exists;
            }
        }

        if (bCheckForValidity)
        {
            jcheck( this->IsValid() )
        }

        return;
    }

    EVersionType MajorType = EVersionType::NotFound;
    EVersionType MinorType = EVersionType::NotFound;
    EVersionType PatchType = EVersionType::NotFound;
    EVersionType BuildType = EVersionType::NotFound;

    int32 Major = 0;
    int32 Minor = 0;
    int32 Patch = 0;
    int32 Build = 0;

    bool IsValid(void) const
    {
        if (MajorType != EVersionType::Exists)
        {
            return false;
        }
        if (MinorType == EVersionType::NotFound)
        {
            return false;
        }

        if (MinorType == EVersionType::Any && (PatchType != EVersionType::NotFound || BuildType != EVersionType::NotFound))
        {
            return false;
        }

        if (PatchType == EVersionType::Any && BuildType != EVersionType::NotFound)
        {
            return false;
        }

        return Major >= 0 && Minor >= 0 && Patch >= 0 && Build >= 0;
    }

    FORCEINLINE auto operator== (const FHelperVersion& Other) const -> bool
    {
        if (this->IsValid() == false || Other.IsValid() == false)
        {
            return false;
        }

        if (Major != Other.Major)
        {
            return false;
        }

        if (Other.MinorType == EVersionType::Any)
        {
            return true;
        }
        if (Minor != Other.Minor)
        {
            return false;
        }

        if (Other.PatchType == EVersionType::Any)
        {
            return true;
        }
        if (Patch != Other.Patch)
        {
            return false;
        }

        if (Other.BuildType == EVersionType::Any)
        {
            return true;
        }
        if (Build != Other.Build)
        {
            return false;
        }

        return true;
    }

    FORCEINLINE auto operator>= (const FHelperVersion& Other) const -> bool
    {
        if (this->IsValid() == false || Other.IsValid() == false)
        {
            return false;
        }

        if (Major > Other.Major)
        {
            return true;
        }
        if (Major < Other.Major)
        {
            return false;
        }

        if (Other.MinorType == EVersionType::Any)
        {
            return true;
        }
        if (Minor > Other.Minor)
        {
            return true;
        }
        if (Minor < Other.Minor)
        {
            return false;
        }

        if (Other.PatchType == EVersionType::Any)
        {
            return true;
        }
        if (Patch > Other.Patch)
        {
            return true;
        }
        if (Patch < Other.Patch)
        {
            return false;
        }

        if (Other.BuildType == EVersionType::Any)
        {
            return true;
        }
        if (Build > Other.Build)
        {
            return true;
        }
        if (Build < Other.Build)
        {
            return false;
        }

        return true;
    }

    FORCEINLINE auto operator<= (const FHelperVersion& Other) const -> bool
    {
        if (this->IsValid() == false || Other.IsValid() == false)
        {
            return false;
        }

        if (Major < Other.Major)
        {
            return true;
        }
        if (Major > Other.Major)
        {
            return false;
        }

        if (Other.MinorType == EVersionType::Any)
        {
            return true;
        }
        if (Minor < Other.Minor)
        {
            return true;
        }
        if (Minor > Other.Minor)
        {
            return false;
        }

        if (Other.PatchType == EVersionType::Any)
        {
            return true;
        }
        if (Patch < Other.Patch)
        {
            return true;
        }
        if (Patch > Other.Patch)
        {
            return false;
        }

        if (Other.BuildType == EVersionType::Any)
        {
            return true;
        }
        if (Build < Other.Build)
        {
            return true;
        }
        if (Build > Other.Build)
        {
            return false;
        }

        return true;
    }

    FORCEINLINE auto ToString(void) const -> FString
    {
        return FString::Printf(
            TEXT("Version{%d:%d,%d:%d,%d:%d,%d:%d}"),
            MajorType, Major, MinorType, Minor, PatchType, Patch, BuildType, Build
        );
    }
};

bool GamePluginCompatibility::IsGamePluginCompatible(const FString& JAFGVersion, const IPlugin& Plugin)
{
    const FString Range = Plugin.GetDescriptor().JAFGVersionRange;

    if (Range.Compare(TEXT("*")) == 0)
    {
        return true;
    }

    if (Range.Compare(TEXT("/")) == 0)
    {
        return false;
    }

    if (Range.StartsWith(">="))
    {
        return FHelperVersion(JAFGVersion) >= FHelperVersion(Range.RightChop(2));
    }

    if (Range.StartsWith("<="))
    {
        return FHelperVersion(JAFGVersion) <= FHelperVersion(Range.RightChop(2));
    }

    if (Range.Contains("-"))
    {
        TArray<FString> Parts; Range.ParseIntoArray(Parts, TEXT("-"), true);
        jcheck( Parts.Num() == 2 )

        return
               FHelperVersion(JAFGVersion) >= FHelperVersion(Parts[0])
            && FHelperVersion(JAFGVersion) <= FHelperVersion(Parts[1]);
    }

    return FHelperVersion(JAFGVersion) == FHelperVersion(Range);
}

UPluginValidationSubsystem::UPluginValidationSubsystem(void) : Super()
{
    return;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
bool UPluginValidationSubsystem::IsPluginEnabled(const IPlugin& Plugin) const
{
    return Plugin.IsEnabled();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
bool UPluginValidationSubsystem::IsGamePlugin(const IPlugin& Plugin) const
{
    return Plugin.GetType() == EPluginType::Mod;
}

bool UPluginValidationSubsystem::IsGamePluginEnabled(const IPlugin& Plugin) const
{
    return this->IsPluginEnabled(Plugin) && GetDefault<UGamePluginSettings>()->IsPluginEnabled(Plugin.GetName());
}

bool UPluginValidationSubsystem::IsGamePluginCompatible(const IPlugin& Plugin) const
{
    if (this->IsGamePlugin(Plugin) == false)
    {
        return true;
    }

    return GamePluginCompatibility::IsGamePluginCompatible(this->GetCurrentGameVersion(), Plugin);
}

bool UPluginValidationSubsystem::IsGamePluginIncompatible(const IPlugin& Plugin) const
{
    if (this->IsGamePlugin(Plugin) == false)
    {
        return false;
    }

    return this->IsGamePluginCompatible(Plugin) == false;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
bool UPluginValidationSubsystem::IsTestGamePlugin(const IPlugin& Plugin) const
{
    return this->IsGamePlugin(Plugin) && Plugin.GetName().Contains(TEXT("Test"));
}

bool UPluginValidationSubsystem::SmartIsPluginEnabled(const IPlugin& Plugin) const
{
    return this->IsGamePlugin(Plugin) ? this->IsGamePluginEnabled(Plugin) : this->IsPluginEnabled(Plugin);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
FString UPluginValidationSubsystem::GetCurrentGameVersion(void) const
{
    return GetDefault<UGeneralProjectSettings>()->ProjectVersion;
}
