// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/JAFGCommonWidget.h"

#include "EscapeMenu.generated.h"

class UButton;

UCLASS(Abstract, Blueprintable)
class JAFG_API UEscapeMenu : public UJAFGCommonWidget
{
    GENERATED_BODY()

    
protected:

    virtual void NativeConstruct(void) override;

private:

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    UButton* B_Resume;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    UButton* B_ExitToMenu;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    UButton* B_ExitToDesktop;

protected:

    UFUNCTION()
    void OnResumeClicked(/* void */);
    
public:

    void ToggleEscapeMenu(const bool bCollapsed);
};
