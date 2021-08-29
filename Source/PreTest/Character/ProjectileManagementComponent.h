// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Projectile/BaseProjectile.h"
#include "ProjectileManagementComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PRETEST_API UProjectileManagementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProjectileManagementComponent();

	void CreateProjectile(EProjectileType ProjectileType, const FVector& Location, const FRotator& Rotation);

	uint32 GetProjectileObjectCount(EProjectileType ProjectileType);
	void ResetProjectileObjectCounts();

	DECLARE_EVENT_TwoParams(UProjectileManagementComponent, FOnProjectileObjectCountChanged, const EProjectileType&, const uint32&);
	FOnProjectileObjectCountChanged OnProjectileObjectCountChanged;

protected:
	UPROPERTY(EditAnywhere, Category = "PreTest")
	TMap<EProjectileType, TSubclassOf<class ABaseProjectile>> ProjectileClassList;

	TArray<uint32> ProjectileObjectCounts;

	UFUNCTION()
	void EndPlayForSperate(AActor* Actor, EEndPlayReason::Type EndPlayReason);

private:
	void ChangeProjectileObjectCount(const EProjectileType& ProjectileType, const uint32& Count);

};
