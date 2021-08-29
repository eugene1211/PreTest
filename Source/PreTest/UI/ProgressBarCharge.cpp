// Fill out your copyright notice in the Description page of Project Settings.


#include "ProgressBarCharge.h"
#include "Kismet/GameplayStatics.h"
#include "../PreTestCharacter.h"
#include "Components/ProgressBar.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"


void UProgressBarCharge::NativeConstruct()
{
	Super::NativeConstruct();

	if (APreTestCharacter* PrePlayerChar = Cast<APreTestCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		PrePlayerChar->OnActionKeyPressTimeChanged.AddUObject(this, &UProgressBarCharge::OnActionKeyPressTimeChanged);
	}

	SetRatio();
	StartTimerIfPossible();
}

void UProgressBarCharge::NativeDestruct()
{
	StopTimer();

	if (APreTestCharacter* PrePlayerChar = Cast<APreTestCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		PrePlayerChar->OnActionKeyPressTimeChanged.RemoveAll(this);
	}

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
	if (APreTestCharacter* PrePlayerChar = Cast<APreTestCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		float HoldMaxTime = PrePlayerChar->GetAction2HoldTime();
		float HoldCurTime = PrePlayerChar->GetActionKeyPressedTime(EActionKeyType::ActionKey0);
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
				SetRenderOpacity(FMath::Min(Ratio / FadeInRatioMax, 1.0f));
		}

		ProgressBar_Charge->SetPercent(FMath::Min(Ratio, 1.0f));
	}
}

void UProgressBarCharge::StartTimerIfPossible()
{
	if (UWorld* World = GetWorld())
	{
		float PressTime = 0.0f;
		if (APreTestCharacter* PrePlayerChar = Cast<APreTestCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
		{
			PressTime = PrePlayerChar->GetActionKeyPressTime(EActionKeyType::ActionKey0);
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
	if (APreTestCharacter* PrePlayerChar = Cast<APreTestCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		if (0.0f < PrePlayerChar->GetActionKeyPressTime(EActionKeyType::ActionKey0))
		{
			SetRatio();
		}
		else
		{
			StopTimer();
		}
	}
}
