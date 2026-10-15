#include "GridManager.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AGridManager::AGridManager()
{
    PrimaryActorTick.bCanEverTick = false;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void AGridManager::BeginPlay()
{
    Super::BeginPlay();

    // Инициализация массива состояний
    if (CellStates.Num() == 0)
    {
        for (int32 i = 0; i < GridSize * GridSize; ++i)
        {
            CellStates.Add(ECellState::Neutral);
        }
    }

    // Спавн модулей секторов
    if (SectorModuleClass)
    {
        float Offset = (GridSize - 1) * CellSpacing * 0.5f;

        for (int32 Row = 0; Row < GridSize; ++Row)
        {
            for (int32 Col = 0; Col < GridSize; ++Col)
            {
                int32 CellIndex = Row * GridSize + Col;
                FVector Loc(Col * CellSpacing - Offset, Row * CellSpacing - Offset, 50.0f);

                FActorSpawnParameters SpawnParams;
                SpawnParams.Owner = this;

                AActor* NewModule = GetWorld()->SpawnActor<AActor>(SectorModuleClass, Loc, FRotator::ZeroRotator, SpawnParams);

                if (NewModule)
                {
                    UFunction* InitFunc = NewModule->FindFunction(FName("InitializeCell"));
                    if (InitFunc)
                    {
                        struct FInitializeCellParams { int32 NewIndex; } Params;
                        Params.NewIndex = CellIndex;
                        NewModule->ProcessEvent(InitFunc, &Params);
                    }
                }
            }
        }
        UE_LOG(LogTemp, Warning, TEXT("Grid generated with %d sector modules"), GridSize * GridSize);
    }

    if (CorridorClass && GridSize > 1)
    {
        float Offset = (GridSize - 1) * CellSpacing * 0.5f;

        for (int32 Row = 0; Row < GridSize; ++Row)
        {
            for (int32 Col = 0; Col < GridSize; ++Col)
            {
                FVector RoomLoc(Col * CellSpacing - Offset, Row * CellSpacing - Offset, 50.0f);

                // --- ВНУТРЕННИЕ КОРИДОРЫ (Между комнатами) ---

                // Вправо
                if (Col < GridSize - 1)
                {
                    FVector Loc = RoomLoc + FVector(CellSpacing * 0.5f, 0, 0);
                    GetWorld()->SpawnActor<AActor>(CorridorClass, Loc, FRotator(0, 90, 0));
                }
                // Вверх
                if (Row < GridSize - 1)
                {
                    FVector Loc = RoomLoc + FVector(0, CellSpacing * 0.5f, 0);
                    GetWorld()->SpawnActor<AActor>(CorridorClass, Loc, FRotator(0, 0, 0));
                }

                // --- ВНЕШНИЕ КОРИДОРЫ (По краям карты) ---

                // 1. ЛЕВЫЙ КРАЙ (Col == 0)
                if (Col == 0)
                {
                    // Спавним слева от первой колонки
                    FVector Loc = RoomLoc + FVector(-CellSpacing * 0.5f, 0, 0);
                    GetWorld()->SpawnActor<AActor>(CorridorClass, Loc, FRotator(0, 90, 0));
                }

                // 2. ПРАВЫЙ КРАЙ (Col == GridSize - 1)
                if (Col == GridSize - 1)
                {
                    // Спавним справа от последней колонки
                    FVector Loc = RoomLoc + FVector(CellSpacing * 0.5f, 0, 0);
                    GetWorld()->SpawnActor<AActor>(CorridorClass, Loc, FRotator(0, 90, 0));
                }

                // 3. НИЖНИЙ КРАЙ (Row == 0)
                if (Row == 0)
                {
                    // Спавним снизу от первого ряда
                    FVector Loc = RoomLoc + FVector(0, -CellSpacing * 0.5f, 0);
                    GetWorld()->SpawnActor<AActor>(CorridorClass, Loc, FRotator(0, 0, 0));
                }

                // 4. ВЕРХНИЙ КРАЙ (Row == GridSize - 1)
                if (Row == GridSize - 1)
                {
                    // Спавним сверху от последнего ряда
                    FVector Loc = RoomLoc + FVector(0, CellSpacing * 0.5f, 0);
                    GetWorld()->SpawnActor<AActor>(CorridorClass, Loc, FRotator(0, 0, 0));
                }
                // --- ВНЕШНИЕ СТЕНЫ (По самому периметру) ---
// Важно: WallsClass должен быть назначен в редакторе так же, как CorridorClass

                if (WallsClass)
                {
                    // Смещение стены относительно центра комнаты. 
                    // Если CellSpacing=4000, то стена должна стоять примерно на расстоянии 2000+толщина_стены
                    float WallOffset = CellSpacing * 0.5f + 100.0f; // +100 это примерная толщина/отступ стены

                    // 1. ЛЕВАЯ СТЕНА (Col == 0)
                    if (Col == 0)
                    {
                        FVector Loc = RoomLoc + FVector(-WallOffset, 0, 0);
                        FRotator Rot(0, 90, 0); // Поворот, чтобы стена смотрела внутрь
                        GetWorld()->SpawnActor<AActor>(WallsClass, Loc, Rot);
                    }

                    // 2. ПРАВАЯ СТЕНА (Col == GridSize - 1)
                    if (Col == GridSize - 1)
                    {
                        FVector Loc = RoomLoc + FVector(WallOffset, 0, 0);
                        FRotator Rot(0, -90, 0);
                        GetWorld()->SpawnActor<AActor>(WallsClass, Loc, Rot);
                    }

                    // 3. НИЖНЯЯ СТЕНА (Row == 0)
                    if (Row == 0)
                    {
                        FVector Loc = RoomLoc + FVector(0, -WallOffset, 0);
                        FRotator Rot(0, 0, 0);
                        GetWorld()->SpawnActor<AActor>(WallsClass, Loc, Rot);
                    }

                    // 4. ВЕРХНЯЯ СТЕНА (Row == GridSize - 1)
                    if (Row == GridSize - 1)
                    {
                        FVector Loc = RoomLoc + FVector(0, WallOffset, 0);
                        FRotator Rot(0, 180, 0);
                        GetWorld()->SpawnActor<AActor>(WallsClass, Loc, Rot);
                    }
                }
            }
        }
    }

    if (HasAuthority())
    {
        // 1. ЗАПОЛНЯЕМ МАССИВС ВСЕМИ КОМНАТАМИ НА УРОВНЕ
        // Ищем всех акторов типа ASectorModule
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASectorModule::StaticClass(), FoundActors);

        // Очищаем старый список (на всякий случай)
        AllModules.Empty();

        // Переносим найденные акторы в наш типизированный массив
        for (AActor* Actor : FoundActors)
        {
            ASectorModule* Module = Cast<ASectorModule>(Actor);
            if (Module)
            {
                AllModules.Add(Module);
            }
        }


        // Таймер на 5 минут (300 сек). Для теста поставь 10.0f
        GetWorldTimerManager().SetTimer(GateSwitchTimerHandle, this, &AGridManager::SwitchGatesPhase, 60.0f, true);

        // Сразу выбираем первую случайную сторону
        SwitchGatesPhase();
    }
}

void AGridManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AGridManager, CellStates);
}

ECellState AGridManager::GetCellState(int32 CellIndex) const
{
    if (CellStates.IsValidIndex(CellIndex))
        return CellStates[CellIndex];
    return ECellState::Neutral;
}

bool AGridManager::ServerSetCellState_Validate(int32 CellIndex, ECellState NewState)
{
    return CellStates.IsValidIndex(CellIndex) && NewState != ECellState::Neutral;
}

void AGridManager::ServerSetCellState_Implementation(int32 CellIndex, ECellState NewState)
{
    CaptureCellFromServer(CellIndex, NewState);
}

void AGridManager::CaptureCellFromServer(int32 CellIndex, ECellState NewState)
{
    if (GetNetMode() == NM_Client)
    {
        UE_LOG(LogTemp, Error, TEXT("SECURITY: CaptureCellFromServer called on CLIENT!"));
        return;
    }

    if (CellStates.IsValidIndex(CellIndex) && CellStates[CellIndex] == ECellState::Neutral)
    {
        CellStates[CellIndex] = NewState;
        UpdateCellVisual(CellIndex);
        CheckWinCondition(CellIndex);
    }
}

void AGridManager::OnRep_CellStates()
{
    for (int32 i = 0; i < CellStates.Num(); ++i)
    {
        UpdateCellVisual(i);
    }
}

void AGridManager::UpdateCellVisual(int32 CellIndex)
{
    // Заглушка для визуального обновления
    UE_LOG(LogTemp, Log, TEXT("Visual update for cell %d to state %d"), CellIndex, (uint8)CellStates[CellIndex]);
}

void AGridManager::CheckWinCondition(int32 LastIndex)
{
    const int32 Size = GridSize; // Используем переменную класса, а не хардкод!
    ECellState LastState = CellStates[LastIndex];

    if (LastState == ECellState::Neutral) return;

    int32 Row = LastIndex / Size;
    int32 Col = LastIndex % Size;

    int Directions[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };

    for (int d = 0; d < 4; ++d)
    {
        int dr = Directions[d][0];
        int dc = Directions[d][1];
        int Count = 1;
        TArray<int32> LineIndices;
        LineIndices.Add(LastIndex);

        // Проверка в одну сторону
        for (int i = 1; i < Size; ++i)
        {
            int r = Row + dr * i;
            int c = Col + dc * i;
            if (r >= 0 && r < Size && c >= 0 && c < Size)
            {
                int Idx = r * Size + c;
                if (CellStates[Idx] == LastState)
                {
                    Count++;
                    LineIndices.Add(Idx);
                }
                else break;
            }
        }

        // Проверка в другую сторону
        for (int i = 1; i < Size; ++i)
        {
            int r = Row - dr * i;
            int c = Col - dc * i;
            if (r >= 0 && r < Size && c >= 0 && c < Size)
            {
                int Idx = r * Size + c;
                if (CellStates[Idx] == LastState)
                {
                    Count++;
                    LineIndices.Add(Idx);
                }
                else break;
            }
        }

        if (Count >= Size) // Победа при заполнении всей линии размера GridSize
        {
            UE_LOG(LogTemp, Warning, TEXT("GAME WON by Team %d!"), (uint8)LastState);
            if (OnGameWon.IsBound())
            {
                OnGameWon.Broadcast(LastState, LineIndices);
            }
            return;
        }
    }
}

void AGridManager::SwitchGatesPhase()
{
    if (!HasAuthority()) return;

    // Генерируем случайный индекс от 0 до 3
    int32 RandomIndex = UKismetMathLibrary::RandomIntegerInRange(0, 3);


    UpdateAllGates(RandomIndex);
}

void AGridManager::UpdateAllGates(int32 OpenIndex)
{
    for (ASectorModule* Module : AllModules)
    {
        if (Module)
        {
            UE_LOG(LogTemp, Warning, TEXT("RANDOM GATE SWITCHED! New Index: %d"), 1);
            Module->SetGatesStateByIndex(OpenIndex);
        }
    }
}