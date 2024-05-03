// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputTriggers.h"

#include "WorldCharacter.generated.h"

struct FInputActionValue;
class UInputComponent;

UCLASS(NotBlueprintable)
class JAFG_API AWorldCharacter : public ACharacter
{
    GENERATED_BODY()

public:

    explicit AWorldCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // AActor implementation
    virtual void BeginPlay(void) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    // ~AActor implementation

#pragma region Enhanced Input

public:

    //////////////////////////////////////////////////////////////////////////
    // Enhanced Input Extern Events
    //////////////////////////////////////////////////////////////////////////

    // Some subscribe methods

private:

    // The actual events

protected:

    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    /** Override this method to add custom key bindings in derived classes. */
    virtual auto BindAction(const FString& ActionName, UEnhancedInputComponent* EnhancedInputComponent) -> void;

    virtual void OnMove(const FInputActionValue& Value);
    virtual void OnLook(const FInputActionValue& Value);
    virtual void OnTriggerJump(const FInputActionValue& Value);
    virtual void OnCompleteJump(const FInputActionValue& Value);

private:

    auto BindAction(
        const FString& ActionName,
        UEnhancedInputComponent* EnhancedInputComponent,
        const ETriggerEvent Event,
        void (AWorldCharacter::* Method) (const FInputActionValue& Value)
    ) -> void;

#pragma endregion Enhanced Input

protected:

    //////////////////////////////////////////////////////////////////////////
    // Enhanced Event Bindings
    //////////////////////////////////////////////////////////////////////////

    FDelegateHandle EscapeMenuVisibilityChangedHandle;
    virtual auto OnEscapeMenuVisibilityChanged(const bool bVisible) -> void;
};
