#include "TicTacPlayerController.h"
#include "GridManager.h" 
#include "Components/InstancedStaticMeshComponent.h" // Добавь это для Cast
#include "Engine/LocalPlayer.h"
#include "Blueprint/UserWidget.h"

ATicTacPlayerController::ATicTacPlayerController()
{
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;
}

void ATicTacPlayerController::BeginPlay()
{
    Super::BeginPlay();
    SetInputMode(FInputModeGameAndUI());

    // Показываем виджет только локальному игроку
    if (IsLocalPlayerController())
    {
        // Путь к твоему виджету. Проверь его в Content Browser!
        // Обычно это: /Game/UI/WBP_MatchTimer.WBP_MatchTimer_C
        TSubclassOf<UUserWidget> WidgetClass = LoadClass<UUserWidget>(
            nullptr,
            TEXT("/Game/UI/WBP_Timer.WBP_Timer_C")
        );

        if (WidgetClass)
        {
            // Создаем виджет
            UUserWidget* TimerWidget = CreateWidget<UUserWidget>(this, WidgetClass);

            if (TimerWidget)
            {
                // Добавляем на экран (ZOrder 100, чтобы был поверх всего)
                TimerWidget->AddToViewport(100);

                UE_LOG(LogTemp, Warning, TEXT("Timer Widget Created Successfully!"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to load WBP_MatchTimer class! Check path."));
        }
    }
}

void ATicTacPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    InputComponent->BindAction("LeftMouseButton", IE_Pressed, this, &ATicTacPlayerController::OnLeftMouseButtonPressed);
}

void ATicTacPlayerController::OnLeftMouseButtonPressed()
{
    if (!GetPawn()) return;

    FVector Start, Direction;
    // Получаем луч от камеры через мышь
    if (DeprojectMousePositionToWorld(Start, Direction))
    {
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(GetPawn());

        FHitResult Hit;
        // Пускаем луч на 5000 единиц
        bool bHit = GetWorld()->LineTraceSingleByChannel(
            Hit,
            Start,
            Start + (Direction * 5000.0f),
            ECC_Visibility,
            QueryParams
        );

        if (bHit)
        {
            HandleGridInteraction(Hit);
        }
    }
}

void ATicTacPlayerController::HandleGridInteraction(const FHitResult& Hit)
{
    // 1. Проверяем, что попали в Actor типа AGridManager
    AGridManager* GridManager = Cast<AGridManager>(Hit.GetActor());

    if (GridManager)
    {
        // 2. Получаем точку удара в мировых координатах
        FVector HitLocation = Hit.ImpactPoint;

        // 3. Получаем позицию самого GridManager (обычно это центр сетки или угол)
        FVector ManagerLocation = GridManager->GetActorLocation();

        // 4. Вычисляем смещение относительно менеджера
        FVector LocalHit = HitLocation - ManagerLocation;

        // 5. Рассчитываем Row и Col (предполагаем, что сетка лежит в XY平面)
        // CellSpacing должен быть таким же, как в GridManager (у нас 100.0f)
        float Spacing = 100.0f;

        // Округляем до ближайшего целого индекса
        // Добавляем половину размера сетки, чтобы компенсировать центрирование (Offset)
        int32 GridSize = 4; // Твой размер сетки
        float Offset = (GridSize - 1) * Spacing * 0.5f;

        int32 Col = FMath::RoundToInt((LocalHit.X + Offset) / Spacing);
        int32 Row = FMath::RoundToInt((LocalHit.Y + Offset) / Spacing);

        // 6. Проверка границ (чтобы не выйти за пределы массива)
        if (Col >= 0 && Col < GridSize && Row >= 0 && Row < GridSize)
        {
            int32 InstanceIndex = Row * GridSize + Col;

            UE_LOG(LogTemp, Warning, TEXT("Clicked Cell: Row=%d, Col=%d, Index=%d"), Row, Col, InstanceIndex);

            // 7. Вызываем RPC
            GridManager->ServerSetCellState(InstanceIndex, ECellState::TeamX);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Clicked outside grid bounds!"));
        }
    }
}