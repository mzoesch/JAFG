// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGUserWidget.h"

#include "ContainerValueCursor.generated.h"

class USizeBox;
class IContainerOwner;
class UCanvasPanelSlot;
class UCanvasPanel;
class UJAFGTextBlock;
class UImage;

UCLASS(Abstract, Blueprintable)
class COMMONJAFGSLATE_API UContainerValueCursor : public UJAFGUserWidget
{
    GENERATED_BODY()

public:

    explicit UContainerValueCursor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    // ~UUserWidget implementation

    /** Must be wrapped into a canvas panel that wrappes the whole viewport. */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<USizeBox> SizeBox_Wrapper;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UImage> Image_Preview;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGTextBlock> Text_Amount;

private:

    UPROPERTY()
    TObjectPtr<UCanvasPanelSlot> Canvas_WrapperSlot;

    UPROPERTY()
    TScriptInterface<IContainerOwner> ContainerOwner;
};
