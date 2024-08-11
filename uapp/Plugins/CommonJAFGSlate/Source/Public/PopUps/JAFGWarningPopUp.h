// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGUserWidget.h"
#include "Components/TextBlock.h"

#include "JAFGWarningPopUp.generated.h"

class UButton;

struct COMMONJAFGSLATE_API FJAFGWarningPopUpData : public FWidgetPassData
{
    FString Message;
    FString Header;

    TFunction<void(void)> OnPopUpClosedDelegate;
};

UCLASS(Abstract, Blueprintable)
class COMMONJAFGSLATE_API UJAFGWarningPopUp : public UJAFGUserWidget
{
    GENERATED_BODY()

public:

    explicit UJAFGWarningPopUp(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

public:

    virtual void PassDataToWidget(const FWidgetPassData& UncastedData) override;

protected:

    FString Header;
    FString Message;

    UFUNCTION(BlueprintPure, Category = "JAFG|Widget", meta = (AllowPrivateAccess = "true"))
    FString GetHeader( /* void */ ) const;

    UFUNCTION(BlueprintPure, Category = "JAFG|Widget", meta = (AllowPrivateAccess = "true"))
    FString GetMessage( /* void */ ) const;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget, OptionalWidget))
    UButton* B_CloseButton = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget, OptionalWidget))
    UTextBlock* TB_Header = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget, OptionalWidget))
    UTextBlock* TB_Message = nullptr;

    /** Call from Kismet if UJAFGWarningPopUp#B_CloseButton is not bound. */
    UFUNCTION(BlueprintCallable, Category = "JAFG|Widget", meta = (AllowPrivateAccess = "true"))
    virtual void OnClose( /* void */ );

private:

    TFunction<void(void)> OnPopUpClosedDelegate;
};
