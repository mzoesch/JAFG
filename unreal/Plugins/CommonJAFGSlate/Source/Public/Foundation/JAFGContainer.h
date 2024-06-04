// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGDirtyUserWidget.h"

#include "JAFGContainer.generated.h"

class UTileView;

DECLARE_MULTICAST_DELEGATE(FOnContainerLostVisibilitySignature)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnContainerVisibleSignature, const FString Identifier)

UCLASS(Abstract, Blueprintable)
class COMMONJAFGSLATE_API UJAFGContainer : public UJAFGDirtyUserWidget
{
    GENERATED_BODY()

public:

    explicit UJAFGContainer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

    void BuildPlayerInventory(void);

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UTileView> TV_PlayerInventory;
};
