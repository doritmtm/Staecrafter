// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/DecalComponent.h"
#include "EnumBuilding.h"
#include "EnumPlayer.h"
#include "CostDataAsset.h"
#include "Landscape.h"
#include "Math/RandomStream.h"
#include "StaecrafterFunctionLibrary.h"
#include "StaecrafterBuilding.generated.h"

UCLASS(Blueprintable)
class STAECRAFTER_API AStaecrafterBuilding : public ACharacter
{
	GENERATED_BODY()

public:
	AStaecrafterBuilding();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Booleans)
	uint32 bIsUnderConstruction : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Booleans)
	uint32 bWasDestroyed : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health)
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	UCostDataAsset* CostDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	int32 MaterialIndexToColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Units)
	int32 NrOfUnitsToBeTrained;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Units)
	float TrainPercent;

	UPROPERTY()
	uint32 bExecuteOnce : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StaecrafterCharacter)
	UClass* CharacterToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	EEnumBuilding BuildingType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
	EEnumPlayer PlayerType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
	UDecalComponent* SelectedDecal;

	UPROPERTY()
	APlayerState* CurrentPlayerState;

	UFUNCTION(BlueprintCallable)
	void PerformTheDestroy();
	
	UFUNCTION(BlueprintCallable)
	void SpawnACharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Placing);
	uint32 bIsBeingPlaced : 1;

	UPROPERTY()
	ALandscape* TheLandscape;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	AStaecrafterFunctionLibrary* TheFunctionLibrary;

private:
	float NextTime, NextTime2;
	float PassedTime, PassedTime2;
	FRandomStream RandStream;

};
