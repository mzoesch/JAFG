// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class JAFG_API FHyperlaneWorker final : public FRunnable
{
public:

    FHyperlaneWorker(void);
    virtual ~FHyperlaneWorker(void) override;

    virtual bool Init(void) override;
    virtual uint32 Run(void) override;
    virtual void Stop(void) override;
    virtual void Exit(void) override;

    FRunnableThread* Thread;
    bool bShutdownRequested = false;
};
