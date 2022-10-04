// Fill out your copyright notice in the Description page of Project Settings.


#include "StaecrafterBuilding.h"
#include "StaecrafterGameState.h"
#include "StaecrafterCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

AStaecrafterBuilding::AStaecrafterBuilding()
{
	Health = MaxHealth;
	NrOfUnitsToBeTrained = 0;
	TrainPercent = 0.0f;
	BuildingType = EEnumBuilding::NONE;
	bExecuteOnce = true;
	bIsUnderConstruction = false;
	bIsBeingPlaced = false;
	bWasDestroyed = false;

	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	RootComponent = GetCapsuleComponent();

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	StaticMesh->SetupAttachment(RootComponent);

	SelectedDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal Component"));
	SelectedDecal->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/Staecrafter/Materials/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		SelectedDecal->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	SelectedDecal->DecalSize = FVector(16.0f, 64.0f, 64.0f);
	SelectedDecal->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

}

void AStaecrafterBuilding::BeginPlay()
{
	Super::BeginPlay();
	SelectedDecal->SetVisibility(false);
	RandStream.Initialize(FDateTime::Now().ToUnixTimestamp());
	TheLandscape = dynamic_cast<ALandscape*>(UGameplayStatics::GetActorOfClass(GetWorld(), ALandscape::StaticClass()));
	NextTime = RandStream.RandRange(10.0f, 50.0f);
	NextTime2 = RandStream.RandRange(6.0f, 10.0f);
	Health = MaxHealth;
	PassedTime = 0.0f; PassedTime2 = 0.0f;
	TheFunctionLibrary = GetWorld()->GetGameState<AStaecrafterGameState>()->TheFunctionLibrary;
}

void AStaecrafterBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PassedTime += DeltaTime; PassedTime2 += DeltaTime;
	RandStream.GenerateNewSeed();
	
	if (!bIsBeingPlaced)
	{
		if (bIsUnderConstruction)
		{
			FVector NewScale(GetActorScale3D());
			NewScale.Z += DeltaTime / 10.0f;
			Health += MaxHealth * DeltaTime / 10.0f;
			if (NewScale.Z >= 1.0f || PassedTime >= 10.0f)
			{
				NewScale.Z = 1.0f;
				Health = MaxHealth;
				bIsUnderConstruction = false;
				PassedTime = 0.0f;
			}
			SetActorScale3D(NewScale);
		}
		else
		{
			if (bExecuteOnce)
			{
				CurrentPlayerState = (APlayerState*)GetWorld()->GetGameState<AStaecrafterGameState>()->GetPlayerInfoByType(PlayerType).PlayerState;
				AStaecrafterPlayerState* CurrentPlayerStateCasted = dynamic_cast<AStaecrafterPlayerState*>(CurrentPlayerState);
				CurrentPlayerStateCasted->Buildings.Add(this);
				switch (BuildingType)
				{
				case EEnumBuilding::TOWNHALL:
					CurrentPlayerStateCasted->PopulationLimit += 10;
					break;
				case EEnumBuilding::HOUSE:
					CurrentPlayerStateCasted->PopulationLimit += 5;
					break;
				}

				UMaterialInstanceDynamic* MatInstance = UMaterialInstanceDynamic::Create(StaticMesh->GetMaterial(MaterialIndexToColor), this);
				switch (PlayerType)
				{
				case EEnumPlayer::PLAYER:
					MatInstance->SetVectorParameterValue(TEXT("Albedo Tint"), FLinearColor(0.0f, 0.0f, 1.0f, 1.0f));
					break;
				case EEnumPlayer::AI:
					MatInstance->SetVectorParameterValue(TEXT("Albedo Tint"), FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
					break;
				}
				StaticMesh->SetMaterial(MaterialIndexToColor, MatInstance);
				bExecuteOnce = false;
			}
			AStaecrafterPlayerState* CurrentPlayerStateCasted = dynamic_cast<AStaecrafterPlayerState*>(CurrentPlayerState);
			switch (BuildingType)
			{
			case EEnumBuilding::TOWNHALL:
				if (NrOfUnitsToBeTrained > 0 && CurrentPlayerStateCasted->Population < CurrentPlayerStateCasted->PopulationLimit)
				{
					if (PassedTime >= 20.0f)
					{
						TrainPercent = 1.0f;
						SpawnACharacter();
						PassedTime = 0.0f;
					}
					else
					{
						TrainPercent = PassedTime / 20.0f;
					}
				}
				else
				{
					TrainPercent = 0.0f;
					PassedTime = 0.0f;
				}

				break;
			case EEnumBuilding::BARRACKS:
				if (NrOfUnitsToBeTrained > 0 && CurrentPlayerStateCasted->Population < CurrentPlayerStateCasted->PopulationLimit)
				{
					if (PassedTime >= 15.0f)
					{
						TrainPercent = 1.0f;
						PassedTime = 0.0f;
						SpawnACharacter();
					}
					else
					{
						TrainPercent = PassedTime / 15.0f;
					}
				}
				else
				{
					TrainPercent = 0.0f;
					PassedTime = 0.0f;
				}
				break;
			case EEnumBuilding::FARM:
				if (PassedTime >= NextTime2)
				{
					CurrentPlayerStateCasted->ResourcesFood += 8;
					PassedTime = 0.0f;
					NextTime2 = RandStream.RandRange(6.0f, 10.0f);
				}
				break;
			}
			//AI Control
			if (PlayerType == EEnumPlayer::AI)
			{
				switch (BuildingType)
				{
				case EEnumBuilding::TOWNHALL:
				case EEnumBuilding::BARRACKS:

					if (PassedTime2 >= NextTime)
					{
						if (CurrentPlayerStateCasted->ResourcesFood >= CostDataAsset->FindInContent(TEXT("Unit")).Food &&
							CurrentPlayerStateCasted->Population < CurrentPlayerStateCasted->PopulationLimit)
						{
							NrOfUnitsToBeTrained++;
							CurrentPlayerStateCasted->ResourcesFood -= CostDataAsset->FindInContent(TEXT("Unit")).Food;
						}
						PassedTime2 = 0.0f;
						NextTime = RandStream.RandRange(10.0f, 50.0f);
					}
					break;
				}
			}
		}
	}
	else
	{
		PassedTime = 0.0f;
	}
}

void AStaecrafterBuilding::SpawnACharacter()
{
	FVector2D SpawnPos2D;
	FVector SpawnPos, SpawnPos2;
	int32 SideNr;
	AStaecrafterCharacter* SpawnedCharacter = nullptr;
	FHitResult Hit;
	do
	{
		SideNr = RandStream.RandRange(0, 3);
		switch (SideNr)
		{
		case 0:
			SpawnPos2D.X = RandStream.RandRange(-SelectedDecal->DecalSize.Y * 1.12f, -SelectedDecal->DecalSize.Y * 1.03f);
			SpawnPos2D.Y = RandStream.RandRange(-SelectedDecal->DecalSize.Y * 1.12f, SelectedDecal->DecalSize.Y * 1.12f);
		break;
		case 1:
			SpawnPos2D.X = RandStream.RandRange(-SelectedDecal->DecalSize.Y * 1.12f, SelectedDecal->DecalSize.Y * 1.12f);
			SpawnPos2D.Y = RandStream.RandRange(-SelectedDecal->DecalSize.Y * 1.12f, -SelectedDecal->DecalSize.Y * 1.03f);
		break;
		case 2:
			SpawnPos2D.X = RandStream.RandRange(SelectedDecal->DecalSize.Y * 1.03f, SelectedDecal->DecalSize.Y * 1.12f);
			SpawnPos2D.Y = RandStream.RandRange(-SelectedDecal->DecalSize.Y * 1.12f, SelectedDecal->DecalSize.Y * 1.12f);
		break;
		case 3:
			SpawnPos2D.X = RandStream.RandRange(-SelectedDecal->DecalSize.Y * 1.12f, SelectedDecal->DecalSize.Y * 1.12f);
			SpawnPos2D.Y = RandStream.RandRange(SelectedDecal->DecalSize.Y * 1.03f, SelectedDecal->DecalSize.Y * 1.12f);
		break;
		}
		SpawnPos = GetActorLocation();
		SpawnPos.X += SpawnPos2D.X;
		SpawnPos.Y += SpawnPos2D.Y;
		SpawnPos.Z = 3000.0f;
		SpawnPos2 = SpawnPos;
		SpawnPos2.Z = -3000.0f;
		GetWorld()->LineTraceSingleByChannel(Hit, SpawnPos, SpawnPos2, ECC_WorldDynamic);
		if (Hit.Actor.Get() == TheLandscape)
		{
			TArray<ACharacter*> NearbyActors;
			NearbyActors = TheFunctionLibrary->GetAllActorsWithCollisionInRangeNoExecutionPlanning<ACharacter>(this, Hit.ImpactPoint, 100.0f);
			if (NearbyActors.Num() == 0)
			{
				SpawnedCharacter = GetWorld()->SpawnActor<AStaecrafterCharacter>(CharacterToSpawn, Hit.ImpactPoint, FRotator::ZeroRotator);
			}
		}
	} while (!SpawnedCharacter);
	SpawnedCharacter->PlayerType = PlayerType;
	NrOfUnitsToBeTrained--;
}

void AStaecrafterBuilding::PerformTheDestroy()
{
	bWasDestroyed = true;
	AStaecrafterPlayerState* CurrentPlayerStateCasted = dynamic_cast<AStaecrafterPlayerState*>(CurrentPlayerState);
	switch (BuildingType)
	{
	case EEnumBuilding::TOWNHALL:
		CurrentPlayerStateCasted->PopulationLimit -= 10;
		break;
	case EEnumBuilding::HOUSE:
		CurrentPlayerStateCasted->PopulationLimit -= 5;
		break;
	}
	CurrentPlayerStateCasted->Buildings.Remove(this);
	Destroy();
}

