#include "CapturePoint.h"
#include "GridManager.h" // Здесь инклюд ПОЛНЫЙ, так как мы используем функции менеджера
#include "Kismet/GameplayStatics.h"

ACapturePoint::ACapturePoint()
{
    PrimaryActorTick.bCanEverTick = false;

    TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
    RootComponent = TriggerSphere;

    TriggerSphere->SetSphereRadius(150.0f);
    TriggerSphere->SetCollisionProfileName(TEXT("Trigger"));

    TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &ACapturePoint::OnPlayerEnter);
    TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &ACapturePoint::OnPlayerExit);

    CellIndex = 0;

    PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
    PlatformMesh->SetupAttachment(RootComponent);


    PlatformMesh->SetRelativeScale3D(FVector(3.0f, 3.0f, 0.1f)); // Делаем её широкой и тонкой
    PlatformMesh->SetRelativeLocation(FVector(0, 0, -75.0f)); // Опускаем чуть ниже центра сферы
}

void ACapturePoint::BeginPlay()
{
    Super::BeginPlay();

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridManager::StaticClass(), FoundActors);

    if (FoundActors.Num() > 0)
    {
        GridManagerRef = Cast<AGridManager>(FoundActors[0]);
        UE_LOG(LogTemp, Warning, TEXT("CapturePoint [%d]: Linked to GridManager"), CellIndex);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("CapturePoint [%d]: FAILED to find GridManager!"), CellIndex);
    }
}

void ACapturePoint::OnPlayerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    APawn* Pawn = Cast<APawn>(OtherActor);
    if (!Pawn || !Pawn->IsPlayerControlled()) return;

    // ВАЖНО: RPC можно вызывать только с АВТОНОМНОГО ПРОКСИ (клиент) или СЕРВЕРА.
    // Но лучше всего вызывать его всегда, а внутри GridManager уже разберемся.
    if (GridManagerRef)
    {
        UE_LOG(LogTemp, Warning, TEXT("Player entered Point %d. Sending RPC..."), CellIndex);

        // Вызываем RPC. Если мы на сервере - он выполнится сразу.
        // Если на клиенте - уйдет пакет на сервер.
        GridManagerRef->ServerSetCellState(CellIndex, ECellState::TeamX);

    };
}


void ACapturePoint::OnPlayerExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    APawn* Pawn = Cast<APawn>(OtherActor);
    if (Pawn && Pawn->IsPlayerControlled())
    {
        UE_LOG(LogTemp, Warning, TEXT("Player left CapturePoint %d"), CellIndex);
    }
}