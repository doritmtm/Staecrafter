// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataTable.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DataLibrary.generated.h"

UCLASS(Blueprintable)
class STAECRAFTER_API UDataLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
};

USTRUCT(BlueprintType)
struct FMeshDataAssetRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Row Data")
	FString Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Row Data")
	UStaticMesh* Mesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Properties")
	FVector RelativeLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Properties")
	FRotator Rotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Properties")
	FVector Scale = FVector(1.0f, 1.0f, 1.0f);
	
};

USTRUCT(BlueprintType)
struct FCostDataAssetRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Row Data")
	FString Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class Data")
	UClass* TheClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal Data")
	int32 DecalSize;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cost Data")
	int32 Wood;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cost Data")
	int32 Stone;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cost Data")
	int32 Food;
	
};
