// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGWarningPopUp.h"

#include "JAFGWarningPopUpYesNo.generated.h"

struct COMMONJAFGSLATE_API FJAFGWarningPopUpDataYesNo : public FJAFGWarningPopUpData
{
    TFunction<void(bool BAccepted)> OnPopUpYesNoClosedDelegate;
};

UCLASS(Abstract, Blueprintable)
class COMMONJAFGSLATE_API UJAFGWarningPopUpYesNo : public UJAFGWarningPopUp
{
    GENERATED_BODY()

public:

    explicit UJAFGWarningPopUpYesNo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual auto NativeConstruct(void) -> void override;
    // ~UUserWidget implementation

public:

    virtual auto PassDataToWidget(const FWidgetPassData& UncastedData) -> void override;

protected:

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget, OptionalWidget))
    TObjectPtr<UButton> B_ConfirmButton;

    virtual void OnYesNoClose(const bool bAccepted);

    UFUNCTION(BlueprintCallable, Category = "JAFG|Widget", meta = (AllowPrivateAccess = "true"))
    virtual void OnYes( /* void */ );

    UFUNCTION(BlueprintCallable, Category = "JAFG|Widget", meta = (AllowPrivateAccess = "true"))
    virtual void OnNo( /* void */ );

private:

    TFunction<void(bool bAccepted)> OnPopupYesNoClosedDelegate;
};
