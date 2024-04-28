// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "FrontEnd/JAFGWidget.h"

#include "NewSessionFrontEnd.generated.h"

JAFG_VOID

UCLASS(Abstract, Blueprintable)
class MENU_API UNewSessionFrontEnd : public UJAFGWidget
{
    GENERATED_BODY()

protected:

    virtual void NativeConstruct(void) override;

    //////////////////////////////////////////////////////////////////////////
    // Bindings
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // Pure Properties
    //////////////////////////////////////////////////////////////////////////

    inline static const FString   DefaultSessionName          = "Some Generic Session Name";
    inline static constexpr int32 DefaultMaxPublicConnections = 2;
    inline static constexpr bool  bDefaultLAN                 = true;

    UFUNCTION(BlueprintPure, Category = "JAFG|Menu", meta = (AllowPrivateAccess = "true"))
    static int32 GetMaxSessionNameLength( /* void */ ) { return 64; }

    UFUNCTION(BlueprintPure, Category = "JAFG|System", meta = (AllowPrivateAccess = "true"))
    static int32 GetMaxPublicConnections( /* void */ ) { return 4; }

    UFUNCTION(BlueprintPure, Category = "JAFG|System", meta = (AllowPrivateAccess = "true"))
    static FString GetDefaultSessionName( /* void */ ) { return UNewSessionFrontEnd::DefaultSessionName; }

    UFUNCTION(BlueprintPure, Category = "JAFG|System", meta = (AllowPrivateAccess = "true"))
    static int32 GetDefaultMaxPublicConnections( /* void */ ) { return UNewSessionFrontEnd::DefaultMaxPublicConnections; }

    UFUNCTION(BlueprintPure, Category = "JAFG|System", meta = (AllowPrivateAccess = "true"))
    static bool GetDefaultLAN( /* void */ ) { return UNewSessionFrontEnd::bDefaultLAN; }

    //////////////////////////////////////////////////////////////////////////
    // Pure Properties
    //////////////////////////////////////////////////////////////////////////

    UPROPERTY(BlueprintReadWrite, Category = "JAFG|System", meta = (AllowPrivateAccess = "true"))
    FString NewSessionName;

    UPROPERTY(BlueprintReadWrite, Category = "JAFG|System", meta = (AllowPrivateAccess = "true"))
    int32 MaxPublicConnections;

    UPROPERTY(BlueprintReadWrite, Category = "JAFG|System", meta = (AllowPrivateAccess = "true"))
    bool bLAN;

private:

    UFUNCTION(BlueprintCallable, Category = "JAFG|System", meta = (AllowPrivateAccess = "true"))
    void HostListenServerAsync( /* void */ );
};
