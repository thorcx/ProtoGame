#pragma once
#include "Common/CommonDataTypes.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "AriesDialogManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogEventStarted, FName, EventName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogEventFinished, FName, EventName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogConversationFinished);

class AAriesGameModeBase;
class UAriesDialogComponent;
class APawn;


//对话文本的条目
USTRUCT(BlueprintType)
struct FDialogEventData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	
	//一行对话文本文字
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dialog Event")
	FText LineText;

	//对应文本的声音资源
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialog Event")
	USoundBase* Sound;

	//对话播放优先级
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialog Event")
	EAriesDialogPriority	Priority;
	
	//播放等待时间,超时后还未能播放则移除
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialog Event")
	float TriggerWaitTime;
};

//代表一句对话条目及其对应的播放者
USTRUCT(BlueprintType)
struct FDialogConversationEntry
{
	GENERATED_USTRUCT_BODY()

public:
	FDialogConversationEntry();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue Conversation Entry")
	FName DialogueEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue Conversation Entry")
	UAriesDialogComponent* DialogComponent;
};

UCLASS(BlueprintType)
class UDialogRequest : public UObject
{
	GENERATED_BODY()

public:
	UDialogRequest();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Requeset")
	UAriesDialogComponent* DialogComponent;

	//对白请求的RowName
	UPROPERTY()
	FName EventName;

	//对白的DataTable内一个row单元的数据
	UPROPERTY()
	FDialogEventData Data;

	UPROPERTY()
	APawn* Player;

	//对白开始代理
	UPROPERTY(BlueprintAssignable, Category = "Dialog Request")
	FOnDialogEventStarted   OnStart;

	//对白结束代理
	UPROPERTY(BlueprintAssignable, Category = "Dialog Request")
	FOnDialogEventFinished	OnFinished;

	bool bIsDead;
	bool bIsHeard;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UAriesDialogManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAriesDialogManager();

	//Begin ActorComponent interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	//End ActorComponent interface

	UDialogRequest* RequestDialogEvent(UAriesDialogComponent *DialogComp, FName EventName, FDialogEventData const& Data);

	FORCEINLINE void SetGameModeOwner(AAriesGameModeBase* GameMode){ CurrentGameMode = GameMode;}

public:
	UFUNCTION(BlueprintCallable, Category="Dialog")
	void TriggerConversation(TArray<FDialogConversationEntry> const & ConversationEntries);

	//蓝图可绑定的EventDispatcher
	
	//对白一个条目开始Event
	UPROPERTY(BlueprintAssignable, Category="Dialog")
	FOnDialogEventStarted	OnDialogEventStart;
	
	//对白一个条目结束Event
	UPROPERTY(BlueprintAssignable, Category="Dialog")
	FOnDialogEventFinished  OnDialogEventFinished;

	UPROPERTY(BlueprintAssignable, Category = "Dialog")
	FOnDialogConversationFinished OnDialogConversationFinished;

private:
	
	
	bool IsRequestPlaying() const;

	bool ShouldPlayNewRequest();

	void StopCurrentRequest();

	void ProcessNextDialogRequest();


	void StartRequest(UDialogRequest* request);

	void FinishRequest(UDialogRequest* request);

	UDialogRequest* GetNextDialogRequest();
	

	UFUNCTION()
	void AdvanceConversation();

	UPROPERTY()
	TArray<UDialogRequest*>	DialogRequestsArray;

	UPROPERTY()
	UDialogRequest*	CurrentDialogRequest;

	FScriptDelegate ConversationAdvanceDelegate;
	//保存一段连续对白的数组
	TArray<FDialogConversationEntry> CurrentConversation;

	AAriesGameModeBase* CurrentGameMode;
};
