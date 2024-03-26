// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/CommonHUD.h"

#include "WorldHUD.generated.h"

UCLASS(Abstract, Blueprintable)
class JAFG_API AWorldHUD : public ACommonHUD
{
    GENERATED_BODY()

public:

    virtual void BeginPlay() override;
};
