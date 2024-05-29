// Copyright 2024 mzoesch. All rights reserved.

#include "Player/WorldPlayerController.h"

#include "ChatComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/CustomInputNames.h"
#include "Network/MyHyperlaneComponent.h"
#include "SettingsData/JAFGInputSubsystem.h"

#define ENHANCED_INPUT_SUBSYSTEM                                    \
    ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>( \
        this->GetLocalPlayer()                                      \
    )

AWorldPlayerController::AWorldPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->HyperlaneComponent = CreateDefaultSubobject<UMyHyperlaneComponent>(TEXT("HyperlaneComponent"));
    this->ChatComponent      = CreateDefaultSubobject<UChatComponent>(TEXT("ChatComponent"));

    return;
}

void AWorldPlayerController::BeginPlay(void)
{
    Super::BeginPlay();

    if (this->IsLocalController() == false)
    {
        return;
    }

    this->SetupCommonPlayerInput();

    return;
}

#pragma region Enhanced Input

FDelegateHandle AWorldPlayerController::SubscribeToEscapeMenuVisibilityChanged(const FSlateVisibilityChangedSignature::FDelegate& Delegate)
{
    if (this->IsLocalController() == false)
    {
        LOG_FATAL(LogWorldChar, "Not a local controller.")
        return FDelegateHandle();
    }

    return this->EscapeMenuVisibilityChangedDelegate.Add(Delegate);
}

bool AWorldPlayerController::UnSubscribeToEscapeMenuVisibilityChanged(const FDelegateHandle& Handle)
{
    return this->EscapeMenuVisibilityChangedDelegate.Remove(Handle);
}

FDelegateHandle AWorldPlayerController::SubscribeToDebugScreenVisibilityChanged(const FSlateVisibilityChangedSignature::FDelegate& Delegate)
{
    if (this->IsLocalController() == false)
    {
        LOG_FATAL(LogWorldChar, "Not a local controller.")
        return FDelegateHandle();
    }

    return this->DebugScreenVisibilityChangedDelegate.Add(Delegate);
}

bool AWorldPlayerController::UnSubscribeToDebugScreenVisibilityChanged(const FDelegateHandle& Handle)
{
    return this->DebugScreenVisibilityChangedDelegate.Remove(Handle);
}

FDelegateHandle AWorldPlayerController::SubscribeToChatVisibilityChanged(const FSlateVisibilityChangedSignature::FDelegate& Delegate)
{
    if (this->IsLocalController() == false)
    {
        LOG_FATAL(LogWorldChar, "Not a local controller.")
        return FDelegateHandle();
    }

    return this->ChatVisibilityChangedDelegate.Add(Delegate);
}

bool AWorldPlayerController::UnSubscribeToChatVisibilityChanged(const FDelegateHandle& Handle)
{
    return this->ChatVisibilityChangedDelegate.Remove(Handle);
}

FDelegateHandle AWorldPlayerController::SubscribeToQuickSessionPreviewVisibilityChanged(const FSlateVisibilityChangedSignature::FDelegate& Delegate)
{
    if (this->IsLocalController() == false)
    {
        LOG_FATAL(LogWorldChar, "Not a local controller.")
        return FDelegateHandle();
    }

    return this->OnQuickSessionPreviewVisibilityChangedDelegate.Add(Delegate);
}

bool AWorldPlayerController::UnSubscribeToQuickSessionPreviewVisibilityChanged(const FDelegateHandle& Handle)
{
    return this->OnQuickSessionPreviewVisibilityChangedDelegate.Remove(Handle);
}

FDelegateHandle AWorldPlayerController::SubscribeToChatHistoryLookup(const FChatHistoryLookupSignature::FDelegate& Delegate)
{
    if (this->IsLocalController() == false)
    {
        LOG_FATAL(LogWorldChar, "Not a local controller.")
        return FDelegateHandle();
    }

    return this->ChatHistoryLookupDelegate.Add(Delegate);
}

bool AWorldPlayerController::UnSubscribeToChatHistoryLookup(const FDelegateHandle& Handle)
{
    return this->ChatHistoryLookupDelegate.Remove(Handle);
}

void AWorldPlayerController::SetupCommonPlayerInput(void)
{
    LOG_VERBOSE(LogWorldChar, "Called.")

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
    EnhancedSubsystem->AddMappingContext(JAFGInputSubsystem->GetSafeContextValue(InputContexts::FootWalk), 0);

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

    else if (ActionName == InputActions::ToggleDebugScreen)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldPlayerController::OnToggleDebugScreen);
    }

    else if (ActionName == InputActions::ToggleChat)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldPlayerController::OnToggleChat);
    }

    else if (ActionName == InputActions::ToggleQuickSessionPreview)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldPlayerController::OnStartedQuickSessionPreview);
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Completed, &AWorldPlayerController::OnCompletedQuickSessionPreview);
    }

    else if (ActionName == InputActions::PreviousChatStdIn)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldPlayerController::OnPreviousChatStdIn);
    }

    else if (ActionName == InputActions::NextChatStdIn)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldPlayerController::OnNextChatStdIn);
    }

    return;
}

void AWorldPlayerController::OnToggleEscapeMenu(const FInputActionValue& Value)
{
    if (this->EscapeMenuVisibilityChangedDelegate.IsBound() == false)
    {
        LOG_FATAL(LogWorldChar, "No subscribers to Escape Menu Visibility Changed.")
        return;
    }

    this->bEscapeMenuVisible = !this->bEscapeMenuVisible;
    this->ShowMouseCursor(this->bEscapeMenuVisible);

    this->EscapeMenuVisibilityChangedDelegate.Broadcast(this->bEscapeMenuVisible);

    return;
}

void AWorldPlayerController::OnToggleDebugScreen(const FInputActionValue& Value)
{
    if (this->DebugScreenVisibilityChangedDelegate.IsBound() == false)
    {
        LOG_FATAL(LogWorldChar, "No subscribers to Debug Screen Visibility Changed.")
        return;
    }

    this->bDebugScreenVisible = !this->bDebugScreenVisible;
    this->DebugScreenVisibilityChangedDelegate.Broadcast(this->bDebugScreenVisible);

    return;
}

void AWorldPlayerController::OnToggleChat(const FInputActionValue& Value)
{
    if (this->ChatVisibilityChangedDelegate.IsBound() == false)
    {
        LOG_FATAL(LogWorldChar, "No subscribers to Chat Visibility Changed.")
        return;
    }

    this->bChatVisible = !this->bChatVisible;
    this->ShowMouseCursor(this->bChatVisible);

    this->ChatVisibilityChangedDelegate.Broadcast(this->bChatVisible);

    return;
}

void AWorldPlayerController::OnStartedQuickSessionPreview(const FInputActionValue& Value)
{
    if (this->OnQuickSessionPreviewVisibilityChangedDelegate.IsBound() == false)
    {
        LOG_FATAL(LogWorldChar, "No subscribers to Quick Session Preview Visibility Changed.")
        return;
    }

    this->bQuickSessionPreviewVisible = true;

    this->OnQuickSessionPreviewVisibilityChangedDelegate.Broadcast(this->bQuickSessionPreviewVisible);

    return;
}

void AWorldPlayerController::OnCompletedQuickSessionPreview(const FInputActionValue& Value)
{
    if (this->OnQuickSessionPreviewVisibilityChangedDelegate.IsBound() == false)
    {
        LOG_FATAL(LogWorldChar, "No subscribers to Quick Session Preview Visibility Changed.")
        return;
    }

    this->bQuickSessionPreviewVisible = false;

    this->OnQuickSessionPreviewVisibilityChangedDelegate.Broadcast(this->bQuickSessionPreviewVisible);

    return;
}

void AWorldPlayerController::OnPreviousChatStdIn(const FInputActionValue& Value)
{
    this->ChatHistoryLookupDelegate.Broadcast(true);
}

void AWorldPlayerController::OnNextChatStdIn(const FInputActionValue& Value)
{
    this->ChatHistoryLookupDelegate.Broadcast(false);
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
        JAFGInputSubsystem->GetSafeActionValue(ActionName),
        Event,
        this,
        Method
    );

    return;
}

bool AWorldPlayerController::SafelyIncreaseStrikeCount(void)
{
    if (UNetStatics::IsSafeClient(this))
    {
        LOG_FATAL(LogStrikeSystem, "Disallowed call on client.")
        return false;
    }

    if (this->IsLocalController())
    {
        LOG_WARNING(LogStrikeSystem, "Local controller is disallowed from having strikes.")
        return false;
    }

    this->RemoveOutDatedStrikes();

    this->Strikes.Add(this->GetCurrentStrikeTime());
    LOG_WARNING(LogStrikeSystem, "Added strike for client [%s]. Timestamp: %d.", *this->GetDisplayName(), this->GetCurrentStrikeTime())

    if (this->Strikes.Num() >= this->MaxStrikeCount)
    {
        LOG_WARNING(LogStrikeSystem, "Client [%s] reached strike maximum.", *this->GetDisplayName())
        if (this->GetWorldGameSession()->KickPlayer(this, FText::FromString(TEXT("Reached strike maximum."))) == false)
        {
            LOG_FATAL(LogStrikeSystem, "Failed to kick player [%s].", *this->GetDisplayName())
        }

        return true;
    }

    return false;
}

void AWorldPlayerController::RemoveOutDatedStrikes(void)
{
    this->Strikes.RemoveAll( [this] (const FStrike Strike)
    {
        if (this->IsStrikeOutDated(Strike))
        {
            LOG_WARNING(LogStrikeSystem, "Removed outdated strike for client [%s]. Timestamp: %d.", *this->GetDisplayName(), Strike)
            return true;
        }

        return false;
    });

    return;
}

bool AWorldPlayerController::IsStrikeOutDated(const FStrike& Strike) const
{
    return  this->GetCurrentStrikeTime() - Strike > this->StrikeDurationInSeconds;
}

int32 AWorldPlayerController::GetCurrentStrikeTime(void) const
{
    return this->GetGameTimeSinceCreation();
}

#pragma endregion Enhanced Input

#undef ENHANCED_INPUT_SUBSYSTEM
