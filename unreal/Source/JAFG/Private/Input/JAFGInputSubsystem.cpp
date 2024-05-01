// Copyright 2024 mzoesch. All rights reserved.

#include "Input/JAFGInputSubsystem.h"

#include "InputAction.h"
#include "InputMappingContext.h"

void UJAFGInputSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    LOG_DISPLAY(LogTemp, "Called.")

    MyInputMappingContext = NewObject<UInputMappingContext>(this, FName("MyInputMappingContext"));

    MyInputAction = NewObject<UInputAction>(this, FName("MyInputAction"));

    MyInputMappingContext->MapKey(MyInputAction, EKeys::SpaceBar);


    return;
}

void UJAFGInputSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
}
