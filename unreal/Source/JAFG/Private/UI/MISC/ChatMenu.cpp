// Copyright 2024 mzoesch. All rights reserved.

#include "UI/MISC/ChatMenu.h"

#include "InputActionValue.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Network/ChatComponent.h"
#include "World/WorldCharacter.h"

#define OWNING_CHARACTER Cast<AWorldCharacter>(this->GetOwningPlayerPawn())

void UChatMenuEntry::NativeConstruct()
{
    Super::NativeConstruct();
}

void UChatMenuEntry::PassDataToWidget(const FMyPassData& MyPassedData)
{
    if (const FChatMessageData* ChatMessageData = static_cast<const FChatMessageData*>(&MyPassedData); ChatMessageData == nullptr)
    {
#if WITH_EDITOR
        UE_LOG(LogTemp, Error, TEXT("UChatMenuEntry::PassDataToWidget: MyPassedData is not of type FChatMessageData."))
#else
        UE_LOG(LogTemp, Fatal, TEXT("UChatMenuEntry::PassDataToWidget: MyPassedData is not of type FChatMessageData."))
#endif /* WITH_EDITOR */
    }
    else
    {
        this->EntryData = *ChatMessageData;
    }

    this->TB_ChatMessage->SetText(FText::FromString(this->EntryData.Message));

    this->OnDeferredConstruct();
    
    return;
}

void UChatMenu::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->HideChatPreviewTimer = 0.0f;
    
    this->SB_StdOut->ClearChildren();

    this->ET_StdIn->OnTextChanged.AddDynamic(this, &UChatMenu::OnTextChatChanged);
    /*
     * This line causes an Linker Error : error LNK2019: Why?
     *
     * Module.JAFG.cpp.obj : error LNK2019:
     *      unresolved external symbol "__declspec(dllimport) class UEnum * __cdecl Z_Construct_UEnum_SlateCore_ETextCommit(void)"
     *      (__imp_?Z_Construct_UEnum_SlateCore_ETextCommit@@YAPEAVUEnum@@XZ) referenced in function "void __cdecl `dynamic
     *      initializer for 'public: static struct UECodeGen_Private::FBytePropertyParams const
     *      Z_Construct_UFunction_UChatMenu_Test_Statics::NewProp_CommitMethod''(void)"
     *      (??__E?NewProp_CommitMethod@Z_Construct_UFunction_UChatMenu_Test_Statics@@2UFBytePropertyParams@UECodeGen_Private@@B@@YAXXZ)
     *
     * Currently solved with a workaround in the derived blueprint class implementation.
     */
    /* this->ET_StdIn->OnTextCommitted.AddDynamic(this, &UChatMenu::OnChatTextCommitted); */
    
    this->ToggleChatMenu(true);
    
    return;
}

void UChatMenu::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (this->W_StdInWrapper->GetVisibility() == ESlateVisibility::Visible)
    {
        this->HideChatPreviewTimer = 0.0f;
        return;
    }

    this->HideChatPreviewTimer += InDeltaTime;

    if (this->HideChatPreviewTimer >= UChatMenu::HideChatPreviewDelay)
    {
        this->ToggleChatMenu(true);
    }
    
    return;
}

/** Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void UChatMenu::OnTextChatChanged(const FText& Text)
{
    if (Text.ToString().Len() > UChatComponent::MaxChatInputLength)
    {
        this->ET_StdIn->SetText(FText::FromString(Text.ToString().Left(UChatComponent::MaxChatInputLength)));
    }
}

/** Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppUE4BlueprintCallableFunctionMayBeConst
void UChatMenu::OnChatTextCommitted(const FText& Text)
{
    if (this->IsStdInValid() == false)
    {
        if (Text.ToString().Len() <= 0)
        {
            OWNING_CHARACTER->OnToggleChatMenu(FInputActionValue());
            return;
        }

        UE_LOG(LogTemp, Warning, TEXT("UChatMenu::OnChatTextCommitted: Invalid input detected [%s]."), *Text.ToString());
        
        return;
    }

    OWNING_CHARACTER->OnToggleChatMenu(FInputActionValue());

    if (this->GetWorld() == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("UChatMenu::OnChatTextCommitted: World is invalid."));
        return;
    }

    OWNING_CHARACTER->ChatComponent->QueueMessage(Text.ToString());
    
    return;
}

// ReSharper disable once CppUE4BlueprintCallableFunctionMayBeStatic
void UChatMenu::HideChatMenu(void) const
{
    /*
     * Hiding chat by default input. See blueprint implementation for more information.
     * Currently unused as this event implicit calls the Toggle Chat Input Action from the currently applied Chat
     * Menu Input Mapping Context. Not clean but works.
     * This method currently does not serve any purpose and is only kept for future reference.
     */
}

void UChatMenu::ToggleChatMenu(const bool bCollapsed, const bool bShowPreview)
{
    /*
     * We might want to do some additional stuff here later.
     */

    if (bCollapsed)
    {
        this->HideChatPreviewTimer = 0.0f;
        
        if (bShowPreview)
        {
            this->SetVisibility(ESlateVisibility::Visible);
            this->W_StdInWrapper->SetVisibility(ESlateVisibility::Hidden);
        }
        else
        {
            this->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
    else
    {
        this->SetVisibility(ESlateVisibility::Visible);
        this->W_StdInWrapper->SetVisibility(ESlateVisibility::Visible);
        
        this->ET_StdIn->SetText(FText::GetEmpty());
        this->ET_StdIn->SetFocus();

        this->OnDeferredConstruct();
    }

    return;
}

void UChatMenu::AddMessageToChat(const FString& Message)
{
    FChatMessageData Data; Data.Message = Message;
    UChatMenuEntry* Entry = CreateWidget<UChatMenuEntry>(this->GetWorld(), this->WChatMenuEntryClass);
    Entry->PassDataToWidget(Data);
    this->SB_StdOut->AddChild(Entry);

    if (this->GetVisibility() == ESlateVisibility::Collapsed)
    {
        this->ToggleChatMenu(true, true);
    }
    
    return;
}

bool UChatMenu::IsStdInValid() const
{
    if (this->ET_StdIn->GetText().ToString().Len() <= 0 || this->ET_StdIn->GetText().ToString().Len() > UChatComponent::MaxChatInputLength)
    {
        return false;
    }

    /* Some more validations have to be added here in the future. */
    
    return true;
}

#undef OWNING_CHARACTER
