// Copyright 2024 mzoesch. All rights reserved.

#include "Input/JAFGInputSubsystem.h"

#include "InputMappingContext.h"

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

    // this->Contexts.Add(MoveTemp(PrivateContext)); // TODO Is this correct?
    this->Contexts.Add(PrivateContext); // TODO Is this correct?

    LOG_VERBOSE(LogGameSettings, "Added context: %s.", *InContext.Name)

    return;
}

void UJAFGInputSubsystem::AddAction(const FJAFGInputAction& InAction)
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
    PrivateAction.Config = InAction;
    PrivateAction.Action = Action;

    for (const FString& ContextName : InAction.Contexts)
    {
        const FJAFGPrivateInputContext* Context = this->GetSafeContext(ContextName);

        Context->Context->MapKey(Action, InAction.DefaultKeyA);
        Context->Context->MapKey(Action, InAction.DefaultKeyB);

        continue;
    }

    this->Actions.Add(PrivateAction); // TODO Is this correct?

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

    // for (const auto& [Config, Context] : this->Contexts)
    // {
    //     if (Config.Name == Name)
    //     {
    //         return Context;
    //     }
    //
    //     continue;
    // }

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

    for (const auto& [Config, Action] : this->Actions)
    {
        Names.Add(Config.Name);
    }

    return Names;
}

UInputAction* UJAFGInputSubsystem::GetActionByName(const FString& Name)
{
    for (const auto& [Config, Action] : this->Actions)
    {
        if (Config.Name == Name)
        {
            return Action;
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
    for (const auto& [Config, Action] : this->Actions)
    {
        if (Config.Name == Name)
        {
            return true;
        }

        continue;
    }

    return false;
}
