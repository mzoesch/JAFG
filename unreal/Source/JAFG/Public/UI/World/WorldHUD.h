// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/CommonHUD.h"

#include "WorldHUD.generated.h"

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
    TSubclassOf<UEscapeMenu> WEscapeMenuClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    TSubclassOf<UChatMenu> WChatMenuClass;
    
private:

    TObjectPtr<UEscapeMenu> WEscapeMenu;
    TObjectPtr<UChatMenu> WChatMenu;
    
public:

    void ToggleEscapeMenu(const bool bCollapsed) const;

    void ToggleChatMenu(const bool bCollapsed) const;
    void AddMessageToChat(const FString& Message);
};
