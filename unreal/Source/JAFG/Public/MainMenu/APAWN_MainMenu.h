// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "APAWN_MainMenu.generated.h"


UCLASS()
class JAFG_API APAWN_MainMenu : public APawn {
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APAWN_MainMenu();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
