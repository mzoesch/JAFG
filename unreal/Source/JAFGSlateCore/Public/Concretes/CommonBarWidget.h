// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGWidget.h"

#include "CommonBarWidget.generated.h"

class UWidgetSwitcher;
class UCommonBarPanelWidget;
class UCommonBarEntryWidget;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTabPressedSignature, const FString& /* Identifier */)

USTRUCT(BlueprintType)
struct JAFGSLATECORE_API FCommonBarTabDescriptor
{
    GENERATED_BODY()

    FCommonBarTabDescriptor(void)
    {
    };
    ~FCommonBarTabDescriptor(void) = default;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Identifier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UCommonBarEntryWidget> EntryWidgetClass;

    /**
     * Optional.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UCommonBarPanelWidget> PanelWidgetClass;

    /**
     * Optional padding that can be applied to an entry widget.
     * Currently only vertical and horizontal boxes are supported for the padding.
     */
    FMargin Padding = FMargin(0.0f);

    /**
     * Optional font size that can be applied to an entry widget.
     */
    int32 PreferredFontSize = -1;
};

/**
 * Has common functionality for all widgets that need some kind of bar.
 * A sidebar or a top bar is than up to derived classes.
 * This class handles activation and deactivation of the specific tabs, etc.
 */
UCLASS(Abstract, NotBlueprintable)
class JAFGSLATECORE_API UCommonBarWidget : public UJAFGWidget
{
    GENERATED_BODY()

public:

    explicit  UCommonBarWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    /**
     * This event is called when a tab was pressed.
     */
    FOnTabPressedSignature OnTabPressedEvent;

    /**
     * Will focus the No Active Tab.
     */
    void UnfocusAllTabs(void);

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    virtual void NativeDestruct(void) override;
    // ~UUserWidget implementation

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UPanelWidget> P_BarEntries;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UWidgetSwitcher> WS_BarPanels;

    UFUNCTION(BlueprintCallable, Category = "JAFG|Widgets", meta = (AllowPrivateAccess = "true"))
    void RegisterTab(const FCommonBarTabDescriptor& Descriptor);

private:

    /**
     * Called manually by the entries themselves when they receive their pressed event.
     */
    auto OnTabPressed(const FString& Identifier) -> void;
    friend UCommonBarEntryWidget;

    inline static const FString NoActiveTabIdentifier = L"";
                        FString ActiveTabIdentifier   = UCommonBarWidget::NoActiveTabIdentifier;

    /** Order is based on the children of the UCommonBarWidget#WS_BarPanels. */
    TArray<FString> RegisteredTabIdentifiersInOrder;
    /** @return The index in the Widget Switcher of the specific identifier. -1 if it was not found. */
    auto FindIndexOfIdentifier(const FString& Identifier) const -> int32;
};
