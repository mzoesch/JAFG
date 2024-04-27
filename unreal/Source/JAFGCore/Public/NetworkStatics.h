// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "NetworkStatics.generated.h"

JAFG_VOID

/**
 * Common network related functions useful for this application.
 */
UCLASS(MinimalAPI)
class UNetworkStatics : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintPure, Category = "JAFG|NetworkStatics", meta = (WorldContext = "WorldContextObject"))
    static JAFGCORE_API bool IsStandalone(const UObject* WorldContextObject)
    {
        return UKismetSystemLibrary::IsStandalone(WorldContextObject);
    }

    UFUNCTION(BlueprintPure, Category = "JAFG|NetworkStatics", meta = (WorldContext = "WorldContextObject"))
    static JAFGCORE_API bool IsServer(const UObject* WorldContextObject)
    {
        return UKismetSystemLibrary::IsServer(WorldContextObject);
    }

    UFUNCTION(BlueprintPure, Category = "JAFG|NetworkStatics", meta = (WorldContext = "WorldContextObject"))
    static JAFGCORE_API bool IsDedicatedServer(const UObject* WorldContextObject)
    {
        return UKismetSystemLibrary::IsDedicatedServer(WorldContextObject);
    }

    UFUNCTION(BlueprintPure, Category = "JAFG|NetworkStatics", meta = (WorldContext = "WorldContextObject"))
    static JAFGCORE_API bool IsSafeStandalone(const UObject* WorldContextObject)
    {
        return UKismetSystemLibrary::IsStandalone(WorldContextObject);
    }

    UFUNCTION(BlueprintPure, Category = "JAFG|NetworkStatics", meta = (WorldContext = "WorldContextObject"))
    static JAFGCORE_API bool IsSafeServer(const UObject* WorldContextObject)
    {
        return UKismetSystemLibrary::IsServer(WorldContextObject);
    }

    UFUNCTION(BlueprintPure, Category = "JAFG|NetworkStatics", meta = (WorldContext = "WorldContextObject"))
    static JAFGCORE_API bool IsSafeDedicatedServer(const UObject* WorldContextObject)
    {
        return UKismetSystemLibrary::IsDedicatedServer(WorldContextObject);
    }

    UFUNCTION(BlueprintPure, Category = "JAFG|NetworkStatics", meta = (WorldContext = "WorldContextObject"))
    static JAFGCORE_API bool IsSafeListenServer(const UObject* WorldContextObject)
    {
        return
               UKismetSystemLibrary::IsStandalone(WorldContextObject)      == false
            && UKismetSystemLibrary::IsDedicatedServer(WorldContextObject) == false
            && UKismetSystemLibrary::IsServer(WorldContextObject)          == true;
    }

    UFUNCTION(BlueprintPure, Category = "JAFG|NetworkStatics", meta = (WorldContext = "WorldContextObject"))
    static JAFGCORE_API bool IsSafeClient(const UObject* WorldContextObject)
    {
        return
               UKismetSystemLibrary::IsStandalone(WorldContextObject)      == false
            && UKismetSystemLibrary::IsDedicatedServer(WorldContextObject) == false
            && UKismetSystemLibrary::IsServer(WorldContextObject)          == false;
    }
};
