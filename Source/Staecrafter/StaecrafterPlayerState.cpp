// Fill out your copyright notice in the Description page of Project Settings.


#include "StaecrafterPlayerState.h"
#include "StaecrafterGameState.h"

AStaecrafterPlayerState::AStaecrafterPlayerState()
{
	PrimaryActorTick.bCanEverTick = true;
	ResourcesWood = 100; ResourcesStone = 100; ResourcesFood = 100;
	Population = 0;
	PopulationLimit = 0;
	//For AI
	bStartAttacking = false;
}
