// © 2023 mzoesch. All rights reserved.


#include "World/WorldManipulation.h"

#include "CORE/AACTR_BLOCKCORE.h"
#include "CORE/AGM_CORE.h"
#include "World/Blocks.h"

TSubclassOf<AACTR_BLOCKCORE> WorldManipulation::GetBlockClass(UWorld* World, const FString& BlockID) {
	AGM_CORE* GM = Cast<AGM_CORE>(World->GetAuthGameMode());
	check(GM)

	if (BlockID.Equals(Blocks::Base))
		return GM->Base;

	if (BlockID.Equals(Blocks::Stone))
		return GM->Stone;

	if (BlockID.Equals(Blocks::Dirt))
		return GM->Dirt;

	if (BlockID.Equals(Blocks::Grass))
		return GM->Grass;

	if (BlockID.Equals(Blocks::Sand))
		return GM->Sand;

	if (BlockID.Equals(Blocks::Gravel))
		return GM->Gravel;

	if (BlockID.Equals(Blocks::OakLeaves))
		return GM->OakLeaves;

	if (BlockID.Equals(Blocks::OakWood))
		return GM->OakWood;

	if (BlockID.Equals(Blocks::Iron))
		return GM->Iron;

	if (BlockID.Equals(Blocks::Gold))
		return GM->Gold;

	return nullptr;
}

void WorldManipulation::SpawnBlock(
	UWorld* World,
	const FJAFGCoordinateSystem& BlockPosition,
	const FString& BlockID
) {
	TSubclassOf<AACTR_BLOCKCORE> BlockClass = GetBlockClass(World, BlockID);
	if (!BlockClass) {
		UE_LOG(LogTemp, Error, TEXT("BlockID %s not found."), *BlockID)
		return;
	}

	FActorSpawnParameters SpawnParams = FActorSpawnParameters();
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
	AACTR_BLOCKCORE* Block = World->SpawnActor<AACTR_BLOCKCORE>(
		BlockClass,
		BlockPosition.GetAsUnrealTransform(),
		SpawnParams
	);

	return;
}
