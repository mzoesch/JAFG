// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/EditableText.h"

#include "JAFGEditableText.generated.h"

UCLASS(Blueprintable)
class COMMONJAFGSLATE_API UJAFGEditableText : public UEditableText
{
    GENERATED_BODY()

public:

    void SetCustomEventToKeyDown(const FOnKeyDown& InOnKeyDownHandler) const;
};
