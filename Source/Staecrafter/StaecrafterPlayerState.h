// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "StaecrafterFunctionLibrary.h"
#include "StaecrafterBuilding.h"
#include "StaecrafterPlayerState.generated.h"

UCLASS(Blueprintable)
class STAECRAFTER_API AStaecrafterPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AStaecrafterPlayerState();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resources)
	int32 ResourcesWood;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resources)
	int32 ResourcesStone;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resources)
	int32 ResourcesFood;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resources)
	int32 PopulationLimit;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resources)
	int32 Population;

	//For AI
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resources)
	uint32 bStartAttacking : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	TArray<AStaecrafterBuilding*> Buildings;
	
};
