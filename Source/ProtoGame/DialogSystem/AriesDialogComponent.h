#pragma once
#include "Components/AudioComponent.h"
#include "AriesDialogManager.h"
#include "AriesDialogComponent.generated.h"

/**用来给各种Actor挂载实现对话的组件，由AriesDialogManager统一调度

*/

class UAriesDialogManager;

UCLASS(meta=(BlueprintSpawnableComponent))
class UAriesDialogComponent : public UAudioComponent
{
	GENERATED_UCLASS_BODY()

public:

	virtual void BeginPlay() override;

	//播放对话声音资源，如果当前正在播放且传入的对话优先级大于当前正播放的则立即切换，否则播当前正在播放的
	//如果没在播，立即播放新对话,并设置优先级.
	void Speak(USoundBase* soundAsset, EAriesDialogPriority dialogPriority);

	//根据Row名称找到对应对白条目,提交播放请求到Manager处理
	UFUNCTION(BlueprintCallable, Category="Dialog")
	UDialogRequest* TriggerDialogEvent(FName EventName);

	UFUNCTION(BlueprintCallable, Category="Dialog")
	void SetDialogEvents(UDataTable* Events);

	UFUNCTION(BlueprintCallable, Category = "Dialog")
	void SetEnabled(bool Enabled);

	UFUNCTION(BlueprintCallable, Category = "Dialog")
	FORCEINLINE bool GetEnabled() const{ return bIsEnabled;};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialog Events")
	UDataTable* DialogEvents;

	UPROPERTY(BlueprintAssignable, Category="Dialog")
	FOnDialogEventStarted	OnDialogEventStarted;

	UPROPERTY(BlueprintAssignable, Category = "Dialog")
	FOnDialogEventFinished	OnDialogEventFinished;

private:
	
	EAriesDialogPriority	CurrentPriority;
	UAriesDialogManager*	DialogManager;

	//设置对话组件使用的音源和优先级
	void InitSound(USoundBase* soundAsset, EAriesDialogPriority dialogPriority);

	UPROPERTY(EditAnywhere, Category="Dialog Events")
	bool bIsEnabled;

	//声音渐隐最小音量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialog Controll", meta = (AllowPrivateAccess="true"))
	float FadeOutVolumeLevel;

	//声音渐隐持续秒数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Controll", meta = (AllowPrivateAccess = "true"))
	float FadeOutDuration;
};


