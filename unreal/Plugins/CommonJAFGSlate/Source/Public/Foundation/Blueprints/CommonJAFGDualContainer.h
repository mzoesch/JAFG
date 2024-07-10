// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Foundation/JAFGContainer.h"

#include "CommonJAFGDualContainer.generated.h"

class IContainer;
class UJAFGTextBlock;

/**
 * A base class that can be used to create a widget that contains two different containers (one of them being
 * the player inventory).
 */
UCLASS(Abstract, Blueprintable)
class COMMONJAFGSLATE_API UCommonJAFGDualContainer : public UJAFGContainer
{
    GENERATED_BODY()

public:

    explicit UCommonJAFGDualContainer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    inline static const FString Identifier = TEXT("CommonDualContainer");

    FORCEINLINE void SetOtherContainerDisplayName(const FString& DisplayName) { this->OtherContainerDisplayName = DisplayName; }
    FORCEINLINE void SetOtherContainerData(IContainer* Container) { this->OtherContainerData = Container; }

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

    // UJAFGContainer implementation
    virtual void OnBuild(void) override;
    virtual void TryUpdateDisplayNames(void) override;
    // ~UJAFGContainer implementation

    virtual void BuildOtherContainer(void);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|Container")
    FORCEINLINE FString GetOtherContainerDisplayName( /* void */ ) const { return this->OtherContainerDisplayName; }

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGScrollBox> ScrollBox_OtherContainerWrapper;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UTileView> TileView_OtherContainer;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true", OptionalWidget))
    TObjectPtr<UJAFGTextBlock> TextBlock_OtherContainerDisplayName;

    IContainer* OtherContainerData;

private:

    FString OtherContainerDisplayName = TEXT("Other Container");
};
