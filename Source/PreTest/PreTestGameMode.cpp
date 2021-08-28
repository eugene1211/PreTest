// Copyright Epic Games, Inc. All Rights Reserved.

#include "PreTestGameMode.h"
#include "PreTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

APreTestGameMode::APreTestGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/SideScrollerCPP/Blueprints/SideScrollerCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
