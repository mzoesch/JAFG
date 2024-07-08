// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MaskFactory.h"

FVoxelMask CreateMask_Barrel(void)
{
    FVoxelMask Mask = FVoxelMask(JAFGModNamespace, TEXT("Barrel"));
    Mask.OnCustomSecondaryActionDelegate.BindLambda(
    [] (const FCustomSecondaryActionDelegateParams& Params) -> const bool
    {

        return false;
    });

    return Mask;
};
