// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataLibrary.h"
#include "CostDataAsset.generated.h"

UCLASS(Blueprintable)
class STAECRAFTER_API UCostDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Content)
	TArray<FCostDataAssetRow> Content;

	UFUNCTION(BlueprintCallable)
	FCostDataAssetRow FindInContent(FString Name);
	
};
