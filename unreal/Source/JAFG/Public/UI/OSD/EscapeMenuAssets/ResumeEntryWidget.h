// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Concretes/CommonBarEntryWidget.h"

#include "ResumeEntryWidget.generated.h"

JAFG_VOID

UCLASS(Abstract, Blueprintable)
class JAFG_API UResumeEntryWidget : public UCommonBarEntryWidget
{
    GENERATED_BODY()

public:

    explicit UResumeEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void OnThisTabPressed( /* void */ ) const override;
    virtual auto OnTabPressed(const FString& Identifier) -> void override;
};
