// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGWidget.h"

#include "JAFGDirtyWidget.generated.h"

UCLASS(Abstract, Blueprintable)
class JAFGSLATECORE_API UJAFGDirtyWidget : public UJAFGWidget
{
    GENERATED_BODY()

public:

    /** Will set the bDirty flag to true and enables a refresh of this widget on the next tick. */
    FORCEINLINE auto MarkAsDirty(void) -> void
    {
        this->bDirty = true;
    }

protected:

    virtual auto NativeConstruct(void) -> void override;
    virtual auto NativeTick(const FGeometry& MyGeometry, const float InDeltaTime) -> void final override;

    /** Called on the next tick if the user widget is marked as dirty. */
    virtual auto OnRefresh(void) -> void PURE_VIRTUAL(UJAFGCommonDirtyWidget::OnRefresh)

private:

    bool bDirty = false;
};
