#include "ProtoGame.h"
#include "AriesDialogManager.h"
#include "AriesDialogComponent.h"

FDialogConversationEntry::FDialogConversationEntry()
	:DialogComponent(),DialogueEvent()
{

}

UDialogRequest::UDialogRequest()
	:DialogComponent()
	,EventName()
	,Data()
	,Player()
	,bIsDead(false)
	,bIsHeard(false)
{
}



UAriesDialogManager::UAriesDialogManager()
:CurrentDialogRequest()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_DuringPhysics;
}

void UAriesDialogManager::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (ShouldPlayNewRequest())
	{
		StopCurrentRequest();
		ProcessNextDialogRequest();
	}

	//对话Array为空，但是当前Request已经播放完毕，执行finish并广播通知
	if (CurrentDialogRequest && !IsRequestPlaying())
	{
		FinishRequest(CurrentDialogRequest);
		CurrentDialogRequest = nullptr;
	}
}

UDialogRequest* UAriesDialogManager::RequestDialogEvent(UAriesDialogComponent *DialogComp, FName EventName, FDialogEventData const& Data)
{
	UDialogRequest* request = NewObject<UDialogRequest>(this);
	request->DialogComponent = DialogComp;
	request->EventName		 = EventName;
	request->Data			 = Data;

	if (UWorld* world = GetWorld())
	{
		if (APlayerController* pc = world->GetFirstPlayerController())
		{
			request->Player = pc->GetPawn();
		}
	}

	DialogRequestsArray.Add(request);
	return request;
}


void UAriesDialogManager::TriggerConversation(TArray<FDialogConversationEntry> const & ConversationEntries)
{
	CurrentConversation = MoveTemp(ConversationEntries);
	AdvanceConversation();
}

bool UAriesDialogManager::IsRequestPlaying() const
{
	if (CurrentDialogRequest && CurrentDialogRequest->DialogComponent->IsValidLowLevel())
	{
		return CurrentDialogRequest->DialogComponent->IsPlaying();
	}
	return false;
}

bool UAriesDialogManager::ShouldPlayNewRequest()
{
	if (DialogRequestsArray.Num() == 0)
		return false;

	if(IsRequestPlaying())
		return false;

	return true;
}

void UAriesDialogManager::StopCurrentRequest()
{
	if (IsRequestPlaying())
	{
		CurrentDialogRequest->DialogComponent->Stop();
	}

	CurrentDialogRequest = nullptr;
}

void UAriesDialogManager::ProcessNextDialogRequest()
{
	CurrentDialogRequest = GetNextDialogRequest();

	if (CurrentDialogRequest)
	{
		if (CurrentDialogRequest->DialogComponent->IsValidLowLevel())
		{
			CurrentDialogRequest->DialogComponent->Speak(CurrentDialogRequest->Data.Sound, CurrentDialogRequest->Data.Priority);
			CurrentDialogRequest->bIsHeard = true;
			DialogRequestsArray.RemoveAt(DialogRequestsArray.Num() - 1);
			StartRequest(CurrentDialogRequest);
		}
		else
		{
			DialogRequestsArray.RemoveAt(DialogRequestsArray.Num() - 1);
		}
	}
}


void UAriesDialogManager::StartRequest(UDialogRequest* request)
{
	if (request && !request->bIsDead)
	{
		request->OnStart.Broadcast(request->EventName);
		OnDialogEventStart.Broadcast(request->EventName);

		if (request->DialogComponent->IsValidLowLevel())
		{
			request->DialogComponent->OnDialogEventStarted.Broadcast(request->EventName);
		}
	}
}

void UAriesDialogManager::FinishRequest(UDialogRequest* request)
{
	if (request && !request->bIsDead)
	{
		request->bIsDead = true;
		request->OnFinished.Broadcast(request->EventName);

		OnDialogEventFinished.Broadcast(request->EventName);

		if (request->DialogComponent->IsValidLowLevel())
		{
			request->DialogComponent->OnDialogEventFinished.Broadcast(request->EventName);
		}
	}
}

UDialogRequest* UAriesDialogManager::GetNextDialogRequest()
{
	return DialogRequestsArray.Num() == 0 ? nullptr : DialogRequestsArray.Last();
}

void UAriesDialogManager::AdvanceConversation()
{
	if (ConversationAdvanceDelegate.IsBound())
	{
		ConversationAdvanceDelegate.Unbind();
	}

	if (CurrentConversation.Num() > 0)
	{
		FDialogConversationEntry &entry = CurrentConversation[0];

		//这里检查Component的完整性，不光是指针有效,Component被PendingKill这里就过不了
		if (entry.DialogComponent->IsValidLowLevel())
		{
			UDialogRequest* request = entry.DialogComponent->TriggerDialogEvent(entry.DialogueEvent);

			if (request)
			{
				//注意这里的用法，OnFinished这个DynamicDelegate没有用常用的AddDynamic宏而是直接用Add加入一个FScriptDelegate
				//这里的Add避免了使用AddDynamic要求函数签名必须要跟OnFinished保持一致的问题
				//AdvaceConversation不带参数，而OnFinished要求带单参数
				//这里是个比较有趣的用法，一般来说会写一个带参数的回调函数由AddDynamic绑定
				//但是AdvanceConversation函数不需要参数，它只是每调用一次，执行Manager内部保存的CurrentConversation的一个条目
				//然后删除这个条目,这里的用法保证了当一条对话执行完后才会通知执行下一条对话
				//思考:
				//一般情况下会做一个循环依次执行，但是这里如果用循环每执行一次需要等待的世界是不定的,而且你不能阻塞住循环
				//如果不用这里的做法，可能就需要写一个异步Task来执行循环，阻塞并等待每一句话播完再执行下一句话
				ConversationAdvanceDelegate.BindUFunction(this, "AdvanceConversation");
				request->OnFinished.Add(ConversationAdvanceDelegate);
				//request->OnFinished.AddDynamic(this, &UAriesDialogManager::AdvanceConversation);
				CurrentConversation.RemoveAt(0);
			}
		}
	}
	else
	{
		OnDialogConversationFinished.Broadcast();
	}
}