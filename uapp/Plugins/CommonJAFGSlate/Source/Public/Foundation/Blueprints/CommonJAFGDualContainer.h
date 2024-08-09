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

    FORCEINLINE auto SetOtherContainerDisplayName(const FString& DisplayName) -> void { this->OtherContainerDisplayName = DisplayName; }
                auto SetOtherContainerData(IContainer* Container) -> void ;
    FORCEINLINE auto SetAutoUnsubscribeOtherContainerOnKill(const bool bValue) -> void { this->bAutoUnsubscribeOtherContainerOnKill = bValue; }

    // UJAFGContainer implementation
    FORCEINLINE virtual auto ConditionalMarkAsDirty(const IContainer* TargetedContainer) -> void override
    {
        if (this->OtherContainerData == TargetedContainer)
        {
            this->MarkAsDirty();
        }
    }
    // ~UJAFGContainer implementation

protected:

    // UUserWidget implementation
    virtual auto NativeConstruct(void) -> void override;
    virtual auto NativeDestruct(void) -> void override;
    // ~UUserWidget implementation

    // UJAFGContainer implementation
    virtual auto OnBuild(void) -> void override;
    virtual auto TryUpdateDisplayNames(void) -> void override;
    // ~UJAFGContainer implementation

    virtual void BuildOtherContainer(void);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|Container")
    FORCEINLINE FString GetOtherContainerDisplayName( /* void */ ) const { return this->OtherContainerDisplayName; }

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UPanelWidget> Panel_OtherContainerWrapper;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UTileView> TileView_OtherContainer;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true", OptionalWidget))
    TObjectPtr<UJAFGTextBlock> TextBlock_OtherContainerDisplayName;

    IContainer* OtherContainerData;

private:

    FString OtherContainerDisplayName = TEXT("Other Container");
    bool bAutoUnsubscribeOtherContainerOnKill = true;
    FDelegateHandle OnOtherContainerChangedDelegateHandle;
};
