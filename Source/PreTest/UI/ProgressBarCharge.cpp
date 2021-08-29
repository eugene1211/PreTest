// Fill out your copyright notice in the Description page of Project Settings.


#include "ProgressBarCharge.h"
#include "Kismet/GameplayStatics.h"
#include "../PreTestCharacter.h"
#include "Components/ProgressBar.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "../Character/ActionKeyManagementComponent.h"


void UProgressBarCharge::NativeConstruct()
{
	Super::NativeConstruct();

	PrevRatio = 0.0f;

	SetRatio();
	StartTimerIfPossible();

	if (UActionKeyManagementComponent* Component = GetActionKeymanagementComponent())
	{
		Component->OnActionKeyPressTimeChanged.AddUObject(this, &UProgressBarCharge::OnActionKeyPressTimeChanged);
	}
}

void UProgressBarCharge::NativeDestruct()
{
	if (UActionKeyManagementComponent* Component = GetActionKeymanagementComponent())
	{
		Component->OnActionKeyPressTimeChanged.RemoveAll(this);
	}

	StopTimer();

	Super::NativeDestruct();
}

void UProgressBarCharge::OnActionKeyPressTimeChanged(const EActionKeyType& ActionKeyType, const float& Time)
{
	if (ActionKeyType != EActionKeyType::ActionKey0)
		return;

	if (0.0f < Time)
	{
		StartTimerIfPossible();
	}
	else
	{
		SetRatio();
	}
}

void UProgressBarCharge::SetRatio()
{
	if (UActionKeyManagementComponent* Component = GetActionKeymanagementComponent())
	{
		float HoldMaxTime = Component->GetAction2HoldTime();
		float HoldCurTime = Component->GetActionKeyPressedTime(EActionKeyType::ActionKey0);
		float Ratio = 0.0f;
		if (0.0f < HoldMaxTime)
			Ratio = HoldCurTime / HoldMaxTime;

		if (Ratio <= 0.0f)
		{
			SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			if (FadeInRatioMax < 1.0f)
			{
				SetRenderOpacity(FMath::Min(Ratio / FadeInRatioMax, 1.0f));
			}
		}


		ProgressBar_Charge->SetPercent(FMath::Min(Ratio, 1.0f));
		PlayAnimWithRatio(Ratio);
	}
}

void UProgressBarCharge::StartTimerIfPossible()
{
	if (UWorld* World = GetWorld())
	{
		float PressTime = 0.0f;
		if (UActionKeyManagementComponent* Component = GetActionKeymanagementComponent())
		{
			PressTime = Component->GetActionKeyPressTime(EActionKeyType::ActionKey0);
		}

		if (0.0f < PressTime && !TimerHandle.IsValid())
		{
			World->GetTimerManager().SetTimer(TimerHandle, this, &UProgressBarCharge::PerformSetRatioWithTimer, 0.01f, true);
		}
	}
}

void UProgressBarCharge::StopTimer()
{
	if (UWorld* World = GetWorld())
	{
		if (TimerHandle.IsValid())
		{
			World->GetTimerManager().ClearTimer(TimerHandle);
		}
	}
}

void UProgressBarCharge::PerformSetRatioWithTimer()
{
	if (UActionKeyManagementComponent* Component = GetActionKeymanagementComponent())
	{
		if (0.0f < Component->GetActionKeyPressTime(EActionKeyType::ActionKey0))
		{
			SetRatio();
		}
		else
		{
			StopTimer();
		}
	}
}

class UActionKeyManagementComponent* UProgressBarCharge::GetActionKeymanagementComponent()
{
	if (APreTestCharacter* PrePlayerChar = Cast<APreTestCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		return Cast<UActionKeyManagementComponent>(PrePlayerChar->GetComponentByClass(UActionKeyManagementComponent::StaticClass()));
	}

	return nullptr;
}

void UProgressBarCharge::PlayAnimWithRatio(float Ratio)
{
	if (1.0f <= Ratio)
	{
		if (Anim_Min && IsAnimationPlaying(Anim_Min))
			StopAnimation(Anim_Min);

		if (PrevRatio < 1.0f && Anim_Max && !IsAnimationPlaying(Anim_Max))
			PlayAnimation(Anim_Max);
	}
	else
	{
		if (Anim_Max && IsAnimationPlaying(Anim_Max))
			StopAnimation(Anim_Max);

		if (Anim_Min && !IsAnimationPlaying(Anim_Min))
			PlayAnimation(Anim_Min, 0.0f, 0);
	}

	PrevRatio = Ratio;
}
