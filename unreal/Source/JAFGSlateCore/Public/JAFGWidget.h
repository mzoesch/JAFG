// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Definitions.h"

#include "JAFGWidget.generated.h"

/**
 * Empty declaration of a data class that can be used to pass data to a specific widget without knowing both the
 * widget class or / and the passed data class. Highly sketchy solution but helps to avoid some boilerplate code
 * and unnecessary class declarations. Derived classes may implement some basic data references.
 */
struct JAFGSLATECORE_API FMyPassedData
{

protected:

    ~FMyPassedData(void) = default;

public:

    virtual FString ToString(void) const = 0;
};

UCLASS(Abstract, Blueprintable)
class JAFGSLATECORE_API UJAFGWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    explicit UJAFGWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

public:

    inline static constexpr FLinearColor CommonForegroundColor = FLinearColor(0.02f, 0.02f, 0.02f, 1.0f);
    UPROPERTY(BlueprintReadOnly, Category = "JAFG|Color")
    FLinearColor MyCommonForegroundColor;

    inline static constexpr FLinearColor CommonForegroundColorReducedAlpha = FLinearColor(UJAFGWidget::CommonForegroundColor.R, UJAFGWidget::CommonForegroundColor.G, UJAFGWidget::CommonForegroundColor.B, 0.5f);
    UPROPERTY(BlueprintReadOnly, Category = "JAFG|Color")
    FLinearColor MyCommonForegroundColorReducedAlpha;

    inline static constexpr FLinearColor CommonBackgroundColor = FLinearColor(0.2f, 0.2f, 0.2f, 1.0f);
    UPROPERTY(BlueprintReadOnly, Category = "JAFG|Color")
    FLinearColor MyCommonBackgroundColor;

    /**
     * Generic virtual method declaration that some derived classes may implement to function as a deferred constructor
     * call. Highly sketchy solution as it requires anyone who works with this method to be extra carefully with
     * types. There is no type checking by the compiler. Any method will never know what type it gets as only the
     * superclass is known. And then it is the job for every derived class to cast to the needed type. The equivalent
     * blueprint implementable event must be called manually by the derived class in this method to allow derived
     * blueprint widgets to implement their own functionality.
     * See FMyPassData for more information.
     */
    virtual void PassDataToWidget(const FMyPassedData& MyPassedData) PURE_VIRTUAL(UJAFGWidget::PassDataToWidget)

protected:

    /**
     * A deferred constructor call to a widget. This virtual method may be called multiple times and is not a
     * replacement for the native construct method. Derived classes may implement this method to reset all data to
     * their default value, destroy children that have been constructed by such a widget, ... It should help function
     * as a late constructor, and the widget implementing this event must be able to be refreshed to their default
     * state at any given time by this method.
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "JAFG|Widgets", meta = (AllowPrivateAccess = "true"))
    void OnDeferredConstruct( /* void */ );

private:

    /**
     * Allows Blueprints to also crash the application if needed. There are no more log verbosity levels to use as if
     * you need to log something, then this should probably be implemented in CPP anyway.
     */
    UFUNCTION(BlueprintCallable, Category = "JAFG|Widgets", meta = (AllowPrivateAccess = "true"))
    static void LogFatal(const FString& Message)
    {
        LOG_FATAL(LogCommonSlate, "Kismet logged: [%s].", *Message)
    }
};
