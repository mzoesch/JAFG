// Copyright 2024 mzoesch. All rights reserved.

#include "Settings/SettingScreenSideTab.h"

void USettingScreenSideTab::PassDataToWidget(const FMyPassedData& MyPassedData)
{
    if (const FPassedSettingTabDescriptor* Data = static_cast<const FPassedSettingTabDescriptor*>(&MyPassedData); Data == nullptr)
    {
#if WITH_EDITOR
        LOG_ERROR(LogCommonSlate, "MyPassedData is not of type FPassedSettingTabDescriptor.")
#else /* WITH_EDITOR */
        LOG_FATAL(LogCommonSlate, "MyPassedData is not of type FPassedSettingTabDescriptor.")
#endif /* !WITH_EDITOR */
    }
    else
    {
        this->TabDescriptor = Data->Descriptor;
    }

    this->OnDeferredConstruct();

    return;
}
