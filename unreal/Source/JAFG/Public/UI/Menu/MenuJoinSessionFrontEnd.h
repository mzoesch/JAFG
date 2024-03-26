// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Network/LocalSessionSupervisorSubsystem.h"
#include "UI/Common/JAFGCommonWidget.h"

#include "MenuJoinSessionFrontEnd.generated.h"

class UOnlineSessionEntry;
class UOverlay;
class UButton;
class UScrollBox;
struct FOnlineSessionEntryData;

UCLASS(Abstract, Blueprintable)
class JAFG_API UMenuJoinSessionFrontEnd : public UJAFGCommonWidget, public IOnlineSessionSearchCallback
{
    GENERATED_BODY()

protected:

    ////////////////////////////////////////////////////////////////
    // Bindings
    ////////////////////////////////////////////////////////////////
    
    UPROPERTY(BlueprintReadOnly, Category = "Menu", meta = (AllowPrivateAccess = "true", BindWidget))
    UButton* B_CancelJoining;
    
    UPROPERTY(BlueprintReadOnly, Category = "Menu", meta = (AllowPrivateAccess = "true", BindWidget))
    UButton* B_JoinSelectedSession;
    
    UPROPERTY(BlueprintReadOnly, Category = "Menu", meta = (AllowPrivateAccess = "true", BindWidget))
    UScrollBox* SB_FoundSessions;

    /** To server its purpose as a placeholder it should be placed inside the SB_FoundSessions scroll box. */
    UPROPERTY(BlueprintReadOnly, Category = "Menu", meta = (AllowPrivateAccess = "true", BindWidget))
    UOverlay* O_EmptySessionPlaceholder;

    /** To server its purpose as a placeholder it should be placed inside the SB_FoundSessions scroll box. */
    UPROPERTY(BlueprintReadOnly, Category = "Menu", meta = (AllowPrivateAccess = "true", BindWidget))
    UOverlay* O_LoadingSessionsPlaceholder;

private:

    /** The indices that are used by the placeholder inside the scroll box. */
    inline static constexpr int32 UsedSBIndices = 0x2;
    
protected:
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menu")
    TSubclassOf<UOnlineSessionEntry> WB_OnlineSessionEntryClass;

#if WITH_EDITORONLY_DATA
private:

    UPROPERTY(EditDefaultsOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    bool bMockOnlineSubsystem = false;

    UPROPERTY(EditDefaultsOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    int32 MockSessionCount = 0x40;
    
#endif /* WITH_EDITORONLY_DATA */
#if WITH_EDITOR
private:

    static void MakeMockSessionEntryData(const int32 Index, FOnlineSessionEntryData& OutSessionEntryData);
#endif
    
protected:

    virtual void NativeConstruct(void) override;

    UFUNCTION(BlueprintCallable, Category = "Menu", meta = (AllowPrivateAccess = "true"))
    void JoinSession(const int32 EntryIndex) const;
    
public:

    // ~IOnlineSessionSearchCallback Interface
    virtual void OnOnlineSessionFoundCompleteDelegate(const bool bSuccess, const ULocalSessionSupervisorSubsystem* Subsystem) override;
    
    UFUNCTION(BlueprintCallable, Category = "Menu" , meta = (AllowPrivateAccess = "true"))
    void ReloadFoundSessions();
};
