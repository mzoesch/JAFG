// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Character/CharacterContainerChangeLogic.h"
#include "WorldCore/WorldCharacter.h"

bool CharacterContainerChangeLogic::EasyChangeContainer(
    const AWorldCharacter* Self,
    IContainer* TargetContainer,
    const FOnContainerChangedDelegateSignature* ContainerChangeBroadcastDelegate,
    const int32 InIndex,
    const accamount_t_signed InAmount,
    const ELocalContainerChange::Type InReason
)
{
    if (UNetStatics::IsSafeClient(Self))
    {
        LOG_FATAL(LogWorldChar, "Cannot change container on client.")
        return false;
    }

    if (TargetContainer->GetContainerValueRef(InIndex).SafeAddAmountRet(InAmount))
    {
        if (Self->IsLocallyControlled())
        {
            Self->OnCursorValueChangedDelegate.Broadcast();
            ContainerChangeBroadcastDelegate->Broadcast(InReason, InIndex);
        }

        return true;
    }

    LOG_WARNING(LogWorldChar, "Failed to change container [%d with delta %d]." , InIndex, InAmount)

    return false;
}

bool CharacterContainerChangeLogic::EasyChangeContainer(
    AWorldCharacter* Self,
    IContainer* TargetContainer,
    const FOnContainerChangedDelegateSignature* ContainerChangeBroadcastDelegate,
    const int32 InIndex,
    IContainerOwner* InOwner,
    const TFunctionRef<bool(const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner)>& InAlternator,
    const ELocalContainerChange::Type InReason
)
{
    if (UNetStatics::IsSafeClient(Self))
    {
        LOG_FATAL(LogWorldChar, "Cannot change container on client.")
        return false;
    }

#if !UE_BUILD_SHIPPING
    if (InOwner != Self->AsContainerOwner())
    {
        jcheckNoEntry()
        return false;
    }
#endif /* !UE_BUILD_SHIPPING */

    if (InAlternator(InIndex, TargetContainer, InOwner))
    {
        if (Self->IsLocallyControlled())
        {
            Self->OnCursorValueChangedDelegate.Broadcast();
            ContainerChangeBroadcastDelegate->Broadcast(InReason, InIndex);
        }

        return true;
    }

    if (Self->IsLocallyControlled() == false)
    {
        jrelaxedCheckNoEntry()
    }

    return false;
}

bool CharacterContainerChangeLogic::EasyChangeContainerCl(
    const AWorldCharacter* Self,
    IContainer* TargetContainer,
    const FOnContainerChangedDelegateSignature* ContainerChangeBroadcastDelegate,
    const int32 InIndex,
    IContainerOwner* InOwner,
    const TFunctionRef<bool(const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner)>& InAlternator,
    const ELocalContainerChange::Type InReason
)
{
    if (UNetStatics::IsSafeClient(Self) == false)
    {
        LOG_FATAL(LogWorldChar, "Cannot change container on server.")
        return false;
    }

    if (InAlternator(InIndex, TargetContainer, InOwner))
    {
        Self->OnCursorValueChangedDelegate.Broadcast();
        ContainerChangeBroadcastDelegate->Broadcast(InReason, InIndex);
        return true;
    }

    return false;
}

bool CharacterContainerChangeLogic::EasyOverrideContainerOnCl(
    const AWorldCharacter* Self,
    IContainer* TargetContainer,
    const FOnContainerChangedDelegateSignature* ContainerChangeBroadcastDelegate,
    const int32 InIndex,
    const FAccumulated& InContent,
    const ELocalContainerChange::Type InReason
)
{
    if (UNetStatics::IsSafeClient(Self) == false)
    {
        LOG_FATAL(LogWorldChar, "Cannot change container on server.")
        return false;
    }

    ContainerChangeBroadcastDelegate->Broadcast(InReason, InIndex);

    return true;
}
