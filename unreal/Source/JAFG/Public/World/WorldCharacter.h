// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "WorldGeneratorInfo.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"

#include "WorldCharacter.generated.h"

JAFG_VOID

class ULocalChunkValidator;
class UChatComponent;
class UChatMenu;
class UEscapeMenu;
class UInputAction;
class UInputComponent;
class UInputMappingContext;
struct FInputActionValue;

UCLASS(Abstract, Blueprintable)
class JAFG_API AWorldCharacter : public ACharacter
{
    GENERATED_BODY()

public:

    explicit AWorldCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

public:

    virtual void Tick(const float DeltaTime) override;

private:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UChatComponent* ChatComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ULocalChunkValidator> LocalChunkValidator;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FirstPersonCameraComponent;

public:

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IMC")
    UInputMappingContext* IMCFoot;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IMC")
    UInputMappingContext* IMCMenu;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IMC")
    UInputMappingContext* IMCChatMenu;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Movement")
    UInputAction* IAJump;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Movement")
    UInputAction* IALook;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Movement")
    UInputAction* IAMove;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Interraction")
    UInputAction* IAPrimary;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Interraction")
    UInputAction* IASecondary;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|MISC")
    UInputAction* IAToggleEscapeMenu;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|MISC")
    UInputAction* IAToggleChatMenu;

private:

    void OnTriggerJump(const FInputActionValue& Value);
    void OnCompleteJump(const FInputActionValue& Value);
    void OnLook(const FInputActionValue& Value);
    void OnMove(const FInputActionValue& Value);

    void OnPrimary(const FInputActionValue& Value);
    UFUNCTION(Server, Reliable)
    void OnPrimary_ServerRPC(const FInputActionValue& Value);
    void OnSecondary(const FInputActionValue& Value);
    UFUNCTION(Server, Reliable)
    void OnSecondary_ServerRPC(const FInputActionValue& Value);

    void OnToggleEscapeMenu(const FInputActionValue& Value);
    friend UEscapeMenu;
    void OnToggleChatMenu(const FInputActionValue& Value);
    friend UChatMenu;

public:

    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:

    /** The added offset to the First Person Camera Component. */
    inline static const FVector TorsoOffset { 0.0f, 0.0f, -50.0f };

    /**
     * APawn#RemoteViewPitch is a uint8 and represents the pitch of the remote view replicated back to the server.
     * This method converts the pitch to degrees.
     * Does not work on listens servers.
     */
    FORCEINLINE auto GetRemoteViewPitchAsDeg(void) const -> float
    {
        return this->RemoteViewPitch * 360.0f / /* 2^8 - 1 = */ 255.0f;
    }

    /** Equivalent to 4 x Chunk => 4 x 16 Voxels => 64 Voxels. */
    static constexpr float MaxPOVLineTraceLength { (AWorldGeneratorInfo::ChunkSize * 4.0f ) * AWorldGeneratorInfo::JToUScale };
    // ReSharper disable once CppMemberFunctionMayBeStatic
    FORCEINLINE auto GetCharacterReachInVoxels(void) const -> float { return 4.5f; }
    FORCEINLINE auto GetCharacterReach(void) const -> float { return this->GetCharacterReachInVoxels() * AWorldGeneratorInfo::JToUScale; }

    FORCEINLINE auto GetFirstPersonTraceStart(void) const -> FTransform { return this->FirstPersonCameraComponent->GetComponentTransform(); }
    FORCEINLINE auto GetFirstPersonTraceStart_DedServer(void) const -> FTransform
    {
        return FTransform(
            FQuat(FRotator(
                this->GetRemoteViewPitchAsDeg(),
                this->GetFirstPersonTraceStart().Rotator().Yaw,
                this->GetFirstPersonTraceStart().Rotator().Roll
            )),
            this->GetFirstPersonTraceStart().GetLocation(),
            FVector::OneVector
        );
    }
    void GetTargetedVoxel(ACommonChunk*& OutChunk, FVector& OutWorldHitLocation, FVector_NetQuantizeNormal& OutWorldNormalHitLocation, FIntVector& OutLocalHitVoxelLocation, const bool bUseRemotePitch, const float UnrealReach = AWorldCharacter::MaxPOVLineTraceLength) const;

public:

    FORCEINLINE auto GetTorsoLocation(void) const -> FVector { return this->FirstPersonCameraComponent->GetComponentLocation() + AWorldCharacter::TorsoOffset; }
};
