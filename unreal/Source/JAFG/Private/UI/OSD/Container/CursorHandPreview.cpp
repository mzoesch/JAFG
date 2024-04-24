// Copyright 2024 mzoesch. All rights reserved.

#include "UI/OSD/Container/CursorHandPreview.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/UserInterfaceSettings.h"
#include "System/TextureSubsystem.h"
#include "World/WorldCharacter.h"

#define OWNING_CHARACTER                                                    \
    Cast<AWorldCharacter>(this->GetOwningPlayer()->GetCharacter())
#define CHECK_OWNING_CHARACTER                                              \
    check( this->GetOwningPlayer() )                                        \
    check( this->GetOwningPlayer()->GetCharacter() )                        \
    check( Cast<AWorldCharacter>(this->GetOwningPlayer()->GetCharacter()) )
#define CHECKED_OWNING_CHARACTER                                            \
    check( this->GetOwningPlayer() )                                        \
    check( this->GetOwningPlayer()->GetCharacter() )                        \
    check( Cast<AWorldCharacter>(this->GetOwningPlayer()->GetCharacter()) ) \
    Cast<AWorldCharacter>(this->GetOwningPlayer()->GetCharacter())

void UCursorHandPreview::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->SetVisibility(ESlateVisibility::HitTestInvisible);

    this->CPS_Wrapper = Cast<UCanvasPanelSlot>(this->CP_Wrapper->Slot);
    check( this->CPS_Wrapper )

    CHECK_OWNING_CHARACTER

    if (OWNING_CHARACTER->GetCursorHand() == Accumulated::Null)
    {
        LOG_ERROR(LogCommonSlate, "Cursor hand is null. But was instantiated.")
        this->RemoveFromParent();
    }

    this->TB_Amount->SetText(
        OWNING_CHARACTER->GetCursorHand().Amount == 1
            ? FText::GetEmpty()
            : FText::FromString(FString::FromInt(OWNING_CHARACTER->GetCursorHand().Amount))
    );

    check( this->GetGameInstance() )
    UTextureSubsystem* TextureSubsystem = this->GetGameInstance()->GetSubsystem<UTextureSubsystem>();
    check( TextureSubsystem )

    if (UTexture2D* Texture = TextureSubsystem->GetTexture2D(OWNING_CHARACTER->GetCursorHand()))
    {
        this->I_Preview->SetBrushFromTexture(Texture);
        this->I_Preview->SetColorAndOpacity(FLinearColor::White);

        return;
    }

    LOG_FATAL(LogCommonSlate, "Failed to load any texture for the cursor hand.")

    return;
}

void UCursorHandPreview::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    FVector2D ViewportSize;
    this->GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);

    const float DPIScaling = GetDefault<UUserInterfaceSettings>()->GetDPIScaleBasedOnSize(FIntPoint(ViewportSize.X, ViewportSize.Y));

    FVector2D MousePosition;
    if (!this->GetOwningPlayer()->GetMousePosition(MousePosition.X, MousePosition.Y))
    {
        return;
    }

    this->CPS_Wrapper->SetPosition(MousePosition / DPIScaling - FVector2D(25.0f, 25.0f));

    return;
}

#undef OWNING_CHARACTER
#undef CHECK_OWNING_CHARACTER
#undef CHECKED_OWNING_CHARACTER
