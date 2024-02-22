// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Lib/FAccumulated.h"

#include "PrescriptionSeeker.generated.h"

class UGI_Master;

enum EPrescriptionType : uint8
{
    ERT_Invalid,
    ERT_CraftingShaped,
    ERT_CraftingShapeless,
};

inline TMap<FString, EPrescriptionType> PrescriptionTypeMap =
{
    {   "Cr_Sd",  ERT_CraftingShaped      },
    {   "Cr_Sl",  ERT_CraftingShapeless   },
};

/**
 * For the Delivery field the following Java Script Object Notation (JSON) is valid.
 * Null values are discarded if the prescription is shapeless.
 *
 * {
 *   "Delivery": "Name"
 *   "Delivery": ["Name1", null, "Name2"]
 *   "Delivery": { // Shaped crafting
 *     "Accumulates": ["Name1", null, "Name2"],
 *     "Width": 2
 *   }
 * }
 *
 *
 * For the Product field the following Java Script Object Notation (JSON) is valid.
 * 
 * {
 *   "Product": "Name"
 *   "Product": {
 *     "Name": "Name",
 *     "Amount": 1
 *   }
 * }
 */
struct FPrescriptionJSON
{
    inline static const FString Type                        = "Type";
    
    inline static const FString Delivery                    = "Delivery";
    inline static const FString DeliveryAccumulates         = "Accumulates";
    inline static const FString DeliveryWidth               = "Width";
    
    inline static const FString Product                     = "Product";
    inline static const FString ProductAccumulatedName      = "Name";
    inline static const FString ProductAccumulatedAmount    = "Amount";
};

struct FDelivery
{
    /** If with shaped crafting. NullAccumulates must be provided in the array as a placeholder. */
    TArray<FAccumulated>    Contents;
    /** Must be annotated with ShapelessDeliveryWidth if the delivery is shapeless. */
    int                     ContentWidth;

public:

    FORCEINLINE FDelivery() : Contents(), ContentWidth(FDelivery::ShapelessContentWidth) { return; }
    FORCEINLINE FDelivery(const TArray<FAccumulated>& InContent, const int InWidth) : Contents(InContent), ContentWidth(InWidth) { return; }

    static const FDelivery NullDelivery;
    
    inline static constexpr int ShapelessContentWidth = -1;

    FORCEINLINE int GetRequiredAccumulatedAmountOfType(const FAccumulated& Accumulated) const
    {
        int Amount = 0;
        for (int i = 0; i < this->Contents.Num(); ++i)
        { if (this->Contents[i] == Accumulated) { Amount++; } }
        return Amount;
    }
    /** @return True if NullAccumulated and if delivery contents has InAccumulated. */
    FORCEINLINE bool HasAccumulated(const FAccumulated& Accumulated) const
    { return Accumulated == FAccumulated::NullAccumulated || this->Contents.Contains(Accumulated); }
    bool operator==(const FDelivery& O) const;
    FORCEINLINE static bool Equals(const FDelivery& A, const FDelivery& B) { return A == B; }
    FORCEINLINE FString ToString() const
    {
        FString DeliveryString = TEXT("[");
        for (int i = 0; i < this->Contents.Num(); ++i)
        { DeliveryString += FString::Printf(TEXT("%s%s"), *this->Contents[i].ToString(), i >= this->Contents.Num() - 1 ? TEXT("") : TEXT(", ")); }
        DeliveryString += TEXT("]");
        return FString::Printf(TEXT("{Delivery:%s, Width:%d}"), *DeliveryString, this->ContentWidth);
    }
};

struct FPrescription
{
    FString             Name;
    EPrescriptionType   Type;
    FDelivery           Delivery;
    FAccumulated        Product;

public:

    FORCEINLINE FPrescription() : Name(), Type(EPrescriptionType::ERT_Invalid), Delivery(), Product(FAccumulated::NullAccumulated) { return; }
    
    static const FPrescription NullPrescription;

public:

    /** @return True if NullAccumulated and if delivery contents has InAccumulated. */
    FORCEINLINE bool HasAccumulatedInDelivery(const FAccumulated Accumulated) const { return this->Delivery.HasAccumulated(Accumulated); }
    FORCEINLINE bool HasInvalidEntries() const { return this->Name == TEXT("") || this->Type == EPrescriptionType::ERT_Invalid || this->Delivery == FDelivery::NullDelivery || this->Product == FAccumulated::NullAccumulated; }
    /** Will only check for the name. The program will assume that only one prescription with the same name exists. */
    FORCEINLINE bool operator==(const FPrescription& O) const { return this->Name == O.Name; }
    /** Will only check for the name. The program will assume that only one prescription with the same name exists. */
    FORCEINLINE static bool Equals(const FPrescription& A, const FPrescription& B) { return A == B; }
    FORCEINLINE static bool DeepEqualQuery(const FPrescription& A, const FPrescription& B) { return A.Name == B.Name && A.Type == B.Type && A.Delivery == B.Delivery && A.Product == B.Product; }
    FORCEINLINE FString ToString() const { return FString::Printf(TEXT("{Name:%s, Type:%d, Delivery:%s, Product:%s}"), *this->Name, this->Type, *this->Delivery.ToString(), *this->Product.ToString()); }
};

/** TODO Should this class inherit from UObject? */
UCLASS()
class JAFG_API UPrescriptionSeeker : public UObject
{
    GENERATED_BODY()
    
public:

    UPrescriptionSeeker() = default;

private:

    UPROPERTY()
    const UGI_Master*       GIPtr;
    TArray<FPrescription>   Prescriptions;
    
public:

    FORCEINLINE void Init(const UGI_Master* GIPointer) { this->GIPtr = GIPointer; }
    void ReloadPrescriptions();

private:
    
    FPrescription ParsePrescription(const FString& Name, const FJsonObject& Obj) const;

private:

    static bool IsDeliveryEmpty(const FDelivery& Delivery);
    
public:

    void GetPrescription(const FDelivery& Delivery, FPrescription& OutPrescription);
    void GetProduct(const FDelivery& Delivery, FAccumulated& OutProduct);
};
