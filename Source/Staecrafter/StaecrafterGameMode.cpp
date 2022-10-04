// Copyright Epic Games, Inc. All Rights Reserved.

#include "StaecrafterGameMode.h"
#include "StaecrafterPlayerController.h"
#include "StaecrafterCharacter.h"
#include "GameFramework/GameUserSettings.h"
#include "UObject/ConstructorHelpers.h"

AStaecrafterGameMode::AStaecrafterGameMode()
{
	PlayerControllerClass = AStaecrafterPlayerController::StaticClass();
}

void AStaecrafterGameMode::BeginPlay()
{
	Super::BeginPlay();
	ChangeMenuWidget(StartingWidgetClass);
	if (GEngine)
	{
		UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
		GameUserSettings->SetScreenResolution(GameUserSettings->GetDesktopResolution());
		GameUserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
		GameUserSettings->SetVSyncEnabled(true);
		GameUserSettings->ApplySettings(false);
	}
}

void AStaecrafterGameMode::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}
	if (NewWidgetClass)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidgetClass);
		if (CurrentWidget)
		{
			CurrentWidget->AddToViewport();
		}
	}
}
