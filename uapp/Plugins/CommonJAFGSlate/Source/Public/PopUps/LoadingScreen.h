// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGUserWidget.h"

#include "LoadingScreen.generated.h"

UCLASS(Abstract, Blueprintable)
class COMMONJAFGSLATE_API ULoadingScreen : public UJAFGUserWidget
{
    GENERATED_BODY()

public:

    explicit ULoadingScreen(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void NativeTick(const FGeometry& MyGeometry, const float InDeltaTime) override;
};
