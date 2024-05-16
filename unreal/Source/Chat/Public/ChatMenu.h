// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGSlateCore/Public/JAFGWidget.h"

#include "ChatMenu.generated.h"

class UTextBlock;
class UScrollBox;
class UEditableText;

struct CHAT_API FChatMessageData final : public FMyPassedData
{
    FChatMessageData(void) = default;
    FChatMessageData(const FString& Sender, const FText& Message) : Sender(Sender), Message(Message)
    {
    }
    ~FChatMessageData(void) = default;

    FString Sender;
    FText   Message;

    FORCEINLINE virtual auto ToString(void) const -> FString override
    {
        return FString::Printf(TEXT("FChatMessageData{Sender:%s, Message:%s}"), *this->Sender, *this->Message.ToString());
    }
};

UCLASS(Abstract, Blueprintable)
class CHAT_API UChatMenuEntry : public UJAFGWidget
{
    GENERATED_BODY()

public:

    virtual void PassDataToWidget(const FMyPassedData& MyPassedData) override;

protected:

    FChatMessageData Data;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    UTextBlock* TB_Message;

    virtual void ConstructMessage(void);
};

UCLASS(Abstract, Blueprintable)
class CHAT_API UChatMenu : public UJAFGWidget
{
    GENERATED_BODY()

public:

    explicit UChatMenu(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual auto NativeConstruct(void) -> void override;
    virtual auto NativeDestruct(void) -> void override;
    virtual auto NativeTick(const FGeometry& MyGeometry, const float InDeltaTime) -> void override;
    // ~UUserWidget implementation

    const int HideChatStdOutDelayInSeconds { 0xA };
    float HideChatStdOutTimer = 0.0f;

public:

    void AddMessageToChatLog(const FString& Sender, const FText& Message);

protected:

    FDelegateHandle ChatMenuVisibilityChangedHandle;
    virtual auto OnChatMenuVisibilityChanged(const bool bVisible) -> void;
    /**
     * If the chat menu decided on its own to hide again and not through the enhanced input subsystem.
     * E.g.: Due to text commited.
     */
    virtual auto HideChatMenu(void) -> void;
    /** *Only* show the stdout. */
    virtual auto ShowStdOut(void) -> void;
    virtual auto HideStdOut(void) -> void;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UPanelWidget> PW_StdOutWrapper;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UEditableText> ET_StdIn;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UPanelWidget> PW_StdInWrapper;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UScrollBox> SB_StdOut;

    UFUNCTION()
    void OnChatTextChanged(const FText& Text);

    UFUNCTION()
    void OnChatTextCommitted(const FText& Text, const ETextCommit::Type CommitMethod);

    auto FocusStdIn(void) const -> void;
    auto ClearStdIn(void) const -> void;

    auto IsStdToLong(void) const -> bool;
    auto IsStdInValid(void) const -> bool;
};
