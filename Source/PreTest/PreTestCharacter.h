// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Projectile/BaseProjectile.h"
#include "PreTestCharacter.generated.h"

UENUM()
enum class EActionKeyType : uint8
{
	ActionKey0,
	ActionKey1,
	Max,
};

UCLASS(config=Game)
class APreTestCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PreTest", meta = (AllowPrivateAccess = "true"))
	class UProjectileManagementComponent* ProjectileManagementComponent;

public:
	APreTestCharacter();

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	virtual void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	float GetActionKeyPressTime(const EActionKeyType& ActionKeyType) const;
	float GetActionKeyPressedTime(const EActionKeyType& ActionKeyType) const;

	float GetAction2HoldTime() const { return Action2HoldTime; }

	DECLARE_EVENT_TwoParams(APreTestCharacter, FOnKeyPressedTimeChanged, const EActionKeyType&, const float&);
	FOnKeyPressedTimeChanged OnActionKeyPressTimeChanged;

protected:
	/** Called for side to side input */
	void MoveRight(float Val);

	/** Handle touch inputs. */
	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Handle touch stop event. */
	void TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location);
	 
	void UpdateActionKeyState(EActionKeyType ActionKeyType, bool bPressed);
	bool IsPressedAnyActionKey(EActionKeyType ActionKeyTypeToIgnore) const;

	void OpenHUDWidget();
	void CloseHUDWidget();

	void CreateProjectile(EProjectileType ProjectileType);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	UPROPERTY(EditAnywhere, Category = "PreTest")
	TSubclassOf<class UUserWidget> MainHUDWidgetClass;

	UPROPERTY(EditAnywhere, Category = "PreTest")
	float Action2HoldTime;

	UPROPERTY(EditAnywhere, Category = "PreTest")
	float Action3HoldTime;

private:
	void SetActionKeyPressTime(EActionKeyType ActionKeyType, float Time);

	class UUserWidget* MainHUDWidget;

	bool bBlockActionKey;
	TArray<bool> ActionKeyTypeStates;
	TArray<float> ActionKeyPressTimes;

};
