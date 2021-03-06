// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Projectile/BaseProjectile.h"
#include "PreTestCharacter.generated.h"


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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PreTest", meta = (AllowPrivateAccess = "true"))
	class UActionKeyManagementComponent* ActionKeyManagementComponent;

public:
	APreTestCharacter();

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	virtual void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	/** Called for side to side input */
	void MoveRight(float Val);

	/** Handle touch inputs. */
	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Handle touch stop event. */
	void TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location);
	
	UFUNCTION()
	void CreateProjectile(const EProjectileType& ProjectileType);

	void OpenHUDWidget();
	void CloseHUDWidget();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	UPROPERTY(EditAnywhere, Category = "PreTest")
	TSubclassOf<class UUserWidget> MainHUDWidgetClass;

private:
	class UUserWidget* MainHUDWidget;

};
