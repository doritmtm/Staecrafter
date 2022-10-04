// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataLibrary.h"
#include "MeshDataAsset.generated.h"

UCLASS(Blueprintable)
class STAECRAFTER_API UMeshDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Content)
	TArray<FMeshDataAssetRow> Content;

	UFUNCTION(BlueprintCallable)
	FMeshDataAssetRow FindInContent(FString Name);
	
};
