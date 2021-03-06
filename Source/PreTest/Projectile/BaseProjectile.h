// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

UENUM(BlueprintType)
enum class EProjectileType : uint8
{
	Normal = 0,
	Charge,
	Seperate,
	Reflect,

	Max,
};

UCLASS()
class PRETEST_API ABaseProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	bool bDestroyedByHit;

	UPROPERTY(EditAnywhere, Category = "PreTest")
	bool bDestroyByHit;

public:
	UFUNCTION()
	virtual void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	bool IsDestroyedByHit() const { return bDestroyedByHit; }
};
