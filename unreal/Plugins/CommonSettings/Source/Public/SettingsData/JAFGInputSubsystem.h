// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "InputMappingContext.h"
#include "InputAction.h"

#include "JAFGInputSubsystem.generated.h"

class UJAFGSettingsLocal;

//////////////////////////////////////////////////////////////////////////
// External Initialization Data Structures
// Use these types to load custom input actions and contexts.
//////////////////////////////////////////////////////////////////////////

struct COMMONSETTINGS_API FJAFGUpperInputContext
{
    FString         Name;
    TArray<FString> InputContextRedirections;
};

struct COMMONSETTINGS_API FJAFGInputContext
{
    FString Name;
};

struct COMMONSETTINGS_API FMappableKeyPair
{
    FKey KeyA;
    FKey KeyB;
};

struct COMMONSETTINGS_API FJAFGSingleInputAction
{
    FString          Name;
    FMappableKeyPair Keys;
    TArray<FString>  Contexts;
};

struct COMMONSETTINGS_API FJAFGDualInputAction
{
    FString                  Name;
    TArray<FMappableKeyPair> Keys;
    TArray<TArray<FString>>  Contexts;
};

struct COMMONSETTINGS_API FJAFGOneDimensionalInputAction
{
    FString          Name;
    FMappableKeyPair NorthKeys;
    FMappableKeyPair SouthKeys;
    TArray<FString>  Contexts;
};


struct COMMONSETTINGS_API FJAFGTwoDimensionalInputAction
{
    FString          Name;
    FMappableKeyPair NorthKeys;
    FMappableKeyPair SouthKeys;
    FMappableKeyPair WestKeys;
    FMappableKeyPair EastKeys;
    TArray<FString>  Contexts;
};

struct COMMONSETTINGS_API FJAFGTwoDimensionalMouseInputAction
{
    FString         Name;
    TArray<FString> Contexts;
};

//////////////////////////////////////////////////////////////////////////
// ~External Initialization Data Structures
//////////////////////////////////////////////////////////////////////////

USTRUCT(NotBlueprintType)
struct COMMONSETTINGS_API FLoadedContext
{
    GENERATED_BODY()

    FString Name;

    UPROPERTY() /* Do not remove UPROPERTY - Garbage Collection!!! */
    TObjectPtr<UInputMappingContext> Context;
};

namespace EJAFGInputActionType
{

enum COMMONSETTINGS_API Type
{
    Invalid,
    Single,
    Dual,
    OneDimensional,
    TwoDimensional,
    TwoDimensionalMouse
};

}

USTRUCT(NotBlueprintType)
struct COMMONSETTINGS_API FLoadedInputAction
{
    GENERATED_BODY()

    FString Name;

    EJAFGInputActionType::Type Type = EJAFGInputActionType::Invalid;

    /* Note that keys might not be meaningful if the type does not allow them. */

    FKey NorthDefaultKeyA;
    FKey NorthDefaultKeyB;
    FKey SouthDefaultKeyA;
    FKey SouthDefaultKeyB;
    FKey WestDefaultKeyA;
    FKey WestDefaultKeyB;
    FKey EastDefaultKeyA;
    FKey EastDefaultKeyB;

    /** Start of the new context subdivision if dual. */
    int32 ContextDivider;
    TArray<FString> Contexts;

    UPROPERTY() /* Do not remove UPROPERTY - Garbage Collection!!! */
    TObjectPtr<UInputAction> Action;
};

USTRUCT(NotBlueprintType)
struct COMMONSETTINGS_API FUpperContextValue
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FString> InputContextRedirections;
};

/**
 * The backbone of the input system. Handles all input related tasks.
 * Bindings, actions, key mappings / re-mappings, etc.
 *
 * Other subsystems may add their own actions and contexts to this system as it is empty by default.
 */
UCLASS(NotBlueprintType)
class COMMONSETTINGS_API UJAFGInputSubsystem : public ULocalPlayerSubsystem
{
    GENERATED_BODY()

    friend UJAFGSettingsLocal;

public:

    // Subsystem implementation
    virtual auto Initialize(FSubsystemCollectionBase& Collection) -> void override;
    virtual auto Deinitialize(void) -> void override;
    // ~Subsystem implementation

    auto AddUpperContext(const FJAFGUpperInputContext& InContext) -> void;
    auto AddContext(const FJAFGInputContext& InContext) -> void;

    auto AddAction(const FJAFGSingleInputAction& InAction) -> void;
    auto AddAction(const FJAFGDualInputAction& InAction) -> void;
    auto AddAction(const FJAFGOneDimensionalInputAction& InAction) -> void;
    auto AddAction(const FJAFGTwoDimensionalInputAction& InAction) -> void;
    auto AddAction(const FJAFGTwoDimensionalMouseInputAction& InAction) -> void;

    auto DoesUpperContextExist(const FString& Name) const -> bool;
    auto DoesContextExist(const FString& Name) const -> bool;
    auto DoesActionExist(const FString& Name) const -> bool;

    auto GetContextValue(const FString& Name) -> UInputMappingContext*;
    auto GetSafeContextValue(const FString& Name) -> UInputMappingContext*;

    auto GetAllActionNames(void) -> TArray<FString>;
    auto GetActionValue(const FString& Name) -> UInputAction*;
    auto GetSafeActionValue(const FString& Name) -> UInputAction*;

    /** @return Will only return non-invalid keys. */
    auto GetAllKeysForAction(const FString& Name) const -> TArray<FKey>;

    auto GetUpperContext(const FString& Name) -> FUpperContextValue*;
    auto GetSafeUpperContext(const FString& Name) -> FUpperContextValue*;

    auto GetContext(const FString& Name) -> FLoadedContext*;
    auto GetSafeContext(const FString& Name) -> FLoadedContext*;

    auto GetAction(const FString& Name) -> FLoadedInputAction*;
    auto GetSafeAction(const FString& Name) -> FLoadedInputAction*;

    auto GetActionConst(const FString& Name) const -> const FLoadedInputAction*;
    auto GetSafeActionConst(const FString& Name) const -> const FLoadedInputAction*;

    FORCEINLINE auto GetAllLoadedContexts(void) const -> const TArray<FLoadedContext>& { return this->Contexts; }
    FORCEINLINE auto GetAllLoadedInputActions(void) const -> const TArray<FLoadedInputAction>& { return this->Actions; }

private:

    static auto CreateEmptyAction(const FString& Name, const TArray<FString>& Contexts) -> FLoadedInputAction;

    UPROPERTY() /* Do not remove UPROPERTY - Garbage Collection!!! */
    TMap<FString, FUpperContextValue> UpperContexts;
    UPROPERTY() /* Do not remove UPROPERTY - Garbage Collection!!! */
    TArray<FLoadedContext> Contexts;
    UPROPERTY() /* Do not remove UPROPERTY - Garbage Collection!!! */
    TArray<FLoadedInputAction> Actions;
};
