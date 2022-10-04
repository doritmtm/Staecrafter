// Fill out your copyright notice in the Description page of Project Settings.


#include "StaecrafterPlayerCameraManager.h"

AStaecrafterPlayerCameraManager::AStaecrafterPlayerCameraManager()
{
	FVector DesiredLocation(-65631.0f, 34172.0f, 3890.0f);
	TheCameraLocation = DesiredLocation;
	FRotator DesiredRotation(-60.0f, 0.0f, 0.0f);
	TheCameraRotation = DesiredRotation;
}

void AStaecrafterPlayerCameraManager::CalcCamera(float DeltaTime, FMinimalViewInfo &OutResult)
{
	Super::CalcCamera(DeltaTime, OutResult);
	OutResult.Location = TheCameraLocation;
	OutResult.Rotation = TheCameraRotation;
}
