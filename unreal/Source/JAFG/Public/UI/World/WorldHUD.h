// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "UI/Common/CommonHUD.h"

#include "WorldHUD.generated.h"

class UHotbarSelector;
class UHotbarSlot;
JAFG_VOID

class UHotbar;
class UDebugScreen;
class UCrosshair;
class UChatMenu;
class UEscapeMenu;

UCLASS(Abstract, Blueprintable)
class JAFG_API AWorldHUD : public ACommonHUD
{
    GENERATED_BODY()

public:

    explicit AWorldHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void BeginPlay(void) override;

public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    TSubclassOf<UDebugScreen> WDebugScreenClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    TSubclassOf<UHotbar> WHotbarClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    TSubclassOf<UHotbarSlot> WHotbarSlotClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    TSubclassOf<UHotbarSelector> WHotbarSelectorClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    TSubclassOf<UChatMenu> WChatMenuClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    TSubclassOf<UEscapeMenu> WEscapeMenuClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    TSubclassOf<UCrosshair> WCrosshairClass;

private:

    TObjectPtr<UDebugScreen> WDebugScreen;
    TObjectPtr<UHotbar>      WHotbar;
    TObjectPtr<UChatMenu>    WChatMenu;
    TObjectPtr<UEscapeMenu>  WEscapeMenu;
    TObjectPtr<UCrosshair>   WCrosshair;

public:

    void ToggleEscapeMenu(const bool bCollapsed) const;

    void ToggleChatMenu(const bool bCollapsed) const;
    void AddMessageToChat(const FString& Message) const;

    void ToggleDebugScreen(void) const;
};
