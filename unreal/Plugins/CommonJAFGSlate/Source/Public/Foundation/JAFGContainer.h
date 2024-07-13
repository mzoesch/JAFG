// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGDirtyUserWidget.h"
#include "Components/JAFGScrollBox.h"

#include "JAFGContainer.generated.h"

class UJAFGScrollBox;
class IContainer;
class UJAFGTextBlock;
class UTileView;

DECLARE_MULTICAST_DELEGATE(FOnContainerLostVisibilitySignature)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnContainerVisibleSignature, const FString Identifier)

UCLASS(Abstract, Blueprintable)
class COMMONJAFGSLATE_API UJAFGContainer : public UJAFGDirtyUserWidget
{
    GENERATED_BODY()

public:

    explicit UJAFGContainer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    /** Call this method if this deferred container should be build now. */
    void BuildDeferred(void);

    FORCEINLINE virtual auto ConditionalMarkAsDirty(const IContainer* TargetedContainer) -> void { }

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

    // UJAFGDirtyUserWidget implementation
    virtual void OnRefresh(void) override;
    // ~UJAFGDirtyUserWidget implementation

    bool bDeferredBuild = false;
    virtual void OnBuild(void);
    virtual void TryUpdateDisplayNames(void);

    void BuildPlayerInventory(void);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|Container")
    // ReSharper disable once CppUE4BlueprintCallableFunctionMayBeStatic
    FORCEINLINE FString GetPlayerInventoryDisplayName( /* void */ ) const { return TEXT("Inventory"); }

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGScrollBox> ScrollBox_PlayerInventoryWrapper;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UTileView> TV_PlayerInventory;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true", OptionalWidget))
    TObjectPtr<UJAFGTextBlock> TextBlock_PlayerInventoryDisplayName;

    void BuildContainerWithCommonLogic(IContainer* Container, UJAFGScrollBox* ScrollBox_TargetWrapper, UTileView* TileView_Target);
};
