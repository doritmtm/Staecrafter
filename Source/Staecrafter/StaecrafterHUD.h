// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "StaecrafterCharacter.h"
#include "StaecrafterHUD.generated.h"

UCLASS(Blueprintable)
class STAECRAFTER_API AStaecrafterHUD : public AHUD
{
	GENERATED_BODY()

public:
	AStaecrafterHUD();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TArray<AStaecrafterCharacter*> SelectedPawns;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	uint32 bDragEnabled : 1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	float StartDragX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	float StartDragY;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	float ContinueDragX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	float ContinueDragY;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	float StopDragX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	float StopDragY;

protected:
	virtual void DrawHUD() override;
	
};
