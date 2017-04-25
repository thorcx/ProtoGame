#pragma once

#include "SteamVRChaperoneComponent.h"
#include "MotionControllerComponent.h"
#include "DefaultHand.generated.h"

UCLASS()
class AAriesDefaultHand : public AActor
{
	GENERATED_BODY()

public:
	AAriesDefaultHand();

	UFUNCTION(BlueprintCallable, Category="DefaultHand|Chaperone")
	void SetupRoomScaleOutline();


public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultHandComp")
	USteamVRChaperoneComponent*	SteamVRChaperone;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultHandComp")
	UMotionControllerComponent* MotionController;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultHandComp")
	USkeletalMeshComponent*		HandMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultHandComp")
	UStaticMeshComponent*		TeleportMarker;

private:	
	bool bIsRoomScale;

	//设置Chaperone显示的高度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ChaperoneSetup", meta =(AllowPrivateAccess="true") )
	float ChaperoneMeshHeight;

};