#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CoCCharacterStats.generated.h"

/**
 * FCoCCharacteristics
 *
 * The eight core characteristics from Call of Cthulhu 7th Edition.
 * Each characteristic is a value typically between 1 and 99 (or 1-18 in some
 * representations before being multiplied by 5 for percentage rolls).
 */
USTRUCT(BlueprintType)
struct PTAHSTALE_API FCoCCharacteristics
{
	GENERATED_BODY()

	/** Strength – physical power */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CoC|Characteristics", meta = (ClampMin = "1", ClampMax = "99"))
	int32 Strength = 50;

	/** Constitution – health and stamina */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CoC|Characteristics", meta = (ClampMin = "1", ClampMax = "99"))
	int32 Constitution = 50;

	/** Size – body mass */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CoC|Characteristics", meta = (ClampMin = "1", ClampMax = "99"))
	int32 Size = 50;

	/** Dexterity – agility and reaction speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CoC|Characteristics", meta = (ClampMin = "1", ClampMax = "99"))
	int32 Dexterity = 50;

	/** Appearance – physical attractiveness and social presence */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CoC|Characteristics", meta = (ClampMin = "1", ClampMax = "99"))
	int32 Appearance = 50;

	/** Intelligence – reasoning ability */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CoC|Characteristics", meta = (ClampMin = "1", ClampMax = "99"))
	int32 Intelligence = 50;

	/** Power – strength of will and psychic sensitivity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CoC|Characteristics", meta = (ClampMin = "1", ClampMax = "99"))
	int32 Power = 50;

	/** Education – academic knowledge and experience */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CoC|Characteristics", meta = (ClampMin = "1", ClampMax = "99"))
	int32 Education = 50;

	/** Luck – fortune; not a characteristic but rolled at creation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CoC|Characteristics", meta = (ClampMin = "1", ClampMax = "99"))
	int32 Luck = 50;

	FCoCCharacteristics() = default;
};

/**
 * FCoCDerivedStats
 *
 * Stats derived from the core characteristics per CoC 7th Edition rules.
 */
USTRUCT(BlueprintType)
struct PTAHSTALE_API FCoCDerivedStats
{
	GENERATED_BODY()

	/** Maximum Hit Points = (CON + SIZ) / 10, rounded down */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CoC|Derived")
	int32 MaxHitPoints = 10;

	/** Current Hit Points */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CoC|Derived", meta = (ClampMin = "0"))
	int32 CurrentHitPoints = 10;

	/** Maximum Magic Points = POW / 5 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CoC|Derived")
	int32 MaxMagicPoints = 10;

	/** Current Magic Points */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CoC|Derived", meta = (ClampMin = "0"))
	int32 CurrentMagicPoints = 10;

	/** Maximum Sanity = POW (starting); absolute maximum is 99 - Cthulhu Mythos skill */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CoC|Derived")
	int32 MaxSanity = 50;

	/** Current Sanity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CoC|Derived", meta = (ClampMin = "0", ClampMax = "99"))
	int32 CurrentSanity = 50;

	/** Move Rate – typically 8 for humans */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CoC|Derived")
	int32 MoveRate = 8;

	/** Damage Bonus string (e.g. "-2", "0", "+1D4") */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CoC|Derived")
	FString DamageBonus = TEXT("0");

	/** Build value (-2 to +2 for typical humans) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CoC|Derived")
	int32 Build = 0;

	FCoCDerivedStats() = default;
};

/**
 * UCoCCharacterStats
 *
 * UObject component encapsulating all Call of Cthulhu 7th Edition character statistics.
 * Attach to any AActor (investigator, NPC) that needs CoC stats.
 * Provides helper functions to roll checks and calculate derived values.
 */
UCLASS(BlueprintType, Blueprintable)
class PTAHSTALE_API UCoCCharacterStats : public UObject
{
	GENERATED_BODY()

public:
	UCoCCharacterStats();

	// ── Core Characteristics ──────────────────────────────────────────────

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CoC")
	FCoCCharacteristics Characteristics;

	// ── Derived Stats ─────────────────────────────────────────────────────

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CoC")
	FCoCDerivedStats DerivedStats;

	// ── Investigator Identity ─────────────────────────────────────────────

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CoC|Identity")
	FString InvestigatorName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CoC|Identity")
	FString Occupation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CoC|Identity")
	int32 Age = 25;

	// ── Utility ───────────────────────────────────────────────────────────

	/**
	 * Recalculates all derived stats from the current characteristics.
	 * Call after modifying any characteristic.
	 */
	UFUNCTION(BlueprintCallable, Category = "CoC")
	void RecalculateDerivedStats();

	/**
	 * Performs a characteristic roll (1d100 <= value for success).
	 * Returns true on success.
	 *
	 * @param CharacteristicValue  The characteristic value to roll against.
	 * @param bHardSuccess         If true, the threshold is halved (hard success).
	 * @param bExtremeSuccess      If true, the threshold is one-fifth (extreme success).
	 */
	UFUNCTION(BlueprintCallable, Category = "CoC")
	static bool RollCharacteristic(int32 CharacteristicValue, bool bHardSuccess = false, bool bExtremeSuccess = false);

private:
	/** Compute Damage Bonus / Build from STR + SIZ total (CoC 7e table). */
	void CalculateDamageBonus(int32 StrSizTotal);
};
