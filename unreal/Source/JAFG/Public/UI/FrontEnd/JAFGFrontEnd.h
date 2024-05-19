// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Concretes/CommonBarWidget.h"

#include "JAFGFrontEnd.generated.h"

JAFG_VOID

UCLASS(Abstract, Blueprintable)
class JAFG_API UJAFGFrontEnd : public UCommonBarWidget
{
    GENERATED_BODY()

public:

    explicit UJAFGFrontEnd(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    virtual void NativeDestruct(void) override;
    // ~UUserWidget implementation

    virtual void RegisterAllTabs(void);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UCommonBarEntryWidget> NewSessionWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UCommonBarEntryWidget> JoinSessionWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UCommonBarEntryWidget> SettingsWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UCommonBarEntryWidget> QuitGameWidgetClass;
};
