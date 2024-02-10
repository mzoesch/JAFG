#pragma once

#include "CoreMinimal.h"

#include "Lib/FAccumulated.h"
#include "Net/UnrealNetwork.h"

#include "PrescriptionSeeker.generated.h"

class UGI_Master;

enum EPrescriptionType : uint8
{
    ERT_CraftingShaped,
    ERT_CraftingShapeless,
};

inline TMap<FString, EPrescriptionType> PrescriptionTypeMap =
{
    {   "Cr_Sd",  ERT_CraftingShaped      },
    {   "Cr_Sl",  ERT_CraftingShapeless   },
};

struct FPrescription
{
    FString             Name;
    EPrescriptionType   Type;
    FString             Delivery;
    FString             Product;
    FAccumulated        DeliveryAccumulated;
    FAccumulated        ProductAccumulated;

public:

    static const FPrescription NullPrescription;

public:
    /** Will only check for the name. The program will assume that only one prescription with the same name exists. */
    FORCEINLINE bool operator==(const FPrescription& O) const { return this->Name == O.Name; }
    /** Will only check for the name. The program will assume that only one prescription with the same name exists. */
    static FORCEINLINE bool Equals(const FPrescription& A, const FPrescription& B) { return A == B; }
};

struct FDelivery
{
    TArray<FAccumulated>    Delivery;
    int                     DeliveryWidth;
};

UCLASS()
class JAFG_API UPrescriptionSeeker : public UObject
{
    GENERATED_BODY()
    
public:

    UPrescriptionSeeker() = default;

private:

    UPROPERTY()
    const UGI_Master* GIPtr;
    TArray<FPrescription> Prescriptions;
    
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
