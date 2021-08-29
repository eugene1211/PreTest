// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileReflect.h"
#include "GameFramework/ProjectileMovementComponent.h"


void AProjectileReflect::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || !OtherActor)
		return;
}