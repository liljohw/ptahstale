#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralWorldGenerator.generated.h"

/**
 * ELocationTag
 *
 * High-level classification of a generated location, used by
 * the lore and encounter systems to seed appropriate content.
 */
UENUM(BlueprintType)
enum class ELocationTag : uint8
{
	None          UMETA(DisplayName = "None"),
	Town          UMETA(DisplayName = "Town"),
	Countryside   UMETA(DisplayName = "Countryside"),
	Coastal       UMETA(DisplayName = "Coastal"),
	Forest        UMETA(DisplayName = "Forest"),
	Ruins         UMETA(DisplayName = "Ruins"),
	Underground   UMETA(DisplayName = "Underground"),
	DeeplyWrong   UMETA(DisplayName = "Deeply Wrong")  // A location tainted by the Mythos
};

/**
 * FGeneratedRoom
 *
 * Represents a single room / area in a procedurally generated location.
 */
USTRUCT(BlueprintType)
struct PTAHSTALE_API FGeneratedRoom
{
	GENERATED_BODY()

	/** Grid position in the location map */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	FIntPoint GridPosition = FIntPoint(0, 0);

	/** World-space centre of the room */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	FVector WorldPosition = FVector::ZeroVector;

	/** Half-extents of the room */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	FVector HalfExtents = FVector(300.f, 300.f, 150.f);

	/** Tag controlling what content is spawned */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	ELocationTag Tag = ELocationTag::None;

	/** True if this room contains a Mythos clue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	bool bHasMythosClue = false;

	/** True if a hostile encounter can occur here */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	bool bHasEncounter = false;

	FGeneratedRoom() = default;
};

/**
 * FProceduralLocationParams
 *
 * Parameters passed to the generator to control map generation.
 */
USTRUCT(BlueprintType)
struct PTAHSTALE_API FProceduralLocationParams
{
	GENERATED_BODY()

	/** Seed for deterministic generation (0 = random) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	int32 Seed = 0;

	/** Number of rooms to generate */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator", meta = (ClampMin = "1", ClampMax = "200"))
	int32 RoomCount = 20;

	/** Overall theme for the location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	ELocationTag PrimaryTag = ELocationTag::Town;

	/** Probability (0-1) that a room contains a Mythos clue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MythosClueChance = 0.15f;

	/** Probability (0-1) that a room contains an encounter */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float EncounterChance = 0.2f;

	/** Base room size in Unreal units */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	FVector RoomBaseSize = FVector(600.f, 600.f, 300.f);

	FProceduralLocationParams() = default;
};

/**
 * AProceduralWorldGenerator
 *
 * Actor responsible for generating procedural game worlds for PtahStale.
 *
 * Generation algorithm (BSP-inspired room placement):
 *  1. Place the first room at the origin.
 *  2. Repeatedly pick a random existing room and carve a corridor to a
 *     new adjacent room until RoomCount is reached.
 *  3. Tag each room based on the PrimaryTag and a noise value.
 *  4. Scatter Mythos clues and encounters according to configured chances.
 *
 * Integration with UE5 PCG:
 *  This actor emits a list of FGeneratedRoom structs that downstream PCG
 *  graphs can query to spawn meshes, foliage, and encounter triggers.
 */
UCLASS(BlueprintType, Blueprintable)
class PTAHSTALE_API AProceduralWorldGenerator : public AActor
{
	GENERATED_BODY()

public:
	AProceduralWorldGenerator();

	// ── Configuration ─────────────────────────────────────────────────────

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	FProceduralLocationParams GenerationParams;

	// ── Generated Data ────────────────────────────────────────────────────

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generator")
	TArray<FGeneratedRoom> GeneratedRooms;

	// ── API ───────────────────────────────────────────────────────────────

	/**
	 * Run the procedural generation with the current GenerationParams.
	 * Clears any previously generated data first.
	 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Generator")
	void Generate();

	/** Clear all generated room data. */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Generator")
	void ClearGeneration();

	/** Return all rooms that match the given tag. */
	UFUNCTION(BlueprintPure, Category = "Generator")
	TArray<FGeneratedRoom> GetRoomsByTag(ELocationTag Tag) const;

	/** Return all rooms that have a Mythos clue. */
	UFUNCTION(BlueprintPure, Category = "Generator")
	TArray<FGeneratedRoom> GetMythosClueRooms() const;

protected:
	virtual void BeginPlay() override;

private:
	/** Attempt to place a new room adjacent to an existing one. Returns true on success. */
	bool TryPlaceRoom(FGeneratedRoom& OutRoom, const FProceduralLocationParams& Params);

	/** Assign content (clues, encounters) to a newly placed room. */
	void AssignRoomContent(FGeneratedRoom& Room, const FProceduralLocationParams& Params);

	/** Occupied grid cells (for collision avoidance). */
	TSet<FIntPoint> OccupiedCells;

	/** Current random stream (seeded per generation run). */
	FRandomStream RandomStream;
};
