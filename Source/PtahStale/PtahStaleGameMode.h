#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PtahStaleGameMode.generated.h"

/**
 * PtahStaleGameMode
 *
 * Default game mode for PtahStale - a procedural RPG based on Call of Cthulhu 7th Edition.
 * Manages game rules, scenario flow, and overall session state.
 *
 * Configure the DefaultPawnClass via a Blueprint subclass (BP_PtahStaleGameMode)
 * or through Project Settings → Maps & Modes.
 */
UCLASS(minimalapi)
class APtahStaleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APtahStaleGameMode();
};
