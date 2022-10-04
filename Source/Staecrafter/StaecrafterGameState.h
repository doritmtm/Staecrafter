// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "StaecrafterPlayerState.h"
#include "EnumPlayer.h"
#include "StaecrafterFunctionLibrary.h"
#include "StaecrafterGameState.generated.h"

USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerInfo)
	AStaecrafterPlayerState* PlayerState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerInfo)
	EEnumPlayer PlayerType;
	
};

UCLASS(Blueprintable)
class STAECRAFTER_API AStaecrafterGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AStaecrafterGameState();

	UPROPERTY()
	AStaecrafterFunctionLibrary* TheFunctionLibrary;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerInfo)
	TArray<FPlayerInfo> PlayerInfo;

	UFUNCTION(BlueprintCallable)
	FPlayerInfo GetPlayerInfoByType(EEnumPlayer Type);

protected:
	virtual void BeginPlay();
	
};
