// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseProjectile.h"
#include "Components/BoxComponent.h"


// Sets default values
ABaseProjectile::ABaseProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	bDestroyByHit = false;

	OnActorHit.AddDynamic(this, &ABaseProjectile::OnHit);

	TArray<UActorComponent*> CollisionComponentList = GetComponentsByTag(UBoxComponent::StaticClass(), "Collision");
	if (0 == CollisionComponentList.Num())
		return;

	UBoxComponent* CollisionComp = Cast<UBoxComponent>(CollisionComponentList[0]);
	if (CollisionComp)
	{
		FVector Dir = GetActorForwardVector();
		FVector CurLoc = GetActorLocation();
		FVector BoxExtent = CollisionComp->GetScaledBoxExtent();
		SetActorLocation(CurLoc + Dir * BoxExtent.X);
	}
}

void ABaseProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ABaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseProjectile::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		bDestroyByHit = true;
		Destroy();
	}
}
