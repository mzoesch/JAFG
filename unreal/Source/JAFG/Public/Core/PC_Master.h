// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "PC_Master.generated.h"

UCLASS()
class JAFG_API APC_Master : public APlayerController
{
    GENERATED_BODY()

public:

    void ShowMouseCursor(const bool bShow, const bool bCenter);
    
};
