// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseProjectile.h"
#include "ProjectileReflect.generated.h"

/**
 * 
 */
UCLASS()
class PRETEST_API AProjectileReflect : public ABaseProjectile
{
	GENERATED_BODY()
	
public:
	void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit) override;

};
