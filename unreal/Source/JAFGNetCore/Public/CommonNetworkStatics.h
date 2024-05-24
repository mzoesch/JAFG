// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "CommonNetworkStatics.generated.h"

/**
 * Common network related functions useful for this application.
 */
UCLASS(MinimalAPI)
class UNetStatics : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintPure, Category = "JAFG|NetworkStatics", meta = (WorldContext = "WorldContextObject"))
    static JAFGNETCORE_API bool IsStandalone(const UObject* WorldContextObject)
    {
        return UKismetSystemLibrary::IsStandalone(WorldContextObject);
    }

    UFUNCTION(BlueprintPure, Category = "JAFG|NetworkStatics", meta = (WorldContext = "WorldContextObject"))
    static JAFGNETCORE_API bool IsServer(const UObject* WorldContextObject)
    {
        return UKismetSystemLibrary::IsServer(WorldContextObject);
    }

    UFUNCTION(BlueprintPure, Category = "JAFG|NetworkStatics", meta = (WorldContext = "WorldContextObject"))
    static JAFGNETCORE_API bool IsDedicatedServer(const UObject* WorldContextObject)
    {
        return UKismetSystemLibrary::IsDedicatedServer(WorldContextObject);
    }

    UFUNCTION(BlueprintPure, Category = "JAFG|NetworkStatics", meta = (WorldContext = "WorldContextObject"))
    static JAFGNETCORE_API bool IsSafeStandalone(const UObject* WorldContextObject)
    {
        return UKismetSystemLibrary::IsStandalone(WorldContextObject);
    }

    UFUNCTION(BlueprintPure, Category = "JAFG|NetworkStatics", meta = (WorldContext = "WorldContextObject"))
    static JAFGNETCORE_API bool IsSafeStandaloneNoServer(const UObject* WorldContextObject)
    {
        return
               UKismetSystemLibrary::IsStandalone(WorldContextObject) == true
            && UKismetSystemLibrary::IsServer(WorldContextObject)     == false;
    }

    UFUNCTION(BlueprintPure, Category = "JAFG|NetworkStatics", meta = (WorldContext = "WorldContextObject"))
    static JAFGNETCORE_API bool IsSafeServer(const UObject* WorldContextObject)
    {
        return UKismetSystemLibrary::IsServer(WorldContextObject);
    }

    UFUNCTION(BlueprintPure, Category = "JAFG|NetworkStatics", meta = (WorldContext = "WorldContextObject"))
    static JAFGNETCORE_API bool IsSafeDedicatedServer(const UObject* WorldContextObject)
    {
        return UKismetSystemLibrary::IsDedicatedServer(WorldContextObject);
    }

    UFUNCTION(BlueprintPure, Category = "JAFG|NetworkStatics", meta = (WorldContext = "WorldContextObject"))
    static JAFGNETCORE_API bool IsSafeListenServer(const UObject* WorldContextObject)
    {
#if WITH_EDITOR
        return
               UKismetSystemLibrary::IsStandalone(WorldContextObject)      == false
            && UKismetSystemLibrary::IsDedicatedServer(WorldContextObject) == false
            && UKismetSystemLibrary::IsServer(WorldContextObject)          == true;
#else /* WITH_EDITOR */
        return
            /*
             * This is a little bit unique here. When in the editor, we have the option to directly start a listen
             * server PIE session. But this is almost never the case in a packaged / shipped game. There we start a
             * *standalone* game and then act as a server in the same instance of the game. Therefore, IsStandalone
             * is true in a packaged game when we are a listen server but never in the editor when directly starting
             * via the PIE interface for starting a listen server.
             * Currently just commented out, as it behaves very differently in the editor and in a packaged game. We
             * need to do some more testing to see if this is a reliable way to determine if we are a listen server.
             * Are there any edge cases where this can get falsely true or truly false?
             */
            /* UKismetSystemLibrary::IsStandalone(WorldContextObject)      == true */
               UKismetSystemLibrary::IsDedicatedServer(WorldContextObject) == false
            && UKismetSystemLibrary::IsServer(WorldContextObject)          == true;
#endif /* !WITH_EDITOR */
    }

    UFUNCTION(BlueprintPure, Category = "JAFG|NetworkStatics", meta = (WorldContext = "WorldContextObject"))
    static JAFGNETCORE_API bool IsSafeClient(const UObject* WorldContextObject)
    {
        return
               UKismetSystemLibrary::IsStandalone(WorldContextObject)      == false
            && UKismetSystemLibrary::IsDedicatedServer(WorldContextObject) == false
            && UKismetSystemLibrary::IsServer(WorldContextObject)          == false;
    }
};
