// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Chunk/ConvexChunk.h"

AConvexChunk::AConvexChunk(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;
}

void AConvexChunk::GenerateProceduralMesh(void)
{
}
