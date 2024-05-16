// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "CommonHUD.h"

#include "WorldHUD.generated.h"

JAFG_VOID

class UEscapeMenu;
class UDebugScreen;
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

#if WITH_EDITOR
    /**
     * If the PIE is running in simulation mode, many HUD elements are not needed or cannot even be constructed.
     * But some may still be needed for debugging purposes. Those are created here.
     */
    virtual void CreateSimulationHUD(void);
#endif /* WITH_EDITOR */

private:

    TObjectPtr<UEscapeMenu>  EscapeMenu;
    TObjectPtr<UDebugScreen> DebugScreen;
};
