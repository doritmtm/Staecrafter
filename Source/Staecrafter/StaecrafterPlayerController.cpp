// Copyright Epic Games, Inc. All Rights Reserved.

#include "StaecrafterPlayerController.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "Engine/World.h"
#include "StaecrafterGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/Application/SlateApplication.h"

AStaecrafterPlayerController::AStaecrafterPlayerController()
{
	bShowMouseCursor = true;
	bBuildingPlacing = false;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	bDragEnabled = false;
	bShowVictory = false; bShowDefeat = false;
	NrTicks = 0;
}

void AStaecrafterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	RandStream.Initialize(FDateTime::Now().ToUnixTimestamp());
	PassedTime = 0.0f;
	NextTime = RandStream.RandRange(10.0f, 50.0f);
	NrPopulationToAttack = RandStream.RandRange(30, 60);
	NrBuildingToBeSpawned = -1;
	TheLandscape = dynamic_cast<ALandscape*>(UGameplayStatics::GetActorOfClass(GetWorld(), ALandscape::StaticClass()));
	AIPlayerState = GetWorld()->GetGameState<AStaecrafterGameState>()->GetPlayerInfoByType(EEnumPlayer::AI).PlayerState;
	
	FInputModeGameAndUI DesiredInputMode;
	DesiredInputMode.SetHideCursorDuringCapture(false);
	SetInputMode(DesiredInputMode);
	
	ThePlayerCameraManager = dynamic_cast<AStaecrafterPlayerCameraManager*>(PlayerCameraManager);
	SetViewTarget(ThePlayerCameraManager);
	
	TheHUD = dynamic_cast<AStaecrafterHUD*>(GetHUD());
	
	FPlayerInfo PlayerInfo;
	PlayerInfo.PlayerState = GetPlayerState<AStaecrafterPlayerState>();
	PlayerInfo.PlayerType = EEnumPlayer::PLAYER;
	GetWorld()->GetGameState<AStaecrafterGameState>()->PlayerInfo.Add(PlayerInfo);
	ThePlayerState = GetPlayerState<AStaecrafterPlayerState>();
	TheFunctionLibrary = GetWorld()->GetGameState<AStaecrafterGameState>()->TheFunctionLibrary;

}

void AStaecrafterPlayerController::Tick(float DeltaTime)
{
	PassedTime += DeltaTime;
	RandStream.GenerateNewSeed();
	TheFunctionLibrary->Tick(DeltaTime);
	MoveCameraAccordingToMouse(DeltaTime);

	if (bBuildingPlacing)
	{
		BuildingPlacingMove();
	}
	if (bDragEnabled)
	{
		ContinueDrag();
	}
	if (AIPlayerState->Buildings.Num() == 0 && NrTicks > 2)
	{
		bShowVictory = true;
	}
	if (ThePlayerState->Buildings.Num() == 0 && NrTicks > 2)
	{
		bShowDefeat = true;
	}
	if (!bShowVictory && !bShowDefeat)
	{
		AIControlTick();
	}
	
	NrTicks++;
	
}

void AStaecrafterPlayerController::MoveCameraAccordingToMouse(float DeltaTime)
{
	float LocationChangeValue = 4000.0f * DeltaTime;
	float MouseX, MouseY;
	GetMousePosition(MouseX, MouseY);
	int32 ViewportX, ViewportY;
	GetViewportSize(ViewportX, ViewportY);
	int32 IMouseX = MouseX, IMouseY = MouseY;
	float ZoomFactor = ZoomLevel;
	if (ZoomFactor < 0.25f)
	{
		ZoomFactor = 0.25f;
	}
	if (FSlateApplication::Get().IsActive())
	{
		if (0 <= IMouseX && IMouseX <= 50)
		{
			FVector LocationChange(0.0, -LocationChangeValue, 0.0);
			ThePlayerCameraManager->TheCameraLocation = ThePlayerCameraManager->TheCameraLocation + LocationChange * ZoomFactor;
		}
		if (ViewportX - 50 <= IMouseX && IMouseX <= ViewportX)
		{
			FVector LocationChange(0.0, LocationChangeValue, 0.0);
			ThePlayerCameraManager->TheCameraLocation = ThePlayerCameraManager->TheCameraLocation + LocationChange * ZoomFactor;
		}
		if (0 <= IMouseY && IMouseY <= 50)
		{
			FVector LocationChange(LocationChangeValue, 0.0, 0.0);
			ThePlayerCameraManager->TheCameraLocation = ThePlayerCameraManager->TheCameraLocation + LocationChange * ZoomFactor;
		}
		if (ViewportY - 50 <= IMouseY && IMouseY <= ViewportY)
		{
			FVector LocationChange(-LocationChangeValue, 0.0, 0.0);
			ThePlayerCameraManager->TheCameraLocation = ThePlayerCameraManager->TheCameraLocation + LocationChange * ZoomFactor;
		}
	}

}

void AStaecrafterPlayerController::ZoomCamera(float AxisValue)
{
	ZoomLevel = ThePlayerCameraManager->TheCameraLocation.Z / 3890.0f;
	FVector LocationChange(150.0, 0.0, -130);
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_WorldDynamic, false, Hit);
	LocationChange = Hit.ImpactPoint - ThePlayerCameraManager->TheCameraLocation;
	LocationChange.Normalize();
	ThePlayerCameraManager->TheCameraLocation = ThePlayerCameraManager->TheCameraLocation + LocationChange * AxisValue * 100.0f;
}

void AStaecrafterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAction("Train", IE_Pressed, this, &AStaecrafterPlayerController::TrainUnit);
	InputComponent->BindAction("Stop", IE_Pressed, this, &AStaecrafterPlayerController::StopTheMovement);
	InputComponent->BindAction("SetDestination", IE_Pressed, this, &AStaecrafterPlayerController::MoveCommand);
	InputComponent->BindAction("Drag", IE_Pressed, this, &AStaecrafterPlayerController::StartDrag);
	InputComponent->BindAction("Drag", IE_Released, this, &AStaecrafterPlayerController::StopDrag);
	InputComponent->BindAction("Select", IE_Pressed, this, &AStaecrafterPlayerController::SelectAPawn);
	InputComponent->BindAction("Place", IE_Pressed, this, &AStaecrafterPlayerController::PlaceBuilding);
	InputComponent->BindAction("Collect", IE_Pressed, this, &AStaecrafterPlayerController::CollectResource);
	InputComponent->BindAction("Attack", IE_Pressed, this, &AStaecrafterPlayerController::AttackCommand);
	
	InputComponent->BindAxis("Zoom", this, &AStaecrafterPlayerController::ZoomCamera);
	
}

void AStaecrafterPlayerController::StopTheMovement()
{
	for (AStaecrafterCharacter* SC : SelectedPawns)
	{
		if (SC && !SC->bWasDestroyed)
		{
			SC->CurrentMission = EEnumMission::NONE;
			SC->GetController()->StopMovement();
		}
	}
}

void AStaecrafterPlayerController::SelectAPawn()
{
	for (AStaecrafterCharacter* S : SelectedPawns)
	{
		if (S && !S->bWasDestroyed)
		{
			S->SelectedDecal->SetVisibility(false);
		}
	}
	SelectedPawns.Empty();
	TheHUD->SelectedPawns.Empty();
	if (SelectedResource)
	{
		SelectedResource->SelectedDecal->SetVisibility(false);
		SelectedResource = nullptr;
	}
	if (SelectedBuilding)
	{
		SelectedBuilding->SelectedDecal->SetVisibility(false);
		SelectedBuilding = nullptr;
	}
	
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_WorldDynamic, false, Hit);
	AStaecrafterCharacter* PawnToSelect1 = dynamic_cast<AStaecrafterCharacter*>(Hit.Actor.Get());
	if (PawnToSelect1 && PawnToSelect1->PlayerType == EEnumPlayer::PLAYER)
	{
		SelectedPawns.Add(PawnToSelect1);
		PawnToSelect1->SelectedDecal->SetVisibility(true);
	}
	if (AStaecrafterResource* PawnToSelect2 = dynamic_cast<AStaecrafterResource*>(Hit.Actor.Get()))
	{
		SelectedResource = PawnToSelect2;
		PawnToSelect2->SelectedDecal->SetVisibility(true);
	}
	AStaecrafterBuilding* PawnToSelect3 = dynamic_cast<AStaecrafterBuilding*>(Hit.Actor.Get());
	if (PawnToSelect3 && PawnToSelect3->PlayerType == EEnumPlayer::PLAYER)
	{
		SelectedBuilding = PawnToSelect3;
		PawnToSelect3->SelectedDecal->SetVisibility(true);
	}
	
}

void AStaecrafterPlayerController::StartDrag()
{
	float MouseX, MouseY;
	GetMousePosition(MouseX, MouseY);
	StartDragX = MouseX; StartDragY = MouseY;
	TheHUD->StartDragX = MouseX; TheHUD->StartDragY = MouseY;
	bDragEnabled = true;
	TheHUD->bDragEnabled = true;
	for (AStaecrafterCharacter* S : SelectedPawns)
	{
		if (S && !S->bWasDestroyed)
		{
			S->SelectedDecal->SetVisibility(false);
		}
	}
	SelectedPawns.Empty();
}

void AStaecrafterPlayerController::ContinueDrag()
{
	float MouseX, MouseY;
	GetMousePosition(MouseX, MouseY);
	ContinueDragX = MouseX; ContinueDragY = MouseY;
	TheHUD->ContinueDragX = MouseX; TheHUD->ContinueDragY = MouseY;
}

void AStaecrafterPlayerController::StopDrag()
{
	float MouseX, MouseY;
	GetMousePosition(MouseX, MouseY);
	
	ContinueDragX = MouseX; ContinueDragY = MouseY;
	TheHUD->ContinueDragX = MouseX; TheHUD->ContinueDragY = MouseY;
	StopDragX = MouseX; StopDragY = MouseY;
	TheHUD->StopDragX = MouseX; TheHUD->StopDragY = MouseY;
	bDragEnabled = false;
	TheHUD->bDragEnabled = false;
	
	for (AStaecrafterCharacter* S : TheHUD->SelectedPawns)
	{
		if (S && !S->bWasDestroyed)
		{
			if (S->PlayerType == EEnumPlayer::PLAYER)
			{
				SelectedPawns.Add(S);
			}
		}
	}
	for (AStaecrafterCharacter* S : SelectedPawns)
	{
		if (S && !S->bWasDestroyed)
		{
			S->SelectedDecal->SetVisibility(true);
		}
	}
	
}

void AStaecrafterPlayerController::AttackCommand()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_WorldDynamic, false, Hit);
	
	AStaecrafterCharacter* SelectedC = dynamic_cast<AStaecrafterCharacter*>(Hit.Actor.Get());
	if (SelectedC && SelectedC->PlayerType == EEnumPlayer::AI)
	{
		SelectedCharacter = SelectedC;
		for (AStaecrafterCharacter* S : SelectedPawns)
		{
			if (S && !S->bWasDestroyed)
			{
				S->CharacterToAttack = SelectedC;
				S->CurrentMission = EEnumMission::ATTACK;
				TheFunctionLibrary->MoveToActorCollision(S->GetController<AAIController>(), S, SelectedC, 100.0f);
			}
		}
		AnimateAttackOrMoveCommand();
	}
	
	AStaecrafterBuilding* SelectedB = dynamic_cast<AStaecrafterBuilding*>(Hit.Actor.Get());
	if (SelectedB && SelectedB->PlayerType == EEnumPlayer::AI)
	{
		SelectedBuilding = SelectedB;
		for (AStaecrafterCharacter* S : SelectedPawns)
		{
			if (S && !S->bWasDestroyed)
			{
				S->BuildingToAttack = SelectedB;
				S->CurrentMission = EEnumMission::ATTACK;
				TheFunctionLibrary->MoveToActorCollision(S->GetController<AAIController>(), S, SelectedB, 100.0f);
			}
		}
		AnimateAttackOrMoveCommand();
	}
	
}

void AStaecrafterPlayerController::AnimateAttackOrMoveCommand()
{
	if (SelectedCharacter)
	{
		SelectedCharacter->SelectedDecal->SetVisibility(true);
		FTimerHandle Timer1, Timer2, Timer3;
		GetWorldTimerManager().SetTimer(Timer1, [this] {if (SelectedCharacter) { SelectedCharacter->SelectedDecal->SetVisibility(false); }}, 0.33f, false);
		GetWorldTimerManager().SetTimer(Timer2, [this] {if (SelectedCharacter) { SelectedCharacter->SelectedDecal->SetVisibility(true); }}, 0.66f, false);
		GetWorldTimerManager().SetTimer(Timer3, [this] {if (SelectedCharacter) { SelectedCharacter->SelectedDecal->SetVisibility(false); }}, 1.0f, false);
	}
	
	if (SelectedBuilding)
	{
		SelectedBuilding->SelectedDecal->SetVisibility(true);
		FTimerHandle Timer1, Timer2, Timer3;
		GetWorldTimerManager().SetTimer(Timer1, [this] {if (SelectedBuilding) { SelectedBuilding->SelectedDecal->SetVisibility(false); }}, 0.33f, false);
		GetWorldTimerManager().SetTimer(Timer2, [this] {if (SelectedBuilding) { SelectedBuilding->SelectedDecal->SetVisibility(true); }}, 0.66f, false);
		GetWorldTimerManager().SetTimer(Timer3, [this] {if (SelectedBuilding) { SelectedBuilding->SelectedDecal->SetVisibility(false); }}, 1.0f, false);
	}
	
}

void AStaecrafterPlayerController::CollectResource()
{
	if (SelectedResource)
	{
		SelectedResource->SelectedDecal->SetVisibility(false);
		SelectedResource = nullptr;
	}
	
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_WorldDynamic, false, Hit);
	if (AStaecrafterResource* Selected = dynamic_cast<AStaecrafterResource*>(Hit.Actor.Get()))
	{
		SelectedResource = Selected;
		AnimateCollectResource();
	}
	
	if (SelectedResource)
	{
		for (int32 i = 0; i < SelectedPawns.Num(); i++)
		{
			if (SelectedPawns[i] && !SelectedPawns[i]->bWasDestroyed)
			{
				AAIController* PawnsAIController = dynamic_cast<AAIController*>(SelectedPawns[i]->GetController());
				TheFunctionLibrary->MoveToActorCollision(SelectedPawns[i]->GetController<AAIController>(), SelectedPawns[i], SelectedResource, 100.0f);
				SelectedPawns[i]->CurrentMission = EEnumMission::COLLECT;
				SelectedPawns[i]->ResourceToCollect = SelectedResource;
			}
		}
	}
	
}

void AStaecrafterPlayerController::AnimateCollectResource()
{
	if (SelectedResource)
	{
		SelectedResource->SelectedDecal->SetVisibility(true);
		FTimerHandle Timer1,Timer2,Timer3;
		GetWorldTimerManager().SetTimer(Timer1, [this] {if (SelectedResource) { SelectedResource->SelectedDecal->SetVisibility(false); }}, 0.33f, false);
		GetWorldTimerManager().SetTimer(Timer2, [this] {if (SelectedResource) { SelectedResource->SelectedDecal->SetVisibility(true); }}, 0.66f, false);
		GetWorldTimerManager().SetTimer(Timer3, [this] {if (SelectedResource) { SelectedResource->SelectedDecal->SetVisibility(false); }}, 1.0f, false);
	}
}

void AStaecrafterPlayerController::MoveCommand()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_WorldDynamic, false, Hit);

	if (Hit.bBlockingHit)
	{
		AAIController* PawnsAIController;
		AStaecrafterBuilding* HitBuilding;
		HitBuilding = dynamic_cast<AStaecrafterBuilding*>(Hit.Actor.Get());
		if (HitBuilding && HitBuilding->PlayerType == EEnumPlayer::PLAYER)
		{
			SelectedBuilding = HitBuilding;
			for (int32 i = 0; i < SelectedPawns.Num(); i++)
			{
				if (SelectedPawns[i] && !SelectedPawns[i]->bWasDestroyed)
				{
					PawnsAIController = dynamic_cast<AAIController*>(SelectedPawns[i]->GetController());
					TheFunctionLibrary->MoveToActorCollision(PawnsAIController, SelectedPawns[i], HitBuilding, 100.0f);
					SelectedPawns[i]->CurrentMission = EEnumMission::MOVE;
				}
			}
			AnimateAttackOrMoveCommand();
		}
		else
		{
			for (int32 i = 0; i < SelectedPawns.Num(); i++)
			{
				if (SelectedPawns[i] && !SelectedPawns[i]->bWasDestroyed)
				{
					PawnsAIController = dynamic_cast<AAIController*>(SelectedPawns[i]->GetController());
					PawnsAIController->MoveToLocation(Hit.ImpactPoint, 100.0f);
					SelectedPawns[i]->CurrentMission = EEnumMission::MOVE;
				}
			}
		}
	}
	
}

void AStaecrafterPlayerController::TrainUnit()
{
	if (SelectedBuilding)
	{
		if (SelectedBuilding->BuildingType == EEnumBuilding::TOWNHALL || SelectedBuilding->BuildingType == EEnumBuilding::BARRACKS)
		{
			if (SelectedBuilding->PlayerType == EEnumPlayer::PLAYER)
			{
				if (ThePlayerState->ResourcesFood >= CostDataAsset->FindInContent(TEXT("Unit")).Food &&
					ThePlayerState->Population < ThePlayerState->PopulationLimit)
				{
					SelectedBuilding->NrOfUnitsToBeTrained++;
					ThePlayerState->ResourcesFood -= CostDataAsset->FindInContent(TEXT("Unit")).Food;
				}
			}
		}
	}
}

void AStaecrafterPlayerController::TriggerBuildingPlacing(FString BuildingName)
{
	if (BuildingToBePlaced)
	{
		BuildingToBePlaced->Destroy();
		BuildingToBePlaced = nullptr;
	}
	
	UClass* BuildingToBeSpawned = CostDataAsset->FindInContent(BuildingName).TheClass;
	if (ThePlayerState->ResourcesWood >= CostDataAsset->FindInContent(BuildingName).Wood &&
		ThePlayerState->ResourcesStone >= CostDataAsset->FindInContent(BuildingName).Stone)
	{
		FVector TheSpawnLocation(-1000.0f, -1000.0f, -1000.0f);
		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(BuildingToBeSpawned, TheSpawnLocation, FRotator::ZeroRotator);
		BuildingToBePlaced = SpawnedActor;
		BuildingToBePlacedName = BuildingName;
		bBuildingPlacing = true;
		
		AStaecrafterBuilding* BuildingToBePlacedCasted = dynamic_cast<AStaecrafterBuilding*>(BuildingToBePlaced);
		BuildingToBePlacedCasted->StaticMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		BuildingToBePlacedCasted->bIsBeingPlaced = true;
	}
	
}

void AStaecrafterPlayerController::BuildingPlacingMove()
{
	FHitResult Hit;
	if (BuildingToBePlaced)
	{
		FVector pos(0.0, 0.0, -1000.0);
		BuildingToBePlaced->SetActorLocation(pos);
		GetHitResultUnderCursor(ECC_WorldDynamic, false, Hit);
		BuildingToBePlaced->SetActorLocation(Hit.ImpactPoint);
		
		if (Hit.Actor.Get() != TheLandscape)
		{
			BuildingToBePlaced->SetActorLocation(pos);
		}
		else
		{
			TArray<AActor*> OverlappingActors;
			BuildingToBePlaced->GetOverlappingActors(OverlappingActors);
			uint32 Ok = true;

			for (AActor* A : OverlappingActors)
			{
				if (A != TheLandscape)
				{
					Ok = false;
					break;
				}
			}
			
			if (!Ok)
			{
				BuildingToBePlaced->SetActorLocation(pos);
			}
		}
	}
}

void AStaecrafterPlayerController::CancelBuildingPlacing()
{
	bBuildingPlacing = false;
	if (BuildingToBePlaced)
	{
		BuildingToBePlaced->Destroy();
		BuildingToBePlaced = nullptr;
	}
}

void AStaecrafterPlayerController::PlaceBuilding()
{
	FVector pos(0.0, 0.0, -1000.0);
	if (bBuildingPlacing && BuildingToBePlaced && !BuildingToBePlaced->GetActorLocation().Equals(pos))
	{
		AStaecrafterBuilding* BuildingToBePlacedCasted = dynamic_cast<AStaecrafterBuilding*>(BuildingToBePlaced);
		BuildingToBePlacedCasted->bIsUnderConstruction = true;
		BuildingToBePlacedCasted->bIsBeingPlaced = false;
		BuildingToBePlacedCasted->Health = BuildingToBePlacedCasted->MaxHealth / 100.0f;
		
		FVector NewScale(BuildingToBePlaced->GetActorScale3D());
		NewScale.Z = 0.01f;
		BuildingToBePlaced->SetActorScale3D(NewScale);
		bBuildingPlacing = false;
		
		ThePlayerState->ResourcesWood -= CostDataAsset->FindInContent(BuildingToBePlacedName).Wood;
		ThePlayerState->ResourcesStone -= CostDataAsset->FindInContent(BuildingToBePlacedName).Stone;
		
		BuildingToBePlacedCasted->StaticMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		BuildingToBePlaced = nullptr;
		
		if (SelectedBuilding)
		{
			SelectedBuilding->SelectedDecal->SetVisibility(false);
			SelectedBuilding = nullptr;
		}
	}
}

void AStaecrafterPlayerController::AIControlTick()
{
	//Spawn building
	if (PassedTime >= NextTime)
	{
		SpawnABuilding();
		PassedTime = 0.0f;
		NextTime = RandStream.RandRange(10.0f, 50.0f);
	}
	//First Attack timeout
	if (AIPlayerState->Population >= NrPopulationToAttack)
	{
		AIPlayerState->bStartAttacking = true;
	}
	else
	{
		AIPlayerState->bStartAttacking = false;
	}
}

void AStaecrafterPlayerController::SpawnABuilding()
{
	FVector2D SpawnPos2D;
	FVector SpawnPos, SpawnPos2;
	AStaecrafterBuilding* SpawnedBuilding = nullptr;
	FHitResult Hit;
	if (NrBuildingToBeSpawned == -1)
	{
		AStaecrafterBuilding* FoundTownhall = nullptr;
		for (AStaecrafterBuilding* SB : AIPlayerState->Buildings)
		{
			if (SB->BuildingType == EEnumBuilding::TOWNHALL)
			{
				FoundTownhall = SB;
			}
		}


		int32 RandomNumber = RandStream.RandRange(0, 100);
		NrBuildingToBeSpawned = 3;
		if (FoundTownhall)
		{
			if (AIPlayerState->PopulationLimit - AIPlayerState->Population >= 5)
			{
				if (0 <= RandomNumber && RandomNumber < 10)
				{
					NrBuildingToBeSpawned = 0;
				}

				if (10 <= RandomNumber && RandomNumber < 30)
				{
					NrBuildingToBeSpawned = 1;
				}

				if (30 <= RandomNumber && RandomNumber < 60)
				{
					NrBuildingToBeSpawned = 2;
				}

				if (60 <= RandomNumber && RandomNumber <= 100)
				{
					NrBuildingToBeSpawned = 3;
				}
			}
		}
		else
		{
			NrBuildingToBeSpawned = 0;
		}
	}

	if (AIPlayerState->ResourcesWood >= CostDataAsset->Content[NrBuildingToBeSpawned].Wood &&
		AIPlayerState->ResourcesStone >= CostDataAsset->Content[NrBuildingToBeSpawned].Stone)
	{
		do
		{
			SpawnPos2D.X = RandStream.RandRange(-6000.0f, 6000.0f);
			SpawnPos2D.Y = RandStream.RandRange(-6000.0f, 6000.0f);
			SpawnPos = AIPlayerState->Buildings[RandStream.RandRange(0, AIPlayerState->Buildings.Num() - 1)]->GetActorLocation();
			SpawnPos.X += SpawnPos2D.X;
			SpawnPos.Y += SpawnPos2D.Y;
			SpawnPos.Z = 3000.0f;
			SpawnPos2 = SpawnPos;
			SpawnPos2.Z = -3000.0f;
			
			GetWorld()->LineTraceSingleByChannel(Hit, SpawnPos, SpawnPos2, ECC_WorldDynamic);
			
			TArray<ACharacter*> NearbyActors;
			if (Hit.Actor.Get() == TheLandscape)
			{
				float ScanRange = CostDataAsset->Content[NrBuildingToBeSpawned].DecalSize / 2.0f + 300.0f;
				NearbyActors = TheFunctionLibrary->GetAllActorsWithCollisionInRangeNoExecutionPlanning<ACharacter>(TheLandscape, Hit.ImpactPoint, ScanRange);
				if (NearbyActors.Num() == 0)
				{
					SpawnedBuilding = GetWorld()->SpawnActor<AStaecrafterBuilding>(CostDataAsset->Content[NrBuildingToBeSpawned].TheClass, Hit.ImpactPoint, FRotator::ZeroRotator);
				}
			}
			
		} while (!SpawnedBuilding);
		
		SpawnedBuilding->PlayerType = EEnumPlayer::AI;
		SpawnedBuilding->bIsUnderConstruction = true;
		SpawnedBuilding->Health = SpawnedBuilding->MaxHealth / 100.0f;
		
		FVector NewScale(SpawnedBuilding->GetActorScale3D());
		NewScale.Z = 0.01f;
		SpawnedBuilding->SetActorScale3D(NewScale);
		
		AIPlayerState->ResourcesWood -= CostDataAsset->Content[NrBuildingToBeSpawned].Wood;
		AIPlayerState->ResourcesStone -= CostDataAsset->Content[NrBuildingToBeSpawned].Stone;

		NrBuildingToBeSpawned = -1;
	}
	
}
