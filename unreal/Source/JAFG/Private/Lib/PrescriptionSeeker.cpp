// © 2023 mzoesch. All rights reserved.

#include "Lib/PrescriptionSeeker.h"

#include "Assets/General.h"
#include "Core/GI_Master.h"

#define UIL_LOG(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)

const FPrescription     FPrescription::NullPrescription     = FPrescription();
const FDelivery         FDelivery::NullDelivery             = FDelivery();

bool FDelivery::operator==(const FDelivery& O) const
{
    if (this->Contents.Num() != O.Contents.Num())
    {
        return false;
    }
    
    if (this->ContentWidth != O.ContentWidth)
    {
        return false;
    }
    
    for (int i = 0; i < this->Contents.Num(); ++i)
    { if (this->Contents[i] != O.Contents[i]) { return false; } }

    return true;
}

void UPrescriptionSeeker::ReloadPrescriptions()
{
    this->Prescriptions.Empty();

    TArray<FString> PrescriptionNames = FGeneral::LoadPrescriptionNames();

    if (PrescriptionNames.Num() < 1)
    {
        UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ReloadPrescriptions: Failed to load prescription names. No prescriptions found."));
        return;
    }
    
    for (const FString& PrescriptionName : PrescriptionNames)
    {
        this->Prescriptions.Add(this->ParsePrescription(PrescriptionName, *FGeneral::LoadPrescription(PrescriptionName).Get()));

        if (this->Prescriptions.Last() == FPrescription::NullPrescription)
        {
            UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ReloadPrescriptions: Failed to parse prescription. An unknown error occurred: %s."), *this->Prescriptions.Last().ToString());
            return;
        }

        if (this->Prescriptions.Last().HasInvalidEntries())
        {
#if WITH_EDITOR
            UIL_LOG(Error, TEXT("UPrescriptionSeeker::ReloadPrescriptions: Failed to parse prescription. Invalid entries: Fault: %s."), *this->Prescriptions.Last().ToString());
#else
            UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ReloadPrescriptions: Failed to parse prescription. Invalid entries: Fault: %s."), *this->Prescriptions.Last().ToString());
#endif /* WITH_EDITOR */
            return;
        }

        UIL_LOG(Log, TEXT("UPrescriptionSeeker::ReloadPrescriptions: Successfully parsed prescription: %s."), *this->Prescriptions.Last().ToString())
        
        continue;
    }

    UIL_LOG(Log, TEXT("UPrescriptionSeeker::ReloadPrescriptions: Successfully loaded %d prescriptions."), this->Prescriptions.Num());

    return;
}

FPrescription UPrescriptionSeeker::ParsePrescription(const FString& Name, const FJsonObject& Obj) const
{
    if (this->Prescriptions.ContainsByPredicate( [&Name] (const FPrescription& Prescription) { return Prescription.Name == Name; } ))
    {
        UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Prescription with name %s already exists."), *Name);
        return FPrescription::NullPrescription;
    }

    if (Obj.HasField(FPrescriptionJSON::Type) == false)
    {
        UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Prescription with name %s has no type."), *Name);
        return FPrescription::NullPrescription;
    }

    if (Obj.HasField(FPrescriptionJSON::Delivery) == false)
    {
        UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Prescription with name %s has no delivery."), *Name);
        return FPrescription::NullPrescription;
    }

    if (Obj.HasField(FPrescriptionJSON::Product) == false)
    {
        UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Prescription with name %s has no product."), *Name);
        return FPrescription::NullPrescription;
    }
    
    FPrescription Prescription;
    
    Prescription.Name     = Name;
    Prescription.Type     = PrescriptionTypeMap[Obj.GetStringField(FPrescriptionJSON::Type)];

#pragma region Delivery

    /* Assuming { "Delivery": { "Accumulates": ["Name", "Name"], <"Width": 1> } }. */
    if (Obj.HasTypedField<EJson::Object>(FPrescriptionJSON::Delivery))
    {
        if (Prescription.Type == EPrescriptionType::ERT_CraftingShapeless)
        {
            UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Object not allowed for shapeless prescription in prescprition delivery. Fault prescription: %s."), *Name);
            return FPrescription::NullPrescription;
        }

        Prescription.Delivery.Contents.Empty();
        for (const TSharedPtr<FJsonValue>& Accumulated : Obj.GetObjectField(FPrescriptionJSON::Delivery)->GetArrayField(FPrescriptionJSON::DeliveryAccumulates))
        { Prescription.Delivery.Contents.Add(this->GIPtr->GetAccumulatedByName(Accumulated->AsString())); } // TODO Remove null
        Prescription.Delivery.ContentWidth = Obj.GetObjectField(FPrescriptionJSON::Delivery)->GetIntegerField(FPrescriptionJSON::DeliveryWidth);
    }
    /* Assuming { "Delivery": ["Name", "Name"] }. */
    else if (Obj.HasTypedField<EJson::Array>(FPrescriptionJSON::Delivery))
    {
        if (Prescription.Type == EPrescriptionType::ERT_CraftingShaped)
        {
            UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Array not allowed for shaped prescription in prescprition delivery. Fault prescription: %s."), *Name);
            return FPrescription::NullPrescription;
        }

        Prescription.Delivery.Contents.Empty();
        for (const TSharedPtr<FJsonValue>& Accumulated : Obj.GetArrayField(FPrescriptionJSON::Delivery))
        { Prescription.Delivery.Contents.Add(this->GIPtr->GetAccumulatedByName(Accumulated->AsString())); } // TODO Remove null
        Prescription.Delivery.ContentWidth = FDelivery::ShapelessContentWidth;
    }
    /* Assuming { "Delivery": "Name" }. */
    else if (Obj.HasTypedField<EJson::String>(FPrescriptionJSON::Delivery))
    {
        if (Prescription.Type == EPrescriptionType::ERT_CraftingShaped)
        {
            UIL_LOG(Error, TEXT("UPrescriptionSeeker::ParsePrescription: String not allowed for shaped prescription in prescprition delivery. Fault prescription: %s."), *Name);
            return FPrescription::NullPrescription;
        }
        
        Prescription.Delivery.Contents.Empty();
        Prescription.Delivery.Contents.Add(this->GIPtr->GetAccumulatedByName(Obj.GetStringField(FPrescriptionJSON::Delivery)));
        Prescription.Delivery.ContentWidth = FDelivery::ShapelessContentWidth;
    }
#if WITH_EDITOR
    else
    {
        UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Unsupported type for prescription delivery. Fault prescription: %s."), *Name);
        return FPrescription::NullPrescription;
    }
#endif /* WITH_EDITOR */

#pragma endregion Delivery
    
#pragma region Delivery

    /* Assuming { "Product": { "Name": "Name", "Amount", Amount } }. */
    if (Obj.HasTypedField<EJson::Object>(FPrescriptionJSON::Product))
    {
        Prescription.Product = this->GIPtr->GetAccumulatedByName(Obj.GetObjectField(FPrescriptionJSON::Product)->GetStringField(FPrescriptionJSON::ProductAccumulatedName));
        Prescription.Product.Amount = Obj.GetObjectField(FPrescriptionJSON::Product)->GetIntegerField(FPrescriptionJSON::ProductAccumulatedAmount);
    }
    /* Assuming { "Product": "Name" }. */
    else if (Obj.HasTypedField<EJson::String>(FPrescriptionJSON::Product))
    {
        Prescription.Product = this->GIPtr->GetAccumulatedByName(Obj.GetStringField(FPrescriptionJSON::Product));
    }
#if WITH_EDITOR
    else
    {
        UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Unsupported type for prescription product. Fault prescription: %s."), *Name);
        return FPrescription::NullPrescription;
    }
#endif /* WITH_EDITOR */

#pragma endregion Delivery
    
    return Prescription;
}

bool UPrescriptionSeeker::IsDeliveryEmpty(const FDelivery& Delivery)
{
    for (const FAccumulated& Content : Delivery.Contents)
    {
        if (Content == FAccumulated::NullAccumulated)
        {
            continue;
        }
        
        return false;
    }

    return true;
}

void UPrescriptionSeeker::GetPrescription(const FDelivery& Delivery, FPrescription& OutPrescription)
{
    if (UPrescriptionSeeker::IsDeliveryEmpty(Delivery))
    {
        return;
    }

    for (const FPrescription& Prescription : this->Prescriptions)
    {
        if (Prescription.Type == EPrescriptionType::ERT_CraftingShapeless)
        {
            /* TODO Do we need this extra struct? Can we just increment the Amount in the FAccumulated? */
            /* Count every occurrence of every unique accumulated item. */
            struct FContainer { FAccumulated Accumulated; int Count;};
            TArray<FContainer> Containers = TArray<FContainer>();
            
            for (const FAccumulated& DeliveredAccumulated : Delivery.Contents)
            {
                if (DeliveredAccumulated == FAccumulated::NullAccumulated)
                {
                    continue;
                }
                
                if (Prescription.HasAccumulatedInDelivery(DeliveredAccumulated) == false)
                {
                    goto NextPrescription;
                }

                if (Containers.ContainsByPredicate( [&DeliveredAccumulated] (const FContainer& Container) { return Container.Accumulated == DeliveredAccumulated; } ))
                {
                    Containers.FindByPredicate( [&DeliveredAccumulated] (const FContainer& Container) { return Container.Accumulated == DeliveredAccumulated; } )->Count++;
                    continue;
                }

                Containers.Add(FContainer{DeliveredAccumulated, 1});
                
                continue;
            }

            /* Check if we have more / less accumulates of one accumulated item in the delivery. */
            for (const FContainer& Container : Containers)
            {
                if (Prescription.Delivery.Contents.ContainsByPredicate( [&Container] (const FAccumulated& Accumulated) { return Accumulated == Container.Accumulated; } ) == false)
                {
                    goto NextPrescription;
                }

                const FAccumulated* In = Prescription.Delivery.Contents.FindByPredicate( [&Container] (const FAccumulated& Accumulated) { return Accumulated == Container.Accumulated; } );
                const int Required = Prescription.Delivery.GetRequiredAccumulatedAmountOfType(*In);

                if (Container.Count != Required)
                {
                    goto NextPrescription;
                }
            }

            OutPrescription = Prescription;
            return;
        }

        UIL_LOG(Error, TEXT("UPrescriptionSeeker::GetPrescription - Unsupported prescription type: %d."), Pname$.Type);

        NextPrescription: continue;
    }

    return;
}

void UPrescriptionSeeker::GetProduct(const FDelivery& Delivery, FAccumulated& OutProduct)
{
    /* Safety guard. */
    OutProduct = FAccumulated::NullAccumulated;
    
    FPrescription Prescription;
    this->GetPrescription(Delivery, Prescription);

    if (Prescription == FPrescription::NullPrescription)
    {
        return;
    }

    OutProduct.Accumulated  = Prescription.Product.Accumulated;
    OutProduct.Amount       = Prescription.Product.Amount;

    return;
}

#undef UIL_LOG
