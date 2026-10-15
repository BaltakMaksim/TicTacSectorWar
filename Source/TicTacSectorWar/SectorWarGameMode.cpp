#include "SectorWarGameMode.h"
#include "Net/UnrealNetwork.h" // Обязательно для репликации
#include "GridManager.h"
#include "TicTacPlayerController.h"
#include "MyCharacter.h"

ASectorWarGameMode::ASectorWarGameMode()
{
    PlayerControllerClass = ATicTacPlayerController::StaticClass();
    DefaultPawnClass = AMyCharacter::StaticClass();
    // Инициализируем массив 16 нейтральными клетками
    for (int32 i = 0; i < 16; ++i)
    {
        CellStates.Add(ECellState::Neutral);
    }
}

// Эта функция говорит движку: "Эту переменную нужно передавать по сети"
void ASectorWarGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ASectorWarGameMode, CellStates);
}

void ASectorWarGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);

    // Проверяем, что мы на сервере (GameMode живет только там)
    if (GetNetMode() == NM_Standalone || GetNetMode() == NM_ListenServer || GetNetMode() == NM_DedicatedServer)
    {
        UE_LOG(LogTemp, Warning, TEXT("SERVER: Game Initialized! Starting Phase Timer..."));

        // Запускаем таймер смены фаз (60 сек для теста)
        GetWorldTimerManager().SetTimer(PhaseTimerHandle, this, &ASectorWarGameMode::HandlePhaseChange, 60.0f, true);

        // --- СПАВН GRID MANAGER ---
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this; // Владелец - этот GameMode

        // Создаем менеджер сетки в центре мира (0,0,0)
        ActiveGridManager = GetWorld()->SpawnActor<AGridManager>(
            AGridManager::StaticClass(),
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            SpawnParams
        );

        if (ActiveGridManager)
        {
            UE_LOG(LogTemp, Warning, TEXT("SERVER: GridManager spawned successfully!"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("SERVER: FAILED to spawn GridManager! Check logs."));
        }
    }
}
// --- Логика захвата ---

bool ASectorWarGameMode::ServerCaptureCell_Validate(int32 CellIndex, APlayerController* Capturer)
{
    // Проверка: индекс должен быть от 0 до 15
    return CellIndex >= 0 && CellIndex < 16;
}

void ASectorWarGameMode::ServerCaptureCell_Implementation(int32 CellIndex, APlayerController* Capturer)
{
    // Если клетка свободна - захватываем её (пока ставим TeamX для теста)
    if (CellStates[CellIndex] == ECellState::Neutral)
    {
        CellStates[CellIndex] = ECellState::TeamX;
        UE_LOG(LogTemp, Warning, TEXT("Server: Cell %d captured by Team X"), CellIndex);
    }
}

void ASectorWarGameMode::OnRep_CellStates()
{
    // Этот код выполняется на КЛИЕНТЕ, когда сервер прислал новые данные
    UE_LOG(LogTemp, Warning, TEXT("Client: Received grid update!"));
    // Здесь позже будет код обновления UI карты
}

void ASectorWarGameMode::HandlePhaseChange()
{
    // Здесь будет логика открытия/закрытия дверей
    UE_LOG(LogTemp, Warning, TEXT("Server: Phase Changed! Doors updating..."));
}