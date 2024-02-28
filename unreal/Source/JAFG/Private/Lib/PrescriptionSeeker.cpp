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
#if WITH_EDITOR
            UIL_LOG(Error, TEXT("UPrescriptionSeeker::ReloadPrescriptions: Failed to parse prescription '%s'. An unknown error occurred: %s."), *PrescriptionName, *this->Prescriptions.Last().ToString());
#else
            UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ReloadPrescriptions: Failed to parse prescription '%s'. An unknown error occurred: %s."), *PrescriptionName, *this->Prescriptions.Last().ToString());
#endif /* WITH_EDITOR */
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
#if WITH_EDITOR
        UIL_LOG(Error, TEXT("UPrescriptionSeeker::ParsePrescription: Prescription with name %s already exists."), *Name);
#else
        UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Prescription with name %s already exists."), *Name);
#endif /* WITH_EDITOR */
        return FPrescription::NullPrescription;
    }

    if (Obj.HasField(FPrescriptionJSON::Type) == false)
    {
#if WITH_EDITOR
        UIL_LOG(Error, TEXT("UPrescriptionSeeker::ParsePrescription: Prescription with name %s has no type."), *Name);
#else
        UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Prescription with name %s has no type."), *Name);
#endif /* WITH_EDITOR */
        return FPrescription::NullPrescription;
    }

    if (Obj.HasField(FPrescriptionJSON::Delivery) == false)
    {
#if WITH_EDITOR
        UIL_LOG(Error, TEXT("UPrescriptionSeeker::ParsePrescription: Prescription with name %s has no delivery."), *Name);
#else
        UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Prescription with name %s has no delivery."), *Name);
#endif /* WITH_EDITOR */
        return FPrescription::NullPrescription;
    }

    if (Obj.HasField(FPrescriptionJSON::Product) == false)
    {
#if WITH_EDITOR
        UIL_LOG(Error, TEXT("UPrescriptionSeeker::ParsePrescription: Prescription with name %s has no product."), *Name);
#else
        UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Prescription with name %s has no product."), *Name);
#endif /* WITH_EDITOR */
        return FPrescription::NullPrescription;
    }
    
    FPrescription Prescription;

    /* TODO Do we need some validation on the name? */
    Prescription.Name = Name;
    
    if (PrescriptionTypeMap.Contains(Obj.GetStringField(FPrescriptionJSON::Type)) == false)
    {
#if WITH_EDITOR
        UIL_LOG(Error, TEXT("UPrescriptionSeeker::ParsePrescription: Unsupported prescription type: %s. Fault prescription: %s."), *Obj.GetStringField(FPrescriptionJSON::Type), *Name);
#else
        UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Unsupported prescription type: %s. Fault prescription: %s."), *Obj.GetStringField(FPrescriptionJSON::Type), *Name);
#endif /* WITH_EDITOR */
        Prescription.Type = EPrescriptionType::EPT_Invalid;
        return Prescription;
    }
    Prescription.Type = PrescriptionTypeMap[Obj.GetStringField(FPrescriptionJSON::Type)];
    if (Prescription.Type == EPrescriptionType::EPT_Invalid)
    {
        return Prescription;
    }
    
#pragma region Delivery

    /* Assuming { "Delivery": { "Contents": ["Name", "Name"], "Width": 1 } }. */
    if (Obj.HasTypedField<EJson::Object>(FPrescriptionJSON::Delivery))
    {
        if (Prescription.Type == EPrescriptionType::EPT_CraftingShapeless)
        {
#if WITH_EDITOR
            UIL_LOG(Error, TEXT("UPrescriptionSeeker::ParsePrescription: Object not allowed for shapeless prescription in prescprition delivery. Fault prescription: %s."), *Name);
#else
            UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Object not allowed for shapeless prescription in prescprition delivery. Fault prescription: %s."), *Name);
#endif /* WITH_EDITOR */
            Prescription.Type = EPrescriptionType::EPT_Invalid;
            return Prescription;
        }

        Prescription.Delivery.Contents.Empty();
        if (Obj.GetObjectField(FPrescriptionJSON::Delivery)->HasTypedField<EJson::Array>(FPrescriptionJSON::DeliveryContents) == false)
        {
#if WITH_EDITOR
            UIL_LOG(Error, TEXT("UPrescriptionSeeker::ParsePrescription: Field '%s' with type 'Array' not found in prescription delivery. Fault prescription: %s."), *FPrescriptionJSON::DeliveryContents, *Name)
#else
            UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Field '%s' with type 'Array' not found in prescription delivery. Fault prescription: %s."), *FPrescriptionJSON::DeliveryContents, *Name)
#endif /* WITH_EDITOR */
            Prescription.Type = EPrescriptionType::EPT_Invalid;
            return Prescription;
        }
        for (const TSharedPtr<FJsonValue>& Accumulated : Obj.GetObjectField(FPrescriptionJSON::Delivery)->GetArrayField(FPrescriptionJSON::DeliveryContents))
        {
            if (Accumulated->IsNull())
            {
                Prescription.Delivery.Contents.Add(FAccumulated::NullAccumulated);
                continue;
            }
            Prescription.Delivery.Contents.Add(this->GIPtr->GetAccumulatedByName(Accumulated->AsString()));
            if (Prescription.Delivery.Contents.Last() == FAccumulated::NullAccumulated)
            {
                Prescription.Type = EPrescriptionType::EPT_Invalid;
                return Prescription;
            }
        }
        if (Prescription.Delivery.Contents.IsEmpty())
        {
#if WITH_EDITOR
            UIL_LOG(Error, TEXT("UPrescriptionSeeker::ParsePrescription: Array field '%s' is empty in prescription delivery. Fault prescription: %s."), *FPrescriptionJSON::DeliveryContents, *Name)
#else
            UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Array field '%s' is empty in prescription delivery. Fault prescription: %s."), *FPrescriptionJSON::DeliveryContents, *Name)
#endif /* WITH_EDITOR */
            Prescription.Type = EPrescriptionType::EPT_Invalid;
            return Prescription;
        }
        
        if (Obj.GetObjectField(FPrescriptionJSON::Delivery)->HasTypedField<EJson::Number>(FPrescriptionJSON::DeliveryWidth) == false)
        {
#if WITH_EDITOR
            UIL_LOG(Error, TEXT("UPrescriptionSeeker::ParsePrescription: Number field '%s' not found in prescription delivery. Fault prescription: %s."), *FPrescriptionJSON::DeliveryWidth, *Name)
#else
            UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Number field '%s' not found in prescription delivery. Fault prescription: %s."), *FPrescriptionJSON::DeliveryWidth, *Name)
#endif /* WITH_EDITOR */
            Prescription.Type = EPrescriptionType::EPT_Invalid;
            return Prescription;
        }
        Prescription.Delivery.ContentWidth = Obj.GetObjectField(FPrescriptionJSON::Delivery)->GetField<EJson::Number>(FPrescriptionJSON::DeliveryWidth).Get()->AsNumber();
        if (Prescription.Delivery.ContentWidth < 1)
        {
#if WITH_EDITOR
            UIL_LOG(Error, TEXT("UPrescriptionSeeker::ParsePrescription: Field '%s' is less than 1 in shaped prescription delivery. Fault prescription: %s."), *FPrescriptionJSON::DeliveryWidth, *Name)
#else
            UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Field '%s' is less than 1 in shaped prescription delivery. Fault prescription: %s."), *FPrescriptionJSON::DeliveryWidth, *Name)
#endif /* WITH_EDITOR */
            Prescription.Type = EPrescriptionType::EPT_Invalid;
            return Prescription;
        }
    }

    /* Assuming { "Delivery": ["Name", "Name"] }. */
    else if (Obj.HasTypedField<EJson::Array>(FPrescriptionJSON::Delivery))
    {
        if (Prescription.Type == EPrescriptionType::EPT_CraftingShaped)
        {
#if WITH_EDITOR
            UIL_LOG(Error, TEXT("UPrescriptionSeeker::ParsePrescription: Array not allowed for shaped prescription in prescprition delivery. Fault prescription: %s."), *Name)
#else
            UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Array not allowed for shaped prescription in prescprition delivery. Fault prescription: %s."), *Name)
#endif /* WITH_EDITOR */
            Prescription.Type = EPrescriptionType::EPT_Invalid;
            return Prescription;
        }

        Prescription.Delivery.Contents.Empty();
        for (const TSharedPtr<FJsonValue>& Accumulated : Obj.GetArrayField(FPrescriptionJSON::Delivery))
        {
            if (Accumulated->IsNull())
            {
                UIL_LOG(Warning, TEXT("UPrescriptionSeeker::ParsePrescription: Null value found in prescription delivery array of shapless crafting. Ignoring. Fault prescription: %s."), *Name);
                continue;
            }
            Prescription.Delivery.Contents.Add(this->GIPtr->GetAccumulatedByName(Accumulated->AsString()));
        }
        if (Prescription.Delivery.Contents.IsEmpty())
        {
#if WITH_EDITOR
            UIL_LOG(Error, TEXT("UPrescriptionSeeker::ParsePrescription: Array is empty in prescription delivery. Fault prescription: %s."), *Name);
#else
            UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Array is empty in prescription delivery. Fault prescription: %s."), *Name);
#endif /* WITH_EDITOR */
            Prescription.Type = EPrescriptionType::EPT_Invalid;
            return Prescription;
        }
        for (const FAccumulated& Accumulated : Prescription.Delivery.Contents)
        {
            if (Accumulated == FAccumulated::NullAccumulated)
            {
                Prescription.Type = EPrescriptionType::EPT_Invalid;
                return Prescription;
            }

            continue;
        }
        Prescription.Delivery.ContentWidth = FDelivery::ShapelessContentWidth;
    }

    /* Assuming { "Delivery": "Name" }. */
    else if (Obj.HasTypedField<EJson::String>(FPrescriptionJSON::Delivery))
    {
        if (Prescription.Type == EPrescriptionType::EPT_CraftingShaped)
        {
#if WITH_EDITOR
            UIL_LOG(Error, TEXT("UPrescriptionSeeker::ParsePrescription: String not allowed for shaped prescription in prescprition delivery. Fault prescription: %s."), *Name);
#else
            UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: String not allowed for shaped prescription in prescprition delivery. Fault prescription: %s."), *Name);
#endif /* WITH_EDITOR */
            return FPrescription::NullPrescription;
        }
        
        Prescription.Delivery.Contents.Empty();
        Prescription.Delivery.Contents.Add(this->GIPtr->GetAccumulatedByName(Obj.GetStringField(FPrescriptionJSON::Delivery)));
        if (Prescription.Delivery.Contents.Last() == FAccumulated::NullAccumulated)
        {
            Prescription.Type = EPrescriptionType::EPT_Invalid;
            return Prescription;
        }
        Prescription.Delivery.ContentWidth = FDelivery::ShapelessContentWidth;
    }
    else
    {
#if WITH_EDITOR
        UIL_LOG(Error, TEXT("UPrescriptionSeeker::ParsePrescription: Unsupported type for prescription delivery. Fault prescription: %s."), *Name);
#else
        UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Unsupported type for prescription delivery. Fault prescription: %s."), *Name);
#endif /* WITH_EDITOR */
        Prescription.Type = EPrescriptionType::EPT_Invalid;
        return Prescription;
    }

#pragma endregion Delivery
    
#pragma region Product

    /* Assuming { "Product": { "Content": "Name", "Amount", Amount } }. */
    if (Obj.HasTypedField<EJson::Object>(FPrescriptionJSON::Product))
    {
        if (Obj.GetObjectField(FPrescriptionJSON::Product)->HasTypedField<EJson::String>(FPrescriptionJSON::ProductContent) == false)
        {
#if WITH_EDITOR
            UIL_LOG(Error, TEXT("UPrescriptionSeeker::ParsePrescription: Field '%s' with type 'String' not found in prescription product. Fault prescription: %s."), *FPrescriptionJSON::ProductContent, *Name)
#else
            UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Field '%s' with type 'String' not found in prescription product. Fault prescription: %s."), *FPrescriptionJSON::ProductAccumulatedName, *Name)
#endif /* WITH_EDITOR */
            Prescription.Type = EPrescriptionType::EPT_Invalid;
            return Prescription;
        }
        Prescription.Product = this->GIPtr->GetAccumulatedByName(Obj.GetObjectField(FPrescriptionJSON::Product)->GetStringField(FPrescriptionJSON::ProductContent));
        if (Prescription.Product == FAccumulated::NullAccumulated)
        {
            Prescription.Type = EPrescriptionType::EPT_Invalid;
            return Prescription;
        }
        
        if (Obj.GetObjectField(FPrescriptionJSON::Product)->HasTypedField<EJson::Number>(FPrescriptionJSON::ProductAmount) == false)
        {
#if WITH_EDITOR
            UIL_LOG(Error, TEXT("UPrescriptionSeeker::ParsePrescription: Field '%s' with type 'Number' not found in prescription product. Fault prescription: %s."), *FPrescriptionJSON::ProductAmount, *Name)
#else
            UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Field '%s' with type 'Number' not found in prescription product. Fault prescription: %s."), *FPrescriptionJSON::ProductAccumulatedAmount, *Name)
#endif /* WITH_EDITOR */
            Prescription.Type = EPrescriptionType::EPT_Invalid;
            return Prescription;
        }
        Prescription.Product.Amount = Obj.GetObjectField(FPrescriptionJSON::Product)->GetIntegerField(FPrescriptionJSON::ProductAmount);
    }
    
    /* Assuming { "Product": "Name" }. */
    else if (Obj.HasTypedField<EJson::String>(FPrescriptionJSON::Product))
    {
        Prescription.Product = this->GIPtr->GetAccumulatedByName(Obj.GetStringField(FPrescriptionJSON::Product));
        if (Prescription.Product == FAccumulated::NullAccumulated)
        {
            Prescription.Type = EPrescriptionType::EPT_Invalid;
            return Prescription;
        }
    }
    
    else
    {
#if WITH_EDITOR
        UIL_LOG(Error, TEXT("UPrescriptionSeeker::ParsePrescription: Unsupported type for prescription product. Fault prescription: %s."), *Name);
#else
        UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::ParsePrescription: Unsupported type for prescription product. Fault prescription: %s."), *Name);
#endif /* WITH_EDITOR */
        Prescription.Type = EPrescriptionType::EPT_Invalid;
        return Prescription;
    }

#pragma endregion Product
    
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
    /* Safety guard. */
    OutPrescription = FPrescription::NullPrescription;
    
    if (UPrescriptionSeeker::IsDeliveryEmpty(Delivery))
    {
        return;
    }

    /* We might want to find a faster way to implement this? If we have to many prescriptions this may take a while. */
    for (const FPrescription& Prescription : this->Prescriptions)
    {
        if (Prescription.Type == EPrescriptionType::EPT_CraftingShapeless)
        {
            /*
             * Note:
             *          The Amount of FAccumulated is misused to store the amount
             *          of one unique accumulated item in the delivery. As this is
             *          shapeless crafting, we must only care about the amounts and
             *          not the order of the delivery contents.
             */
            TArray<FAccumulated> DeliveryContentCounter = TArray<FAccumulated>();
            
            for (const FAccumulated& DeliveryContent : Delivery.Contents)
            {
                if (DeliveryContent == FAccumulated::NullAccumulated)
                {
                    continue;
                }

                if (Prescription.HasAccumulatedInDelivery(DeliveryContent) == false)
                {
                    goto NextPrescription;
                }

                if (DeliveryContentCounter.Contains(DeliveryContent))
                {
                    DeliveryContentCounter[DeliveryContentCounter.Find(DeliveryContent)].Amount++;
                    continue;
                }

                check( DeliveryContent.Amount == 1 ) DeliveryContentCounter.Add(DeliveryContent);
                
                continue;
            }

            /* Check if we have all the required accumulated items in the delivery. */
            for (const FAccumulated& PrescriptionDeliveryContent : Prescription.Delivery.Contents)
            {
                if (PrescriptionDeliveryContent == FAccumulated::NullAccumulated)
                {
                    continue;
                }
            
                if (DeliveryContentCounter.Contains(PrescriptionDeliveryContent) == false)
                {
                    goto NextPrescription;
                }    
                
                continue;
            }
            
            /* Check if we have more or less accumulates of one accumulated item in the delivery. */
            for (const FAccumulated& DeliveryContent : DeliveryContentCounter)
            {
                /* TODO Do we need this?? */
                if (Prescription.Delivery.Contents.ContainsByPredicate( [&DeliveryContent] (const FAccumulated& Accumulated) { return Accumulated == DeliveryContent; } ) == false)
                {
                    goto NextPrescription;
                }

                // const FAccumulated*     In          = Prescription.Delivery.Contents.FindByPredicate( [&DeliveryContent] (const FAccumulated& Accumulated) { return Accumulated == DeliveryContent; } );
                // const int               Required    = Prescription.Delivery.GetRequiredAccumulatedAmountOfType(*In);

                if (DeliveryContent.Amount != Prescription.Delivery.GetRequiredAccumulatedAmountOfType(DeliveryContent))
                {
                    goto NextPrescription;
                }
            }

            OutPrescription = Prescription;
            return;
        }

        /*
         * TODO ...
         *      We can definitely do some optimization here. We can check if the delivery has even enough indices left
         *      so it can satisfy the prescription. Open for suggestions.
         */
        if (Prescription.Type == EPrescriptionType::EPT_CraftingShaped)
        {
#define BEGIN_OF_ROW 0
            /*
             * If this is a 3x3 prescription, we ofc cannot craft this in a 2x2 crafting grid.
             * But we ofc can craft a 2x2 prescription in a 3x3 crafting grid.
             */
            if (Prescription.Delivery.ContentWidth > Delivery.ContentWidth)
            {
                goto NextPrescription;
            }

            /*
             * Check if we have a mismatch of the amounts of each type basically we treat
             * the prescription as shapeless and check if it would be a valid prescription.)
             */
            
            /* The same as above. We misuse the amount variable in the FAccumulated struct. */
            TArray<FAccumulated> DeliveryContentCounter         = TArray<FAccumulated>();
            /* Check if we have a mismatch of accumulated item types in the delivery and prescription. */
            for (const FAccumulated& DeliveryContent : Delivery.Contents)
            {
                /*
                 * Here ofc we have to neglect NullAccumulates as the delivery grid may
                 * be bigger than the prescription delivery grid.
                 */
                if (DeliveryContent == FAccumulated::NullAccumulated)
                {
                    continue;
                }
                
                if (Prescription.Delivery.Contents.Contains(DeliveryContent) == false)
                {
                    goto NextPrescription;
                }

                if (DeliveryContentCounter.Contains(DeliveryContent))
                {
                    DeliveryContentCounter[DeliveryContentCounter.Find(DeliveryContent)].Amount++;
                    continue;
                }
                
                check( DeliveryContent.Amount == 1 ) DeliveryContentCounter.Add(DeliveryContent);
                
                continue;
            }
            for (const FAccumulated& PrescriptionDeliveryContent : Prescription.Delivery.Contents)
            {
                /* We intentionally do not sort out NullAccumulates as some shaped description depend on them. */
                
                if (Delivery.Contents.Contains(PrescriptionDeliveryContent) == false)
                {
                    goto NextPrescription;
                }

                continue;
            }

            for (const FAccumulated& DeliveryContent : DeliveryContentCounter)
            {
                if (DeliveryContent.Amount != Prescription.Delivery.GetRequiredAccumulatedAmountOfType(DeliveryContent))
                {
                    goto NextPrescription;
                }
            }

            /*
             * The index that we currently check if it matches with the prescription delivery.
             * All indices that are below this value are already checked and are not valid.
             *
             *
             * Example:
             * 
             * The Hexadecimal numbers in the grids represent the indices,
             * the symbols are a placeholder for arbitrary accumulated items.
             * 
             * The Delivery:            The Prescription:
             * +---+---+---+---+        +---+---+               If DeliveryStartIndexCursor is five.
             * |0  |1  |2  |3  |        |0  |1@ |               In this example all indices below five are already
             * +---+---+---+---+        +---+---+               checked and are invalid.
             * |4  |5  |6@ |7  |        |2@ |3# |               The first valid index is five.
             * +---+---+---+---+        +---+---+
             * |8  |9@ |A# |B  |
             * +---+---+---+---+
             * |C  |D  |E  |F  |
             * +---+---+---+---+
             * Width: 4                 Width: 2
             * 
             * Note that if we have a cursor greater than one, we have to add the width of the cursor to the left side
             * as we progress to the next row. We do not care about stuff outside of currently seeking subgrid of the
             * delivery grid.
             * The same is true for the prescription grid. If the prescription grid progresses to a next row, we have
             * to add this also to the cursor.
             * We have to match both grid sizes as there will be size mismatches.
             */
            int DeliveryStartIndexCursor = 0;
            
            while (DeliveryStartIndexCursor < Delivery.Contents.Num())
            {
                int DeliveryContentCursor       = DeliveryStartIndexCursor;
                int PrescriptionContentCursor   = 0;

                while (true)
                {
                    /*
                     * We have to check this because if a prescription row reached to an end we add the indices that
                     * we expect to be in the delivery content grid cursor. But these can be out of bounds.
                     */
                    if (DeliveryContentCursor >= Delivery.Contents.Num())
                    {
                        goto NextDeliveryIndex;
                    }
                    
                    if (Delivery.Contents[DeliveryContentCursor] != Prescription.Delivery.Contents[PrescriptionContentCursor])
                    {
                        goto NextDeliveryIndex;
                    }

                    /*
                     * Both fields are matching. Now we have to carefully go to the next index.
                     * Note that we cannot just add one, as we have to keep track of the width of the
                     * grids.
                     */

                    DeliveryContentCursor++; PrescriptionContentCursor++;

                    /* We have a winner here, as we reached the end of the prescription without errors :). Yay. */
                    if (PrescriptionContentCursor >= Prescription.Delivery.Contents.Num())
                    {
                        break;
                    }
                    
                    if (PrescriptionContentCursor % Prescription.Delivery.ContentWidth == BEGIN_OF_ROW)
                    {
                        if (DeliveryContentCursor % Delivery.ContentWidth == BEGIN_OF_ROW)
                        {
                            DeliveryContentCursor += DeliveryStartIndexCursor % Delivery.ContentWidth; /* Margin */
                        }
                        else
                        {
                            const int IndicesLeftToNextRow = Delivery.ContentWidth - (DeliveryContentCursor % Delivery.ContentWidth);
                            DeliveryContentCursor += IndicesLeftToNextRow;
                            DeliveryContentCursor += DeliveryStartIndexCursor % Delivery.ContentWidth; /* Margin */
                        }
                        continue;
                    }
                    
                    if (DeliveryContentCursor % Delivery.ContentWidth == BEGIN_OF_ROW)
                    {
                        if (PrescriptionContentCursor % Prescription.Delivery.ContentWidth != BEGIN_OF_ROW)
                        {
                            goto NextDeliveryIndex;
                        }

#if WITH_EDITOR
                        UIL_LOG(Error, TEXT("UPrescriptionSeeker::GetPrescription: Invalid cursor state. DeliveryContentCursor: %d, PrescriptionContentCursor: %d. Fault: %s."), DeliveryContentCursor, PrescriptionContentCursor, *Prescription.Name)
#else
                        UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::GetPrescription: Invalid cursor state. DeliveryContentCursor: %d, PrescriptionContentCursor: %d. Fault: %s."), DeliveryContentCursor, PrescriptionContentCursor, *Prescription.Name)
#endif /* WITH_EDITOR */

                        return;
                    }

                    continue;
                }

                if (PrescriptionContentCursor >= Prescription.Delivery.Contents.Num())
                {
                    OutPrescription = Prescription;
                    return;
                }
                
                NextDeliveryIndex:
                    DeliveryStartIndexCursor++;
                    continue;
            }

            goto NextPrescription;
#undef BEGIN_OF_ROW
        }

#if WITH_EDITOR
        UIL_LOG(Error, TEXT("UPrescriptionSeeker::GetPrescription: Unsupported prescription type: %d."), static_cast<int>(Prescription.Type));
#else
        UIL_LOG(Fatal, TEXT("UPrescriptionSeeker::GetPrescription: Unsupported prescription type: %d."), static_cast<int>(Prescription.Type));
#endif /* WITH_EDITOR */
        
        NextPrescription:
            continue;
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
