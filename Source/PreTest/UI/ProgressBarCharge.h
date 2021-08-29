// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProgressBarCharge.generated.h"

/**
 * 
 */
UCLASS()
class PRETEST_API UProgressBarCharge : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(Meta = (BindWidget))
	class UProgressBar* ProgressBar_Charge;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Anim_Min;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Anim_Max;

	UPROPERTY(EditAnywhere)
	float FadeInRatioMax;

protected:
	void NativeConstruct() override;
	void NativeDestruct() override;

	UFUNCTION()
	void OnActionKeyPressTimeChanged(const EActionKeyType& ActionKeyType, const float& Time);

private:
	void SetRatio();
	void StartTimerIfPossible();
	void StopTimer();
	void PerformSetRatioWithTimer();
	class UActionKeyManagementComponent* GetActionKeymanagementComponent();

	void PlayAnimWithRatio(float Ratio);

	FTimerHandle TimerHandle;
	float PrevRatio;

};
