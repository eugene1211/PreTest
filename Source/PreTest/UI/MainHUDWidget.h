// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUDWidget.generated.h"


/**
 * 
 */
UCLASS()
class PRETEST_API UMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	UUserWidget* UW_ProjectileInfo;

	UPROPERTY()
	class UProgressBar* ProgressBar_Charge;

};
