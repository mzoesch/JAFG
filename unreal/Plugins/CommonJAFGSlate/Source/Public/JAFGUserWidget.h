// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGLogDefs.h"
#include "Blueprint/UserWidget.h"

#include "JAFGUserWidget.generated.h"

/**
 * Empty declaration of a data class that can be used to pass data to a specific widget without knowing both the
 * widget class or / and the passed data class. Highly sketchy solution but helps to avoid some boilerplate code
 * and unnecessary class declarations. Derived classes may implement some basic data references.
 */
struct COMMONJAFGSLATE_API FWidgetPassData
{
};

/**
 * If FatalYesNo is true, the application will crash if the passed data is not of
 * the expected type. Else it will only throw an error.
 * Use CAST_PASSED_DATA(Type) to only crash outside the editor.
 */
#define CAST_PASSED_DATA_YESNO(Type, FatalYesNo)                                \
    if (                                                                        \
        const Type* Data = static_cast<const Type*>(&UncastedData);             \
        Data == nullptr                                                         \
    )                                                                           \
    {                                                                           \
        if (FatalYesNo == false)                                                \
        {                                                                       \
            LOG_ERROR(LogCommonSlate, "Passed data is not of type %hs.", #Type) \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            LOG_FATAL(LogCommonSlate, "Passed data is not of type %hs.", #Type) \
        }                                                                       \
    }                                                                           \
    else

#if WITH_EDITOR
    #define CAST_PASSED_DATA(Type)          \
        CAST_PASSED_DATA_YESNO(Type, false)
#else /* WITH_EDITOR */
    #define CAST_PASSED_DATA(Type)          \
        CAST_PASSED_DATA_YESNO(Type, true)
#endif /* !WITH_EDITOR */

/** Example use case. */
#if 0

struct COMMONJAFGSLATE_API FExampleDataClass : public FWidgetPassData
{
    FString MyData      = L"";
    FString MyOtherData = L"";
};

FORCEINLINE void PassData(const FWidgetPassData& UncastedData)
{
    CAST_PASSED_DATA(FExampleDataClass)
    {
        /*
         * Get the necessary variables from struct and store for a long-term lifetime.
         */

        LOG_WARNING(LogTemp, "%s", *Data->MyData)
        LOG_WARNING(LogTemp, "%s", *Data->MyOtherData)
    }

    /* Do something ... */

    return;
}

#endif /* 0 */

UCLASS(Abstract, Blueprintable)
class COMMONJAFGSLATE_API UJAFGUserWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    explicit UJAFGUserWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    /**
     * Generic virtual method declaration that some derived classes may implement to function as a deferred constructor
     * call. Highly sketchy solution as it requires anyone who works with this method to be extra carefully with
     * types. There is no type checking by the compiler. Any method will never know what type it gets as only the
     * superclass is known. And then it is the job for every derived class to cast to the needed type. The equivalent
     * blueprint implementable event must be called manually by the derived class in this method to allow derived
     * blueprint widgets to implement their own functionality.
     * See FWidgetPassData for more information.
     */
    virtual void PassDataToWidget(const FWidgetPassData& UncastedData) PURE_VIRTUAL(UJAFGUserWidget::PassDataToWidget)

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
