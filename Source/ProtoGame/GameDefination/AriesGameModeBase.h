#pragma once

#include "GameFramework/GameModeBase.h"
#include "AriesGameModeBase.generated.h"

class UAriesDialogManager;

UCLASS()
class AAriesGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:

	AAriesGameModeBase();

	UAriesDialogManager* GetDialogManager(){ return DialogManager;}

protected:
	UPROPERTY(transient, BlueprintReadOnly, Category="Dialog Manager")
	UAriesDialogManager*	DialogManager;
};
