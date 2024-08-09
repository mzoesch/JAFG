// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "JAFGDirtyUserWidget.h"

#include "Crosshair.generated.h"

JAFG_VOID

class UOverlay;
class UBorder;

UCLASS(Abstract, Blueprintable)
class JAFG_API UCrosshair : public UJAFGDirtyUserWidget
{
    GENERATED_BODY()

public:

    explicit UCrosshair(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

    virtual void OnRefresh(void) override;

    virtual void OnViewportChangedDelegate(FViewport* Viewport, const uint32 ViewportType);

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UOverlay> O_CrosshairContainer = nullptr;
};
