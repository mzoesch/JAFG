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

    /**
     * Sets some default values that must apply to all widgets that are containers.
     * It will e.g. set the cursor and the input mapping contexts of the character.
     */
    void TransitToContainerState(const FString& Identifier, const bool bOpen, const bool bDestroy = true);

private:

    void ShowMouseCursor(const bool bShow, const bool bCenter);
};
