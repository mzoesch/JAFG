// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGWidget.h"
#include "CommonBarWidget.h"

#include "CommonBarEntryWidget.generated.h"

struct JAFGSLATECORE_API FPassedTabDescriptor final :FMyPassedData
{
    virtual ~FPassedTabDescriptor(void) = default;

    TObjectPtr<UCommonBarWidget> Owner;
    FCommonBarTabDescriptor      Descriptor;

    FORCEINLINE virtual FString ToString(void) const override
    {
        return FString::Printf(TEXT("Descriptor{%s}"), *this->Descriptor.DisplayName);
    }
};

/**
 * Should act as a button for the CommonBarWidget.
 */
UCLASS(Abstract, Blueprintable)
class JAFGSLATECORE_API UCommonBarEntryWidget : public UJAFGWidget
{
    GENERATED_BODY()

public:

    explicit UCommonBarEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void PassDataToWidget(const FMyPassedData& MyPassedData) override;

protected:

    UFUNCTION(BlueprintImplementableEvent, Category = "JAFG|Widget", meta = (AllowPrivateAccess = "true"))
    void OnFocusTab( /* void */ );

    UFUNCTION(BlueprintImplementableEvent, Category = "JAFG|Widget", meta = (AllowPrivateAccess = "true"))
    void OnUnfocusTab( /* void */ );

    UPROPERTY()
    TObjectPtr<UCommonBarWidget> Owner;
    FCommonBarTabDescriptor      TabDescriptor;

    /**
     * Call this method if this tab was pressed or focused in any way.
     */
    UFUNCTION(BlueprintCallable, Category = "JAFG|Widget", meta = (AllowPrivateAccess = "true"))
    virtual void OnThisTabPressed( /* void */ ) const;
    /**
     * Will be called if any tab in the collection was pressed.
     */
    virtual auto OnTabPressed(const FString& Identifier) -> void;

private:

    UFUNCTION(BlueprintPure, Category = "JAFG|Widget", meta = (AllowPrivateAccess = "true"))
    FString GetTabDisplayName( /* void */ ) const;
    /**
     * Called after tab data was set, but before the tab is being deferred constructed in kismet.
     * Override this method to add additional functionality.
     */
    virtual void InitializeTab(void);
};
