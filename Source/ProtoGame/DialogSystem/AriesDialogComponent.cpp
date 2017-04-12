﻿#include "ProtoGame.h"
#include "GameDefination/AriesGameModeBase.h"
#include "AriesDialogComponent.h"


UDialogEvent::~UDialogEvent()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Dialog End"));
}


UAriesDialogComponent::UAriesDialogComponent(FObjectInitializer const& objInit)
	:Super(objInit)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	bIsEnabled = true;
	FadeOutDuration = 1.0f;
	FadeOutVolumeLevel = 0.0f;
	CurrentPlayItem = nullptr;
	IsItemInPlay = false;
}

//这里试图在Tick中用Poll的方式检测一段声音是否播完
void UAriesDialogComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (IsItemInPlay && !IsPlaying())
	{
		IsItemInPlay = false;
		if(CurrentPlayItem.IsValid())
			CurrentPlayItem->OnDialogItemEnd.Broadcast();
	}
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

UDialogEvent* UAriesDialogComponent::PlayDialogItem(FName DialogItemName)
{
	UDialogEvent* event = NewObject<UDialogEvent>(this);
	if (bIsEnabled && DialogEvents)
	{
		static const FString contextStr(TEXT("GetDialogRow"));
		FDialogEventData* dialogRow = DialogEvents->FindRow<FDialogEventData>(DialogItemName, contextStr);
		Speak(dialogRow->Sound, dialogRow->Priority);
	}
	CurrentPlayItem = event;
	IsItemInPlay = true;
	return event;
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