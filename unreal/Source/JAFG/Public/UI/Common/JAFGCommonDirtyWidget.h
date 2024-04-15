// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "JAFGCommonWidget.h"

#include "JAFGCommonDirtyWidget.generated.h"

JAFG_VOID

UCLASS(Abstract, Blueprintable)
class JAFG_API UJAFGCommonDirtyWidget : public UJAFGCommonWidget
{
    GENERATED_BODY()

public:

    /** Will set the bDirty flag to true and enables a refresh of this widget on the next tick. */
    FORCEINLINE void MarkAsDirty(void)
    {
        this->bDirty = true;
    }

protected:

    virtual void NativeConstruct(void) override;
    virtual void NativeTick(const FGeometry& MyGeometry, const float InDeltaTime) final override;

    /** Called on the next tick if the user widget is marked as dirty. */
    virtual void OnRefresh(void) PURE_VIRTUAL(UJAFGCommonDirtyWidget::OnRefresh);

private:

    bool bDirty = false;
};
