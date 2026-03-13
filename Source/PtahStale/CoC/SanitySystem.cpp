#include "SanitySystem.h"
#include "Math/UnrealMathUtility.h"

USanitySystem::USanitySystem()
{
	RecalculateMaxSanity();
}

void USanitySystem::RecalculateMaxSanity()
{
	MaxSanity = FMath::Clamp(99 - CthulhuMythos, 0, 99);
	CurrentSanity = FMath::Clamp(CurrentSanity, 0, MaxSanity);
}

void USanitySystem::ApplySanityLoss(int32 MinLoss, int32 MaxLoss, bool bRolledSuccessfully)
{
	int32 Loss = bRolledSuccessfully
		? MinLoss
		: FMath::RandRange(MinLoss, MaxLoss);

	Loss = FMath::Max(0, Loss);

	CurrentSanity = FMath::Clamp(CurrentSanity - Loss, 0, MaxSanity);
	SanityLostThisRound += Loss;
	SanityLostThisSession += Loss;

	OnSanityChanged.Broadcast(CurrentSanity, -Loss);
	CheckInsanityThresholds(SanityLostThisRound);
}

void USanitySystem::RestoreSanity(int32 Amount)
{
	const int32 Cap = FMath::Clamp(99 - CthulhuMythos, 0, 99);
	const int32 OldSanity = CurrentSanity;
	CurrentSanity = FMath::Clamp(CurrentSanity + Amount, 0, Cap);
	const int32 Delta = CurrentSanity - OldSanity;

	if (Delta > 0)
	{
		OnSanityChanged.Broadcast(CurrentSanity, Delta);
	}

	// Re-evaluate state upward if we're recovering
	if (CurrentSanity > 0 && SanityState == ESanityState::PermanentInsanity)
	{
		SetSanityState(ESanityState::Normal);
	}
}

void USanitySystem::OnNewRound()
{
	SanityLostThisRound = 0;
}

void USanitySystem::OnNewSession()
{
	SanityLostThisSession = 0;
}

void USanitySystem::CheckInsanityThresholds(int32 LostThisRound)
{
	if (CurrentSanity <= 0)
	{
		SetSanityState(ESanityState::PermanentInsanity);
		OnPermanentInsanity.Broadcast();
		return;
	}

	// Temporary insanity: lost >=5 SAN in a single round
	if (LostThisRound >= 5 && SanityState == ESanityState::Normal)
	{
		SetSanityState(ESanityState::TemporaryInsanity);
		OnTemporaryInsanity.Broadcast();
		return;
	}

	// Indefinite insanity: lost >= 1/5 of original MaxSanity within a session
	const int32 IndefiniteThreshold = FMath::Max(1, MaxSanity / 5);
	if (SanityLostThisSession >= IndefiniteThreshold && SanityState != ESanityState::IndefiniteInsanity)
	{
		SetSanityState(ESanityState::IndefiniteInsanity);
		OnIndefiniteInsanity.Broadcast();
	}
}

void USanitySystem::SetSanityState(ESanityState NewState)
{
	SanityState = NewState;
}
