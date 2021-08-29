// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionKeyManagementComponent.h"

// Sets default values for this component's properties
UActionKeyManagementComponent::UActionKeyManagementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

float UActionKeyManagementComponent::GetActionKeyPressTime(const EActionKeyType& ActionKeyType) const
{
	if (ActionKeyType == EActionKeyType::Max)
		return 0.0f;

	uint8 Index = static_cast<uint8>(ActionKeyType);
	if (ActionKeyPressTimes[Index] <= 0.0f)
		return 0.0f;

	return ActionKeyPressTimes[Index];
}

float UActionKeyManagementComponent::GetActionKeyPressedTime(const EActionKeyType& ActionKeyType) const
{
	float PressTime = GetActionKeyPressTime(ActionKeyType);
	return PressTime <= 0.0f ? 0.0f : GetOwner()->GetGameTimeSinceCreation() - ActionKeyPressTimes[static_cast<uint8>(ActionKeyType)];
}

float UActionKeyManagementComponent::GetActionHoldTime(const EProjectileType& ProjectileType) const
{
	if (const float* HoldTimePtr = ProjectileActionKeyHoldTime.Find(ProjectileType))
		return *HoldTimePtr;

	return 0.0f;
}

bool UActionKeyManagementComponent::IsPressedAnyActionKey(EActionKeyType ActionKeyTypeToIgnore) const
{
	for (int32 Index = 0; Index < static_cast<uint8>(EActionKeyType::Max); ++Index)
	{
		if (Index != static_cast<uint8>(ActionKeyTypeToIgnore) && ActionKeyTypeStates[Index])
		{
			return true;
		}
	}

	return false;
}

// Called when the game starts
void UActionKeyManagementComponent::BeginPlay()
{
	Super::BeginPlay();

	// init variables
	bBlockActionKey = false;
	ActionKeyTypeStates.Init(false, static_cast<uint8>(EActionKeyType::Max));
	ActionKeyPressTimes.Init(0.0f, static_cast<uint8>(EActionKeyType::Max));
}

void UActionKeyManagementComponent::SetActionKeyPressTime(EActionKeyType ActionKeyType, float Time)
{
	if (ActionKeyType == EActionKeyType::Max)
		return;

	uint8 Index = static_cast<uint8>(ActionKeyType);
	ActionKeyPressTimes[Index] = Time;

	OnActionKeyPressTimeChanged.Broadcast(ActionKeyType, Time);
}

void UActionKeyManagementComponent::ExecuteActionIfPossible(EActionKeyType ActionKeyType, bool bPressedNew)
{
	switch (ActionKeyType)
	{
		case EActionKeyType::ActionKey0:
		{
			if (IsPressedAnyActionKey(ActionKeyType))
			{
				bBlockActionKey = true;
				break;
			}

			if (bPressedNew)
			{
				SetActionKeyPressTime(ActionKeyType, GetOwner()->GetGameTimeSinceCreation());
			}
			else
			{
				float ReleaseTime = GetOwner()->GetGameTimeSinceCreation();
				uint8 Index = static_cast<uint8>(ActionKeyType);
				float TimeToPressed = ReleaseTime - ActionKeyPressTimes[Index];
				if (TimeToPressed < GetActionHoldTime(EProjectileType::Charge))
				{
					OnActionEnabled.Broadcast(EProjectileType::Normal);
					bBlockActionKey = true;
				}
				else
				{
					OnActionEnabled.Broadcast(EProjectileType::Charge);
					bBlockActionKey = true;
				}
			}
		}
		break;

		case EActionKeyType::ActionKey1:
		{
			if (bPressedNew)
			{
				uint8 Index = static_cast<uint8>(EActionKeyType::ActionKey0);
				if (ActionKeyPressTimes[Index] <= 0.0f)
					break;

				float ReleaseTime = GetOwner()->GetGameTimeSinceCreation();
				float TimeToPressed = ReleaseTime - ActionKeyPressTimes[Index];
				if (TimeToPressed <= GetActionHoldTime(EProjectileType::Seperate))
				{
					OnActionEnabled.Broadcast(EProjectileType::Seperate);
					bBlockActionKey = true;
				}
			}
			else
			{
				if (IsPressedAnyActionKey(ActionKeyType))
				{
					bBlockActionKey = true;
					break;
				}

				OnActionEnabled.Broadcast(EProjectileType::Reflect);
				bBlockActionKey = true;
			}
		}
		break;
	}

	if (bBlockActionKey)
	{
		SetActionKeyPressTime(EActionKeyType::ActionKey0, 0.0f);
	}
}

void UActionKeyManagementComponent::UpdateActionKeyState(EActionKeyType ActionKeyType, bool bPressedNew)
{
	if (!bBlockActionKey)
	{
		ExecuteActionIfPossible(ActionKeyType, bPressedNew);
	}

	// update key pressed state
	uint8 Index = static_cast<uint8>(ActionKeyType);
	ActionKeyTypeStates[Index] = bPressedNew;

	if (false == IsPressedAnyActionKey(EActionKeyType::Max))
		bBlockActionKey = false;
}
