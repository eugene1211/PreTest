// Copyright Epic Games, Inc. All Rights Reserved.

#include "PreTestCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainHUDWidget.h"


APreTestCharacter::APreTestCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Rotation of the character should not affect rotation of boom
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->SocketOffset = FVector(0.f,0.f,75.f);
	CameraBoom->SetRelativeRotation(FRotator(0.f,180.f,0.f));

	// Create a camera and attach to boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	SideViewCameraComponent->bUsePawnControlRotation = false; // We don't want the controller rotating the camera

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Face in the direction we are moving..
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxFlySpeed = 600.f;

	PressQTime = 0.0f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void APreTestCharacter::BeginPlay()
{
	Super::BeginPlay();

	OpenHUDWidget();
}

void APreTestCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CloseHUDWidget();

	Super::EndPlay(EndPlayReason);
}

//////////////////////////////////////////////////////////////////////////
// Input

void APreTestCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis("MoveRight", this, &APreTestCharacter::MoveRight);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &APreTestCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &APreTestCharacter::TouchStopped);

	PlayerInputComponent->BindAction("Action0", IE_Pressed, this, &APreTestCharacter::ActionQPreesed);
	PlayerInputComponent->BindAction("Action0", IE_Released, this, &APreTestCharacter::ActionQReleased);

	PlayerInputComponent->BindAction("Action1", IE_Pressed, this, &APreTestCharacter::ActionWPreesed);
	PlayerInputComponent->BindAction("Action1", IE_Released, this, &APreTestCharacter::ActionWReleased);
}

void APreTestCharacter::MoveRight(float Value)
{
	// add movement in that direction
	AddMovementInput(FVector(0.f,-1.f,0.f), Value);
}

void APreTestCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// jump on any touch
	Jump();
}

void APreTestCharacter::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	StopJumping();
}

void APreTestCharacter::ActionQPreesed()
{
	PressQTime = GetGameTimeSinceCreation();
	bWPressedPossible = true;
}

void APreTestCharacter::ActionQReleased()
{
	float ReleaseTime = GetGameTimeSinceCreation();
	float TimeToPressed = ReleaseTime - PressQTime;
	if (TimeToPressed < 3.0f)
		CreateProjectile(EProjectileType::Normal);
	else
		CreateProjectile(EProjectileType::Charge);

	PressQTime = 0.0f;
}

void APreTestCharacter::ActionWPreesed()
{
	if (PressQTime <= 0.0f || !bWPressedPossible)
		return;

	bWPressedPossible = false;

	float ReleaseTime = GetGameTimeSinceCreation();
	float TimeToPressed = ReleaseTime - PressQTime;
	if (TimeToPressed <= 1.0f)
	{
		CreateProjectile(EProjectileType::Seperate);
	}
}

void APreTestCharacter::ActionWReleased()
{
	CreateProjectile(EProjectileType::Reflect);
}

void APreTestCharacter::OpenHUDWidget()
{
	if (nullptr == MainHUDWidget)
	{
		UUserWidget* NewWidget = CreateWidget(GetGameInstance(), MainHUDWidgetClass);
		MainHUDWidget = Cast<UMainHUDWidget>(NewWidget);
	}

	if (MainHUDWidget)
		MainHUDWidget->AddToViewport();
}

void APreTestCharacter::CloseHUDWidget()
{
	if (nullptr != MainHUDWidget && MainHUDWidget->IsInViewport())
		MainHUDWidget->RemoveFromViewport();
}

void APreTestCharacter::CreateProjectile(EProjectileType ProjectileType)
{
	UWorld* World = GetWorld();
	if (nullptr == World)
		return;

	FVector Location;
	FRotator Rotation;
	TArray<UActorComponent*> StartPointComponents = GetComponentsByTag(UActorComponent::StaticClass(), "ProjectileStartPoint");
	if (StartPointComponents.IsValidIndex(0))
	{
		USceneComponent* StartPointComponent = Cast<USceneComponent>(StartPointComponents[0]);
		if (StartPointComponent)
		{
			Location = StartPointComponent->GetComponentLocation();
			Rotation = StartPointComponent->GetComponentRotation();
		}
	}

	CreateProjectileImpl(ProjectileType, Location, Rotation);
}

void APreTestCharacter::CreateProjectileImpl(EProjectileType ProjectileType, const FVector& Location, const FRotator& Rotation)
{
	UWorld* World = GetWorld();
	if (nullptr == World)
		return;

	TSubclassOf<ABaseProjectile>* SubClass = ProjectileClassList.Find(ProjectileType);
	if (!SubClass)
		return;

	ABaseProjectile* NewProjectile = World->SpawnActor<ABaseProjectile>(SubClass->Get(), Location, Rotation);
	if (NewProjectile && ProjectileType == EProjectileType::Seperate)
	{
		NewProjectile->OnEndPlay.AddDynamic(this, &APreTestCharacter::EndPlayForSperate);
	}
}

void APreTestCharacter::EndPlayForSperate(AActor* Actor, EEndPlayReason::Type EndPlayReason)
{
	if (!Actor)
		return;

	ABaseProjectile* Projectile = Cast<ABaseProjectile>(Actor);
	if (!Projectile || Projectile->IsDestroyByHit())
		return;

	Projectile->ForEachComponent<UArrowComponent>(false, [&](const UArrowComponent* InArrowComp)
	{
		if (!InArrowComp)
			return;

		FVector Location = InArrowComp->GetComponentLocation();
		FRotator Rotation = InArrowComp->GetComponentRotation();
		float OriginPitch = Rotation.Pitch;

		CreateProjectileImpl(EProjectileType::Normal, Location, Rotation);

		Rotation.Pitch = OriginPitch - 45.0f;
		CreateProjectileImpl(EProjectileType::Normal, Location, Rotation);

		Rotation.Pitch = OriginPitch + 45.0f;
		CreateProjectileImpl(EProjectileType::Normal, Location, Rotation);
	});
}
