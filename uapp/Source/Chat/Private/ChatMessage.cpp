// Copyright 2024 mzoesch. All rights reserved.

#include "ChatMessage.h"
#include "JAFGMacros.h"

FColor LexToColor(UJAFGSettingsLocal* Settings, const EChatMessageType::Type Type, const EChatMessageFormat::Type Format)
{
    if (Type == EChatMessageType::Player)
    {
        if (Format == EChatMessageFormat::Default)
        {
            return Settings->GetChatMessageColorPlayer();
        }
        if (Format == EChatMessageFormat::Whisper)
        {
            return Settings->GetChatMessageColorWhisper();
        }

        jcheckNoEntry()
    }

    check( Type == EChatMessageType::Client || Type == EChatMessageType::Authority )

    if (Format == EChatMessageFormat::Default)
    {
        return Settings->GetChatMessageColorInfo();
    }
    if (Format == EChatMessageFormat::Verbose)
    {
        return Settings->GetChatMessageColorVerbose();
    }
    if (Format == EChatMessageFormat::Info)
    {
        return Settings->GetChatMessageColorInfo();
    }
    if (Format == EChatMessageFormat::Warning)
    {
        return Settings->GetChatMessageColorWarning();
    }
    if (Format == EChatMessageFormat::Error)
    {
        return Settings->GetChatMessageColorError();
    }
    if (Format == EChatMessageFormat::Success)
    {
        return Settings->GetChatMessageColorSuccess();
    }

    if (Format == EChatMessageFormat::Custom)
    {
        jrelaxedCheckNoEntry()
        return FColor::White;
    }

    return FColor::Transparent;
}

FSlateColor LexToSlateColor(UJAFGSettingsLocal* Settings, const EChatMessageType::Type Type, const EChatMessageFormat::Type Format)
{
    return FSlateColor(LexToColor(Settings, Type, Format));
}
