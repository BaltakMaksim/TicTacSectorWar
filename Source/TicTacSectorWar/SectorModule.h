#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SectorModule.generated.h"

UCLASS()
class TICTACSECTORWAR_API ASectorModule : public AActor
{
    GENERATED_BODY()

public:
    // Конструктор
    ASectorModule();


    UFUNCTION(BlueprintNativeEvent, Category = "Gates")
    void SetGatesStateByIndex(int32 OpenGateIndex);
};