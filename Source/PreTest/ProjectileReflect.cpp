// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileReflect.h"

void AProjectileReflect::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || !OtherActor)
		return;

	FRotator Rotation = GetActorRotation();
	Rotation.Yaw += 180.0f;
	SetActorRotation(Rotation, ETeleportType::None);

}
