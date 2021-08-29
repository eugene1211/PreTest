// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileManagementComponent.h"
#include "Components/ArrowComponent.h"


// Sets default values for this component's properties
UProjectileManagementComponent::UProjectileManagementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	ProjectileObjectCounts.Init(false, static_cast<uint8>(EProjectileType::Max));
}

void UProjectileManagementComponent::CreateProjectile(EProjectileType ProjectileType, const FVector& Location, const FRotator& Rotation)
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
		NewProjectile->OnEndPlay.AddDynamic(this, &UProjectileManagementComponent::EndPlayForSperate);
	}

	uint8 Index = static_cast<uint8>(ProjectileType);
	if (ProjectileObjectCounts[Index] < TNumericLimits<uint32>::Max())
	{
		++ProjectileObjectCounts[Index];
	}

	OnProjectileObjectCountChanged.Broadcast(ProjectileType, ProjectileObjectCounts[Index]);
}

uint32 UProjectileManagementComponent::GetProjectileObjectCount(EProjectileType ProjectileType)
{
	uint8 Index = static_cast<uint8>(ProjectileType);
	return ProjectileObjectCounts[Index];
}

void UProjectileManagementComponent::ResetProjectileObjectCounts()
{
	for (uint32& Count : ProjectileObjectCounts)
		Count = 0;
}

void UProjectileManagementComponent::EndPlayForSperate(AActor* Actor, EEndPlayReason::Type EndPlayReason)
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

			CreateProjectile(EProjectileType::Normal, Location, Rotation);

			Rotation.Pitch = OriginPitch - 45.0f;
			CreateProjectile(EProjectileType::Normal, Location, Rotation);

			Rotation.Pitch = OriginPitch + 45.0f;
			CreateProjectile(EProjectileType::Normal, Location, Rotation);
		}
	);
}
