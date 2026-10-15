#include "SectorModule.h"

ASectorModule::ASectorModule()
{
    PrimaryActorTick.bCanEverTick = false; // Нам не нужен тик для базовой логики
}

void ASectorModule::SetGatesStateByIndex_Implementation(int32 OpenGateIndex)
{

}