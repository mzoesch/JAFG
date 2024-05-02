// Copyright 2024 mzoesch. All rights reserved.

#include "Player/WorldPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/JAFGInputSubsystem.h"
#include "Input/CustomInputNames.h"

#define ENHANCED_INPUT_SUBSYSTEM                                    \
    ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>( \
        this->GetLocalPlayer()                                      \
    )

AWorldPlayerController::AWorldPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void AWorldPlayerController::BeginPlay(void)
{
    Super::BeginPlay();

    if (this->IsLocalController() == false)
    {
        LOG_WARNING(LogWorldChar, "Initialized on sv")
        return;
    }

    LOG_WARNING(LogWorldChar, "Initialized on cl")
    this->SetupCommonPlayerInput();

    return;
}

#pragma region Enhanced Input

void AWorldPlayerController::SetupCommonPlayerInput(void)
{
    if (this->InputEnabled() == false)
    {
        LOG_FATAL(LogWorldChar, "Input not enabled.")
        return;
    }

    this->SetupInputComponent();

#if WITH_EDITOR
    if (this->InputComponent == nullptr)
    {
        LOG_FATAL(LogWorldChar, "Input component is invalid.")
        return;
    }
#endif /* WITH_EDITOR */

    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(this->InputComponent);

#if WITH_EDITOR
    if (EnhancedInputComponent == nullptr)
    {
        LOG_ERROR(LogWorldChar, "Enhanced input component is invalid.")
        return;
    }
#endif /* WITH_EDITOR */

    UEnhancedInputLocalPlayerSubsystem* EnhancedSubsystem = ENHANCED_INPUT_SUBSYSTEM;

#if WITH_EDITOR
    if (EnhancedSubsystem == nullptr)
    {
        LOG_FATAL(LogWorldChar, "Subsystem is invalid.")
        return;
    }
#endif /* WITH_EDITOR */

    UJAFGInputSubsystem* JAFGInputSubsystem = this->GetLocalPlayer()->GetSubsystem<UJAFGInputSubsystem>();

#if WITH_EDITOR
    if (JAFGInputSubsystem == nullptr)
    {
        LOG_FATAL(LogWorldChar, "JAFG Input Subsystem is invalid.")
        return;
    }
#endif /* WITH_EDITOR */

    EnhancedSubsystem->ClearAllMappings();
    EnhancedSubsystem->AddMappingContext(JAFGInputSubsystem->GetSafeContextByName(InputContexts::Foot), 0);

    for (const FString& Name : JAFGInputSubsystem->GetAllActionNames())
    {
        this->BindAction(Name, EnhancedInputComponent);
    }

    return;
}

void AWorldPlayerController::BindAction(const FString& ActionName, UEnhancedInputComponent* EnhancedInputComponent)
{
    if (ActionName == InputActions::ToggleEscapeMenu)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldPlayerController::OnToggleEscapeMenu);
    }

    return;
}

void AWorldPlayerController::OnToggleEscapeMenu(const FInputActionValue& Value)
{
    LOG_WARNING(LogWorldChar, "Toggling escape menu.")
}

void AWorldPlayerController::BindAction(
    const FString& ActionName,
    UEnhancedInputComponent* EnhancedInputComponent,
    const ETriggerEvent Event,
    void(AWorldPlayerController::* Method) (const FInputActionValue& Value)
)
{
    /* Maybe we want to make this a member variable? */
    UJAFGInputSubsystem* JAFGInputSubsystem = this->GetLocalPlayer()->GetSubsystem<UJAFGInputSubsystem>();
    check( JAFGInputSubsystem )

    EnhancedInputComponent->BindAction(
        JAFGInputSubsystem->GetActionByName(ActionName),
        Event,
        this,
        Method
    );

    return;
}

#pragma endregion Enhanced Input

#undef ENHANCED_INPUT_SUBSYSTEM
