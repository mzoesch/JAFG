// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ItemMask.generated.h"

USTRUCT()
struct JAFG_API FItemMask
{
    GENERATED_BODY()

public:

    FItemMask() = default;
    FItemMask(const FString& NameSpace, const FString& Name);
    
public:
    
    FString NameSpace;
    FString Name;
};
