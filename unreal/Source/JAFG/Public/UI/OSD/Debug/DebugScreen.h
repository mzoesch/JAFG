// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "JAFGWidget.h"

#include "DebugScreen.generated.h"

JAFG_VOID

class UEditorWorldCommandsSimulation;

namespace DebugScreen
{

FORCEINLINE auto GetGAverageFPS(void) -> float;
FORCEINLINE auto GetMaxFPS(void) -> float;

}

UCLASS(Abstract, Blueprintable)
class JAFG_API UDebugScreen : public UJAFGWidget
{
    GENERATED_BODY()

public:

    explicit UDebugScreen(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual auto NativeConstruct(void) -> void override;
    virtual auto NativeDestruct(void) -> void override;
    // ~UUserWidget implementation

public:

    FORCEINLINE static auto GAverageFPS(void) -> float
    {
        extern ENGINE_API float GAverageFPS;
        return GAverageFPS;
    }

private:

#if WITH_EDITOR
    /** While simulation. A console command may call this method to toggle the debug screen. */
    void ToggleInSimulateMode(void);
    friend UEditorWorldCommandsSimulation;
#endif /* WITH_EDITOR */

protected:

    FDelegateHandle DebugScreenVisibilityChangedDelegateHandle;
    virtual auto OnDebugScreenVisibilityChanged(const bool bVisible) -> void;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|DebugScreen")
    FString GetSectionFPS( /* void */ ) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|DebugScreen")
    FString GetSectionClientCharacterLocation( /* void */ ) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|DebugScreen")
    FString GetSectionClientCharacterChunkLocation( /* void */ ) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|DebugScreen")
    FString GetSectionClientCharacterFacing( /* void */ ) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|DebugScreen")
    FString GetSectionTargetVoxelData( /* void */ ) const;
};

FORCEINLINE auto DebugScreen::GetGAverageFPS(void) -> float
{
    return UDebugScreen::GAverageFPS();
}

FORCEINLINE auto DebugScreen::GetMaxFPS(void) -> float
{
    checkSlow( GEngine )

    if (const UEngine* Engine = GEngine)
    {
        return Engine->GetMaxFPS();
    }

    return MAX_FLT;
}
