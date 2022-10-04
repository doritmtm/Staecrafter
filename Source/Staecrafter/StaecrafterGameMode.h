// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Blueprint/UserWidget.h"
#include "StaecrafterGameMode.generated.h"

UCLASS(Blueprintable)
class AStaecrafterGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AStaecrafterGameMode();

	UFUNCTION(BlueprintCallable, Category = "UMG UI")
	void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG UI")
	TSubclassOf<UUserWidget> StartingWidgetClass;

	UPROPERTY()
	UUserWidget* CurrentWidget;
	
};



