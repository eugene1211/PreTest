// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileInfo.h"
#include "../Projectile/BaseProjectile.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/ProjectileManagementComponent.h"
#include "GameFramework/Character.h"
#include "Components/TextBlock.h"


void UProjectileInfo::NativeConstruct()
{
	Super::NativeConstruct();

	CreateEntries();

	if (ButtonReset)
	{
		ButtonReset->OnClicked.AddDynamic(this, &UProjectileInfo::OnResetButtonClicked);
	}

	UProjectileManagementComponent* ProjManageComp = GetProjectileManagementCompFromPlayer();
	if (!ProjManageComp)
		return;

	ProjManageComp->OnProjectileObjectCountChanged.AddUObject(this, &UProjectileInfo::OnProjectileCountChanged);
}

void UProjectileInfo::NativeDestruct()
{
	Super::NativeDestruct();
	
	UProjectileManagementComponent* ProjManageComp = GetProjectileManagementCompFromPlayer();
	if (!ProjManageComp)
		return;

	ProjManageComp->OnProjectileObjectCountChanged.RemoveAll(this);
}

void UProjectileInfo::OnResetButtonClicked()
{
	UProjectileManagementComponent* ProjManageComp = GetProjectileManagementCompFromPlayer();
	if (ProjManageComp)
	{
		ProjManageComp->ResetProjectileObjectCounts();
	}
}

void UProjectileInfo::OnProjectileCountChanged(const EProjectileType& ProjectileType, const uint32& Count)
{
	SetEntryCountText(ProjectileType, Count);
}

class UProjectileManagementComponent* UProjectileInfo::GetProjectileManagementCompFromPlayer()
{
	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!PlayerChar)
		return nullptr;

	UActorComponent* Comp = PlayerChar->GetComponentByClass(UProjectileManagementComponent::StaticClass());
	if (!Comp)
		return nullptr;

	UProjectileManagementComponent* ProjManageComp = Cast<UProjectileManagementComponent>(Comp);
	if (!ProjManageComp)
		return nullptr;

	return ProjManageComp;
}

#if WITH_EDITOR
void UProjectileInfo::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();
	
	if (!IsDesignTime())
		return;

	if (!CountInfosEntryClass || !VerticalBox_CountInfos)
		return;

	UUserWidget* EntryWidget = CreateWidget(this, CountInfosEntryClass);;
	if (EntryWidget)
	{
		VerticalBox_CountInfos->AddChildToVerticalBox(EntryWidget);
	}
}
#endif

void UProjectileInfo::CreateEntries()
{
	if (!CountInfosEntryClass || !VerticalBox_CountInfos)
		return;

	if (VerticalBox_CountInfos->GetChildrenCount())
		VerticalBox_CountInfos->ClearChildren();

	UProjectileManagementComponent* ProjManageComp = GetProjectileManagementCompFromPlayer();
	if (!ProjManageComp)
		return;

	UUserWidget* EntryWidget = nullptr;
	for (uint8 Index = 0; Index < static_cast<uint8>(EProjectileType::Max); ++Index)
	{
		EntryWidget = CreateWidget(GetGameInstance(), CountInfosEntryClass);
		if (EntryWidget)
		{
			VerticalBox_CountInfos->AddChildToVerticalBox(EntryWidget);

			EProjectileType ProjType = static_cast<EProjectileType>(Index);
			uint32 Count = ProjManageComp->GetProjectileObjectCount(ProjType);

			SetEntryCountText(ProjType, Count);

			// Set Name Text
			FText* NameTextPtr = ProjectileTypeNames.Find(ProjType);
			if (NameTextPtr)
			{
				UTextBlock* TextBlock = Cast<UTextBlock>(EntryWidget->GetWidgetFromName("TextName"));
				if (TextBlock)
					TextBlock->SetText(*NameTextPtr);
			}
		}
	}
}

void UProjectileInfo::SetEntryCountText(const EProjectileType& ProjectileType, const uint32& Count)
{
	if (!VerticalBox_CountInfos)
		return;

	uint8 Index = static_cast<uint8>(ProjectileType);
	UUserWidget* EntryWidget = Cast<UUserWidget>(VerticalBox_CountInfos->GetChildAt(Index));
	if (EntryWidget)
	{
		// Set Count
		UTextBlock* TextBlock = Cast<UTextBlock>(EntryWidget->GetWidgetFromName("TextCount"));
		if (TextBlock)
		{
			FText CountText = FText::AsNumber(Count);
			TextBlock->SetText(CountText);
		}
	}
}
