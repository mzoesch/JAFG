// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Commands/ShippedWorldChatCommands.h"

JAFG_VOID

namespace ChatCommands
{

void RegisterJAFGCoreChatCommands(UShippedWorldChatCommandRegistry* ChatCommands);

}

namespace ChatCommands::Callbacks::Client
{

void OnHelp(const FChatCommandParams& Params);

void OnClear(const FChatCommandParams& Params);
void OnQuit(const FChatCommandParams& Params);

}

namespace ChatCommands::Callbacks::Authority
{

void OnBroadcast(const FChatCommandParams& Params);
void OnFly(const FChatCommandParams& Params);
void OnAllowInputFly(const FChatCommandParams& Params);
void OnShowOnlinePlayers(const FChatCommandParams& Params);
void OnKick(const FChatCommandParams& Params);
void OnChangeDisplayName(const FChatCommandParams& Params);
void OnGiveAccumulated(const FChatCommandParams& Params);
void OnShowReadOnlyPlayerInventory(const FChatCommandParams& Params);

}
