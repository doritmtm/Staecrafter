// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "StaecrafterPlayerCameraManager.generated.h"

UCLASS(Blueprintable)
class STAECRAFTER_API AStaecrafterPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	AStaecrafterPlayerCameraManager();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	FVector TheCameraLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	FRotator TheCameraRotation;

protected:
	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult);

};
