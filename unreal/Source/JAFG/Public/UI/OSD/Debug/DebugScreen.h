// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "JAFGUserWidget.h"

#include "DebugScreen.generated.h"

JAFG_VOID

class UEditorWorldCommandsSimulation;

namespace DebugScreen
{

FORCEINLINE auto GetGAverageFPS(void) -> float;
FORCEINLINE auto GetMaxFPS(void) -> float;

}

UCLASS(Abstract, Blueprintable)
class JAFG_API UDebugScreen : public UJAFGUserWidget
{
    GENERATED_BODY()

public:

    explicit UDebugScreen(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual auto NativeConstruct(void) -> void override;
    virtual auto NativeTick(const FGeometry& MyGeometry, const float InDeltaTime) -> void override;
    virtual auto NativeDestruct(void) -> void override;
    // ~UUserWidget implementation

    void UpdateCachedSections(void) const;

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

    /*
     * We get the unused warnings here because all these functions are pure and currently only used as bindings
     * and therefore not called from C++ oder blueprints. This is intended.
     */

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|DebugScreen")
    // ReSharper disable once CppUEBlueprintCallableFunctionUnused
    FString GetSectionProject( /* void */ ) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|DebugScreen")
    // ReSharper disable once CppUEBlueprintCallableFunctionUnused
    FString GetSectionProjectGamePlay( /* void */ ) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|DebugScreen")
    // ReSharper disable once CppUEBlueprintCallableFunctionUnused
    FString GetSectionEngine( /* void */ ) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|DebugScreen")
    // ReSharper disable once CppUEBlueprintCallableFunctionUnused
    FString GetSectionFPS( /* void */ ) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|DebugScreen")
    // ReSharper disable once CppUEBlueprintCallableFunctionUnused
    FString GetSectionNet( /* void */ ) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|DebugScreen")
    // ReSharper disable once CppUEBlueprintCallableFunctionUnused
    FString GetSectionSession( /* void */ ) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|DebugScreen")
    // ReSharper disable once CppUEBlueprintCallableFunctionUnused
    FString GetSectionUWorld( /* void */ ) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|DebugScreen")
    // ReSharper disable once CppUEBlueprintCallableFunctionUnused
    FString GetSectionChunks( /* void */ ) const;


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|DebugScreen")
    // ReSharper disable once CppUEBlueprintCallableFunctionUnused
    FString GetSectionClientCharacterLocation( /* void */ ) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|DebugScreen")
    // ReSharper disable once CppUEBlueprintCallableFunctionUnused
    FString GetSectionClientCharacterVelocity( /* void */ ) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|DebugScreen")
    // ReSharper disable once CppUEBlueprintCallableFunctionUnused
    FString GetSectionClientCharacterChunkLocation( /* void */ ) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|DebugScreen")
    // ReSharper disable once CppUEBlueprintCallableFunctionUnused
    FString GetSectionClientCharacterFacing( /* void */ ) const;


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|DebugScreen")
    // ReSharper disable once CppUEBlueprintCallableFunctionUnused
    FString GetSectionRAMMisc( /* void */ ) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|DebugScreen")
    // ReSharper disable once CppUEBlueprintCallableFunctionUnused
    FString GetSectionCPUMisc( /* void */ ) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|DebugScreen")
    // ReSharper disable once CppUEBlueprintCallableFunctionUnused
    FString GetSectionGPUMisc( /* void */ ) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|DebugScreen")
    // ReSharper disable once CppUEBlueprintCallableFunctionUnused
    FString GetSectionDisplayMisc( /* void */ ) const;


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|DebugScreen")
    // ReSharper disable once CppUEBlueprintCallableFunctionUnused
    FString GetSectionTargetVoxelData( /* void */ ) const;

private:

    mutable int32 ActorCountCache = 0;
    mutable int32 ActorCommonChunkCountCache = 0;

    const   double UpdateInterval = 0.5;
    mutable double LastUpdateTime = 0.0;

    mutable FString ProjectSectionCache = TEXT("");
    mutable FString EngineSectionCache  = TEXT("");

    mutable FString UWorldSectionCache  = TEXT("");

    mutable FString GPUSectionCache     = TEXT("");
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
