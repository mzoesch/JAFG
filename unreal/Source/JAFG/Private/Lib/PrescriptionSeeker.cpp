#include "Lib/PrescriptionSeeker.h"

#include "Assets/General.h"
#include "Core/GI_Master.h"

#define UIL_LOG(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)

const FPrescription FPrescription::NullPrescription = FPrescription();

void UPrescriptionSeeker::ReloadPrescriptions()
{
    this->Prescriptions.Empty();

    TArray<FString> PrescriptionNames = FGeneral::LoadPrescriptionNames();

    if (PrescriptionNames.Num() < 1)
    {
        UE_LOG(LogTemp, Error, TEXT("UPrescriptionLookup::ReloadPrescriptions - Failed to load prescription names. No prescriptions found."));
        return;
    }
    
    for (const FString& PrescriptionName : PrescriptionNames)
    {
        this->Prescriptions.Add(UPrescriptionSeeker::ParsePrescription(PrescriptionName, *FGeneral::LoadPrescription(PrescriptionName).Get()));
        continue;
    }

    UE_LOG(LogTemp, Warning, TEXT("Successfully loaded %d prescriptions."), this->Prescriptions.Num());

    return;
}

FPrescription UPrescriptionSeeker::ParsePrescription(const FString& Name, const FJsonObject& Obj) const
{
    FPrescription Prescription;
    
    Prescription.Name     = Name;
    Prescription.Type     = PrescriptionTypeMap[Obj.GetStringField("Type")];
    Prescription.Delivery = Obj.GetStringField("Delivery");
    Prescription.Product  = Obj.GetStringField("Product");
    
    Prescription.DeliveryAccumulated    = this->GIPtr->GetAccumulatedByName(Prescription.Delivery);
    Prescription.ProductAccumulated     = this->GIPtr->GetAccumulatedByName(Prescription.Product);
    
    return Prescription;
}

bool UPrescriptionSeeker::IsDeliveryEmpty(const FDelivery& Delivery)
{
    for (const FAccumulated& D : Delivery.Delivery)
    {
        if (D == FAccumulated::NullAccumulated)
        {
            continue;
        }
        
        return false;
    }

    return true;
}

void UPrescriptionSeeker::GetPrescription(const FDelivery& Delivery, FPrescription& OutPrescription)
{
    if (Delivery.Delivery.Num() < 1 || UPrescriptionSeeker::IsDeliveryEmpty(Delivery))
    {
        return;
    }

    for (const FPrescription& P : this->Prescriptions)
    {
        if (P.Type == EPrescriptionType::ERT_CraftingShapeless)
        {
            for (const FAccumulated& D : Delivery.Delivery)
            {
                if (D == FAccumulated::NullAccumulated || D != P.DeliveryAccumulated)
                {
                    continue;
                }

                OutPrescription = P;

                return;
            }
            
            continue;
        }

        UIL_LOG(Error, TEXT("UPrescriptionSeeker::GetPrescription - Unsupported prescription type: %d."), P.Type);

        continue;
    }

    return;
}

void UPrescriptionSeeker::GetProduct(const FDelivery& Delivery, FAccumulated& OutProduct)
{
    if (Delivery.Delivery.Num() < 1 || UPrescriptionSeeker::IsDeliveryEmpty(Delivery))
    {
        return;
    }

    FPrescription Prescription;
    this->GetPrescription(Delivery, Prescription);

    if (Prescription == FPrescription::NullPrescription)
    {
        return;
    }

    OutProduct.Accumulated  = Prescription.ProductAccumulated.Accumulated;
    OutProduct.Amount       = 1;

    return;
}

#undef UIL_LOG
