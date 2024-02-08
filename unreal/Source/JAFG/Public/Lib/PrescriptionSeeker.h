#pragma once

#include "CoreMinimal.h"

#include "Lib/FAccumulated.h"

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

    void GetProduct(const FDelivery& Delivery, FAccumulated& OutProduct);
};
