// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "UI/Common/JAFGCommonWidget.h"

#include "DebugScreen.generated.h"

JAFG_VOID

UCLASS(Abstract, Blueprintable)
class JAFG_API UDebugScreen : public UJAFGCommonWidget
{
    GENERATED_BODY()

public:

    void Toggle(void);

private:

    FORCEINLINE static float GAverageFPS(void)
    {
        extern ENGINE_API float GAverageFPS;
        return GAverageFPS;
    }

    FORCEINLINE static float GetMaxFPS(void)
    {
        check( GEngine )

        if (const UEngine* Engine = Cast<UEngine>(GEngine))
        {
            return Engine->GetMaxFPS();
        }

        return MAX_flt;
    }

public:

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Debug|DebugScreen")
    FString GetSectionFPS( /* void */ ) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Debug|DebugScreen")
    FString GetSectionClientCharacterLocation( /* void */ ) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Debug|DebugScreen")
    FString GetSectionClientCharacterChunkLocation( /* void */ ) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Debug|DebugScreen")
    FString GetSectionClientCharacterFacing( /* void */ ) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Debug|DebugScreen")
    FString GetSectionTargetVoxelData( /* void */ ) const;
};
