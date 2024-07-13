// Copyright 2024 mzoesch. All rights reserved.

//
// Kinda cheeky calling this file "Names" as these are all strings but not names.
//

#pragma once

#include "JAFGExternalCoreIncludes.h"

namespace RegisteredWorlds
{

static const FString FrontEnd      = TEXT("L_FrontEnd");
static const FString GenPrevAssets = TEXT("L_GenPrevAssets");
static const FString World         = TEXT("L_World");
static const FString Dev           = TEXT("L_Dev");

}

namespace ERegisteredWorlds
{

enum Type
{
    FrontEnd,
    GenPrevAssets,
    World,
    Dev
};

}

FORCEINLINE auto LexToString(const ERegisteredWorlds::Type InType) -> FString
{
    switch (InType)
    {
    case ERegisteredWorlds::FrontEnd:
    {
        return RegisteredWorlds::FrontEnd;
    }
    case ERegisteredWorlds::GenPrevAssets:
    {
        return RegisteredWorlds::GenPrevAssets;
    }
    case ERegisteredWorlds::World:
    {
        return RegisteredWorlds::World;
    }
    case ERegisteredWorlds::Dev:
    {
        return RegisteredWorlds::Dev;
    }
    default:
    {
        jrelaxedCheckNoEntry()
        return FString();
    }
    }
}
