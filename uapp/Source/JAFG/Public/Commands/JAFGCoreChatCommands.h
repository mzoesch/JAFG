// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Commands/ShippedWorldChatCommands.h"

JAFG_VOID

namespace ChatCommands
{

JAFG_API void RegisterJAFGCoreChatCommands(UShippedWorldChatCommandRegistry* ChatCommands);

}

namespace ChatCommands::Callbacks::Client
{

JAFG_API void OnHelp(const FChatCommandParams& Params);

JAFG_API void OnClear(const FChatCommandParams& Params);
JAFG_API void OnQuit(const FChatCommandParams& Params);

}

namespace ChatCommands::Callbacks::Authority
{

JAFG_API void OnBroadcast(const FChatCommandParams& Params);
JAFG_API void OnFly(const FChatCommandParams& Params);
JAFG_API void OnAllowInputFly(const FChatCommandParams& Params);
JAFG_API void OnShowOnlinePlayers(const FChatCommandParams& Params);
JAFG_API void OnKick(const FChatCommandParams& Params);
JAFG_API void OnChangeDisplayName(const FChatCommandParams& Params);
JAFG_API void OnGiveAccumulated(const FChatCommandParams& Params);
JAFG_API void OnShowReadOnlyPlayerInventory(const FChatCommandParams& Params);

}
