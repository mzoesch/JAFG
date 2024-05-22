// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Player/CommonPlayerController.h"
#include "InputTriggers.h"
#include "GameFramework/GameModeBase.h"
#include "WorldCore/WorldGameSession.h"
#include "WorldCore/WorldPlayerState.h"

#include "WorldPlayerController.generated.h"

/** The time in seconds after this AActor creation, when this strike was marked. */
typedef int32 FStrike;

JAFG_VOID

class UChatMenu;
class UChatComponent;
class UResumeEntryWidget;
class UMyHyperlaneComponent;

DECLARE_MULTICAST_DELEGATE(FSlateVisibilityChangedOwnerVisSignature)
DECLARE_MULTICAST_DELEGATE_OneParam(FSlateVisibilityChangedSignature, const bool /* bVisible */ )

DECLARE_MULTICAST_DELEGATE_OneParam(FChatHistoryLookupSignature, const bool /* bPrevious */ )

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

    UPROPERTY()
    TObjectPtr<UMyHyperlaneComponent> HyperlaneComponent;

    UPROPERTY()
    TObjectPtr<UChatComponent> ChatComponent;

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

    virtual void OnToggleEscapeMenu(const FInputActionValue& Value); friend UResumeEntryWidget;
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
     *         They are going to be disconnected short after. Safely
     */
    bool SafelyIncreaseStrikeCount(void);

protected:

    void RemoveOutDatedStrikes(void);
    bool IsStrikeOutDated(const FStrike& Strike) const;
    int32 GetCurrentStrikeTime(void) const;

    /** How long a strike is valid for. */
    const int32 StrikeDurationInSeconds = 60;
    TArray<FStrike> Strikes = TArray<FStrike>();
    const uint8 MaxStrikeCount = 3;

#pragma endregion Strike

    //////////////////////////////////////////////////////////////////////////
    // Useful Internal Getters
    //////////////////////////////////////////////////////////////////////////

    /** Server only. */
    FORCEINLINE auto GetWorldGameSession(void) const -> AWorldGameSession* { return Cast<AWorldGameSession>(this->GetWorld()->GetAuthGameMode()->GameSession); }

public:

    //////////////////////////////////////////////////////////////////////////
    // Public Getters
    //////////////////////////////////////////////////////////////////////////

    FORCEINLINE auto GetWorldPlayerState(void) const -> AWorldPlayerState* { return this->GetPlayerState<AWorldPlayerState>(); }
    FORCEINLINE auto GetDisplayName(void) const -> FString { return this->GetWorldPlayerState()->GetPlayerName(); }
};
