// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "DerivedClassHolder.generated.h"

class UW_Container;

/** The sole purpose of this class is to hold derived classes for the Unreal Engine reflection system. */
UCLASS(Abstract, Blueprintable)
class JAFG_API UDerivedClassHolder : public UObject
{
    GENERATED_BODY()

public:
    
    UDerivedClassHolder() = default;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Derived Classes")
    TSubclassOf<UW_Container> CraftingTableContainer;
};
