// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. Настройка размеров капсулы (стандарт для человека ~86x180 или меньше для FPS)
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// 2. Настройка скорости бега и прыжка
	GetCharacterMovement()->bOrientRotationToMovement = false; // Характер не поворачивается носом по движению (важно для FPS!)
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// 3. Создаем камеру
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FPSCameraComponent->SetupAttachment(GetCapsuleComponent());
	FPSCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Позиция глаз
	FPSCameraComponent->bUsePawnControlRotation = true; // Камера вращается мышкой

	// 4. Создаем меш рук (опционально, если хочешь видеть руки)
	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FPSMesh->SetOnlyOwnerSee(false); // Только владелец видит руки
	FPSMesh->SetupAttachment(FPSCameraComponent);
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;
	FPSMesh->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	FPSMesh->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Скрываем обычный меш тела, чтобы не видеть его изнутри головы
	GetMesh()->SetOwnerNoSee(false);

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();

	// 1. Скорость бега (стандарт 600, ставим 1200 для супер-скорости)
	MoveComp->MaxWalkSpeed = 2000.0f;

	// 2. Скорость в воздухе (чтобы не замедлялся в прыжке)
	MoveComp->AirControl = 1.0f;

	// 3. Высота прыжка (стандарт 420, ставим 800-1000 для высокого прыжка)
	MoveComp->JumpZVelocity = 700.0f;

	// 4. Гравитация (опционально: 980 - стандарт. 
	// Если поставить меньше (например 600), прыжок будет более "лунным" и долгим)
	MoveComp->GravityScale = 2.0f;

}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{


	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Биндинг осей движения (WASD)
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);

	// Биндинг поворота камеры (Мышь)
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// Биндинг прыжка (Пробел)
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
}

void  AMyCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// Двигаемся вперед относительно направления взгляда контроллера
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMyCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// Двигаемся вправо относительно направления взгляда контроллера
		AddMovementInput(GetActorRightVector(), Value);
	}
}


