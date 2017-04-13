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

	//�Ի�ArrayΪ�գ����ǵ�ǰRequest�Ѿ�������ϣ�ִ��finish���㲥֪ͨ
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

		//������Component�������ԣ�������ָ����Ч,Component��PendingKill����͹�����
		if (entry.DialogComponent->IsValidLowLevel())
		{
			UDialogRequest* request = entry.DialogComponent->TriggerDialogEvent(entry.DialogueEvent);

			if (request)
			{
				//ע��������÷���OnFinished���DynamicDelegateû���ó��õ�AddDynamic�����ֱ����Add����һ��FScriptDelegate
				//�����Add������ʹ��AddDynamicҪ����ǩ������Ҫ��OnFinished����һ�µ�����
				//AdvaceConversation������������OnFinishedҪ���������
				//�����Ǹ��Ƚ���Ȥ���÷���һ����˵��дһ���������Ļص�������AddDynamic��
				//����AdvanceConversation��������Ҫ��������ֻ��ÿ����һ�Σ�ִ��Manager�ڲ������CurrentConversation��һ����Ŀ
				//Ȼ��ɾ�������Ŀ,������÷���֤�˵�һ���Ի�ִ�����Ż�ִ֪ͨ����һ���Ի�
				//˼��:
				//һ������»���һ��ѭ������ִ�У��������������ѭ��ÿִ��һ����Ҫ�ȴ��������ǲ�����,�����㲻������סѭ��
				//���������������������ܾ���Ҫдһ���첽Task��ִ��ѭ�����������ȴ�ÿһ�仰������ִ����һ�仰
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