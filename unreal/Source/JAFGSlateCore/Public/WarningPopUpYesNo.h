// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WarningPopUp.h"

#include "WarningPopUpYesNo.generated.h"

class UButton;

struct JAFGSLATECORE_API FWarningPopUpYesNoData final : FWarningPopUpData
{
    virtual ~FWarningPopUpYesNoData(void) override = default;

    TFunction<void(bool bAccepted)> OnPopupClosedDelegate;
};

UCLASS(Abstract, Blueprintable)
class JAFGSLATECORE_API UWarningPopUpYesNo : public UWarningPopUp
{
    GENERATED_BODY()

public:

    explicit UWarningPopUpYesNo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual auto NativeConstruct(void) -> void override;
    // ~UUserWidget implementation

public:

    virtual auto PassDataToWidget(const FMyPassedData& MyPassedData) -> void override;

protected:

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget, OptionalWidget))
    UButton* B_ConfirmButton;

    virtual void OnYesNoClose(const bool bAccepted);

    UFUNCTION(BlueprintCallable, Category = "JAFG|Widget", meta = (AllowPrivateAccess = "true"))
    virtual void OnYes( /* void */ );

    UFUNCTION(BlueprintCallable, Category = "JAFG|Widget", meta = (AllowPrivateAccess = "true"))
    virtual void OnNo( /* void */ );

private:

    TFunction<void(bool bAccepted)> OnPopupClosedDelegate;
};
