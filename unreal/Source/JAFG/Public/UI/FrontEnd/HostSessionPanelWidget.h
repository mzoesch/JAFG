// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "LocalSessionSupervisorSubsystem.h"
#include "TabBar/JAFGTabBarPanel.h"

#include "HostSessionPanelWidget.generated.h"

JAFG_VOID

class UEditableText;
class UScrollBox;
class ULocalSaveEntry;

UCLASS(Abstract, Blueprintable)
class JAFG_API UHostSessionPanelWidget : public UJAFGTabBarPanel
{
    GENERATED_BODY()

public:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

    // UCommonBarPanelWidget implementation
    virtual void OnNativeMadeVisible(void) override;
    // ~UCommonBarPanelWidget implementation

    UFUNCTION(BlueprintCallable, Category = "UI|FrontEnd")
    void LoadLocalSessionsToScrollBox( /* void */);

    //////////////////////////////////////////////////////////////////////////
    // Classes To Create
    //////////////////////////////////////////////////////////////////////////

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<ULocalSaveEntry> LocalSaveEntryWidgetClass;

    //////////////////////////////////////////////////////////////////////////
    // Instances To Set
    //////////////////////////////////////////////////////////////////////////

    /** Found local sessions are going to be instanced here. */
    UPROPERTY(BlueprintReadWrite)
    UScrollBox* SB_SavedLocalSessions = nullptr;

    UPROPERTY(BlueprintReadWrite)
    UEditableText* ET_SessionName = nullptr;

    UPROPERTY(BlueprintReadWrite)
    UEditableText* ET_NewSessionMaxPublicConnections = nullptr;

protected:

    const int32   MaxSessionNameLength = ULocalSessionSupervisorSubsystem::MaxSessionNameLength;
    const FString DefaultSessionName   = L"Some Generic Session Name";
          FString NewSessionName       = this->DefaultSessionName;
    UFUNCTION(BlueprintPure, Category = "UI|FrontEnd")
    FString GetSavePathForSessionName( /* void */ ) const;

    const int32 MaxPublicConnectionsLength        = ULocalSessionSupervisorSubsystem::MaxPublicConnections;
          int32 MaxPublicConnections              = 0;
          FText LastValidMaxPublicConnectionsText = FText::GetEmpty();

    UFUNCTION(BlueprintPure)
    bool IsHostFromNewSaveInputValid( /* void */ ) const;

    const int32 InvalidFocusedLocalSaveEntryIndex   = -1;
          int32 CurrentlyFocusedLocalSaveEntryIndex = this->InvalidFocusedLocalSaveEntryIndex;

    UFUNCTION(BlueprintPure, Category = "UI|FrontEnd")
    bool HasFocusedLocalSaveEntry( /* void */ ) const { return this->CurrentlyFocusedLocalSaveEntryIndex != this->InvalidFocusedLocalSaveEntryIndex; }
    UFUNCTION(BlueprintImplementableEvent, Category = "UI|FrontEnd")
            void OnLocalSaveEntryClicked(const int32 WidgetIdentifier);
    virtual void OnNativeLocalSaveEntryClicked(const int32 WidgetIdentifier);

    bool bBoundToDynamicWidgetEvents = false;
    UFUNCTION()
    virtual void OnSessionNameChanged(const FText& Text);
    UFUNCTION()
    virtual void OnNewSessionMaxPublicConnectionsChanged(const FText& Text);

    UFUNCTION(BlueprintCallable)
    void ResetHostFromSessionSettingsToDefault( /* void */ );
    UFUNCTION(BlueprintCallable)
    void ResetHostFromNewSessionSettingsToDefault( /* void */ );

    UFUNCTION(BlueprintCallable)
    void HostSessionFromNewSave( /* void */ ) const;
};
