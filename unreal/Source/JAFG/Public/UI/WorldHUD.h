// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "CommonHUD.h"

#include "WorldHUD.generated.h"

JAFG_VOID

class UJAFGContainer;
class UJAFGWidget;
class UJAFGUserWidget;
class UEditorWorldCommandsSimulation;

UCLASS(NotBlueprintable)
class JAFG_API AWorldHUD : public ACommonHUD
{
    GENERATED_BODY()

    friend UEditorWorldCommandsSimulation;

public:

    explicit AWorldHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

public:

    auto RegisterContainer(const FString& Identifier, const TFunction<TSubclassOf<UJAFGContainer>(void)>& ContainerClassGetter) -> bool;

protected:

#if WITH_EDITOR
    /**
     * If the PIE is running in simulation mode, many HUD elements are not needed or cannot even be constructed.
     * But some may still be needed for debugging purposes. Those are created here.
     */
    virtual void CreateSimulationHUD(void);
#endif /* WITH_EDITOR */

    void OnContainerVisible(const FString& Identifier);
    void OnContainerLostVisibility(void);
    /** Maps identifiers to a UClass of containers. */
    TMap<FString, TFunction<TSubclassOf<UJAFGContainer>(void)>> ContainerClassMap;
    TObjectPtr<UJAFGContainer> CurrentContainer;

private:

    TObjectPtr<UJAFGUserWidget> Crosshair;
    TObjectPtr<UJAFGUserWidget> QuickSessionPreview;
    TObjectPtr<UJAFGUserWidget> ChatMenu;
    TObjectPtr<UJAFGUserWidget> DebugScreen;
    TObjectPtr<UJAFGUserWidget> EscapeMenu;

    FDelegateHandle ContainerVisibleDelegateHandle;
    FDelegateHandle ContainerLostVisibilityDelegateHandle;
};
