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

void UProjectileManagementComponent::CreateProjectile(const EProjectileType& ProjectileType, const FVector& Location, const FRotator& Rotation)
{
	UWorld* World = GetWorld();
	if (nullptr == World || ProjectileType == EProjectileType::Max)
		return;
	
	if (TSubclassOf<ABaseProjectile>* SubClass = ProjectileClassList.Find(ProjectileType))
	{
		ABaseProjectile* NewProjectile = World->SpawnActor<ABaseProjectile>(SubClass->Get(), Location, Rotation);
		if (NewProjectile && ProjectileType == EProjectileType::Seperate)
		{
			NewProjectile->OnEndPlay.AddDynamic(this, &UProjectileManagementComponent::EndPlayForSperate);
		}

		uint8 Index = static_cast<uint8>(ProjectileType);
		if (ProjectileObjectCounts[Index] < TNumericLimits<uint32>::Max())
		{
			ChangeProjectileObjectCount(ProjectileType, ProjectileObjectCounts[Index] + 1);
		}
	}
}

uint32 UProjectileManagementComponent::GetProjectileObjectCount(EProjectileType ProjectileType)
{
	if(ProjectileType == EProjectileType::Max)
		return 0;

	uint8 Index = static_cast<uint8>(ProjectileType);
	return ProjectileObjectCounts[Index];
}

void UProjectileManagementComponent::ResetProjectileObjectCounts()
{
	for (uint8 Index = 0; Index < static_cast<uint8>(EProjectileType::Max); ++Index)
	{
		ChangeProjectileObjectCount(static_cast<EProjectileType>(Index), 0);
	}
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

void UProjectileManagementComponent::ChangeProjectileObjectCount(const EProjectileType& ProjectileType, const uint32& Count)
{
	if (ProjectileType == EProjectileType::Max)
		return;

	uint8 Index = static_cast<uint8>(ProjectileType);
	ProjectileObjectCounts[Index] = Count;

	OnProjectileObjectCountChanged.Broadcast(ProjectileType, ProjectileObjectCounts[Index]);
}
