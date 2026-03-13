#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SanitySystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSanityChanged, int32, NewSanity, int32, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTemporaryInsanity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIndefiniteInsanity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPermanentInsanity);

/**
 * ESanityState
 *
 * Represents the investigator's current mental state per CoC 7e rules.
 */
UENUM(BlueprintType)
enum class ESanityState : uint8
{
	Normal            UMETA(DisplayName = "Normal"),
	TemporaryInsanity UMETA(DisplayName = "Temporary Insanity"),
	IndefiniteInsanity UMETA(DisplayName = "Indefinite Insanity"),
	PermanentInsanity UMETA(DisplayName = "Permanent Insanity (0 SAN)")
};

/**
 * USanitySystem
 *
 * Implements the full CoC 7th Edition Sanity subsystem.
 *
 * Rules implemented:
 *  - Sanity loss from shocks (immediate and POW-save variants)
 *  - Temporary insanity: losing ≥5 SAN in a single round triggers a Bout of Madness
 *  - Indefinite insanity: losing ≥ 1/5 of current SAN within an hour
 *  - Permanent insanity: SAN drops to 0
 *  - Sanity gain from therapy / rewards
 *  - Cthulhu Mythos knowledge cap on maximum SAN
 */
UCLASS(BlueprintType, Blueprintable)
class PTAHSTALE_API USanitySystem : public UObject
{
	GENERATED_BODY()

public:
	USanitySystem();

	// ── State ─────────────────────────────────────────────────────────────

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sanity")
	int32 CurrentSanity = 50;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sanity")
	int32 MaxSanity = 50;

	/** Tracks SAN lost in the current round (resets each round) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sanity")
	int32 SanityLostThisRound = 0;

	/** Tracks SAN lost in the past ~hour of game time */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sanity")
	int32 SanityLostThisSession = 0;

	/** Points of Cthulhu Mythos knowledge (caps MaxSanity at 99 - CthulhuMythos) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sanity", meta = (ClampMin = "0", ClampMax = "99"))
	int32 CthulhuMythos = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sanity")
	ESanityState SanityState = ESanityState::Normal;

	// ── Events ────────────────────────────────────────────────────────────

	UPROPERTY(BlueprintAssignable, Category = "Sanity|Events")
	FOnSanityChanged OnSanityChanged;

	UPROPERTY(BlueprintAssignable, Category = "Sanity|Events")
	FOnTemporaryInsanity OnTemporaryInsanity;

	UPROPERTY(BlueprintAssignable, Category = "Sanity|Events")
	FOnIndefiniteInsanity OnIndefiniteInsanity;

	UPROPERTY(BlueprintAssignable, Category = "Sanity|Events")
	FOnPermanentInsanity OnPermanentInsanity;

	// ── API ───────────────────────────────────────────────────────────────

	/**
	 * Apply a sanity loss.
	 *
	 * @param MinLoss  Minimum SAN lost (rolled on failed POW save).
	 * @param MaxLoss  Maximum SAN lost (rolled on failed POW save).
	 * @param bRolledSuccessfully  True if the character succeeded on their POW save;
	 *                             in that case only MinLoss is applied.
	 */
	UFUNCTION(BlueprintCallable, Category = "Sanity")
	void ApplySanityLoss(int32 MinLoss, int32 MaxLoss, bool bRolledSuccessfully);

	/**
	 * Restore sanity points (therapy, spell, reward).
	 * Capped at MaxSanity (99 - CthulhuMythos).
	 */
	UFUNCTION(BlueprintCallable, Category = "Sanity")
	void RestoreSanity(int32 Amount);

	/** Called at the start of each new combat round to reset round-based tracking. */
	UFUNCTION(BlueprintCallable, Category = "Sanity")
	void OnNewRound();

	/** Called at the start of each new in-game session / extended rest to reset session tracking. */
	UFUNCTION(BlueprintCallable, Category = "Sanity")
	void OnNewSession();

	/** Recalculates MaxSanity based on CthulhuMythos knowledge. */
	UFUNCTION(BlueprintCallable, Category = "Sanity")
	void RecalculateMaxSanity();

private:
	void CheckInsanityThresholds(int32 LostThisRound);
	void SetSanityState(ESanityState NewState);
};
