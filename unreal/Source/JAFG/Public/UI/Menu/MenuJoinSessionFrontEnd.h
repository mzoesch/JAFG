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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menu")
    TSubclassOf<UOnlineSessionEntry> WB_OnlineSessionEntryClass;
    
protected:

    virtual void NativeConstruct(void) override;

public:

    // IOnlineSessionSearchCallback
    virtual void OnOnlineSessionFoundComplete(const bool bSuccess, const ULocalSessionSupervisorSubsystem* Subsystem) override;
    
    UFUNCTION(BlueprintCallable, Category = "Menu" , meta = (AllowPrivateAccess = "true"))
    void ReloadFoundSessions();
};
