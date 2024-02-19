// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "JCoordinate.generated.h"

UCLASS()
class JAFG_API UJCoordinate : public UObject
{
    GENERATED_BODY()
    
public:

    inline static constexpr float J_TO_U_SCALE { 100.0f };
    inline static constexpr float U_TO_J_SCALE { 1.0f / UJCoordinate::J_TO_U_SCALE };
};
