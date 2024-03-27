// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/JAFGCommonWidget.h"
#include "Components/EditableText.h"

#include "ChatMenu.generated.h"

class UTextBlock;
class UScrollBox;
class UEditableText;

struct JAFG_API FChatMessageData final : public FMyPassData
{
    FChatMessageData() = default;
    virtual ~FChatMessageData() = default;
    
    FString Message;
};

UCLASS(Abstract, Blueprintable)
class UChatMenuEntry : public UJAFGCommonWidget
{
    GENERATED_BODY()

protected:

    FChatMessageData EntryData;

private:

    UPROPERTY(BlueprintReadOnly, Category = "ChatMenuEntry", meta = (AllowPrivateAccess = "true", BindWidget))
    UTextBlock* TB_ChatMessage;
    
public:

    virtual void NativeConstruct(void) override;
    virtual void PassDataToWidget(const FMyPassData& MyPassedData) override;
};

UCLASS(Abstract, Blueprintable)
class JAFG_API UChatMenu : public UJAFGCommonWidget
{
    GENERATED_BODY()

protected:

    virtual void NativeConstruct(void) override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
        
private:

    inline static constexpr int HideChatPreviewDelay = 0xA;
    float HideChatPreviewTimer;
    
    /* Used to toggle on and off while all other chat elements are being shown. */
    UPROPERTY(BlueprintReadOnly, Category = "ChatMenu", meta = (AllowPrivateAccess = "true", BindWidget))
    UWidget* W_StdInWrapper;

    UPROPERTY(BlueprintReadOnly, Category = "ChatMenu", meta = (AllowPrivateAccess = "true", BindWidget))
    UEditableText* ET_StdIn;
    
    UPROPERTY(BlueprintReadOnly, Category = "ChatMenu", meta = (AllowPrivateAccess = "true", BindWidget))
    UScrollBox* SB_StdOut;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChatMenu", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UChatMenuEntry> WChatMenuEntryClass;

    UFUNCTION()
    void OnTextChatChanged(const FText& Text);

    /**
     * Must be manually called by the derived blueprint implementation. See this classes Native Constructor
     * for more information about this problem.
     */
    UFUNCTION(BlueprintCallable, Category = "ChatMenu", meta = (AllowPrivateAccess = "true"))
    void OnChatTextCommitted(const FText& Text /* , ETextCommit::Type CommitMethod */);

    UFUNCTION(BlueprintCallable, Category = "ChatMenu", meta = (AllowPrivateAccess = "true"))
    void HideChatMenu(/* void */) const;
    
public:
    
    /**
     * @param bCollapsed   If true, the chat menu as a whole will be collapsed.
     * @param bShowPreview If true and bCollapsed true, only the preview widget will be shown. Otherwise ignored.
     */
    void ToggleChatMenu(const bool bCollapsed, const bool bShowPreview = false);
    void AddMessageToChat(const FString& Message);

private:

    bool IsStdInValid(void) const;
};
