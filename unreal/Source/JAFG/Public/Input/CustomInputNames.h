// Copyright 2024 mzoesch. All rights reserved.

//
// Kinda cheeky calling this file "Names" as these are all strings but not names.
//

#pragma once

#include "MyCore.h"

JAFG_VOID

namespace InputContexts
{

static const FString Footlike = TEXT("Footlike");
static const FString FootWalk = TEXT("FootWalk");
static const FString FootFly  = TEXT("FootFly");
static const FString Escape   = TEXT("Escape");
static const FString Chat     = TEXT("Chat");

}

namespace InputActions
{

static const FString Move                      = TEXT("Move");
static const FString Look                      = TEXT("Look");
static const FString Jump                      = TEXT("Jump");
static const FString Sprint                    = TEXT("Sprint");
static const FString FlyUp                     = TEXT("FlyUp");
static const FString Crouch                    = TEXT("Crouch");
static const FString FlyDown                   = TEXT("FlyDown");
static const FString Primary                   = TEXT("Primary");
static const FString Secondary                 = TEXT("Secondary");
static const FString ToggleEscapeMenu          = TEXT("ToggleEscapeMenu");
static const FString ToggleDebugScreen         = TEXT("ToggleDebugScreen");
static const FString ToggleChat                = TEXT("ToggleChat");
static const FString ToggleQuickSessionPreview = TEXT("ToggleQuickSessionPreview");
static const FString PreviousChatStdIn         = TEXT("PreviousChatStdIn");
static const FString NextChatStdIn             = TEXT("NextChatStdIn");
static const FString ToggleCameras             = TEXT("ToggleCameras");
static const FString ZoomCameras               = TEXT("ZoomCameras");
static const FString TogglePerspective         = TEXT("TogglePerspective");
static const FString UpMaxFlySpeed             = TEXT("UpMaxFlySpeed");
static const FString DownMaxFlySpeed           = TEXT("DownMaxFlySpeed");

}
