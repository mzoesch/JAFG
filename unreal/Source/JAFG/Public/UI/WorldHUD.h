// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "WorldHUDBase.h"

#include "WorldHUD.generated.h"

JAFG_VOID

class UHotbar;
class UJAFGContainer;
class UJAFGWidget;
class UJAFGUserWidget;
class UEditorWorldCommandsSimulation;

UCLASS(NotBlueprintable)
class JAFG_API AWorldHUD : public AWorldHUDBase
{
    GENERATED_BODY()

    friend UEditorWorldCommandsSimulation;

public:

    explicit AWorldHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

public:

    // AWorldHUDBase implementation
    virtual auto IsContainerRegistered(const FString& Identifier) const -> bool override;
    virtual auto RegisterContainer(const FString& Identifier, const TFunction<TSubclassOf<UJAFGContainer>(void)>& ContainerClassGetter) -> bool override;
    virtual auto PushContainerToViewport(const FString& Identifier) -> UJAFGContainer* override;
    // ~AWorldHUDBase implementation

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
    UPROPERTY()
    TObjectPtr<UJAFGContainer> CurrentContainer;

public:

    UPROPERTY()
    TObjectPtr<UJAFGUserWidget> Crosshair;
    UPROPERTY()
    TObjectPtr<UJAFGUserWidget> QuickSessionPreview;
    UPROPERTY()
    TObjectPtr<UJAFGUserWidget> ChatMenu;
    UPROPERTY()
    TObjectPtr<UJAFGUserWidget> DebugScreen;
    UPROPERTY()
    TObjectPtr<UJAFGUserWidget> EscapeMenu;
    UPROPERTY()
    TObjectPtr<UHotbar> Hotbar;

private:

    FDelegateHandle ContainerVisibleDelegateHandle;
    FDelegateHandle ContainerLostVisibilityDelegateHandle;
};
