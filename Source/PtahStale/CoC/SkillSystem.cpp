#include "SkillSystem.h"
#include "Math/UnrealMathUtility.h"

USkillSystem::USkillSystem()
{
	InitialiseDefaultSkills();
}

void USkillSystem::InitialiseDefaultSkills()
{
	Skills.Empty();

	// --- Combat ---
	AddOrUpdateSkill(FCoCSkill(TEXT("Fighting (Brawl)"),     ECoCSkillCategory::Combat,        25));
	AddOrUpdateSkill(FCoCSkill(TEXT("Firearms (Handgun)"),   ECoCSkillCategory::Combat,        20));
	AddOrUpdateSkill(FCoCSkill(TEXT("Firearms (Rifle)"),     ECoCSkillCategory::Combat,        25));
	AddOrUpdateSkill(FCoCSkill(TEXT("Firearms (Shotgun)"),   ECoCSkillCategory::Combat,        25));
	AddOrUpdateSkill(FCoCSkill(TEXT("Dodge"),                ECoCSkillCategory::Combat,        0));  // DEX/2, set at character creation

	// --- Academic ---
	AddOrUpdateSkill(FCoCSkill(TEXT("Accounting"),           ECoCSkillCategory::Academic,       5));
	AddOrUpdateSkill(FCoCSkill(TEXT("Anthropology"),         ECoCSkillCategory::Academic,       1));
	AddOrUpdateSkill(FCoCSkill(TEXT("Archaeology"),          ECoCSkillCategory::Academic,       1));
	AddOrUpdateSkill(FCoCSkill(TEXT("History"),              ECoCSkillCategory::Academic,       5));
	AddOrUpdateSkill(FCoCSkill(TEXT("Law"),                  ECoCSkillCategory::Academic,       5));
	AddOrUpdateSkill(FCoCSkill(TEXT("Library Use"),          ECoCSkillCategory::Academic,      20));
	AddOrUpdateSkill(FCoCSkill(TEXT("Medicine"),             ECoCSkillCategory::Academic,       1));
	AddOrUpdateSkill(FCoCSkill(TEXT("Natural World"),        ECoCSkillCategory::Academic,      10));
	AddOrUpdateSkill(FCoCSkill(TEXT("Occult"),               ECoCSkillCategory::Occult,         5));
	AddOrUpdateSkill(FCoCSkill(TEXT("Cthulhu Mythos"),       ECoCSkillCategory::Occult,         0));
	AddOrUpdateSkill(FCoCSkill(TEXT("Science (Any)"),        ECoCSkillCategory::Academic,       1));

	// --- Interpersonal ---
	AddOrUpdateSkill(FCoCSkill(TEXT("Charm"),                ECoCSkillCategory::Interpersonal, 15));
	AddOrUpdateSkill(FCoCSkill(TEXT("Fast Talk"),            ECoCSkillCategory::Interpersonal,  5));
	AddOrUpdateSkill(FCoCSkill(TEXT("Intimidate"),           ECoCSkillCategory::Interpersonal, 15));
	AddOrUpdateSkill(FCoCSkill(TEXT("Persuade"),             ECoCSkillCategory::Interpersonal, 10));
	AddOrUpdateSkill(FCoCSkill(TEXT("Psychology"),           ECoCSkillCategory::Interpersonal, 10));

	// --- Perception ---
	AddOrUpdateSkill(FCoCSkill(TEXT("Listen"),               ECoCSkillCategory::Perception,    20));
	AddOrUpdateSkill(FCoCSkill(TEXT("Spot Hidden"),          ECoCSkillCategory::Perception,    25));
	AddOrUpdateSkill(FCoCSkill(TEXT("Track"),                ECoCSkillCategory::Perception,    10));

	// --- Physical ---
	AddOrUpdateSkill(FCoCSkill(TEXT("Climb"),                ECoCSkillCategory::Physical,      20));
	AddOrUpdateSkill(FCoCSkill(TEXT("Jump"),                 ECoCSkillCategory::Physical,      20));
	AddOrUpdateSkill(FCoCSkill(TEXT("Swim"),                 ECoCSkillCategory::Physical,      20));
	AddOrUpdateSkill(FCoCSkill(TEXT("Throw"),                ECoCSkillCategory::Physical,      20));
	AddOrUpdateSkill(FCoCSkill(TEXT("First Aid"),            ECoCSkillCategory::Physical,      30));

	// --- Stealth ---
	AddOrUpdateSkill(FCoCSkill(TEXT("Stealth"),              ECoCSkillCategory::Stealth,       20));
	AddOrUpdateSkill(FCoCSkill(TEXT("Disguise"),             ECoCSkillCategory::Stealth,        5));
	AddOrUpdateSkill(FCoCSkill(TEXT("Sleight of Hand"),      ECoCSkillCategory::Stealth,       10));

	// --- Technical ---
	AddOrUpdateSkill(FCoCSkill(TEXT("Electrical Repair"),    ECoCSkillCategory::Technical,     10));
	AddOrUpdateSkill(FCoCSkill(TEXT("Mechanical Repair"),    ECoCSkillCategory::Technical,     10));
	AddOrUpdateSkill(FCoCSkill(TEXT("Locksmith"),            ECoCSkillCategory::Technical,      1));
	AddOrUpdateSkill(FCoCSkill(TEXT("Drive Auto"),           ECoCSkillCategory::Technical,     20));
	AddOrUpdateSkill(FCoCSkill(TEXT("Navigate"),             ECoCSkillCategory::Technical,     10));
	AddOrUpdateSkill(FCoCSkill(TEXT("Pilot (Any)"),          ECoCSkillCategory::Technical,      1));

	// --- Miscellaneous ---
	AddOrUpdateSkill(FCoCSkill(TEXT("Art/Craft (Any)"),      ECoCSkillCategory::Miscellaneous,  5));
	AddOrUpdateSkill(FCoCSkill(TEXT("Language (Own)"),       ECoCSkillCategory::Miscellaneous,  0)); // EDU, set at character creation
	AddOrUpdateSkill(FCoCSkill(TEXT("Language (Other)"),     ECoCSkillCategory::Miscellaneous,  1));
	AddOrUpdateSkill(FCoCSkill(TEXT("Appraise"),             ECoCSkillCategory::Miscellaneous,  5));
	AddOrUpdateSkill(FCoCSkill(TEXT("Credit Rating"),        ECoCSkillCategory::Miscellaneous,  0)); // Occupation-dependent
}

void USkillSystem::AddOrUpdateSkill(FCoCSkill Skill)
{
	Skills.Add(Skill.SkillName, Skill);
}

void USkillSystem::SyncDerivedSkillsFromCharacteristics(int32 DEX, int32 EDU)
{
	// Dodge base = DEX / 2 (CoC 7e rule)
	if (FCoCSkill* Dodge = Skills.Find(TEXT("Dodge")))
	{
		Dodge->BaseValue    = FMath::Max(0, DEX / 2);
		Dodge->CurrentValue = FMath::Max(Dodge->CurrentValue, Dodge->BaseValue);
	}

	// Language (Own) base = EDU (CoC 7e rule)
	if (FCoCSkill* LangOwn = Skills.Find(TEXT("Language (Own)")))
	{
		LangOwn->BaseValue    = FMath::Clamp(EDU, 0, 99);
		LangOwn->CurrentValue = FMath::Max(LangOwn->CurrentValue, LangOwn->BaseValue);
	}
}

int32 USkillSystem::GetSkillValue(FName SkillName) const
{
	const FCoCSkill* Found = Skills.Find(SkillName);
	return Found ? Found->CurrentValue : -1;
}

bool USkillSystem::RollSkill(FName SkillName, bool bHardSuccess, bool bExtremeSuccess, bool& bOutSuccess)
{
	FCoCSkill* Skill = Skills.Find(SkillName);
	if (!Skill)
	{
		bOutSuccess = false;
		return false;
	}

	const int32 Roll = FMath::RandRange(1, 100);

	int32 Threshold = Skill->CurrentValue;
	if (bExtremeSuccess)
	{
		Threshold = FMath::Max(1, Skill->CurrentValue / 5);
	}
	else if (bHardSuccess)
	{
		Threshold = FMath::Max(1, Skill->CurrentValue / 2);
	}

	bOutSuccess = (Roll <= Threshold);

	// Tick the skill for improvement if we succeeded on a regular (not hard/extreme) roll
	if (bOutSuccess && !bHardSuccess && !bExtremeSuccess)
	{
		Skill->bExperienceTick = true;
	}

	return true;
}

void USkillSystem::PerformImprovementRolls()
{
	for (auto& Pair : Skills)
	{
		FCoCSkill& Skill = Pair.Value;
		if (!Skill.bExperienceTick)
		{
			continue;
		}

		// Roll 1d100; if result > current value, improve by 1d10
		const int32 ImprovementRoll = FMath::RandRange(1, 100);
		if (ImprovementRoll > Skill.CurrentValue)
		{
			const int32 Gain = FMath::RandRange(1, 10);
			Skill.CurrentValue = FMath::Clamp(Skill.CurrentValue + Gain, 0, 99);
		}

		Skill.bExperienceTick = false;
	}
}
