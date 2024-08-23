// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Chunk/ChunkArena.h"
#include "WorldCore/Chunk/GreedyChunk.h"

UChunkArena::UChunkArena(void)
{
    return;
}

void UChunkArena::ReserveGladiators(int32 InTotalCount, const int32 InMaxCount)
{
    InTotalCount = InTotalCount - this->GetFreeChunkCount();

    if (InTotalCount <= 0)
    {
        return;
    }

    if (InMaxCount > -1)
    {
        InTotalCount = FMath::Min(InTotalCount, InMaxCount);
    }

    for (int32 i = 0; i < InTotalCount; ++i)
    {
        this->FreeChunks.Emplace(this->SpawnChunk());
    }

    return;
}

void UChunkArena::ObliterateFreeGladiators(int32 InCount)
{
    if (InCount < 0)
    {
        InCount = this->GetFreeChunkCount();
    }

    for (int32 i = 0; i < InCount; ++i)
    {
        if (this->FreeChunks.IsEmpty())
        {
            break;
        }

        this->FreeChunks.Last()->OnObliterate();
        this->FreeChunks.Pop(false);

        continue;
    }

    return;
}

bool UChunkArena::Alloc(const FChunkKey& InChunkKey, IChunkArenaGladiator*& OutGladiator)
{
    if (this->GetFreeChunkCount() <= 0)
    {
        LOG_WARNING(
            LogChunkValidation,
            "Tried to allocate chunk %s but no free gladiators are available. Synchronously reserving one now.",
            *InChunkKey.ToString()
        );
        this->ReserveGladiators(1);
    }

    OutGladiator = this->FreeChunks.Pop(false);
    if (OutGladiator == nullptr)
    {
        jcheckNoEntry()
        return false;
    }

    this->AllocatedChunks.Emplace(InChunkKey, OutGladiator);

    OutGladiator->OnAllocate(InChunkKey);

    return true;
}

bool UChunkArena::Free(IChunkArenaGladiator& InGladiator)
{
    if (const int32 Removed = this->AllocatedChunks.Remove(InGladiator.GetChunkKeyOnTheFly_Gladiator()); Removed == 1)
    {
        this->FreeChunks.Emplace(&InGladiator);
        InGladiator.OnFree();
    }
    else if (Removed == 0)
    {
        if (const FChunkKey* const Key = this->AllocatedChunks.FindKey(&InGladiator); Key)
        {
            LOG_WARNING(
                LogChunkValidation,
                "Tried to free chunk but it its key is not the map key (found by value): [m:%s,c:%s]. Removing it now.",
                *Key->ToString(),
                *InGladiator.GetChunkKeyOnTheFly_Gladiator().ToString()
            )

            this->AllocatedChunks.Remove(*Key);
            this->FreeChunks.Emplace(&InGladiator);
            InGladiator.OnFree();
        }
        else
        {
            if (this->FreeChunks.Contains(&InGladiator))
            {
                LOG_RELAXED_FATAL(
                    LogChunkValidation,
                    "Tried to free chunk %s but it was already in the free chunks.",
                    *InGladiator.GetChunkKeyOnTheFly_Gladiator().ToString()
                )

                check( InGladiator.AsChunk()->GetChunkState() == EChunkState::Freed )

                return false;
            }

            LOG_FATAL(
                LogChunkValidation,
                "Tried to free chunk %s but it was nowhere found.",
                *InGladiator.GetChunkKeyOnTheFly_Gladiator().ToString()
            )

            return false;
        }
    }
    else
    {
        LOG_FATAL(
            LogChunkValidation,
            "Tried to free chunk %s but to many instances were found in the allocated chunks map: %d.",
            *InGladiator.GetChunkKeyOnTheFly_Gladiator().ToString(),
            Removed
        )
    }

    return true;
}

IChunkArenaGladiator* UChunkArena::SpawnChunk(void) const
{
    return this->GetWorld()->SpawnActor<IChunkArenaGladiator>(AGreedyChunk::StaticClass());
}
