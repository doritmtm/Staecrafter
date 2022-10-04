// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumMission.generated.h"

UENUM(BlueprintType)
enum class EEnumMission : uint8
{
	NONE UMETA(DisplayName="NONE"),
	COLLECT UMETA(DisplayName="COLLECT"),
	MOVE UMETA(Displayname="MOVE"),
	ATTACK UMETA(DisplayName="ATTACK")
};

UCLASS(Blueprintable)
class STAECRAFTER_API UEnumMissionTranslator : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static FString Translate(EEnumMission Enum)
	{
		switch (Enum)
		{
		case EEnumMission::COLLECT:
			return TEXT("Collect");
		case EEnumMission::MOVE:
			return TEXT("Move");
		case EEnumMission::ATTACK:
			return TEXT("Attack");
		}
		return TEXT("None");
	}
	
};
