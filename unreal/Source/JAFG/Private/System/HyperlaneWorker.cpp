// Copyright 2024 mzoesch. All rights reserved.

#include "System/HyperlaneWorker.h"

FHyperlaneWorker::FHyperlaneWorker(void) : Thread(nullptr), bShutdownRequested(false)
{
    this->Thread = FRunnableThread::Create(this, TEXT("HyperlaneWorker"));
    return;
}

FHyperlaneWorker::~FHyperlaneWorker(void)
{
    if (this->Thread)
    {
        this->Thread->Kill(true);

        delete this->Thread;

        UE_LOG(LogTemp, Log, TEXT("FHyperlaneWorker::~FHyperlaneWorker: Hyperlane Worker Destroyed."))
    }

    return;
}

bool FHyperlaneWorker::Init(void)
{
    UE_LOG(LogTemp, Warning, TEXT("Hyperlane Worker Initialized"))
    return true;
}

uint32 FHyperlaneWorker::Run(void)
{
    while (!bShutdownRequested)
    {
        UE_LOG(LogTemp, Warning, TEXT("Hyperlane Worker Running"))
        FPlatformProcess::Sleep(1.0f);
    }

    return 0x0;
}

void FHyperlaneWorker::Stop(void)
{
    this->bShutdownRequested = true;
    UE_LOG(LogTemp, Log, TEXT("Hyperlane Worker Stop requested"))
    return;
}

void FHyperlaneWorker::Exit(void)
{
    /* cleanup ports here */
    UE_LOG(LogTemp, Log, TEXT("FHyperlaneWorker::Exit: Hyperlane Worker Exited."))
}
