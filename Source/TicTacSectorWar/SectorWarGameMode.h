#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CellTypes.h" 
#include "SectorWarGameMode.generated.h"

class AGridManager;
/**
 * 2. Основной класс правил игры
 */
UCLASS()
class TICTACSECTORWAR_API ASectorWarGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ASectorWarGameMode();

    // Вызывается при старте матча на сервере
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

    // 3. Функция захвата клетки (будет вызываться через RPC)
    // CellIndex: индекс клетки от 0 до 15 (для поля 4x4)
    // Capturer: контроллер игрока, который пытается захватить
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerCaptureCell(int32 CellIndex, APlayerController* Capturer);

    // Вспомогательная функция для проверки валидности RPC
    bool ServerCaptureCell_Validate(int32 CellIndex, APlayerController* Capturer);

    // Реализация логики на сервере
    void ServerCaptureCell_Implementation(int32 CellIndex, APlayerController* Capturer);

protected:
    // Массив состояний всех 16 клеток
    // ReplicatedUsing означает: "Когда это меняется на сервере, обнови у всех клиентов"
    UPROPERTY(ReplicatedUsing = OnRep_CellStates)
    TArray<ECellState> CellStates;

    // Таймер фазы игры
    FTimerHandle PhaseTimerHandle;

private:
    // Функция, которая срабатывает на клиентах при обновлении массива
    UFUNCTION()
    void OnRep_CellStates();

    // Логика смены фаз (открыть/закрыть двери)
    void HandlePhaseChange();

private:
    // Ссылка на созданный GridManager
    AGridManager* ActiveGridManager;
};