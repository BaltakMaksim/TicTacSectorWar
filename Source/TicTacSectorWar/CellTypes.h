#pragma once

#include "CoreMinimal.h"
#include "CellTypes.generated.h"

UENUM(BlueprintType)
enum class ECellState : uint8
{
    Neutral     UMETA(DisplayName = "Neutral"),
    TeamX       UMETA(DisplayName = "Team X"),      // <-- Добавлена запятая
    TeamO       UMETA(DisplayName = "Team O"),      // <-- Добавлена запятая
    Contested   UMETA(DisplayName = "Contested")
};