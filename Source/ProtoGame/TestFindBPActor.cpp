// Fill out your copyright notice in the Description page of Project Settings.

#include "ProtoGame.h"
#include "TestFindBPActor.h"


// Sets default values
ATestFindBPActor::ATestFindBPActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UTexture2D> TestTex(TEXT("/Game/StarterContent/Textures/T_Brick_Clay_Beveled_D"));

	static ConstructorHelpers::FObjectFinder<APawn> Pawn(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	
	if (TestTex.Object)
	{
		TexFromCppSet = TestTex.Object;
	}
}

// Called when the game starts or when spawned
void ATestFindBPActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestFindBPActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

