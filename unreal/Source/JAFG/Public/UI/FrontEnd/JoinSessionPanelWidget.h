// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "LocalSessionSupervisorSubsystem.h"
#include "Concretes/CommonBarPanelWidget.h"

#include "JoinSessionPanelWidget.generated.h"

class UButton;
JAFG_VOID

class URemoteSessionEntry;
class UScrollBox;

UCLASS(Abstract, Blueprintable)
class JAFG_API UJoinSessionPanelWidget : public UCommonBarPanelWidget, public IOnlineSessionSearchCallback
{
    GENERATED_BODY()

public:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

    // UCommonBarPanelWidget implementation
    virtual void OnNativeMadeVisible(void) override;
    // ~UCommonBarPanelWidget implementation

    // IOnlineSessionSearchCallback implementation
    virtual void OnOnlineSessionFoundCompleteDelegate(const bool bSuccess, const ULocalSessionSupervisorSubsystem* Subsystem) override;
    // ~IOnlineSessionSearchCallback implementation

    //////////////////////////////////////////////////////////////////////////
    // Classes To Create
    //////////////////////////////////////////////////////////////////////////

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<URemoteSessionEntry> RemoteSessionEntryWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UUserWidget> NoRemoteSessionsFoundPlaceholderWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UUserWidget> WaitingForSearchPlaceholderWidgetClass;

    //////////////////////////////////////////////////////////////////////////
    // Instances To Set
    //////////////////////////////////////////////////////////////////////////

    /** Found remote sessions are going to be instanced here. */
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true", OptionalWidget = "true"))
    UScrollBox* SB_RemoteSessions = nullptr;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true", OptionalWidget = "true"))
    UButton* B_Search = nullptr;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true", OptionalWidget = "true"))
    UButton* B_Join = nullptr;

protected:

    const int32 InvalidFocusedRemoteSessionEntryIndex   = -1;
          int32 CurrentlyFocusedRemoteSessionEntryIndex = this->InvalidFocusedRemoteSessionEntryIndex;

    UFUNCTION(BlueprintPure, Category = "UI|FrontEnd")
    bool HasFocusedRemoteSessionEntry( /* void */ ) const { return this->CurrentlyFocusedRemoteSessionEntryIndex != this->InvalidFocusedRemoteSessionEntryIndex; }
    UFUNCTION(BlueprintImplementableEvent, Category = "UI|FrontEnd")
            void OnRemoteSessionEntryClicked(const int32 WidgetIdentifier);
    virtual void OnNativeRemoteSessionEntryClicked(const int32 WidgetIdentifier);

    UFUNCTION(BlueprintCallable)
    void RefreshRemoteSessions( /* void */ );

    UFUNCTION(BlueprintCallable)
    void JoinSession( /* void */ );

    UFUNCTION(BlueprintCallable)
    void SetPlaceholderToNoRemoteSessionsFound( /* void */ );

    UFUNCTION(BlueprintCallable)
    void SetPlaceholderToWaitingForSearch( /* void */ );
};
