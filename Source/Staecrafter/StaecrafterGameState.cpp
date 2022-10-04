// Fill out your copyright notice in the Description page of Project Settings.


#include "StaecrafterGameState.h"

AStaecrafterGameState::AStaecrafterGameState()
{
	FPlayerInfo PlayerInfoAI;
	AStaecrafterPlayerState* OnePlayerState = CreateDefaultSubobject<AStaecrafterPlayerState>(TEXT("One Player State"));
	PlayerInfoAI.PlayerState = OnePlayerState;
	PlayerInfoAI.PlayerType = EEnumPlayer::AI;
	PlayerInfo.Add(PlayerInfoAI);
	TheFunctionLibrary = CreateDefaultSubobject<AStaecrafterFunctionLibrary>("Staecrafter Function Library");
}

void AStaecrafterGameState::BeginPlay()
{
	Super::BeginPlay();
	
}

FPlayerInfo AStaecrafterGameState::GetPlayerInfoByType(EEnumPlayer Type)
{
	for (FPlayerInfo I : PlayerInfo)
	{
		if (I.PlayerType == Type)
		{
			return I;
		}
	}
	return FPlayerInfo();
}
