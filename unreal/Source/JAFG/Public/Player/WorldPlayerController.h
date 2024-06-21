// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Player/CommonPlayerController.h"
#include "InputTriggers.h"
#include "GameFramework/GameModeBase.h"
#include "WorldCore/WorldGameSession.h"
#include "WorldCore/WorldPlayerState.h"

#include "WorldPlayerController.generated.h"

JAFG_VOID

/** The time in seconds after this AActor creation, when this strike was marked. */
typedef int32 FStrike;

class AWorldCharacter;
class UServerWorldSettingsReplicationComponent;
class UChatMenu;
class UChatComponent;
class UEscapeMenuResumeButton;
class UMyHyperlaneComponent;

DECLARE_MULTICAST_DELEGATE(FSlateVisibilityChangedOwnerVisSignature)
DECLARE_MULTICAST_DELEGATE_OneParam(FSlateVisibilityChangedSignature, const bool /* bVisible */)

DECLARE_MULTICAST_DELEGATE_OneParam(FChatHistoryLookupSignature, const bool /* bPrevious */)

/**
 * Only called on the owing local connection.
 * @note Both values can be null.
 */
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnWorldCharacterChangeSignature, AWorldCharacter* StrongOldCharacter, AWorldCharacter* NewCharater)

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

    // UObject implementation
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    // ~UObject implementation

    UPROPERTY()
    TObjectPtr<UMyHyperlaneComponent> HyperlaneComponent = nullptr;

    UPROPERTY()
    TObjectPtr<UChatComponent> ChatComponent = nullptr;

    UPROPERTY()
    TObjectPtr<UServerWorldSettingsReplicationComponent> ServerWorldSettingsReplicationComponent = nullptr;

#pragma region Enhanced Input

public:

    //////////////////////////////////////////////////////////////////////////
    // Enhanced Input Extern Events
    //////////////////////////////////////////////////////////////////////////

    auto SubscribeToEscapeMenuVisibilityChanged(const FSlateVisibilityChangedSignature::FDelegate& Delegate) -> FDelegateHandle;
    auto UnSubscribeToEscapeMenuVisibilityChanged(const FDelegateHandle& Handle) -> bool;

    auto SubscribeToDebugScreenVisibilityChanged(const FSlateVisibilityChangedSignature::FDelegate& Delegate) -> FDelegateHandle;
    auto UnSubscribeToDebugScreenVisibilityChanged(const FDelegateHandle& Handle) -> bool;

    auto SubscribeToChatVisibilityChanged(const FSlateVisibilityChangedSignature::FDelegate& Delegate) -> FDelegateHandle;
    auto UnSubscribeToChatVisibilityChanged(const FDelegateHandle& Handle) -> bool;

    auto SubscribeToQuickSessionPreviewVisibilityChanged(const FSlateVisibilityChangedSignature::FDelegate& Delegate) -> FDelegateHandle;
    auto UnSubscribeToQuickSessionPreviewVisibilityChanged(const FDelegateHandle& Handle) -> bool;

    auto SubscribeToChatHistoryLookup(const FChatHistoryLookupSignature::FDelegate& Delegate) -> FDelegateHandle;
    auto UnSubscribeToChatHistoryLookup(const FDelegateHandle& Handle) -> bool;

private:

    /** Obviously local controller only. */
    FSlateVisibilityChangedSignature EscapeMenuVisibilityChangedDelegate;
    /** Obviously local controller only. */
    FSlateVisibilityChangedSignature DebugScreenVisibilityChangedDelegate;
    /** Obviously local controller only. */
    FSlateVisibilityChangedSignature ChatVisibilityChangedDelegate;
    /** Obviously local controller only. */
    FSlateVisibilityChangedSignature OnQuickSessionPreviewVisibilityChangedDelegate;
    /** Obviously local controller only. */
    FChatHistoryLookupSignature ChatHistoryLookupDelegate;

protected:

    //////////////////////////////////////////////////////////////////////////
    // Enhanced Input
    //////////////////////////////////////////////////////////////////////////

    /** Should bind common behavior that is not unique to any character and should always be bound. */
    void SetupCommonPlayerInput(void);

    /** Override this method to add custom key bindings in derived classes. */
    virtual auto BindAction(const FString& ActionName, UEnhancedInputComponent* EnhancedInputComponent) -> void;

    virtual void OnToggleEscapeMenu(const FInputActionValue& Value); friend UEscapeMenuResumeButton;
    virtual void OnToggleDebugScreen(const FInputActionValue& Value);
    virtual void OnToggleChat(const FInputActionValue& Value); friend UChatMenu;
    virtual void OnStartedQuickSessionPreview(const FInputActionValue& Value);
    virtual void OnCompletedQuickSessionPreview(const FInputActionValue& Value);
    virtual void OnPreviousChatStdIn(const FInputActionValue& Value);
    virtual void OnNextChatStdIn(const FInputActionValue& Value);

private:

    auto BindAction(
        const FString& ActionName,
        UEnhancedInputComponent* EnhancedInputComponent,
        const ETriggerEvent Event,
        void (AWorldPlayerController::* Method) (const FInputActionValue& Value)
    ) -> void;

    bool bEscapeMenuVisible          = false;
    bool bDebugScreenVisible         = false;
    bool bChatVisible                = false;
    bool bQuickSessionPreviewVisible = false;

#pragma endregion Enhanced Input

#pragma region Strike

public:

    //////////////////////////////////////////////////////////////////////////
    // Strike
    //////////////////////////////////////////////////////////////////////////

    /**
     * @return True if the player has reached their maximum strike count.
     *         They are going to be disconnected short after.
     */
    auto SafelyIncreaseStrikeCount(void) -> bool;

protected:

    auto RemoveOutDatedStrikes(void) -> void;
    auto IsStrikeOutDated(const FStrike& Strike) const -> bool;
    auto GetCurrentStrikeTime(void) const -> int32;

    /** How long a strike is valid for. */
    const int32 StrikeDurationInSeconds = 60;
    TArray<FStrike> Strikes = TArray<FStrike>();
    const uint8 MaxStrikeCount = 3;

#pragma endregion Strike

#pragma region World Spawning

public:

    auto HasSuccessfullySpawnedCharacter(void) const -> bool;
    auto IsServerReadyForCharacterSpawn(void) const -> bool;
    auto SetServerReadyForCharacterSpawn(void) -> void;
    auto IsClientReadyForCharacterSpawn(void) const -> bool;
    auto SetClientReadyForCharacterSpawn(void) -> void;
    auto SpawnCharacterToWorld(void) -> void;

    // AController implementation
    virtual void SetPawn(APawn* InPawn) override;
    virtual void OnRep_Pawn(void) override;
    // ~AController implementation

    FOnWorldCharacterChangeSignature OnWorldCharacterChange;

protected:

    void DestroyLoadingScreen(void) const;

private:

    UPROPERTY(Replicated)
    bool bHasSuccessfullySpawnedCharacter = false;
    /** If the server is ready for this client to be spawned. */
    bool bServerReadyForCharacterSpawn    = false;
    /** Evaluated by client and then send and set to server. */
    bool bClientReadyForCharacterSpawn    = false;

    UFUNCTION(Server, Reliable, WithValidation)
    void TellServerThatClientIsReadyForCharacterSpawn_ServerRPC( /* void */ );

    UFUNCTION()
    void PrivateOnPossessedPawnChangeListener(APawn* InOldPawn, APawn* InNewPawn);

#pragma endregion World Spawning

    //////////////////////////////////////////////////////////////////////////
    // Useful Internal Getters
    //////////////////////////////////////////////////////////////////////////

protected:

    /** Server only. */
    FORCEINLINE auto GetWorldGameSession(void) const -> AWorldGameSession* { return Cast<AWorldGameSession>(this->GetWorld()->GetAuthGameMode()->GameSession); }
    template<class T>
    FORCEINLINE auto GetCharacter(void) const -> T* { return CastChecked<T>(this->GetPawn(), ECastCheckedType::NullAllowed); }

public:

    //////////////////////////////////////////////////////////////////////////
    // Public Getters
    //////////////////////////////////////////////////////////////////////////

    auto HasReceivedServerWorldSettings(void) const -> bool;

    FORCEINLINE auto GetWorldPlayerState(void) const -> AWorldPlayerState* { return this->GetPlayerState<AWorldPlayerState>(); }
    FORCEINLINE auto GetDisplayName(void) const -> FString { return this->GetWorldPlayerState()->GetPlayerName(); }

    /** @return True if OutLocation is meaningful. */
    bool GetPredictedCharacterLocation(FVector& OutLocation) const;
};
