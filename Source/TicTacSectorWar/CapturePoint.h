#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "CapturePoint.generated.h"

// Предварительное объявление, чтобы избежать циклической зависимости
class AGridManager;

UCLASS()
class TICTACSECTORWAR_API ACapturePoint : public AActor
{
    GENERATED_BODY()

public:
    ACapturePoint();

protected:
    virtual void BeginPlay() override;

public:
    // Индекс клетки (0-15)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture")
    int32 CellIndex;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visuals")
    class UStaticMeshComponent* PlatformMesh;

    // Ссылка на менеджер (заполняется в BeginPlay)
    AGridManager* GridManagerRef;

    // Компонент сферы
    USphereComponent* TriggerSphere;

    // ОБЪЯВЛЕНИЯ функций (без кода внутри!)
    UFUNCTION()
    void OnPlayerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnPlayerExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};