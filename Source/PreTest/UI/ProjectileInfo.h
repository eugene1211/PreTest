// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Projectile/BaseProjectile.h"
#include "ProjectileInfo.generated.h"

/**
 * 
 */
UCLASS()
class PRETEST_API UProjectileInfo : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(Meta = (BindWidget))
	class UVerticalBox* VerticalBox_CountInfos;

	UPROPERTY(Meta = (BindWidget))
	class UButton* ButtonReset;

public:
	UPROPERTY(EditAnywhere, Category = "PreTest")
	TSubclassOf<UUserWidget> CountInfosEntryClass;

	UPROPERTY(EditAnywhere, Category = "PreTest")
	TMap<EProjectileType, FText> ProjectileTypeNames;

protected:
	void NativeConstruct() override;
	void NativeDestruct() override;

	UFUNCTION()
	void OnResetButtonClicked();
	
	UFUNCTION()
	void OnProjectileCountChanged(const EProjectileType& ProjectileType, const uint32& Count);

	class UProjectileManagementComponent* GetProjectileManagementCompFromPlayer();

#if WITH_EDITOR
	void OnWidgetRebuilt() override;
#endif

private:
	void CreateEntries();
	void SetEntryCountText(const EProjectileType& ProjectileType, const uint32& Count);

};
