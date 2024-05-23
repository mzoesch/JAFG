// Copyright 2024 mzoesch. All rights reserved.

//
// Kinda cheeky calling this file "Names" as these are all strings but not names.
//

#pragma once

#include "CoreMinimal.h"

namespace RegisteredWorlds
{

extern inline const FString FrontEnd = TEXT("L_FrontEnd");
extern inline const FString World    = TEXT("L_World");
extern inline const FString Dev      = TEXT("L_Dev");

}

namespace ERegisteredWorlds
{

enum Type
{
    FrontEnd,
    World,
    Dev
};

FORCEINLINE auto LexToString(const ERegisteredWorlds::Type InType) -> FString
{
    switch (InType)
    {
    case ERegisteredWorlds::FrontEnd:
    {
        return RegisteredWorlds::FrontEnd;
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
        checkNoEntry()
        return FString();
    }
    }
}

}
