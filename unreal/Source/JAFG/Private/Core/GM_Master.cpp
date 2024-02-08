// © 2023 mzoesch. All rights reserved.

#include "Core/GM_Master.h"

#include "Lib/PrescriptionSeeker.h"
#include "Core/GI_Master.h"

void AGM_Master::BeginPlay()
{
    Super::BeginPlay();

    this->PrescriptionSeeker = NewObject<UPrescriptionSeeker>();
    this->PrescriptionSeeker->Init(Cast<UGI_Master>(this->GetGameInstance()));
    this->PrescriptionSeeker->ReloadPrescriptions();

    return;
}
