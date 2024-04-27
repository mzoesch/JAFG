// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Blueprint/UserWidget.h"

#include "JAFGWidget.generated.h"

JAFG_VOID

UCLASS(Abstract, Blueprintable)
class JAFGCORE_API UJAFGWidget : public UUserWidget
{
    GENERATED_BODY()

protected:

    virtual void NativeConstruct(void) override;

private:

    /**
     * Allows Blueprints to also crash the application if needed. There are no more log verbosity levels to use as if
     * you need to log something, then this should probably be implemented in CPP anyway.
     */
    UFUNCTION(BlueprintCallable, Category = "Common", meta = (AllowPrivateAccess = "true"))
    static void LogFatal(const FString& Message)
    {
        LOG_FATAL(LogCommonSlate, "Kismet logged: [%s].", *Message)
    }
};
