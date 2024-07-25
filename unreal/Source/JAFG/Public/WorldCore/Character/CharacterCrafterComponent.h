// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "ContainerCrafter.h"
#include "Components/ActorComponent.h"

#include "CharacterCrafterComponent.generated.h"

JAFG_VOID

class AWorldCharacter;

/**
 * Allows the world character to also be a container crafter as they are already a container, we cannot implement
 * the container crafter interface directly as it would case ambiguity with the container interfaces.
 *
 * @note This is really only meant to be attached to the world character and will therefore not work with other
 *       AActors.
 */
UCLASS(NotBlueprintable)
class JAFG_API UCharacterCrafterComponent : public UActorComponent, public IContainerCrafter
{
    GENERATED_BODY()

public:

    explicit UCharacterCrafterComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // AActor implementation
    virtual void BeginPlay(void) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    // ~AActor implementation

public:

    FORCEINLINE auto AsContainerCrafter(void) -> IContainerCrafter* { return this; }

    // IContainerCrafter interface
    virtual auto EasyAddToContainerCrafter(const FAccumulated& Value) -> bool override;
    virtual auto EasyChangeContainerCrafter(
        const int32 InIndex,
        const accamount_t_signed InAmount,
        const ELocalContainerChange::Type InReason = ELocalContainerChange::Custom
    ) -> bool override;
    virtual auto EasyChangeContainerCrafter(
        const int32 InIndex,
        IContainerOwner* InOwner,
        const TFunctionRef<bool(const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner)>& InAlternator,
        const ELocalContainerChange::Type InReason
    ) -> bool override;
    virtual bool EasyChangeContainerCrafterCl(
        const int32 InIndex,
        IContainerOwner* InOwner,
        const TFunctionRef<bool(const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner)>& InAlternator,
        const ELocalContainerChange::Type InReason
    ) override;
    virtual auto EasyOverrideContainerCrafterOnCl(
        const int32 InIndex,
        const FAccumulated& InContent,
        const ELocalContainerChange::Type InReason
    ) -> bool override;
    virtual auto ToString_ContainerCrafter(void) const -> FString override;
    // ~IContainerCrafter interface

protected:

    auto GetOwnerAsCharacter(void) const -> AWorldCharacter*;
    auto IsLocallyControlled(void) const -> bool;

    // IContainerCrafter interface
    FORCEINLINE virtual auto IsContainerCrafterInitialized(void) const -> bool override { return this->ContainerCrafter.Num() > 0; }
    FORCEINLINE virtual auto GetContainerCrafterSize(void) const -> int32 override { return this->ContainerCrafter.Num(); }
    FORCEINLINE virtual auto GetContainerCrafter(void) -> TArray<FSlot>& override { return this->ContainerCrafter; }
    FORCEINLINE virtual auto GetContainerCrafter(void) const -> const TArray<FSlot>& override { return this->ContainerCrafter; }
    FORCEINLINE virtual auto GetContainerCrafter(const int32 Index) -> FSlot& override { return this->ContainerCrafter[Index]; }
    FORCEINLINE virtual auto GetContainerCrafter(const int32 Index) const -> const FSlot& override { return this->ContainerCrafter[Index]; }
    FORCEINLINE virtual auto GetContainerCrafterValue(const int32 Index) const -> FAccumulated override { return this->ContainerCrafter[Index].Content; }
    FORCEINLINE virtual auto GetContainerCrafterValueRef(const int32 Index) -> FAccumulated& override { return this->ContainerCrafter[Index].Content; }
                virtual bool AddToContainerCrafter(const FAccumulated& Value) override;
    // ~IContainerCrafter interface

    void OnLocalContainerCrafterChangedEventImpl(const ELocalContainerChange::Type InReason, const int32 InIndex);

    UPROPERTY(ReplicatedUsing=OnRep_ContainerCrafter)
    TArray<FSlot> ContainerCrafter;
    UFUNCTION()
    void OnRep_ContainerCrafter( /* void */ );

    UFUNCTION(Server, Reliable, WithValidation)
    void OnContainerCrafterChangedEvent_ServerRPC(const ELocalContainerChange::Type InReason, const int32 InIndex);
};
