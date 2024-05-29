// Copyright 2024 mzoesch. All rights reserved.

//
// Kinda cheeky calling this file "Names" as these are all strings but not names.
//

#pragma once

#include "MyCore.h"

JAFG_VOID

namespace InputContexts
{

extern inline const FString Footlike = TEXT("Footlike");
extern inline const FString FootWalk = TEXT("FootWalk");
extern inline const FString FootFly  = TEXT("FootFly");
extern inline const FString Escape   = TEXT("Escape");
extern inline const FString Chat     = TEXT("Chat");

}

namespace InputActions
{

extern inline const FString Move                      = TEXT("Move");
extern inline const FString Look                      = TEXT("Look");
extern inline const FString Jump                      = TEXT("Jump");
extern inline const FString FlyUp                     = TEXT("FlyUp");
extern inline const FString Crouch                    = TEXT("Crouch");
extern inline const FString FlyDown                   = TEXT("FlyDown");
extern inline const FString Primary                   = TEXT("Primary");
extern inline const FString Secondary                 = TEXT("Secondary");
extern inline const FString ToggleEscapeMenu          = TEXT("ToggleEscapeMenu");
extern inline const FString ToggleDebugScreen         = TEXT("ToggleDebugScreen");
extern inline const FString ToggleChat                = TEXT("ToggleChat");
extern inline const FString ToggleQuickSessionPreview = TEXT("ToggleQuickSessionPreview");
extern inline const FString PreviousChatStdIn         = TEXT("PreviousChatStdIn");
extern inline const FString NextChatStdIn             = TEXT("NextChatStdIn");
extern inline const FString ToggleCameras             = TEXT("ToggleCameras");
extern inline const FString ZoomCameras               = TEXT("ZoomCameras");
extern inline const FString TogglePerspective         = TEXT("TogglePerspective");
extern inline const FString UpMaxFlySpeed             = TEXT("UpMaxFlySpeed");
extern inline const FString DownMaxFlySpeed           = TEXT("DownMaxFlySpeed");

}
