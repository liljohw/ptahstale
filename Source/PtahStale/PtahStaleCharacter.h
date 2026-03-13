#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CoC/CoCCharacterStats.h"
#include "CoC/SanitySystem.h"
#include "CoC/SkillSystem.h"
#include "PtahStaleCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

/**
 * APtahStaleCharacter
 *
 * The player-controlled investigator in PtahStale.
 * Owns the three core CoC subsystems: characteristics/stats, sanity, and skills.
 */
UCLASS(config = Game)
class APtahStaleCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APtahStaleCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

	// ── Camera ────────────────────────────────────────────────────────────

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	// ── Enhanced Input ────────────────────────────────────────────────────

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	// ── CoC Systems ───────────────────────────────────────────────────────

	/** Core CoC characteristics and derived stats */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CoC")
	TObjectPtr<UCoCCharacterStats> CharacterStats;

	/** Sanity subsystem */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CoC")
	TObjectPtr<USanitySystem> SanitySystem;

	/** Skill subsystem */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CoC")
	TObjectPtr<USkillSystem> SkillSystem;

	// ── Helpers ───────────────────────────────────────────────────────────

	/**
	 * Synchronises the sanity system's MaxSanity with the POW characteristic.
	 * Should be called after changing POW.
	 */
	UFUNCTION(BlueprintCallable, Category = "CoC")
	void SyncSanityWithPow();

protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
};
