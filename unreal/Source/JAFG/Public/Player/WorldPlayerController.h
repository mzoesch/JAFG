// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "CommonPlayerController.h"
#include "InputTriggers.h"

#include "WorldPlayerController.generated.h"

JAFG_VOID

DECLARE_MULTICAST_DELEGATE_OneParam(FSlateVisibilityChangedSignature, const bool /* bVisible */ )

#define ADD_SLATE_VIS_DELG(Method)                                            \
    FSlateVisibilityChangedSignature::FDelegate::CreateUObject(this, &Method)

UCLASS(NotBlueprintable)
class JAFG_API AWorldPlayerController : public ACommonPlayerController
{
    GENERATED_BODY()

public:

    explicit AWorldPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // AActor implementation
    virtual void BeginPlay(void) override;
    // ~AActor implementation

#pragma region Enhanced Input

public:

    //////////////////////////////////////////////////////////////////////////
    // Enhanced Input Extern Events
    //////////////////////////////////////////////////////////////////////////

    // Some subscribe methods

    auto SubscribeToEscapeMenuVisibilityChanged(const FSlateVisibilityChangedSignature::FDelegate& Delegate) -> FDelegateHandle;
    auto UnSubscribeToEscapeMenuVisibilityChanged(const FDelegateHandle& Handle) -> bool;

private:

    /** Obviously client only. */
    FSlateVisibilityChangedSignature EscapeMenuVisibilityChangedDelegate;

protected:

    /** Should bind common behavior that is not unique to any character and should always be bound. */
    void SetupCommonPlayerInput(void);

    /** Override this method to add custom key bindings in derived classes. */
    virtual auto BindAction(const FString& ActionName, UEnhancedInputComponent* EnhancedInputComponent) -> void;

    void OnToggleEscapeMenu(const FInputActionValue& Value);

private:

    auto BindAction(
        const FString& ActionName,
        UEnhancedInputComponent* EnhancedInputComponent,
        const ETriggerEvent Event,
        void (AWorldPlayerController::* Method) (const FInputActionValue& Value)
    ) -> void;

    bool bEscapeMenuVisible;

#pragma endregion Enhanced Input

};
