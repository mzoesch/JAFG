// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGWidget.h"

#include "WarningPopUp.generated.h"

class UButton;

struct JAFGSLATECORE_API FWarningPopUpData : public FMyPassedData
{
    virtual ~FWarningPopUpData(void) = default;

    FString                         Message;
    FString                         Header;

    FORCEINLINE virtual FString ToString(void) const override
    {
        return FString::Printf(TEXT("FWarningPopUpData{Message: %s, Header: %s}"), *this->Message, *this->Header);
    }
};

UCLASS(Abstract, Blueprintable)
class JAFGSLATECORE_API UWarningPopUp : public UJAFGWidget
{
    GENERATED_BODY()

public:

    explicit UWarningPopUp(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual auto NativeConstruct(void) -> void override;
    // ~UUserWidget implementation

public:

    virtual auto PassDataToWidget(const FMyPassedData& MyPassedData) -> void override;

protected:

    FString Header;
    FString Message;

    UFUNCTION(BlueprintPure, Category = "JAFG|Widget", meta = (AllowPrivateAccess = "true"))
    FString GetHeader( /* void */ ) const;

    UFUNCTION(BlueprintPure, Category = "JAFG|Widget", meta = (AllowPrivateAccess = "true"))
    FString GetMessage( /* void */ ) const;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget, OptionalWidget))
    UButton* B_CloseButton;

    UFUNCTION(BlueprintCallable, Category = "JAFG|Widget", meta = (AllowPrivateAccess = "true"))
    virtual void OnClose( /* void */ );
};
