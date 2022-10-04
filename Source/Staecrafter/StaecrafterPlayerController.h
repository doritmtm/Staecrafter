// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "DataLibrary.h"
#include "GameFramework/PlayerController.h"
#include "StaecrafterHUD.h"
#include "StaecrafterCharacter.h"
#include "StaecrafterResource.h"
#include "StaecrafterPlayerState.h"
#include "CostDataAsset.h"
#include "StaecrafterPlayerCameraManager.h"
#include "Landscape.h"
#include "Math/RandomStream.h"
#include "StaecrafterFunctionLibrary.h"
#include "StaecrafterPlayerController.generated.h"

UCLASS(Blueprintable)
class AStaecrafterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AStaecrafterPlayerController();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ThePlayerCameraManager)
	AStaecrafterPlayerCameraManager* ThePlayerCameraManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Victory)
	uint32 bShowVictory : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Victory)
	uint32 bShowDefeat : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Building)
	AStaecrafterBuilding* SelectedBuilding;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resources)
	AStaecrafterResource* SelectedResource;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CostData)
	UCostDataAsset* CostDataAsset;

	UFUNCTION(BlueprintCallable, Category = Building)
	void TriggerBuildingPlacing(FString BuildingName);

	UFUNCTION(BlueprintCallable, Category = Building)
	void CancelBuildingPlacing();

	UFUNCTION(BlueprintCallable, Category = Building)
	void TrainUnit();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	uint32 bBuildingPlacing : 1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	uint32 bDragEnabled : 1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	float StartDragX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	float StartDragY;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	float ContinueDragX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	float ContinueDragY;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	float StopDragX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	float StopDragY;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	float ZoomLevel = 1.0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	ALandscape* TheLandscape;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	AStaecrafterPlayerState* AIPlayerState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	AStaecrafterPlayerState* ThePlayerState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	AStaecrafterHUD* TheHUD;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	TArray<AStaecrafterCharacter*> SelectedPawns;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	AStaecrafterCharacter* SelectedCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	AActor* BuildingToBePlaced = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	FString BuildingToBePlacedName;

	UPROPERTY()
	AStaecrafterFunctionLibrary* TheFunctionLibrary;

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable)
	void MoveCommand();

	UFUNCTION(BlueprintCallable)
	void MoveCameraAccordingToMouse(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void BuildingPlacingMove();
	
	UFUNCTION(BlueprintCallable)
	void PlaceBuilding();
	
	UFUNCTION(BlueprintCallable)
	void SelectAPawn();
	
	UFUNCTION(BlueprintCallable)
	void ZoomCamera(float AxisValue);
	
	UFUNCTION(BlueprintCallable)
	void StartDrag();
	
	UFUNCTION(BlueprintCallable)
	void ContinueDrag();
	
	UFUNCTION(BlueprintCallable)
	void StopDrag();

	UFUNCTION(BlueprintCallable)
	void CollectResource();
	
	UFUNCTION(BlueprintCallable)
	void AnimateCollectResource();
	
	UFUNCTION(BlueprintCallable)
	void AttackCommand();
	
	UFUNCTION(BlueprintCallable)
	void AnimateAttackOrMoveCommand();

	UFUNCTION(BlueprintCallable)
	void AIControlTick();
	
	UFUNCTION(BlueprintCallable)
	void SpawnABuilding();

	UFUNCTION(BlueprintCallable)
	void StopTheMovement();

private:
	float PassedTime;
	float NextTime;
	int32 NrPopulationToAttack;
	int32 NrBuildingToBeSpawned;
	FRandomStream RandStream;
	int32 NrTicks;

};


