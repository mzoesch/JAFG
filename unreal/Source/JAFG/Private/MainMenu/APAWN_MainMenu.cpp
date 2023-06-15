// © 2023 mzoesch. All rights reserved.


#include "MainMenu/APAWN_MainMenu.h"

// Sets default values
APAWN_MainMenu::APAWN_MainMenu()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void APAWN_MainMenu::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APAWN_MainMenu::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
