// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "JCoordinate.generated.h"

UCLASS()
class JAFG_API AJCoordinate : public AActor
{
    GENERATED_BODY()
    
public:

    static inline constexpr float J_TO_U_SCALE{100.0f};
    static inline constexpr float U_TO_J_SCALE{1.0f / J_TO_U_SCALE};
    
};
