#include "ProtoGame.h"
#include "DefaultHand.h"
#include "Kismet/KismetMathLibrary.h"
AAriesDefaultHand::AAriesDefaultHand()
:bIsRoomScale(false)
{
	SteamVRChaperone = CreateDefaultSubobject<USteamVRChaperoneComponent>(TEXT("StreamVRChaperone_1"));
	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	MotionController->SetupAttachment(GetRootComponent());
	//不是ScenComponent,不需要attach
	//SteamVRChaperone->SetupAttachment(RootComponent);

	HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandMesh"));
	//展示如何在C++构造中加载资源，一般情况下不这么做，在蓝图内指定就可以了
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skMesh(TEXT("/Game/ArtAssets/Meshes/MannequinHand_Right.MannequinHand_Right"));
	if(skMesh.Succeeded())
		HandMesh->SetSkeletalMesh(skMesh.Object);

	TeleportMarker = CreateOptionalDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportMesh"));
	TeleportMarker->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

}

void AAriesDefaultHand::SetupRoomScaleOutline()
{
	TArray<FVector> verts;

	
	if(SteamVRChaperone)
	{ 
		verts = SteamVRChaperone->GetBounds();
		UWorld *worldContext = GetWorld();
		FVector		outCenter;
		FRotator	outRotator;
		float		outLengthX;
		float		outLengthY;
		
		
		UKismetMathLibrary::MinimumAreaRectangle(worldContext, verts, FVector(0,0,1), outCenter, outRotator,outLengthX, outLengthY);

		bIsRoomScale = !(FMath::IsNearlyEqual(outLengthX, 100.0f, 0.01f) && FMath::IsNearlyEqual(outLengthY, 100.0f, 0.01f));

	
	
	}
	else
	{
		
	}
		

}