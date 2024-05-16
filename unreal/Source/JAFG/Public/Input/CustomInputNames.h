// Copyright 2024 mzoesch. All rights reserved.

//
// Kinda cheeky calling this file "Names" as these are all strings but not names.
//

#pragma once

#include "MyCore.h"

JAFG_VOID

namespace InputContexts
{

extern inline const FString Foot   = TEXT("Foot");
extern inline const FString Escape = TEXT("Escape");
extern inline const FString Chat   = TEXT("Chat");

}

namespace InputActions
{

extern inline const FString Move              = TEXT("Move");
extern inline const FString Look              = TEXT("Look");
extern inline const FString Jump              = TEXT("Jump");
extern inline const FString ToggleEscapeMenu  = TEXT("ToggleEscapeMenu");
extern inline const FString ToggleDebugScreen = TEXT("ToggleDebugScreen");
extern inline const FString ToggleChat        = TEXT("ToggleChat");

}
