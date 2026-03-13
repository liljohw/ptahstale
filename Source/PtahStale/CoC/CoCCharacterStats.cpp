#include "CoCCharacterStats.h"
#include "Math/UnrealMathUtility.h"

UCoCCharacterStats::UCoCCharacterStats()
{
	RecalculateDerivedStats();
}

void UCoCCharacterStats::RecalculateDerivedStats()
{
	// Hit Points = (CON + SIZ) / 10, rounded down
	DerivedStats.MaxHitPoints = (Characteristics.Constitution + Characteristics.Size) / 10;
	DerivedStats.CurrentHitPoints = FMath::Clamp(DerivedStats.CurrentHitPoints, 0, DerivedStats.MaxHitPoints);

	// Magic Points = POW / 5
	DerivedStats.MaxMagicPoints = Characteristics.Power / 5;
	DerivedStats.CurrentMagicPoints = FMath::Clamp(DerivedStats.CurrentMagicPoints, 0, DerivedStats.MaxMagicPoints);

	// Starting Sanity = POW
	DerivedStats.MaxSanity = Characteristics.Power;
	DerivedStats.CurrentSanity = FMath::Clamp(DerivedStats.CurrentSanity, 0, 99);

	// Move Rate: default 8; reduced by 1 if both DEX and STR < SIZ, increased by 1 if both > SIZ
	DerivedStats.MoveRate = 8;
	if (Characteristics.Dexterity < Characteristics.Size && Characteristics.Strength < Characteristics.Size)
	{
		DerivedStats.MoveRate = 7;
	}
	else if (Characteristics.Dexterity > Characteristics.Size && Characteristics.Strength > Characteristics.Size)
	{
		DerivedStats.MoveRate = 9;
	}

	// Age penalties (rough implementation per 7e rules)
	if (Age >= 40)
	{
		const int32 Decades = (Age - 40) / 10 + 1;
		DerivedStats.MoveRate = FMath::Max(1, DerivedStats.MoveRate - Decades);
	}

	// Damage Bonus / Build (STR + SIZ)
	CalculateDamageBonus(Characteristics.Strength + Characteristics.Size);
}

void UCoCCharacterStats::CalculateDamageBonus(int32 StrSizTotal)
{
	// CoC 7th Edition Damage Bonus / Build table
	if (StrSizTotal <= 64)
	{
		DerivedStats.DamageBonus = TEXT("-2");
		DerivedStats.Build = -2;
	}
	else if (StrSizTotal <= 84)
	{
		DerivedStats.DamageBonus = TEXT("-1");
		DerivedStats.Build = -1;
	}
	else if (StrSizTotal <= 124)
	{
		DerivedStats.DamageBonus = TEXT("0");
		DerivedStats.Build = 0;
	}
	else if (StrSizTotal <= 164)
	{
		DerivedStats.DamageBonus = TEXT("+1D4");
		DerivedStats.Build = 1;
	}
	else if (StrSizTotal <= 204)
	{
		DerivedStats.DamageBonus = TEXT("+1D6");
		DerivedStats.Build = 2;
	}
	else
	{
		DerivedStats.DamageBonus = TEXT("+2D6");
		DerivedStats.Build = 3;
	}
}

bool UCoCCharacterStats::RollCharacteristic(int32 CharacteristicValue, bool bHardSuccess, bool bExtremeSuccess)
{
	// Roll 1d100 (percentile)
	const int32 Roll = FMath::RandRange(1, 100);

	int32 Threshold = CharacteristicValue;
	if (bExtremeSuccess)
	{
		Threshold = FMath::Max(1, CharacteristicValue / 5);
	}
	else if (bHardSuccess)
	{
		Threshold = FMath::Max(1, CharacteristicValue / 2);
	}

	return Roll <= Threshold;
}
