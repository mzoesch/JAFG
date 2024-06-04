// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Foundation/JAFGContainer.h"

#include "PlayerInventory.generated.h"

UCLASS(Abstract, Blueprintable)
class JAFG_API UPlayerInventory : public UJAFGContainer
{
    GENERATED_BODY()

public:

    explicit UPlayerInventory(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    inline static const FString Identifier = TEXT("PlayerInventory");
};
