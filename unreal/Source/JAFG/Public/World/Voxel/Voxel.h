// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "UObject/Interface.h"

#include "Voxel.generated.h"

JAFG_VOID

UINTERFACE(MinimalAPI)
class UVoxel : public UInterface
{
    GENERATED_BODY()
};

class JAFG_API IVoxel
{
    GENERATED_BODY()

public:

    UFUNCTION()
    virtual void Initialize( /* void */ ) = 0;

};
