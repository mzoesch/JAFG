// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VoxelMask.h"

#include "PreInternalInitializationSubsystemRequirements.generated.h"

UINTERFACE()
class JAFGEXTERNALCORE_API UPreInternalInitializationSubsystemRequirements : public UInterface
{
    GENERATED_BODY()
};

class JAFGEXTERNALCORE_API IPreInternalInitializationSubsystemRequirements
{
    GENERATED_BODY()

public:

    virtual void InitializeOptionalVoxels(TArray<FVoxelMask>& VoxelMasks) = 0;
};
