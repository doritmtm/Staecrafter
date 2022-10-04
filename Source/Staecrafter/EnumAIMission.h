// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumAIMission.generated.h"

UENUM(BlueprintType)
enum class EEnumAIMission : uint8
{
	NONE UMETA(DisplayName = "NONE"),
	COLLECT UMETA(DisplayName = "COLLECT"),
	ATTACK UMETA(DisplayName = "ATTACK")
};

UCLASS(Blueprintable)
class STAECRAFTER_API UEnumAIMissionTranslator : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static FString Translate(EEnumAIMission Enum)
	{
		switch (Enum)
		{
		case EEnumAIMission::COLLECT:
			return TEXT("Collect");
		case EEnumAIMission::ATTACK:
			return TEXT("Attack");
		}
		return TEXT("None");
	}
	
};
