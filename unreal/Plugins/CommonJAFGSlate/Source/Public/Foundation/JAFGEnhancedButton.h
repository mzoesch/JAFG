// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGUserWidget.h"

#include "JAFGEnhancedButton.generated.h"

class UJAFGTextBlock;
class UJAFGButton;

UCLASS(Abstract, Blueprintable)
class COMMONJAFGSLATE_API UJAFGEnhancedButton : public UJAFGUserWidget
{
    GENERATED_BODY()

public:

    auto SetContent(const FText& InContent) -> void;
    auto GetContent(void) const -> FText;

    DECLARE_EVENT(UJAFGEnhancedButton, FJAFGButtonEvent)

    FJAFGButtonEvent& OnClicked(void) const { return this->OnClickedEvent; }

private:

    /**
     * The actual UButton that we wrap this user widget into.
     * Allows us to get user widget customization and built-in button functionality.
     */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGButton> Button_Root;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGTextBlock> Text_Content;

    mutable FJAFGButtonEvent OnClickedEvent;
};
