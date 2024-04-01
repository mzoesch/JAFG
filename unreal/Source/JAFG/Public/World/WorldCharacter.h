// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "WorldCharacter.generated.h"

class UChatComponent;
class UChatMenu;
class UEscapeMenu;
class UCameraComponent;
class UInputAction;
class UInputComponent;
class UInputMappingContext;
struct FInputActionValue;

UCLASS(Abstract, Blueprintable)
class JAFG_API AWorldCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    
    explicit AWorldCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

private:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UChatComponent* ChatComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FirstPersonCameraComponent;
    
public:

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IMC")
    UInputMappingContext* IMCFoot;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IMC")
    UInputMappingContext* IMCMenu;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IMC")
    UInputMappingContext* IMCChatMenu;
    
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Movement")
    UInputAction* IAJump;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Movement")
    UInputAction* IALook;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Movement")
    UInputAction* IAMove;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|MISC")
    UInputAction* IAToggleEscapeMenu;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|MISC")
    UInputAction* IAToggleChatMenu;
    
private:

    void OnTriggerJump(const FInputActionValue& Value);
    void OnCompleteJump(const FInputActionValue& Value);
    void OnLook(const FInputActionValue& Value);
    void OnMove(const FInputActionValue& Value);

    void OnToggleEscapeMenu(const FInputActionValue& Value);
    friend UEscapeMenu;
    void OnToggleChatMenu(const FInputActionValue& Value);
    friend UChatMenu;
    
public:

    virtual void Tick(const float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
};
