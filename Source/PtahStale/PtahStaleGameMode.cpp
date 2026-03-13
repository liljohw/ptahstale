#include "PtahStaleGameMode.h"

APtahStaleGameMode::APtahStaleGameMode()
{
	// DefaultPawnClass is intentionally left as nullptr here so that it can be
	// configured in a Blueprint subclass (BP_PtahStaleGameMode) or via the
	// World Settings / Project Settings → Maps & Modes panel in the Unreal Editor.
	// This avoids a hard dependency on a Blueprint asset path that may not yet exist.
}
