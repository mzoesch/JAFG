// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "HUD_Master.generated.h"

class UUW_Master;

UCLASS()
class JAFG_API AHUD_Master : public AHUD
{
	GENERATED_BODY()

public:

	AHUD_Master();

public:
	
	virtual void BeginPlay() override;
	virtual void Tick(const float DeltaSeconds)	override;
	
public:

	UPROPERTY(EditDefaultsOnly)
	class TSubclassOf<UUW_Master> UWCrosshairClass;
	class UUW_Master* UWCrosshair;
	
};
