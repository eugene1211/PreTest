// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Projectile/BaseProjectile.h"
#include "ActionKeyManagementComponent.generated.h"

UENUM()
enum class EActionKeyType : uint8
{
	ActionKey0,
	ActionKey1,
	Max,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PRETEST_API UActionKeyManagementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActionKeyManagementComponent();

	void UpdateActionKeyState(EActionKeyType ActionKeyType, bool bPressedNew);

	float GetActionKeyPressTime(const EActionKeyType& ActionKeyType) const;
	float GetActionKeyPressedTime(const EActionKeyType& ActionKeyType) const;

	float GetActionHoldTime(const EProjectileType& ProjectileType) const;

	bool IsPressedAnyActionKey(EActionKeyType ActionKeyTypeToIgnore) const;

	DECLARE_EVENT_TwoParams(APreTestCharacter, FOnKeyPressedTimeChanged, const EActionKeyType&, const float&);
	FOnKeyPressedTimeChanged OnActionKeyPressTimeChanged;

	DECLARE_EVENT_OneParam(APreTestCharacter, FOnActionEnabled, const EProjectileType&);
	FOnActionEnabled OnActionEnabled;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "PreTest")
	TMap<EProjectileType, float> ProjectileActionKeyHoldTime;

private:
	void SetActionKeyPressTime(EActionKeyType ActionKeyType, float Time);
	void ExecuteActionIfPossible(EActionKeyType ActionKeyType, bool bPressedNew);

	bool bBlockActionKey;
	TArray<bool> ActionKeyTypeStates;
	TArray<float> ActionKeyPressTimes;

};
