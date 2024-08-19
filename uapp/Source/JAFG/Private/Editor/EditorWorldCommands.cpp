// Copyright 2024 mzoesch. All rights reserved.

#include "Editor/EditorWorldCommandsDedSv.h"
#include "WorldCore/Chunk/ChunkGenerationSubsystem.h"

void UEditorWorldCommandsDedSv::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

bool UEditorWorldCommandsDedSv::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return UNetStatics::IsSafeDedicatedServer(Outer);
}

void UEditorWorldCommandsDedSv::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);
    this->RegisterConsoleCommands();
    return;
}

void UEditorWorldCommandsDedSv::Deinitialize(void)
{
    Super::Deinitialize();
    this->UnregisterConsoleCommands();
    return;
}

void UEditorWorldCommandsDedSv::ShowVerticalChunksOnDedicatedServer(void) const
{
    if (UNetStatics::IsSafeDedicatedServer(this) == false)
    {
        LOG_ERROR(LogEditorCommands, "Cannot show vertical chunks on a non-dedicated server instance")
        return;
    }

    UChunkGenerationSubsystem* ChunkGenerationSubsystem = this->GetWorld()->GetSubsystem<UChunkGenerationSubsystem>();
    check( ChunkGenerationSubsystem )

    FString Chunks = FString::Printf(TEXT("Vertical chunks: %d;;\n"), ChunkGenerationSubsystem->GetVerticalChunks().Num());
    int It = 0;
    for (const FChunkKey2& ChunkKey : ChunkGenerationSubsystem->GetVerticalChunks())
    {
        if (It++ % 5 == 0)
        {
            Chunks += TEXT("\n");
        }
        Chunks += FString::Printf(TEXT("(%d-%d);; "), ChunkKey.X, ChunkKey.Y);
    }

    LOG_DISPLAY(LogEditorCommands, "%s", *Chunks)

    return;
}

void UEditorWorldCommandsDedSv::ShowPersistentVerticalChunksOnDedicatedServer(void) const
{
    if (UNetStatics::IsSafeDedicatedServer(this) == false)
    {
        LOG_ERROR(LogEditorCommands, "Cannot show vertical chunks on a non-dedicated server instance")
        return;
    }

    UChunkGenerationSubsystem* ChunkGenerationSubsystem = this->GetWorld()->GetSubsystem<UChunkGenerationSubsystem>();
    check( ChunkGenerationSubsystem )

    FString Chunks = FString::Printf(TEXT("Vertical chunks: %d;;\n"), ChunkGenerationSubsystem->GetVerticalChunks().Num());
    int It = 0;
    for (const FChunkKey2& ChunkKey : ChunkGenerationSubsystem->GetPersistentVerticalChunks())
    {
        if (It++ % 5 == 0)
        {
            Chunks += TEXT("\n");
        }
        Chunks += FString::Printf(TEXT("(%d-%d);; "), ChunkKey.X, ChunkKey.Y);
    }

    LOG_DISPLAY(LogEditorCommands, "%s", *Chunks)

    return;
}

void UEditorWorldCommandsDedSv::RegisterConsoleCommands(void)
{
    LOG_DISPLAY(LogEditorCommands, "Registering console commands.")

    this->ShowVerticalChunksOnDedicatedServerCommand = IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("j.ShowVerticalChunksOnDedicatedServer"),
        TEXT("Show vertical chunks on dedicated server."),
        FConsoleCommandDelegate::CreateLambda( [this] (void) { this->ShowVerticalChunksOnDedicatedServer(); } ),
        ECVF_SetByConsole
    );

    this->ShowPersistentVerticalChunksOnDedicatedServerCommand = IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("j.ShowPersistentVerticalChunksOnDedicatedServer"),
        TEXT("Show persistent vertical chunks on dedicated server."),
        FConsoleCommandDelegate::CreateLambda( [this] (void) { this->ShowPersistentVerticalChunksOnDedicatedServer(); } ),
        ECVF_SetByConsole
    );

    return;
}

void UEditorWorldCommandsDedSv::UnregisterConsoleCommands(void)
{

    if (this->ShowVerticalChunksOnDedicatedServerCommand)
    {
        IConsoleManager::Get().UnregisterConsoleObject(this->ShowVerticalChunksOnDedicatedServerCommand);
        this->ShowVerticalChunksOnDedicatedServerCommand = nullptr;
    }

    if (this->ShowPersistentVerticalChunksOnDedicatedServerCommand)
    {
        IConsoleManager::Get().UnregisterConsoleObject(this->ShowPersistentVerticalChunksOnDedicatedServerCommand);
        this->ShowPersistentVerticalChunksOnDedicatedServerCommand = nullptr;
    }

    return;
}
