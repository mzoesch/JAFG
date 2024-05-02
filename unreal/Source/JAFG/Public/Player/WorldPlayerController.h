// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "CommonPlayerController.h"
#include "InputTriggers.h"

#include "WorldPlayerController.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API AWorldPlayerController : public ACommonPlayerController
{
    GENERATED_BODY()

public:

    explicit AWorldPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

#pragma region Enhanced Input

public:

    //////////////////////////////////////////////////////////////////////////
    // Enhanced Input Extern Events
    //////////////////////////////////////////////////////////////////////////

    // Some subscribe methods

private:

    // The actual events

protected:

    void SetupCommonPlayerInput();

    /** Override this method to add custom key bindings in derived classes. */
    virtual auto BindAction(const FString& ActionName, UEnhancedInputComponent* EnhancedInputComponent) -> void;

    void OnToggleEscapeMenu(const FInputActionValue& Value);

private:

    auto BindAction(
        const FString& ActionName,
        UEnhancedInputComponent* EnhancedInputComponent,
        const ETriggerEvent Event,
        void (AWorldPlayerController::*Method) (const FInputActionValue& Value)
    ) -> void;

#pragma endregion Enhanced Input

};
