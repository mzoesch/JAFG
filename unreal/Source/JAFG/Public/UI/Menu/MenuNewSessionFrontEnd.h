// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/JAFGCommonWidget.h"
#include "Network/LocalSessionSupervisorSubsystem.h"

#include "MenuNewSessionFrontEnd.generated.h"

class ULocalSaveEntry;
class UScrollBox;
class UButton;

UCLASS(Abstract, Blueprintable)
class JAFG_API UMenuNewSessionFrontEnd : public UJAFGCommonWidget
{
    GENERATED_BODY()

protected:

    ////////////////////////////////////////////////////////////////
    // Bindings
    ////////////////////////////////////////////////////////////////
    
    UPROPERTY(BlueprintReadOnly, Category = "Menu", meta = (AllowPrivateAccess = "true", BindWidget))
    UButton* B_HostFromExistingSave;

    UPROPERTY(BlueprintReadOnly, Category = "Menu", meta = (AllowPrivateAccess = "true", BindWidget))
    UButton* B_HostFromNewSave;

    UPROPERTY(BlueprintReadOnly, Category = "Menu", meta = (AllowPrivateAccess = "true", BindWidget))
    UButton* B_DeleteLocalSave;

    UPROPERTY(BlueprintReadOnly, Category = "Menu", meta = (AllowPrivateAccess = "true", BindWidget))
    UButton* B_CancelHosting;
    
    UPROPERTY(BlueprintReadOnly, Category = "Menu", meta = (AllowPrivateAccess = "true", BindWidget))
    UScrollBox* SB_LocalSaves;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menu")
    TSubclassOf<ULocalSaveEntry> WB_SaveSlotClass;

    ////////////////////////////////////////////////////////////////
    // Pure Properties
    ////////////////////////////////////////////////////////////////

    inline static const FString   DefaultSessionName          = "Some Generic Session Name";
    inline static constexpr int32 DefaultMaxPublicConnections = 2;
    inline static constexpr bool  bDefaultLAN                 = true;

    UFUNCTION(BlueprintPure, Category = "Menu", meta = (AllowPrivateAccess = "true"))
    static int32 GetMaxSessionNameLength() { return static_cast<int32>(ULocalSessionSupervisorSubsystem::MaxSessionNameLength); }
    
    UFUNCTION(BlueprintPure, Category = "System", meta = (AllowPrivateAccess = "true"))
    static FString GetDefaultSessionName() { return UMenuNewSessionFrontEnd::DefaultSessionName; }

    UFUNCTION(BlueprintPure, Category = "System", meta = (AllowPrivateAccess = "true"))
    static int32 GetDefaultMaxPublicConnections() { return UMenuNewSessionFrontEnd::DefaultMaxPublicConnections; }
    
    UFUNCTION(BlueprintPure, Category = "System", meta = (AllowPrivateAccess = "true"))
    static bool GetDefaultLAN() { return UMenuNewSessionFrontEnd::bDefaultLAN; }
    
    ////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////
    
    UPROPERTY(BlueprintReadWrite, Category = "System", meta = (AllowPrivateAccess = "true"))
    FString NewSessionName;

    UPROPERTY(BlueprintReadWrite, Category = "System", meta = (AllowPrivateAccess = "true"))
    int32 MaxPublicConnections;

    UPROPERTY(BlueprintReadWrite, Category = "System", meta = (AllowPrivateAccess = "true"))
    bool bLAN;
    
protected:

    virtual void NativeConstruct(void) override;

    UFUNCTION(BlueprintCallable, Category = "System", meta = (AllowPrivateAccess = "true"))
    void HostListenServerAsync() const;
    
public:

    UFUNCTION(BlueprintCallable, Category = "Menu", meta = (AllowPrivateAccess = "true"))
    void ReloadLocalSaves();
};
