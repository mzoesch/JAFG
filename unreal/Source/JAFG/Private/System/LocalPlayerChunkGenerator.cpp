// Copyright 2024 mzoesch. All rights reserved.

#include "System/LocalPlayerChunkGenerator.h"

void ULocalPlayerChunkGenerator::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    UE_LOG(LogTemp, Error, TEXT("LocalPlayerChunkGenerator initialized"));

}

void ULocalPlayerChunkGenerator::Deinitialize(void)
{
    Super::Deinitialize();


    UE_LOG(LogTemp, Error, TEXT("LocalPlayerChunkGenerator deinitialized"));

}
