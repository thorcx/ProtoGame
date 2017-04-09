#include "ProtoGame.h"
#include "GameDefination/AriesGameModeBase.h"
#include "AriesDialogComponent.h"
UAriesDialogComponent::UAriesDialogComponent(FObjectInitializer const& objInit)
	:Super(objInit)
{
	bIsEnabled = true;
	FadeOutDuration = 1.0f;
	FadeOutVolumeLevel = 0.0f;
}

void UAriesDialogComponent::BeginPlay()
{
	if (!DialogManager)
	{
		AAriesGameModeBase *GM = GetWorld()->GetAuthGameMode<AAriesGameModeBase>();
		if (GM)
		{
			DialogManager = GM->GetDialogManager();
		}
	}
}

void UAriesDialogComponent::Speak(USoundBase* soundAsset, EAriesDialogPriority dialogPriority)
{
//如果声音正在播，判断新资源优先级如果高于现在的，抢占
	if (IsPlaying())
	{
		if (dialogPriority >= CurrentPriority)
		{
			InitSound(soundAsset, dialogPriority);
		}
	}
	else
	{
		InitSound(soundAsset, dialogPriority);
	}

	Play();
}

UDialogRequest* UAriesDialogComponent::TriggerDialogEvent(FName EventName)
{
	if (bIsEnabled && DialogEvents)
	{
		static const FString contextStr(TEXT("GetDialogRow"));
		FDialogEventData* dialogRow = DialogEvents->FindRow<FDialogEventData>(EventName, contextStr);

		if (dialogRow && DialogManager)
		{
			return DialogManager->RequestDialogEvent(this, EventName, *dialogRow);
		}
	}

	return nullptr;
}

void UAriesDialogComponent::InitSound(USoundBase* soundAsset, EAriesDialogPriority dialogPriority)
{
	CurrentPriority = dialogPriority;
	SetSound(soundAsset);
	
}

void UAriesDialogComponent::SetDialogEvents(UDataTable* Events)
{
	DialogEvents = Events;
}

void UAriesDialogComponent::SetEnabled(bool Enabled)
{
	if(bIsEnabled == Enabled)
		return;

	bIsEnabled = Enabled;

	if (!bIsEnabled && IsPlaying())
	{
		FadeOut(FadeOutDuration, FadeOutVolumeLevel);
	}
}