// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/EditableText.h"

#include "JAFGEditableTextBlock.generated.h"

UCLASS(Blueprintable)
class JAFGSLATECORE_API UJAFGEditableTextBlock : public UEditableText
{
    GENERATED_BODY()

public:

    void SetCustomEventToKeyDown(const FOnKeyDown& InOnKeyDownHandler) const;
};
