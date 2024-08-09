// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Character/CharacterCrafterComponent.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "WorldCore/WorldCharacter.h"
#include "WorldCore/Character/CharacterContainerChangeLogic.h"

UCharacterCrafterComponent::UCharacterCrafterComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryComponentTick.bCanEverTick = false;
}

void UCharacterCrafterComponent::BeginPlay(void)
{
    Super::BeginPlay();

    if (this->GetOwnerAsCharacter() == nullptr)
    {
        jcheckNoEntry()
        return;
    }

    if (this->IsLocallyControlled())
    {
        this->OnContainerChangedDelegate.AddLambda( [this] (const ELocalContainerChange::Type InReason, const int32 InIndex)
        { this->OnLocalContainerCrafterChangedEventImpl(InReason, InIndex); });
    }

    if (UNetStatics::IsSafeServer(this))
    {
        LOG_VERY_VERBOSE(LogWorldChar, "Initializing container crafter on server.")
        this->ContainerCrafter.Init(FSlot(Accumulated::Null), 4);
        LOG_VERY_VERBOSE(LogWorldChar, "Container crafter initialized with %d slots.", this->GetContainerCrafterSize());
        MARK_PROPERTY_DIRTY_FROM_NAME(UCharacterCrafterComponent, ContainerCrafter, this)
    }

    return;
}

void UCharacterCrafterComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    {
        FDoRepLifetimeParams SharedParams = FDoRepLifetimeParams();
        SharedParams.bIsPushBased = true;
        SharedParams.Condition    = ELifetimeCondition::COND_OwnerOnly;

        DOREPLIFETIME_WITH_PARAMS_FAST(UCharacterCrafterComponent, ContainerCrafter, SharedParams)
    }

    return;
}

bool UCharacterCrafterComponent::EasyAddToContainerCrafter(const FAccumulated& Value)
{
    jrelaxedCheckNoEntry()
    return false;
}

bool UCharacterCrafterComponent::EasyChangeContainerCrafter(
    const int32 InIndex,
    const accamount_t_signed InAmount,
    const ELocalContainerChange::Type InReason
)
{
    if (CharacterContainerChangeLogic::EasyChangeContainer(
        this->GetOwnerAsCharacter(),
        this->AsContainerCrafter(),
        &this->OnContainerChangedDelegate,
        InIndex,
        InAmount,
        InReason
    ))
    {
        if (this->IsLocallyControlled() == false)
        {
            MARK_PROPERTY_DIRTY_FROM_NAME(UCharacterCrafterComponent, ContainerCrafter, this)
        }

        return true;
    }

    return false;
}

bool UCharacterCrafterComponent::EasyChangeContainerCrafter(
    const int32 InIndex,
    IContainerOwner* InOwner,
    const TFunctionRef<bool(const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner)>& InAlternator,
    const ELocalContainerChange::Type InReason
)
{
    if (CharacterContainerChangeLogic::EasyChangeContainer(
        this->GetOwnerAsCharacter(),
        this->AsContainerCrafter(),
        &this->OnContainerChangedDelegate,
        InIndex,
        InOwner,
        InAlternator,
        InReason
    ))
    {
        if (this->IsLocallyControlled() == false)
        {
            MARK_PROPERTY_DIRTY_FROM_NAME(UCharacterCrafterComponent, ContainerCrafter, this)
        }

        return true;
    }

    return false;
}

bool UCharacterCrafterComponent::EasyChangeContainerCrafterCl(
    const int32 InIndex,
    IContainerOwner* InOwner,
    const TFunctionRef<bool(const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner)>& InAlternator,
    const ELocalContainerChange::Type InReason
)
{
    return CharacterContainerChangeLogic::EasyChangeContainerCl(
        this->GetOwnerAsCharacter(),
        this->AsContainerCrafter(),
        &this->OnContainerChangedDelegate,
        InIndex,
        InOwner,
        InAlternator,
        InReason
    );
}

bool UCharacterCrafterComponent::EasyOverrideContainerCrafterOnCl(
    const int32 InIndex,
    const FAccumulated& InContent,
    const ELocalContainerChange::Type InReason
)
{
    return CharacterContainerChangeLogic::EasyOverrideContainerOnCl(
        this->GetOwnerAsCharacter(),
        this->AsContainerCrafter(),
        &this->OnContainerChangedDelegate,
        InIndex,
        InContent,
        InReason
    );
}

FString UCharacterCrafterComponent::ToString_ContainerCrafter(void) const
{
    FString Result = TEXT("ContainerCrafter{");
    for (const FSlot& Slot : this->ContainerCrafter) { Result += Slot.Content.ToShortString() + TEXT(","); }
    Result += TEXT("}");
    return Result;
}

AWorldCharacter* UCharacterCrafterComponent::GetOwnerAsCharacter(void) const
{
    return Cast<AWorldCharacter>(this->GetOwner());
}

bool UCharacterCrafterComponent::IsLocallyControlled(void) const
{
    return this->GetOwnerAsCharacter()->IsLocallyControlled();
}

bool UCharacterCrafterComponent::AddToContainerCrafter(const FAccumulated& Value)
{
    jrelaxedCheckNoEntry()
    return false;
}

void UCharacterCrafterComponent::OnLocalContainerCrafterChangedEventImpl(
    const ELocalContainerChange::Type InReason,
    const int32 InIndex
)
{
#if !UE_BUILD_SHIPPING
    if (UNetStatics::IsSafeDedicatedServer(this))
    {
        LOG_FATAL(LogWorldChar, "Cannot handle local container changed event on dedicated server.")
        return;
    }

    if (InReason == ELocalContainerChange::Invalid)
    {
        LOG_FATAL(LogWorldChar, "Invalid local container change reason.")
        return;
    }

    if (this->IsLocallyControlled() == false)
    {
        LOG_FATAL(LogWorldChar, "Cannot handle local container changed event on non-locally controlled character.")
        return;
    }
#endif

    if (UNetStatics::IsSafeClient(this) && InReason != ELocalContainerChange::Replicated)
    {
        this->OnContainerCrafterChangedEvent_ServerRPC(InReason, InIndex);
    }

    return;
}

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void UCharacterCrafterComponent::OnRep_ContainerCrafter(void)
{
#if !UE_BUILD_SHIPPING
    if (this->IsLocallyControlled() == false)
    {
        LOG_FATAL(LogWorldChar, "Cannot handle container replication on non-locally controlled character.")
        return;
    }
#endif /* !UE_BUILD_SHIPPING */

    this->OnContainerChangedDelegate.Broadcast(ELocalContainerChange::Replicated, -1);

    return;
}

bool UCharacterCrafterComponent::OnContainerCrafterChangedEvent_ServerRPC_Validate(const ELocalContainerChange::Type InReason, const int32 InIndex)
{
#if !UE_BUILD_SHIPPING
    if (this->IsLocallyControlled())
    {
        LOG_ERROR(LogWorldChar, "Disallowed for locals.")
        return false;
    }
#endif /* !UE_BUILD_SHIPPING */

    switch (InReason)
    {
    case ELocalContainerChange::Invalid:
    {
        return false;
    }
    case ELocalContainerChange::Primary:
    {
        return this->GetContainerCrafter(InIndex).OnPrimaryClicked(this->GetOwnerAsCharacter()->AsContainerOwner());
    }
    default:
    {
        LOG_ERROR(LogWorldChar, "Unhandled container change reason.")
        return false;
    }
    }
}

void UCharacterCrafterComponent::OnContainerCrafterChangedEvent_ServerRPC_Implementation(const ELocalContainerChange::Type InReason, const int32 InIndex)
{
    MARK_PROPERTY_DIRTY_FROM_NAME(UCharacterCrafterComponent, ContainerCrafter, this)
}
