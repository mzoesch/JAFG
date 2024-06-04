// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGDirtyUserWidget.h"
#include "Hotbar.generated.h"

class UHorizontalBox;

UCLASS(Abstract, Blueprintable)
class COMMONJAFGSLATE_API UHotbar : public UJAFGDirtyUserWidget
{
    GENERATED_BODY()

public:

    explicit UHotbar(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

    // UJAFGDirtyUserWidget implementation
    virtual void OnRefresh(void) override;
    // ~UJAFGDirtyUserWidget implementation

    void RenderHotbar(void);

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UHorizontalBox> HBox_HotbarContainer;
};
