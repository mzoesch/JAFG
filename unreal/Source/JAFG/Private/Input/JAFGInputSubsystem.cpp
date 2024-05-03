// Copyright 2024 mzoesch. All rights reserved.

#include "Input/JAFGInputSubsystem.h"

void UJAFGInputSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    LOG_VERBOSE(LogGameSettings, "Called.")

    this->Contexts.Empty();
    this->Actions.Empty();

    return;
}

void UJAFGInputSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
}

void UJAFGInputSubsystem::AddContext(const FJAFGInputContext& InContext)
{
    if (this->DoesContextExist(InContext.Name))
    {
#if WITH_EDITOR
        LOG_ERROR(LogGameSettings, "Context already exists: %s. Discarding implementation.", *InContext.Name)
#else /* WITH_EDITOR */
        LOG_FATAL(LogGameSettings, "Context already exists: %s.", *InContext.Name)
#endif /* WITH_EDITOR */
        return;
    }

    UInputMappingContext* Context = NewObject<UInputMappingContext>();

    FJAFGPrivateInputContext PrivateContext;
    PrivateContext.Config  = InContext;
    PrivateContext.Context = Context;

    this->Contexts.Add(PrivateContext);

    LOG_VERBOSE(LogGameSettings, "Added context: %s.", *PrivateContext.Config.Name)

    return;
}

void UJAFGInputSubsystem::AddAction(FJAFGInputAction& InAction)
{
    if (this->DoesActionExist(InAction.Name))
    {
#if WITH_EDITOR
        LOG_ERROR(LogGameSettings, "Action already exists: %s. Discarding implementation.", *InAction.Name)
#else /* WITH_EDITOR */
        LOG_FATAL(LogGameSettings, "Action already exists: %s.", *InAction.Name)
#endif /* WITH_EDITOR */
        return;
    }

    UInputAction* Action = NewObject<UInputAction>();

    FJAFGPrivateInputAction PrivateAction;
    PrivateAction.Name             = InAction.Name;
    PrivateAction.NorthDefaultKeyA = InAction.DefaultKeyA;
    PrivateAction.NorthDefaultKeyB = InAction.DefaultKeyB;
    PrivateAction.SouthDefaultKeyA = EKeys::Invalid;
    PrivateAction.SouthDefaultKeyB = EKeys::Invalid;
    PrivateAction.WestDefaultKeyA  = EKeys::Invalid;
    PrivateAction.WestDefaultKeyB  = EKeys::Invalid;
    PrivateAction.EastDefaultKeyA  = EKeys::Invalid;
    PrivateAction.EastDefaultKeyB  = EKeys::Invalid;
    PrivateAction.Contexts         = InAction.Contexts;
    PrivateAction.bIs2DAction      = false;
    PrivateAction.Action           = Action;

    for (const FString& ContextName : PrivateAction.Contexts)
    {
        const FJAFGPrivateInputContext* Context = this->GetSafeContext(ContextName);

        Context->Context->MapKey(Action, PrivateAction.NorthDefaultKeyA);
        Context->Context->MapKey(Action, PrivateAction.NorthDefaultKeyB);
        continue;
    }

    this->Actions.Add(PrivateAction);

    LOG_VERBOSE(LogGameSettings, "Added action: %s.", *PrivateAction.Name)

    return;
}

void UJAFGInputSubsystem::AddAction(FJAFG2DInputAction& InAction)
{
    if (this->DoesActionExist(InAction.Name))
    {
#if WITH_EDITOR
        LOG_ERROR(LogGameSettings, "Action already exists: %s. Discarding implementation.", *InAction.Name)
#else /* WITH_EDITOR */
        LOG_FATAL(LogGameSettings, "Action already exists: %s.", *InAction.Name)
#endif /* WITH_EDITOR */
        return;
    }

    UInputAction* Action = NewObject<UInputAction>();

    FJAFGPrivateInputAction PrivateAction;
    PrivateAction.Name              = InAction.Name;
    PrivateAction.NorthDefaultKeyA  = InAction.NorthDefaultKeyA;
    PrivateAction.NorthDefaultKeyB  = InAction.NorthDefaultKeyB;
    PrivateAction.SouthDefaultKeyA  = InAction.SouthDefaultKeyA;
    PrivateAction.SouthDefaultKeyB  = InAction.SouthDefaultKeyB;
    PrivateAction.WestDefaultKeyA   = InAction.WestDefaultKeyA;
    PrivateAction.WestDefaultKeyB   = InAction.WestDefaultKeyB;
    PrivateAction.EastDefaultKeyA   = InAction.EastDefaultKeyA;
    PrivateAction.EastDefaultKeyB   = InAction.EastDefaultKeyB;
    PrivateAction.Contexts          = InAction.Contexts;
    PrivateAction.bIs2DAction       = true;
    PrivateAction.Action            = Action;
    PrivateAction.Action->ValueType = EInputActionValueType::Axis2D;

    for (const FString& ContextName : PrivateAction.Contexts)
    {
        const FJAFGPrivateInputContext* Context = this->GetSafeContext(ContextName);

        FEnhancedActionKeyMapping& MappingNorthA = Context->Context->MapKey(Action, PrivateAction.NorthDefaultKeyA);
        MappingNorthA.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>());
        FEnhancedActionKeyMapping& MappingNorthB = Context->Context->MapKey(Action, PrivateAction.NorthDefaultKeyB);
        MappingNorthB.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>());

        FEnhancedActionKeyMapping& MappingSouthA =  Context->Context->MapKey(Action, PrivateAction.SouthDefaultKeyA);
        MappingSouthA.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>());
        MappingSouthA.Modifiers.Add(NewObject<UInputModifierNegate>());
        FEnhancedActionKeyMapping& MappingSouthB = Context->Context->MapKey(Action, PrivateAction.SouthDefaultKeyB);
        MappingSouthB.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>());
        MappingSouthB.Modifiers.Add(NewObject<UInputModifierNegate>());

        FEnhancedActionKeyMapping& MappingWestA = Context->Context->MapKey(Action, PrivateAction.WestDefaultKeyA);
        MappingWestA.Modifiers.Add(NewObject<UInputModifierNegate>());
        FEnhancedActionKeyMapping& MappingWestB = Context->Context->MapKey(Action, PrivateAction.WestDefaultKeyB);
        MappingWestB.Modifiers.Add(NewObject<UInputModifierNegate>());

                                                  Context->Context->MapKey(Action, PrivateAction.EastDefaultKeyA);
                                                  Context->Context->MapKey(Action, PrivateAction.EastDefaultKeyB);

        continue;
    }

    this->Actions.Add(PrivateAction);

    LOG_VERBOSE(LogGameSettings, "Added action: %s.", *PrivateAction.Name)

    return;
}

void UJAFGInputSubsystem::AddAction(FJAFG2DMouseInputAction& InAction)
{
    if (this->DoesActionExist(InAction.Name))
    {
#if WITH_EDITOR
        LOG_ERROR(LogGameSettings, "Action already exists: %s. Discarding implementation.", *InAction.Name)
#else /* WITH_EDITOR */
        LOG_FATAL(LogGameSettings, "Action already exists: %s.", *InAction.Name)
#endif /* WITH_EDITOR */
        return;
    }

    UInputAction* Action = NewObject<UInputAction>();
    FJAFGPrivateInputAction PrivateAction;
    PrivateAction.Name              = InAction.Name;
    PrivateAction.NorthDefaultKeyA  = EKeys::Invalid;
    PrivateAction.NorthDefaultKeyB  = EKeys::Invalid;
    PrivateAction.SouthDefaultKeyA  = EKeys::Invalid;
    PrivateAction.SouthDefaultKeyB  = EKeys::Invalid;
    PrivateAction.WestDefaultKeyA   = EKeys::Invalid;
    PrivateAction.WestDefaultKeyB   = EKeys::Invalid;
    PrivateAction.EastDefaultKeyA   = EKeys::Invalid;
    PrivateAction.EastDefaultKeyB   = EKeys::Invalid;
    PrivateAction.Contexts          = InAction.Contexts;
    PrivateAction.bIs2DAction       = true;
    PrivateAction.Action            = Action;
    PrivateAction.Action->ValueType = EInputActionValueType::Axis2D;

    for (const FString& ContextName : PrivateAction.Contexts)
    {
        const FJAFGPrivateInputContext* Context = this->GetSafeContext(ContextName);

        Context->Context->MapKey(Action, EKeys::Mouse2D);

        continue;
    }

    this->Actions.Add(PrivateAction);

    LOG_VERBOSE(LogGameSettings, "Added action: %s.", *InAction.Name)

    return;
}

UInputMappingContext* UJAFGInputSubsystem::GetContextByName(const FString& Name)
{
    for (FJAFGPrivateInputContext& Context : this->Contexts)
    {
        if (Context.Config.Name == Name)
        {
            return Context.Context;
        }

        continue;
    }

    return nullptr;
}

UInputMappingContext* UJAFGInputSubsystem::GetSafeContextByName(const FString& Name)
{
    if (UInputMappingContext* Context = this->GetContextByName(Name); Context)
    {
        return Context;
    }

    LOG_FATAL(LogGameSettings, "Context with name [%s] was not found.", *Name)

    return nullptr;
}

TArray<FString> UJAFGInputSubsystem::GetAllActionNames(void) const
{
    TArray<FString> Names;

    // ReSharper disable once CppUseStructuredBinding
    for (const FJAFGPrivateInputAction& Action : this->Actions)
    {
        Names.Add(Action.Name);
    }

    return Names;
}

UInputAction* UJAFGInputSubsystem::GetActionByName(const FString& Name)
{
    // ReSharper disable once CppUseStructuredBinding
    for (const FJAFGPrivateInputAction& Action : this->Actions)
    {
        if (Action.Name == Name)
        {
            return Action.Action;
        }

        continue;
    }

    return nullptr;
}

bool UJAFGInputSubsystem::DoesContextExist(const FString& Name) const
{
    for (const auto& [Config, Context] : this->Contexts)
    {
        if (Config.Name == Name)
        {
            return true;
        }

        continue;
    }

    return false;
}

FJAFGPrivateInputContext* UJAFGInputSubsystem::GetContext(const FString& Name)
{
    for (FJAFGPrivateInputContext& Context : this->Contexts)
    {
        if (Context.Config.Name == Name)
        {
            return &Context;
        }

        continue;
    }

    return nullptr;
}

FJAFGPrivateInputContext* UJAFGInputSubsystem::GetSafeContext(const FString& Name)
{
    if (FJAFGPrivateInputContext* Context = this->GetContext(Name); Context)
    {
        return Context;
    }

    LOG_FATAL(LogGameSettings, "Context with name [%s] was not found.", *Name)

    return nullptr;
}

bool UJAFGInputSubsystem::DoesActionExist(const FString& Name) const
{
    // ReSharper disable once CppUseStructuredBinding
    for (const FJAFGPrivateInputAction& Action : this->Actions)
    {
        if (Action.Name == Name)
        {
            return true;
        }

        continue;
    }

    return false;
}
