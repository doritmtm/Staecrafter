// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumPlayer.generated.h"

UENUM(BlueprintType)
enum class EEnumPlayer : uint8
{
	NONE UMETA(DisplayName = "NONE"),
	PLAYER UMETA(DisplayName = "PLAYER"),
	AI UMETA(Displayname = "AI")
};

UCLASS(Blueprintable)
class STAECRAFTER_API UEnumPlayerTranslator : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static FString Translate(EEnumPlayer Enum)
	{
		switch (Enum)
		{
		case EEnumPlayer::PLAYER:
			return TEXT("Player");
		case EEnumPlayer::AI:
			return TEXT("AI");
			break;
		}
		return TEXT("None");
	}
	
};

