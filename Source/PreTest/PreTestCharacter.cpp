// Copyright Epic Games, Inc. All Rights Reserved.

#include "PreTestCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/MainHUDWidget.h"
#include "Character/ProjectileManagementComponent.h"


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

	// Create Projectile Management Component
	ProjectileManagementComponent = CreateDefaultSubobject<UProjectileManagementComponent>(TEXT("ProjManageComp"));

	// init variable
	bFired = false;
	ActionKeyTypeStates.Init(false, static_cast<uint8>(EActionKeyType::Max));
	ActionKeyPressTimes.Init(0.0f, static_cast<uint8>(EActionKeyType::Max));

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

DECLARE_DELEGATE_TwoParams(FInputActionKey, EActionKeyType, bool);
#define BIND_ACTION_KEY(InputComponent, ActionName, InputEvent, Param1, Param2) \
	InputComponent->BindAction<FInputActionKey>(ActionName, InputEvent, this, &APreTestCharacter::UpdateActionKeyState, Param1, Param2);

void APreTestCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis("MoveRight", this, &APreTestCharacter::MoveRight);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &APreTestCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &APreTestCharacter::TouchStopped);

	BIND_ACTION_KEY(PlayerInputComponent, "ActionKey0", IE_Pressed, EActionKeyType::ActionKey0, true);
	BIND_ACTION_KEY(PlayerInputComponent, "ActionKey0", IE_Released, EActionKeyType::ActionKey0, false);
	BIND_ACTION_KEY(PlayerInputComponent, "ActionKey1", IE_Pressed, EActionKeyType::ActionKey1, true);
	BIND_ACTION_KEY(PlayerInputComponent, "ActionKey1", IE_Released, EActionKeyType::ActionKey1, false);
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

void APreTestCharacter::UpdateActionKeyState(EActionKeyType ActionKeyType, bool bPressedNew)
{
	if (!bFired)
	{
		switch (ActionKeyType)
		{
			case EActionKeyType::ActionKey0:
			{
				if (IsPressedAnyActionKey(ActionKeyType))
				{
					bFired = true;
					break;
				}

				uint8 Index = static_cast<uint8>(ActionKeyType);
				if (bPressedNew)
				{
					ActionKeyPressTimes[Index] = GetGameTimeSinceCreation();
				}
				else
				{
					float ReleaseTime = GetGameTimeSinceCreation();
					float TimeToPressed = ReleaseTime - ActionKeyPressTimes[Index];
					if (TimeToPressed < 3.0f)
					{
						CreateProjectile(EProjectileType::Normal);
						bFired = true;
					}
					else
					{
						CreateProjectile(EProjectileType::Charge);
						bFired = true;
					}

					ActionKeyPressTimes[Index] = 0.0f;
				}
			}
			break;

			case EActionKeyType::ActionKey1:
			{
				if (bPressedNew)
				{
					uint8 Index = static_cast<uint8>(EActionKeyType::ActionKey0);
					if (ActionKeyPressTimes[Index] <= 0.0f)
						break;

					float ReleaseTime = GetGameTimeSinceCreation();
					float TimeToPressed = ReleaseTime - ActionKeyPressTimes[Index];
					if (TimeToPressed <= 1.0f)
					{
						CreateProjectile(EProjectileType::Seperate);
						bFired = true;
					}
				}
				else
				{
					if (IsPressedAnyActionKey(ActionKeyType))
					{
						bFired = true;
						break;
					}

					CreateProjectile(EProjectileType::Reflect);
					bFired = true;
				}
			}
			break;
		}
	}

	// update key pressed state
	uint8 Index = static_cast<uint8>(ActionKeyType);
	ActionKeyTypeStates[Index] = bPressedNew;

	if (IsPressedAnyActionKey(EActionKeyType::Max))
		return;

	bFired = false;
}

bool APreTestCharacter::IsPressedAnyActionKey(EActionKeyType ActionKeyTypeToIgnore) const
{
	for (int32 Index = 0; Index < static_cast<uint8>(EActionKeyType::Max); ++Index)
	{
		if (Index != static_cast<uint8>(ActionKeyTypeToIgnore) && ActionKeyTypeStates[Index])
		{	
			return true;
		}
	}

	return false;
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
	if (nullptr == World || ProjectileType == EProjectileType::Max)
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

	if (ProjectileManagementComponent)
	{
		ProjectileManagementComponent->CreateProjectile(ProjectileType, Location, Rotation);
	}
}

