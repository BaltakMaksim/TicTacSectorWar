#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SectorModule.h"
#include "CellTypes.h"
//#include "SectorModule.h"
#include "GridManager.generated.h"

// Объявляем делегат для события победы (чтобы GameMode мог подписаться)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameWon, ECellState, WinnerTeam, TArray<int32>, WinningIndices);


UCLASS()
class TICTACSECTORWAR_API AGridManager : public AActor
{
    GENERATED_BODY()

public:
    AGridManager();
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    FTimerHandle GateSwitchTimerHandle;

    // Функция переключения фаз ворот
    void SwitchGatesPhase();

    // Функция обновления всех модулей
    UFUNCTION(BlueprintCallable, Category = "Gates")
    void UpdateAllGates(int32 OpenIndex);

    // --- Логика (твой код) ---
    UFUNCTION(BlueprintPure, Category = "Grid")
    ECellState GetCellState(int32 CellIndex) const;

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerSetCellState(int32 CellIndex, ECellState NewState);

    UFUNCTION(BlueprintCallable, Category = "Grid")
    void CaptureCellFromServer(int32 CellIndex, ECellState NewState);

    // --- Настройки и Визуал ---

    // Размер поля (4x4)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
    int32 GridSize = 4;

    // Расстояние между центрами комнат (например, 1000 см)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
    float CellSpacing = 4000.0f;

    // !!! ВОТ ТУТ УКАЗЫВАЕМ МОДУЛЬ !!!
    // Назначь сюда BP_SectorModule в редакторе
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
    TSubclassOf<AActor> SectorModuleClass;

    UPROPERTY(BlueprintAssignable, Category = "Game Events")
    FOnGameWon OnGameWon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
    TSubclassOf<AActor> CorridorClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
    TSubclassOf<AActor> WallsClass;

private:
    // Реплицируемый массив состояний
    UPROPERTY(ReplicatedUsing = OnRep_CellStates)
    TArray<ECellState> CellStates;

    // Callback на клиентах
    UFUNCTION()
    void OnRep_CellStates();

    // Валидация и реализация RPC
    bool ServerSetCellState_Validate(int32 CellIndex, ECellState NewState);
    void ServerSetCellState_Implementation(int32 CellIndex, ECellState NewState);

    void UpdateCellVisual(int32 CellIndex);

    void CheckWinCondition(int32 LastIndex);


    // Массив ссылок на все модули (если у тебя его нет, нужно создать)
    UPROPERTY()
    TArray<ASectorModule*> AllModules;

};