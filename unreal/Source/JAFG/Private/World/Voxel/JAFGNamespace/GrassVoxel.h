// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "UObject/Object.h"
#include "World/Voxel/Voxel.h"

#include "GrassVoxel.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API UGrassVoxel : public UObject, public IVoxel
{
    GENERATED_BODY()

public:

    virtual void Initialize(void) override;
};
