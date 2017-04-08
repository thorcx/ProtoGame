// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ProtoGame.h"
#include "ProtoGameGameMode.h"
#include "ProtoGameCharacter.h"

AProtoGameGameMode::AProtoGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	
	

	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	GLogConsole->Log(TEXT("Fuck"));
	int a = 1;
}
