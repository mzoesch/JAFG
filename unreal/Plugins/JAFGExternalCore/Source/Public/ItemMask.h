// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ItemMask.generated.h"

USTRUCT(NotBlueprintType)
struct JAFGEXTERNALCORE_API FItemMask
{
    GENERATED_BODY()

    //////////////////////////////////////////////////////////////////////////
    // Constructors
    /** Default constructor. Required by the engine. Do not use. */
    FItemMask(void)  = default;
    ~FItemMask(void) = default;

    FORCEINLINE explicit FItemMask(const FString& Namespace, const FString& Name)
    {
        this->Namespace = Namespace;
        this->Name      = Name;

        return;
    }
    // ~Constructors
    //////////////////////////////////////////////////////////////////////////

    FString Namespace;
    FString Name;
};
