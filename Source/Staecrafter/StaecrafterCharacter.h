// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "EnumMission.h"
#include "StaecrafterResource.h"
#include "DataLibrary.h"
#include "MeshDataAsset.h"
#include "Misc/DateTime.h"
#include "StaecrafterPlayerState.h"
#include "EnumPlayer.h"
#include "EnumAIMission.h"
#include "Math/RandomStream.h"
#include "StaecrafterCharacter.generated.h"

UCLASS(Blueprintable)
class AStaecrafterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AStaecrafterCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health)
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	AActor* MovingToActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CostData)
	UCostDataAsset* CostDataAsset;
	
	UPROPERTY()
	uint32 bExecuteOnce : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	uint32 bWasDestroyed : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	EEnumMission CurrentMission;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	EEnumAIMission AIMission;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UDecalComponent* SelectedDecal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
	EEnumPlayer PlayerType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	int32 MaterialIndexToColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	AStaecrafterCharacter* CharacterToAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	AStaecrafterBuilding* BuildingToAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	AStaecrafterResource* ResourceToCollect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	AStaecrafterPlayerState* CurrentPlayerState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resource)
	UStaticMeshComponent* ResourceCarried;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resource)
	EEnumResource ResourceCarriedType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resource)
	int32 ResourceCarriedAmount;

	UFUNCTION(BlueprintCallable)
	void HandleMission();

	UFUNCTION(BlueprintCallable)
	void PerformTheDestroy();

	UFUNCTION(BlueprintCallable)
	void AIAttackTheOtherPlayer();

	UFUNCTION(BlueprintCallable)
	void FindWhatToAttackAndAttack();

	UFUNCTION(BlueprintCallable)
	void FindCharactersToAttackAndAttack();

	UFUNCTION(BlueprintCallable)
	void FollowCharacterToAttack();

	UFUNCTION(BlueprintCallable)
	void FindWhatToCollectAndCollect();
	
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void FellOutOfWorld(const UDamageType& dmgType) override;

	UPROPERTY()
	AStaecrafterFunctionLibrary* TheFunctionLibrary;

private:
	float NextTime;
	float PassedTime, PassedTime2, PassedTime3, PassedTime4;
	FRandomStream RandStream;
	uint32 bAIAttacking : 1;

	UPROPERTY()
	AStaecrafterBuilding* ClosestTownhall;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MeshDataAsset, meta = (AllowPrivateAccess = "true"))
	UDataAsset* MeshDataAsset;

};

