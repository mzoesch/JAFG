// Copyright 2024 mzoesch. All rights reserved.

#include "Commands/ChatCommandStatics.h"

bool CommandStatics::IsCommand(const FText& StdIn)
{
    return StdIn.ToString().StartsWith(CommandStatics::CommandPrefix);
}

bool CommandStatics::IsServerCommand(const FText& StdIn)
{
    return StdIn.ToString().ToLower().StartsWith(CommandStatics::CommandPrefix + CommandStatics::ServerCommandPrefix);
}

bool CommandStatics::IsClientCommand(const FText& StdIn)
{
    return StdIn.ToString().ToLower().StartsWith(CommandStatics::CommandPrefix + CommandStatics::ClientCommandPrefix);
}

FString CommandStatics::GetCommand(const FText& StdIn)
{
    FString Command = StdIn.ToString().ToLower();
    Command.RemoveFromStart(CommandStatics::CommandPrefix);

    if (Command.IsEmpty())
    {
        return Command;
    }

    Command = Command.Left(
        Command.Find(TEXT(" ")) == INDEX_NONE ? Command.Len() : Command.Find(TEXT(" "),
        ESearchCase::IgnoreCase, ESearchDir::FromStart)
    );

    return Command;
}

FString CommandStatics::GetCommandWithServerPrefix(const FText& StdIn)
{
    FString Command = CommandStatics::GetCommand(StdIn);

    if (Command.IsEmpty())
    {
        return Command;
    }

    if (Command.StartsWith(CommandStatics::ServerCommandPrefix) == false)
    {
        Command = CommandStatics::ServerCommandPrefix + Command;
    }

    return Command;
}

FString CommandStatics::SafePrefixServerCommand(const FString& Command)
{
    if (Command.IsEmpty())
    {
        return L"";
    }

    if (Command.StartsWith(CommandStatics::ServerCommandPrefix) == false)
    {
        return CommandStatics::ServerCommandPrefix + Command;
    }

    return Command;
}

FString CommandStatics::GetCommandWithClientPrefix(const FText& StdIn)
{
    FString Command = CommandStatics::GetCommand(StdIn);

    if (Command.IsEmpty())
    {
        return Command;
    }

    if (Command.StartsWith(CommandStatics::ClientCommandPrefix) == false)
    {
        Command = CommandStatics::ClientCommandPrefix + Command;
    }

    return Command;
}

FString CommandStatics::SafePrefixClientCommand(const FString& Command)
{
    if (Command.IsEmpty())
    {
        return TEXT("");
    }

    if (Command.StartsWith(CommandStatics::ClientCommandPrefix) == false)
    {
        return CommandStatics::ClientCommandPrefix + Command;
    }

    return Command;
}

FString CommandStatics::GetCommandWithArgs(const FText& StdIn, TArray<FString>& OutArgs)
{
    int32 CurrentArgStart;
    if (StdIn.ToString().ToLower().FindChar(TEXT(' '), CurrentArgStart) == false)
    {
        /* The command has not args. */
        return CommandStatics::GetCommand(StdIn);
    }

    /* Adding the index of the whitespace. */
    CurrentArgStart += 1;

    FString StdInAsString = StdIn.ToString();
    for (int32 Cursor = CurrentArgStart; Cursor < StdIn.ToString().Len(); ++Cursor)
    {
        if (StdInAsString[Cursor] == TEXT(' '))
        {
            if (CurrentArgStart == Cursor)
            {
                /* The argument is empty. */
                CurrentArgStart = Cursor + 1;
                continue;
            }

            OutArgs.Add(StdInAsString.Mid(CurrentArgStart, Cursor - CurrentArgStart));
            CurrentArgStart = Cursor + 1;
        }
    }

    if (CurrentArgStart < StdInAsString.Len())
    {
        OutArgs.Add(StdInAsString.Mid(CurrentArgStart, StdInAsString.Len() - CurrentArgStart));
    }

    return CommandStatics::GetCommand(StdIn);
}

bool CommandStatics::DoesCommandStartWithSpecificCommandType(const FChatCommand& Command, bool& bOutClientPrefix)
{
    if (Command.StartsWith(CommandStatics::ClientCommandPrefix))
    {
        bOutClientPrefix = true;
        return true;
    }

    if (Command.StartsWith(CommandStatics::ServerCommandPrefix))
    {
        bOutClientPrefix = false;
        return true;
    }

    return false;

}

bool CommandStatics::DoesStdInStartWithSpecificCommandType(const FText& StdIn, bool& bOutClientPrefix)
{
    if (StdIn.IsEmpty())
    {
        return false;
    }

    if (CommandStatics::IsCommand(StdIn) == false)
    {
        return false;
    }

    return CommandStatics::DoesCommandStartWithSpecificCommandType(CommandStatics::GetCommand(StdIn), bOutClientPrefix);
}
