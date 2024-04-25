// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "UObject/Object.h"

#include "JAFGNamespaceInitializer.generated.h"

JAFG_VOID

class UVoxelSubsystem;

namespace PrivateJAFG
{

const FString SpaceName = FString("JAFG");

}

UCLASS(NotBlueprintable)
class JAFG_API UJAFGNamespaceInitializer : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    static void InitializeOptionalVoxels(UObject* Outer);
};
