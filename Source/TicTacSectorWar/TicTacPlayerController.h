#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TicTacPlayerController.generated.h"

UCLASS()
class TICTACSECTORWAR_API ATicTacPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ATicTacPlayerController();

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

private:
    void OnLeftMouseButtonPressed();
    void HandleGridInteraction(const FHitResult& Hit);
};