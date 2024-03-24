// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "JAFGCommonWidget.generated.h"

/**
 * Empty declaration of a data class that can be used to pass data to a specific widget without knowing both the
 * widget class or / and the passed data class. Highly sketchy solution but helps to avoid some boiler plate code
 * and unnecessary class declarations. Derived classes may implement some basic data references.
 */
struct JAFG_API FMyPassData
{
protected:
    
    ~FMyPassData() = default;

public:
    
    virtual FString ToString() const
    {
        /* Substituted all nested pure virtual macro calls as it does not define a non-void declared method. */
        static_assert(TIsArrayOrRefOfTypeByPredicate<decltype(L"Pure virtual not implemented (%s)"), TIsCharEncodingCompatibleWithTCHAR>::Value, "Formatting string must be a TCHAR array.");
        LowLevelFatalErrorHandler("JAFGCommonWidget.h", 27, _ReturnAddress(), (const TCHAR*)L"Pure virtual not implemented (%s)", L"FMyPassData::ToString");
        return FString(L"FATAL-ERROR");
    }
};

UCLASS(Abstract, Blueprintable)
class JAFG_API UJAFGCommonWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    static inline constexpr FLinearColor CommonWidgetForegroundColor = FLinearColor(0.02f, 0.02f, 0.02f, 1.0f);
    UPROPERTY(BlueprintReadOnly, Category = "Common")
    FLinearColor MyCommonWidgetForegroundColor;
    
    static inline constexpr FLinearColor CommonWidgetBackgroundColor = FLinearColor(0.2f, 0.2f, 0.2f, 1.0f);
    UPROPERTY(BlueprintReadOnly, Category = "Common")
    FLinearColor MyCommonWidgetBackgroundColor;

protected:

    virtual void NativeConstruct(void) override;

private:

    /**
     * Allows Blueprints to also crash the application if needed. There are no more log verbosity levels to use as if
     * you need to log something then this should probably be implemented in CPP anyways.
     */
    UFUNCTION(BlueprintCallable, Category = "Common", meta = (AllowPrivateAccess = "true"))
    static void LogFatal(const FString& Message) { UE_LOG(LogTemp, Fatal, TEXT("UJAFGCommonWidget::LogFatal: [%s]."), *Message); }

protected:
    
    /**
     * A deferred constructor call to a widget. This virtual method may be called multiple times and is not a
     * replacement for the native construct method. Derived classes may implement this method to reset all data to
     * their default value, destroy children that have been constructed by such widget, ... It should help function
     * as a late constructor and the widget implementing this event must be able to be refreshed to their default
     * state at any given time by this method.
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Common", meta = (AllowPrivateAccess = "true"))
    void OnDeferredConstruct();
    
public:

    /**
     * Generic virtual method declaration that some derived classes may implement to function as a deferred constructor
     * call. Highly sketchy solution as it requires anyone who works with this method, to be extra carefully with
     * types. There is no type checking by the compiler. Any method will never know what type it gets as only the super
     * class is known. And then it is the job for every derived class to cast to the needed type.
     * See FMyPassData for more information.
     */
    virtual void PassDataToWidget(const FMyPassData& MyPassedData) PURE_VIRTUAL(UJAFGCommonWidget::PassDataToWidget)
};
