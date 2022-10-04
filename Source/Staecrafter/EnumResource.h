// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumResource.generated.h"

UENUM(BlueprintType)
enum class EEnumResource : uint8
{
	NONE UMETA(DisplayName = "NONE"),
	TREE UMETA(DisplayName = "TREE"),
	ROCK UMETA(DisplayName = "ROCK"),
	FOOD UMETA(DisplayName = "FOOD"),
	TOOL UMETA(DisplayName = "TOOL")
};

UCLASS(Blueprintable)
class STAECRAFTER_API UEnumResourceTranslator : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static FString TranslateResource(EEnumResource Enum)
	{
		switch (Enum)
		{
		case EEnumResource::TREE:
			return TEXT("Tree");
		case EEnumResource::ROCK:
			return TEXT("Rock");
		case EEnumResource::FOOD:
			return TEXT("Food");
		case EEnumResource::TOOL:
			return TEXT("Tool");
		}
		return TEXT("None");
	}
	
	UFUNCTION(BlueprintCallable)
	static FString TranslateTool(EEnumResource Enum)
	{
		switch (Enum)
		{
		case EEnumResource::TREE:
			return TEXT("Axe");
		case EEnumResource::ROCK:
			return TEXT("Pickaxe");
			break;
		}
		return TEXT("None");
	}
	
};
