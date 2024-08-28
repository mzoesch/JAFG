
// Copyright 2024 mzoesch. All rights reserved.

#include "Player/WorldPlayerController.h"
#include "ChatComponentImpl.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/CustomInputNames.h"
#include "Net/UnrealNetwork.h"
#include "Network/MyHyperlaneComponent.h"
#include "SettingsData/JAFGInputSubsystem.h"
#include "UI/WorldHUD.h"
#include "WorldCore/ChunkWorldSettings.h"
#include "WorldCore/WorldCharacter.h"
#include "WorldCore/WorldGameMode.h"
#include "WorldCore/WorldPawn.h"
#include "WorldCore/ActorInfos/ContainerReplicatorActor.h"
#if WITH_EDITOR
    #include "Editor.h"
    #include "LevelEditorViewport.h"
#endif /* WITH_EDITOR */

#define ENHANCED_INPUT_SUBSYSTEM                                    \
    ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>( \
        this->GetLocalPlayer()                                      \
    )

AWorldPlayerController::AWorldPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->HyperlaneComponent                      =
        ObjectInitializer.CreateDefaultSubobject<UMyHyperlaneComponent>(this, TEXT("HyperlaneComponent"));
    this->ChatComponent                           =
        ObjectInitializer.CreateDefaultSubobject<UChatComponentImpl>(this, TEXT("ChatComponent"));
    this->ServerWorldSettingsReplicationComponent =
        ObjectInitializer.CreateDefaultSubobject<UServerWorldSettingsReplicationComponent>(this, TEXT("ServerWorldSettingsReplicationComponent"));
    this->ContainerReplicatorComponent            =
        ObjectInitializer.CreateDefaultSubobject<UContainerReplicatorComponent>(this, TEXT("ContainerReplicatorComponent"));

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

    if (UNetStatics::IsSafeListenServer(this))
    {
        /*
         * We are the client and server. If the server is ready, then everything is ready as the client and server
         * will always share the same world and same generation subsystem.
         */
        this->bClientReadyForCharacterSpawn = true;
    }

    this->OnPossessedPawnChanged.AddDynamic(this, &AWorldPlayerController::PrivateOnPossessedPawnChangeListener);

    /*
     * We have to call manually here as the pawn is set before the client is logged in. There is no waiting for
     * chunk generation to complete in a dev world.
     */
    if (this->GetWorld()->GetName() == RegisteredWorlds::Dev && UNetStatics::IsSafeClient(this))
    {
        AsyncTask(ENamedThreads::GameThread, [this] (void)
        {
            this->PrivateOnPossessedPawnChangeListener(nullptr, this->GetCharacter<AWorldCharacter>());
        });
    }

    return;
}

void AWorldPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AWorldPlayerController, bHasSuccessfullySpawnedCharacter);
    return;
}

#pragma region Enhanced Input

FDelegateHandle AWorldPlayerController::SubscribeToEscapeMenuVisibilityChangeRequest(const FSlateVisibilityChnageRequestDelegateSignature::FDelegate& Delegate)
{
    if (this->IsLocalController() == false)
    {
        LOG_FATAL(LogWorldChar, "Not a local controller.")
        return FDelegateHandle();
    }

    return this->EscapeMenuVisibilityChangeRequestDelegate.Add(Delegate);
}

bool AWorldPlayerController::UnSubscribeToEscapeMenuVisibilityChangeRequest(const FDelegateHandle& Handle)
{
    return this->EscapeMenuVisibilityChangeRequestDelegate.Remove(Handle);
}

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

FDelegateHandle AWorldPlayerController::SubscribeToFillSuggestionToChatStdIn(const FOnFillSuggestionToChatStdInDelegateSignature::FDelegate& Delegate)
{
    if (this->IsLocalController() == false)
    {
        LOG_FATAL(LogWorldChar, "Not a local controller.")
        return FDelegateHandle();
    }

    return this->OnFillSuggestionToChatStdInDelegate.Add(Delegate);
}

bool AWorldPlayerController::UnSubscribeToFillSuggestionToChatStdIn(const FDelegateHandle& Handle)
{
    return this->OnFillSuggestionToChatStdInDelegate.Remove(Handle);
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

    else if (ActionName == InputActions::FillSuggestionToChatStdIn)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldPlayerController::OnFillSuggestionToChatStdIn);
    }

    return;
}

void AWorldPlayerController::OnToggleEscapeMenu(const FInputActionValue& Value)
{
    bool bAllowChange = true;

    this->EscapeMenuVisibilityChangeRequestDelegate.Broadcast(!this->bEscapeMenuVisible, bAllowChange, [this] (void)
    {
        LOG_VERY_VERBOSE(LogCommonSlate, "Late allowed. Now retrying to close the escape menu.")
        this->OnToggleEscapeMenu(FInputActionValue());
    });

    if (bAllowChange == false)
    {
        return;
    }

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

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void AWorldPlayerController::OnPreviousChatStdIn(const FInputActionValue& Value)
{
    this->ChatHistoryLookupDelegate.Broadcast(true);
}

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void AWorldPlayerController::OnNextChatStdIn(const FInputActionValue& Value)
{
    this->ChatHistoryLookupDelegate.Broadcast(false);
}

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void AWorldPlayerController::OnFillSuggestionToChatStdIn(const FInputActionValue& Value)
{
    this->OnFillSuggestionToChatStdInDelegate.Broadcast();
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

#pragma endregion Enhanced Input

#pragma region Strike

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

#if WITH_STRIKE_SUBSYSTEM

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
#endif /* WITH_STRIKE_SUBSYSTEM */

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

#pragma endregion Strike

#pragma region World Spawning

bool AWorldPlayerController::HasSuccessfullySpawnedCharacter(void) const
{
#if WITH_EDITOR
    if (GEditor && GEditor->IsSimulateInEditorInProgress())
    {
        return true;
    }
#endif /* WITH_EDITOR */

    return this->bHasSuccessfullySpawnedCharacter;
}

bool AWorldPlayerController::IsServerReadyForCharacterSpawn(void) const
{
    return this->bServerReadyForCharacterSpawn;
}

void AWorldPlayerController::SetServerReadyForCharacterSpawn(void)
{
    if (UNetStatics::IsSafeClient(this))
    {
        LOG_FATAL(LogWorldController, "Disallowed call on client.")
        return;
    }

    if (this->bServerReadyForCharacterSpawn)
    {
        LOG_WARNING(LogWorldController, "Server is already ready for character spawn.")
        return;
    }

    this->bServerReadyForCharacterSpawn = true;

    return;
}

bool AWorldPlayerController::IsClientReadyForCharacterSpawn(void) const
{
    return this->bClientReadyForCharacterSpawn;
}

void AWorldPlayerController::SetClientReadyForCharacterSpawn(void)
{
    if (UNetStatics::IsSafeClient(this) == false)
    {
        LOG_FATAL(LogWorldController, "Disallowed call on client.")
        return;
    }

    if (this->bClientReadyForCharacterSpawn)
    {
        LOG_FATAL(LogWorldController, "Client is already ready for character spawn.")
        return;
    }

    this->bClientReadyForCharacterSpawn = true;

    this->TellServerThatClientIsReadyForCharacterSpawn_ServerRPC();

    return;
}

void AWorldPlayerController::SpawnCharacterToWorld(void)
{
    if (this->bHasSuccessfullySpawnedCharacter)
    {
        LOG_FATAL(LogWorldController, "Character already spawned.")
        return;
    }

    this->GetWorld()->GetAuthGameMode<AWorldGameMode>()->SpawnCharacterForPlayer(this);

    this->bHasSuccessfullySpawnedCharacter = true;

    return;
}

void AWorldPlayerController::SetPawn(APawn* InPawn)
{
    Super::SetPawn(InPawn);

    if (InPawn && InPawn->IsA<AWorldCharacter>() && InPawn->IsLocallyControlled())
    {
        this->DestroyLoadingScreen();
    }

    return;
}

void AWorldPlayerController::OnRep_Pawn(void)
{
    Super::OnRep_Pawn();

    if (this->GetPawn() && this->GetPawn()->IsA<AWorldCharacter>() && this->IsLocalController())
    {
        this->DestroyLoadingScreen();
    }

    return;
}

void AWorldPlayerController::DestroyLoadingScreen(void) const
{
    jcheck( this->MyHUD ) jcheck( this->GetHUD<AWorldHUD>() )
    this->GetHUD<AWorldHUD>()->DestroyLoadingScreen();
    return;
}

bool AWorldPlayerController::TellServerThatClientIsReadyForCharacterSpawn_ServerRPC_Validate(void)
{
    /* They may only send the request once. */
    return this->bClientReadyForCharacterSpawn == false;
}

void AWorldPlayerController::TellServerThatClientIsReadyForCharacterSpawn_ServerRPC_Implementation(void)
{
    this->bClientReadyForCharacterSpawn = true;
}

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void AWorldPlayerController::PrivateOnPossessedPawnChangeListener(APawn* InOldPawn, APawn* InNewPawn)
{
    if (this->IsLocalController())
    {
        this->OnWorldCharacterChange.Broadcast(Cast<AWorldCharacter>(InOldPawn), Cast<AWorldCharacter>(InNewPawn));
    }

    return;
}

#pragma endregion World Spawning

bool AWorldPlayerController::HasReceivedServerWorldSettings(void) const
{
    return this->ServerWorldSettingsReplicationComponent->HasReplicatedSettings();
}

bool AWorldPlayerController::GetPredictedCharacterLocation(FVector& OutLocation) const
{
    if (this->GetPawnOrSpectator() == nullptr)
    {
        return false;
    }

#if WITH_EDITOR
    if (GEditor && GEditor->IsSimulateInEditorInProgress())
    {
        OutLocation = GCurrentLevelEditingViewportClient->ViewTransformPerspective.GetLocation();
        return true;
    }
#endif /* WITH_EDITOR */

    if (this->GetPawnOrSpectator()->IsA(AWorldCharacter::StaticClass()))
    {
        OutLocation = this->GetPawn<AWorldCharacter>()->GetFeetLocation();
        return true;
    }

    if (this->GetPawnOrSpectator()->IsA(AWorldPawn::StaticClass()))
    {
        OutLocation = FVector::ZeroVector;
        return true;
    }

    return false;
}

#undef ENHANCED_INPUT_SUBSYSTEM
