// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Container.h"

class AWorldCharacter;

/**
 * @note Marking fields as dirty to push them at a later time are not supported by these helper
 *       methods and has to be done by the callee itself.
 */
namespace CharacterContainerChangeLogic
{

bool EasyChangeContainer(
    const AWorldCharacter* Self,
    IContainer* TargetContainer,
    const FOnContainerChangedDelegateSignature* ContainerChangeBroadcastDelegate,
    const int32 InIndex,
    const accamount_t_signed InAmount,
    const ELocalContainerChange::Type InReason
);

bool EasyChangeContainer(
    AWorldCharacter* Self,
    IContainer* TargetContainer,
    const FOnContainerChangedDelegateSignature* ContainerChangeBroadcastDelegate,
    const int32 InIndex,
    IContainerOwner* InOwner,
    const TFunctionRef<bool(const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner)>& InAlternator,
    const ELocalContainerChange::Type InReason
);

bool EasyChangeContainerCl(
    const AWorldCharacter* Self,
    IContainer* TargetContainer,
    const FOnContainerChangedDelegateSignature* ContainerChangeBroadcastDelegate,
    const int32 InIndex,
    IContainerOwner* InOwner,
    const TFunctionRef<bool(const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner)>& InAlternator,
    const ELocalContainerChange::Type InReason
);

bool EasyOverrideContainerOnCl(
    const AWorldCharacter* Self,
    IContainer* TargetContainer,
    const FOnContainerChangedDelegateSignature* ContainerChangeBroadcastDelegate,
    const int32 InIndex,
    const FAccumulated& InContent,
    const ELocalContainerChange::Type InReason = ELocalContainerChange::Replicated
);

}
