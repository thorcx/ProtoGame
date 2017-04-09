#include "ProtoGame.h"
#include "AriesGameModeBase.h"
#include "DialogSystem/AriesDialogManager.h"
AAriesGameModeBase::AAriesGameModeBase()
{
	DialogManager = CreateDefaultSubobject<UAriesDialogManager>(TEXT("DialogManager01"));
	DialogManager->SetGameModeOwner(this);
}