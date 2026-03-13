#include "ProceduralWorldGenerator.h"

AProceduralWorldGenerator::AProceduralWorldGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AProceduralWorldGenerator::BeginPlay()
{
	Super::BeginPlay();

	// Auto-generate on BeginPlay so levels can be tested in PIE immediately.
	if (GeneratedRooms.IsEmpty())
	{
		Generate();
	}
}

void AProceduralWorldGenerator::Generate()
{
	ClearGeneration();

	// Seed the random stream
	const int32 ActualSeed = (GenerationParams.Seed == 0)
		? FMath::Rand()
		: GenerationParams.Seed;
	RandomStream = FRandomStream(ActualSeed);

	// Place the first room at the origin
	{
		FGeneratedRoom StartRoom;
		StartRoom.GridPosition  = FIntPoint(0, 0);
		StartRoom.WorldPosition = FVector::ZeroVector;
		StartRoom.HalfExtents   = GenerationParams.RoomBaseSize * 0.5f;
		StartRoom.Tag           = GenerationParams.PrimaryTag;
		AssignRoomContent(StartRoom, GenerationParams);
		GeneratedRooms.Add(StartRoom);
		OccupiedCells.Add(StartRoom.GridPosition);
	}

	// Iteratively attach new rooms
	int32 Attempts = 0;
	const int32 MaxAttempts = GenerationParams.RoomCount * 10;

	while (GeneratedRooms.Num() < GenerationParams.RoomCount && Attempts < MaxAttempts)
	{
		FGeneratedRoom NewRoom;
		if (TryPlaceRoom(NewRoom, GenerationParams))
		{
			AssignRoomContent(NewRoom, GenerationParams);
			GeneratedRooms.Add(NewRoom);
			OccupiedCells.Add(NewRoom.GridPosition);
		}
		++Attempts;
	}
}

void AProceduralWorldGenerator::ClearGeneration()
{
	GeneratedRooms.Empty();
	OccupiedCells.Empty();
}

bool AProceduralWorldGenerator::TryPlaceRoom(FGeneratedRoom& OutRoom, const FProceduralLocationParams& Params)
{
	if (GeneratedRooms.IsEmpty())
	{
		return false;
	}

	// Pick a random existing room to branch from
	const int32 ParentIndex = RandomStream.RandRange(0, GeneratedRooms.Num() - 1);
	const FGeneratedRoom& Parent = GeneratedRooms[ParentIndex];

	// Pick a random cardinal direction
	static const FIntPoint Directions[4] = {
		FIntPoint( 1,  0),
		FIntPoint(-1,  0),
		FIntPoint( 0,  1),
		FIntPoint( 0, -1)
	};
	const FIntPoint Dir = Directions[RandomStream.RandRange(0, 3)];
	const FIntPoint NewGrid = Parent.GridPosition + Dir;

	// Reject if cell is occupied
	if (OccupiedCells.Contains(NewGrid))
	{
		return false;
	}

	// Compute world position: step by full room diameter
	const FVector Step = FVector(
		Dir.X * Params.RoomBaseSize.X,
		Dir.Y * Params.RoomBaseSize.Y,
		0.f
	);

	OutRoom.GridPosition  = NewGrid;
	OutRoom.WorldPosition = Parent.WorldPosition + Step;
	OutRoom.HalfExtents   = Params.RoomBaseSize * 0.5f;

	// Vary the tag: most rooms match the primary tag; a small percentage are "wrong"
	const float TagRoll = RandomStream.GetFraction();
	if (TagRoll < 0.05f)
	{
		OutRoom.Tag = ELocationTag::DeeplyWrong;
	}
	else if (TagRoll < 0.15f)
	{
		// Rotate through secondary tags
		const int32 SecondaryTagIndex = RandomStream.RandRange(1, 6); // skip None (0) and DeeplyWrong (7)
		OutRoom.Tag = static_cast<ELocationTag>(SecondaryTagIndex);
	}
	else
	{
		OutRoom.Tag = Params.PrimaryTag;
	}

	return true;
}

void AProceduralWorldGenerator::AssignRoomContent(FGeneratedRoom& Room, const FProceduralLocationParams& Params)
{
	Room.bHasMythosClue = RandomStream.GetFraction() < Params.MythosClueChance;
	Room.bHasEncounter  = RandomStream.GetFraction() < Params.EncounterChance;

	// Deeply wrong rooms always have a Mythos clue
	if (Room.Tag == ELocationTag::DeeplyWrong)
	{
		Room.bHasMythosClue = true;
	}
}

TArray<FGeneratedRoom> AProceduralWorldGenerator::GetRoomsByTag(ELocationTag Tag) const
{
	TArray<FGeneratedRoom> Result;
	for (const FGeneratedRoom& Room : GeneratedRooms)
	{
		if (Room.Tag == Tag)
		{
			Result.Add(Room);
		}
	}
	return Result;
}

TArray<FGeneratedRoom> AProceduralWorldGenerator::GetMythosClueRooms() const
{
	TArray<FGeneratedRoom> Result;
	for (const FGeneratedRoom& Room : GeneratedRooms)
	{
		if (Room.bHasMythosClue)
		{
			Result.Add(Room);
		}
	}
	return Result;
}
