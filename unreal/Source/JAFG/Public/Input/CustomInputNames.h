// Copyright 2024 mzoesch. All rights reserved.

//
// Kinda cheeky calling this file "Names" as these are all strings but not names.
//

#pragma once

#include "MyCore.h"

JAFG_VOID

namespace InputContexts
{

extern inline JAFG_API const FString Foot   = TEXT("Foot");
extern inline JAFG_API const FString Escape = TEXT("Escape");

}

namespace InputActions
{

extern inline JAFG_API const FString Move              = TEXT("Move");
extern inline JAFG_API const FString Look              = TEXT("Look");
extern inline JAFG_API const FString Jump              = TEXT("Jump");
extern inline JAFG_API const FString ToggleEscapeMenu  = TEXT("ToggleEscapeMenu");
extern inline JAFG_API const FString ToggleDebugScreen = TEXT("ToggleDebugScreen");

}
