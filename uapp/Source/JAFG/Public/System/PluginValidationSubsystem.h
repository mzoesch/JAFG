// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "PluginValidationSubsystem.generated.h"

JAFG_VOID

class IPlugin;

namespace GamePluginCompatibility
{

/**
 * Function compares the JAFGVersion with the JAFGVersionRange of the plugin and returns true if the JAFGVersionRange
 * is inside the JAFGVersion.
 *
 * Syntax and semantics of JAFGVersionRange:
 *   - "VERSION"           matches the exact VERSION.
 *   - ">=VERSION"         matches any version greater or equal to VERSION (should be avoided, as backward
 *                         compatibility is never guaranteed).
 *   - "<=VERSION"         matches any version less or equal to VERSION.
 *   - "VERSION1-VERSION2" matches any version between VERSION1 and VERSION2 (inclusive).
 *   - "1.X"               matches any version starting with "1.".
 *   - "1.2.X"             matches any version starting with "1.2.".
 *   - "1.2.3.X"           matches any version starting with "1.2.3.".
 *   - "*"                 matches any version (although it should be avoided as it is only for internal use)
 *   - "/"                 matches no version.
 *
 * The rules can be combined to create more complex version ranges.
 * Examples:
 *   - "1.2.X-1.3.X"       matches any version starting with "1.2." or "1.3.".
 *
 * @note The build version (the fourth number) is mainly used for internal builds and may be helpful if while debugging
 *       but should not be used for compatibility checks and not be part of the JAFGVersionRange when publishing.
 */
bool IsGamePluginCompatible(const FString& JAFGVersion, const IPlugin& Plugin);

}

UCLASS(NotBlueprintable)
class JAFG_API UPluginValidationSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    UPluginValidationSubsystem();

    //////////////////////////////////////////////////////////////////////////
    // General Plugin Information
    bool IsPluginEnabled(const IPlugin& Plugin) const;

    //////////////////////////////////////////////////////////////////////////
    // For Game Plugins
    bool IsGamePlugin(const IPlugin& Plugin) const;
    bool IsGamePluginEnabled(const IPlugin& Plugin) const;
    bool IsGamePluginCompatible(const IPlugin& Plugin) const;
    bool IsGamePluginIncompatible(const IPlugin& Plugin) const;
    bool IsTestGamePlugin(const IPlugin& Plugin) const;
    TArray<IPlugin*> GetEnabledGamePlugins(void) const;

    //////////////////////////////////////////////////////////////////////////
    // Smart Plugin Information
    bool SmartIsPluginEnabled(const IPlugin& Plugin) const;
    bool SmartIsPluginEnabledAndIncompatible(const IPlugin& Plugin) const;
    bool HasAnyEnabledGamePluginsThatAreIncompatible(TArray<FString>& OutPlugins) const;


    auto GetCurrentGameVersion(void) const -> FString;
};
