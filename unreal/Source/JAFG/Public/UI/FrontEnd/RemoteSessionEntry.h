// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "JAFGFocusableUserWidget.h"

#include "RemoteSessionEntry.generated.h"

JAFG_VOID

class UTextBlock;

struct FPassedRemoteSessionEntryData final : public FPassedFocusableWidgetData
{
    // FOnlineSessionSearchResult
    FString IDStr;
    int32   PingInMs;
    // ~FOnlineSessionSearchResult

    // FOnlineSession
    /** Is actually a FUniqueNetIdPtr but converted to a string here for better accessibility. */
    FString OwningUserID;
    FString OwningUserName;
    int32   NumOpenPrivateConnections;
    int32   NumOpenPublicConnections;
    // ~FOnlineSession

    // FOnlineSessionSettings
    int32   NumPublicConnections;
    int32   NumPrivateConnections;
    bool    bLANSession;
    bool    bDedicatedSession;
    // ~FOnlineSessionSettings

    /*
     * There is some more information that we could get, but this is enough for now.
     */

    FORCEINLINE auto GetFullSessionName(void) const -> FString { return this->OwningUserName; }
    FORCEINLINE auto GetDisplaySessionName(void) const -> FString { return this->OwningUserName.Left(FPassedRemoteSessionEntryData::MaxDisplaySessionNameLength); }

    FORCEINLINE auto GetCurrentPrivateConnections(void) const -> int32 { return this->NumPrivateConnections - this->NumOpenPrivateConnections; }
    FORCEINLINE auto GetCurrentPublicConnections(void) const -> int32 { return this->NumPublicConnections - this->NumOpenPublicConnections; }

    FORCEINLINE auto ToString(void) const -> FString
    {
        return FString::Printf(
            TEXT("FPassedRemoteSessionEntryData{IDStr: %s, PingInMs: %d, OwningUserID: %s, OwningUserName: %s, NumOpenPrivateConnections: %d, NumPrivateConnections: %d, NumOpenPublicConnections: %d, NumPublicConnections: %d, bLANSession: %d, bDedicatedSession: %d}"),
            *this->IDStr, this->PingInMs, *this->OwningUserID, *this->OwningUserName,
            this->NumOpenPrivateConnections, this->NumPrivateConnections,
            this->NumOpenPublicConnections, this->NumPublicConnections,
            this->bLANSession, this->bDedicatedSession
        );
    }

private:

    inline static constexpr  int32 MaxDisplaySessionNameLength = 0xF;
};

UCLASS(Abstract, Blueprintable)
class JAFG_API URemoteSessionEntry : public UJAFGFocusableUserWidget
{
    GENERATED_BODY()

public:

    explicit URemoteSessionEntry(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void PassDataToWidget(const FWidgetPassData& UncastedData) override;

protected:

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true", OptionalWidget = "true"))
    UTextBlock* TB_SessionName = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true", OptionalWidget = "true"))
    UTextBlock* TB_SessionPing = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true", OptionalWidget = "true"))
    UTextBlock* TB_SessionPlayers = nullptr;

    UFUNCTION(BlueprintPure, Category = "JAFG|Focusable", meta = (AllowPrivateAccess = "true"))
    FORCEINLINE FString GetSaveName( /* void */ ) const { return this->EntryData.GetDisplaySessionName(); }

    UFUNCTION(BlueprintPure, Category = "JAFG|Focusable", meta = (AllowPrivateAccess = "true"))
    FORCEINLINE int32 GetSessionPing( /* void */ ) const { return this->EntryData.PingInMs; }

    UFUNCTION(BlueprintPure, Category = "JAFG|Focusable", meta = (AllowPrivateAccess = "true"))
    FORCEINLINE int32 GetCurrentPublicConnections( /* void */ ) const { return this->EntryData.GetCurrentPublicConnections(); }

    UFUNCTION(BlueprintPure, Category = "JAFG|Focusable", meta = (AllowPrivateAccess = "true"))
    FORCEINLINE int32 GetNumPublicConnections( /* void */ ) const { return this->EntryData.NumPublicConnections; }

    FORCEINLINE auto GetEntryData(void) const -> const FPassedRemoteSessionEntryData& { return this->EntryData; }

private:

    FPassedRemoteSessionEntryData EntryData = FPassedRemoteSessionEntryData();
};
