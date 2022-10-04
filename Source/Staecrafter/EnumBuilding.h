// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumBuilding.generated.h"

UENUM(BlueprintType)
enum class EEnumBuilding : uint8
{
	NONE UMETA(DisplayName="NONE"),
	TOWNHALL UMETA(DisplayName="TOWNHALL"),
	BARRACKS UMETA(Displayname="BARRACKS"),
	FARM UMETA(DisplayName="FARM"),
	HOUSE UMETA(DisplayName="HOUSE")
};

UCLASS(Blueprintable)
class STAECRAFTER_API UEnumBuildingTranslator : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static FString Translate(EEnumBuilding Enum)
	{
		switch (Enum)
		{
		case EEnumBuilding::TOWNHALL:
			return TEXT("Townhall");
		case EEnumBuilding::BARRACKS:
			return TEXT("Barracks");
		case EEnumBuilding::FARM:
			return TEXT("Farm");
		case EEnumBuilding::HOUSE:
			return TEXT("House");
		}
		return TEXT("None");
	}
	
};
