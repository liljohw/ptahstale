#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SkillSystem.generated.h"

/**
 * ECoCSkillCategory
 *
 * Broad groupings of skills from CoC 7th Edition.
 */
UENUM(BlueprintType)
enum class ECoCSkillCategory : uint8
{
	Combat       UMETA(DisplayName = "Combat"),
	Academic     UMETA(DisplayName = "Academic"),
	Interpersonal UMETA(DisplayName = "Interpersonal"),
	Perception   UMETA(DisplayName = "Perception"),
	Physical     UMETA(DisplayName = "Physical"),
	Stealth      UMETA(DisplayName = "Stealth"),
	Technical    UMETA(DisplayName = "Technical"),
	Occult       UMETA(DisplayName = "Occult"),
	Miscellaneous UMETA(DisplayName = "Miscellaneous")
};

/**
 * FCoCSkill
 *
 * Represents a single CoC 7e skill with its base value, current value,
 * and flags for improvement tracking.
 */
USTRUCT(BlueprintType)
struct PTAHSTALE_API FCoCSkill
{
	GENERATED_BODY()

	/** Display name of the skill (e.g. "Library Use", "Cthulhu Mythos") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	FName SkillName;

	/** Category for UI grouping */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	ECoCSkillCategory Category = ECoCSkillCategory::Miscellaneous;

	/** Base value as printed in the rulebook (before investigator points) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill", meta = (ClampMin = "0", ClampMax = "99"))
	int32 BaseValue = 0;

	/** Current value after spending occupation/personal interest points and improvements */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill", meta = (ClampMin = "0", ClampMax = "99"))
	int32 CurrentValue = 0;

	/** Ticked when the skill is used successfully; used during improvement rolls */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	bool bExperienceTick = false;

	FCoCSkill() = default;

	FCoCSkill(FName InName, ECoCSkillCategory InCategory, int32 InBase)
		: SkillName(InName), Category(InCategory), BaseValue(InBase), CurrentValue(InBase)
	{}
};

/**
 * USkillSystem
 *
 * Manages the full skill list for an investigator and exposes helpers for
 * performing skill rolls and advancing skills at scenario end.
 */
UCLASS(BlueprintType, Blueprintable)
class PTAHSTALE_API USkillSystem : public UObject
{
	GENERATED_BODY()

public:
	USkillSystem();

	/** All skills this character has */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	TMap<FName, FCoCSkill> Skills;

	// ── API ───────────────────────────────────────────────────────────────

	/**
	 * Attempt a skill roll.
	 *
	 * @param SkillName       Name of the skill to roll.
	 * @param bHardSuccess    Roll against half the skill value.
	 * @param bExtremeSuccess Roll against one-fifth the skill value.
	 * @param bOutSuccess     Set to true if the roll succeeded.
	 * @return true if the skill was found and the roll was performed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Skills")
	bool RollSkill(FName SkillName, bool bHardSuccess, bool bExtremeSuccess, bool& bOutSuccess);

	/**
	 * Perform end-of-scenario improvement rolls for all ticked skills.
	 * Each ticked skill: roll 1d100 – if result > current value, add 1d10.
	 */
	UFUNCTION(BlueprintCallable, Category = "Skills")
	void PerformImprovementRolls();

	/**
	 * Populate the skill list with the standard CoC 7e investigator skill defaults.
	 * Should be called once when creating a new investigator.
	 */
	UFUNCTION(BlueprintCallable, Category = "Skills")
	void InitialiseDefaultSkills();

	/** Add or overwrite a skill entry */
	UFUNCTION(BlueprintCallable, Category = "Skills")
	void AddOrUpdateSkill(FCoCSkill Skill);

	/**
	 * Synchronise derived skill base values from characteristics.
	 * Must be called after characteristic values are finalised at character creation:
	 *  - "Dodge"          → DEX / 2
	 *  - "Language (Own)" → EDU
	 *
	 * @param DEX  The investigator's Dexterity characteristic value.
	 * @param EDU  The investigator's Education characteristic value.
	 */
	UFUNCTION(BlueprintCallable, Category = "Skills")
	void SyncDerivedSkillsFromCharacteristics(int32 DEX, int32 EDU);

	/** Returns the current value for a skill, or -1 if not found */
	UFUNCTION(BlueprintPure, Category = "Skills")
	int32 GetSkillValue(FName SkillName) const;
};
