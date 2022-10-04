// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/DecalComponent.h"
#include "EnumResource.h"
#include "Math/RandomStream.h"
#include "StaecrafterResource.generated.h"

UCLASS(Blueprintable)
class STAECRAFTER_API AStaecrafterResource : public ACharacter
{
	GENERATED_BODY()
	
public:	
	AStaecrafterResource();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resources)
	int32 ResourcesAvailable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
	UDecalComponent* SelectedDecal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	EEnumResource ResourceType;

	UFUNCTION(BlueprintCallable)
	void PerformTheDestroy();

protected:
	virtual void BeginPlay() override;

private:
	FRandomStream RandStream;

};
