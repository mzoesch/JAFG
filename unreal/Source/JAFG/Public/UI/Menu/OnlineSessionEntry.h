// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/JAFGCommonWidget.h"

#include "OnlineSessionEntry.generated.h"

class UTextBlock;

struct JAFG_API FOnlineSessionEntryData final : FMyPassData
{
    virtual ~FOnlineSessionEntryData() = default;

    uint16 EntryIndex;

    // ~FOnlineSessionSearch
    FString IDStr;
    int32 PingInMs;

    // ~FOnlineSession
    /**
     * FUniqueNetIdPtr OwningUserID;
     */
    FString OwningUserID;
    FString OwningUserName;
    int32 NumOpenPrivateConnections;
    int32 NumOpenPublicConnections;

    // ~FOnlineSessionSettings
    int32 MaxPrivateConnections;
    int32 MaxPublicConnections;
    bool bLANSession;
    bool bDedicatedSession;
    /* There are some more information that we could get but this is enough for now. */
    
    /* We put the session name here behind this method to easily change it later if we work with a specific OSS. */
    inline static constexpr int MaxSessionNameLength { 0xF };
    FORCEINLINE FString GetFullSessionName() const { return  this->OwningUserName; }
    FORCEINLINE FString GetSanitizedSessionName() const { return this->OwningUserName.Left(MaxSessionNameLength); }
    
    FORCEINLINE int32 GetCurrentPrivateConnections() const { return this->MaxPrivateConnections - this->NumOpenPrivateConnections; }
    FORCEINLINE int32 GetCurrentPublicConnections() const { return this->MaxPublicConnections - this->NumOpenPublicConnections; }
    
    FORCEINLINE virtual FString ToString() const override
    {
        return FString::Printf(
            TEXT("FOnlineSessionEntryData{EntryIndex: %d, IDStr: %s, PingInMs: %d, OwningUserID: %s, OwningUserName: %s, NumOpenPrivateConnections: %d, NumOpenPublicConnections: %d, MaxPublicConnections: %d, MaxPrivateConnections: %d, bLANSession: %d, bDedicatedSession: %d}"),
            this->EntryIndex, *this->IDStr, this->PingInMs, *this->OwningUserID, *this->OwningUserName, this->NumOpenPrivateConnections, this->NumOpenPublicConnections, this->MaxPublicConnections, this->MaxPrivateConnections, this->bLANSession, this->bDedicatedSession
        );
    }
};

UCLASS(Abstract, Blueprintable)
class JAFG_API UOnlineSessionEntry : public UJAFGCommonWidget
{
    GENERATED_BODY()

protected:

    FOnlineSessionEntryData EntryData;

    UPROPERTY(BlueprintReadOnly, Category = "Menu", meta = (AllowPrivateAccess = "true", BindWidget))
    UTextBlock* TB_OnlineSessionName;

    UPROPERTY(BlueprintReadOnly, Category = "Menu", meta = (AllowPrivateAccess = "true", BindWidget))
    UTextBlock* TB_OnlineSessionConnections;
    
    UFUNCTION(BlueprintPure, Category = "Menu", meta = (AllowPrivateAccess = "true"))
    int32 GetEntryIndex() const { return this->EntryData.EntryIndex; }

    UFUNCTION(BlueprintPure, Category = "Menu", meta = (AllowPrivateAccess = "true"))
    int32 GetCurrentPrivateConnections() const { return this->EntryData.GetCurrentPrivateConnections(); }

    UFUNCTION(BlueprintPure, Category = "Menu", meta = (AllowPrivateAccess = "true"))
    int32 GetCurrentPublicConnections() const { return this->EntryData.GetCurrentPublicConnections(); }

    UFUNCTION(BlueprintPure, Category = "Menu", meta = (AllowPrivateAccess = "true"))
    bool IsLANSession() const { return this->EntryData.bLANSession; }

    UFUNCTION(BlueprintPure, Category = "Menu", meta = (AllowPrivateAccess = "true"))
    bool IsDedicatedSession() const { return this->EntryData.bDedicatedSession; }
    
public:

    const FOnlineSessionEntryData& GetEntryData() const { return this->EntryData; }
    virtual void PassDataToWidget(const FMyPassData& MyPassedData) override;
};
