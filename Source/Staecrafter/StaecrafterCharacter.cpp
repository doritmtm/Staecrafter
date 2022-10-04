// Copyright Epic Games, Inc. All Rights Reserved.

#include "StaecrafterCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "StaecrafterGameState.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AStaecrafterCharacter::AStaecrafterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	Health = MaxHealth;
	CurrentMission = EEnumMission::NONE;
	ResourceCarriedType = EEnumResource::NONE;
	AIMission = EEnumAIMission::NONE;
	bAIAttacking = false;
	bExecuteOnce = true;
	bWasDestroyed = false;
	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	SelectedDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("Selected Decal"));
	SelectedDecal->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/Staecrafter/Materials/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		SelectedDecal->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	SelectedDecal->DecalSize = FVector(16.0f, 64.0f, 64.0f);
	SelectedDecal->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	ResourceCarried = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Resource"));
	ResourceCarried->SetupAttachment(RootComponent);
}

void AStaecrafterCharacter::BeginPlay()
{
	Super::BeginPlay();
	RandStream.Initialize(FDateTime::Now().ToUnixTimestamp());
	uint32 RandomPercent = RandStream.RandRange(0, 100);
	if (0 <= RandomPercent && RandomPercent < 10)
	{
		AIMission = EEnumAIMission::NONE;
	}
	if (10 <= RandomPercent && RandomPercent < 50)
	{
		AIMission = EEnumAIMission::COLLECT;
	}
	if (50 <= RandomPercent && RandomPercent <= 100)
	{
		AIMission = EEnumAIMission::ATTACK;
	}
	SelectedDecal->SetVisibility(false);
	NextTime = RandStream.RandRange(10.0f, 50.0f);
	Health = MaxHealth;
	PassedTime = 0.0f; PassedTime2 = 0.0f; PassedTime3 = 0.0f; PassedTime4 = 0.0f;
	TheFunctionLibrary = GetWorld()->GetGameState<AStaecrafterGameState>()->TheFunctionLibrary;
}

void AStaecrafterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PassedTime += DeltaTime; PassedTime2 += DeltaTime; PassedTime3 += DeltaTime; PassedTime4 += DeltaTime;
	RandStream.GenerateNewSeed();
	if (CharacterToAttack)
	{
		if (CharacterToAttack->bWasDestroyed)
		{
			CharacterToAttack = nullptr;
		}
	}
	if (BuildingToAttack)
	{
		if (BuildingToAttack->bWasDestroyed)
		{
			BuildingToAttack = nullptr;
		}
	}
	if (bExecuteOnce)
	{
		CurrentPlayerState = GetWorld()->GetGameState<AStaecrafterGameState>()->GetPlayerInfoByType(PlayerType).PlayerState;
		CurrentPlayerState->Population++;

		if (PlayerType == EEnumPlayer::AI)
		{
			if (CurrentPlayerState->Population < 20)
			{
				AIMission = EEnumAIMission::COLLECT;
			}
		}

		UMaterialInstanceDynamic* MatInstance = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(MaterialIndexToColor), this);
		switch (PlayerType)
		{
		case EEnumPlayer::PLAYER:
			MatInstance->SetVectorParameterValue(TEXT("Albedo Tint"), FLinearColor(0.0f, 0.0f, 1.0f, 1.0f));
			break;
		case EEnumPlayer::AI:
			MatInstance->SetVectorParameterValue(TEXT("Albedo Tint"), FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
			break;
		}
		GetMesh()->SetMaterial(MaterialIndexToColor, MatInstance);

		bExecuteOnce = false;
	}
	HandleMission();
}

void AStaecrafterCharacter::HandleMission()
{
	FVector DistanceVector;
	switch (CurrentMission) {
	case EEnumMission::COLLECT:
		ClosestTownhall = nullptr;
		for (AStaecrafterBuilding* B : CurrentPlayerState->Buildings)
		{
			if (B->BuildingType == EEnumBuilding::TOWNHALL && B->PlayerType == PlayerType)
			{
				if (!ClosestTownhall)
				{
					ClosestTownhall = B;
				}
				else
				{
					FVector DistanceVector1, DistanceVector2;
					float Distance1, Distance2;
					DistanceVector1 = B->GetActorLocation() - GetActorLocation();
					Distance1 = DistanceVector1.Size();
					DistanceVector2 = ClosestTownhall->GetActorLocation() - GetActorLocation();
					Distance2 = DistanceVector2.Size();
					if (Distance1 < Distance2)
					{
						ClosestTownhall = B;
					}
				}
			}
		}
		if (ClosestTownhall)
		{
			if (GetController()->IsFollowingAPath())
			{
				PassedTime = 0.0f;
				if (ResourceCarriedType == EEnumResource::NONE || ResourceCarriedType == EEnumResource::TOOL)
				{
					if (!ResourceToCollect)
					{
						GetController()->StopMovement();
						ResourceCarried->SetStaticMesh(nullptr);
						ResourceCarriedType = EEnumResource::NONE;
						CurrentMission = EEnumMission::NONE;
						PassedTime = 0.0f;
					}
				}
			}
			else
			{
				AStaecrafterResource* MovingToActorResource;
				MovingToActorResource = dynamic_cast<AStaecrafterResource*>(MovingToActor);
				AStaecrafterBuilding* MovingToActorBuilding;
				MovingToActorBuilding = dynamic_cast<AStaecrafterBuilding*>(MovingToActor);
				FString ResourceName;
				UMeshDataAsset* MeshDataAssetCasted = dynamic_cast<UMeshDataAsset*>(MeshDataAsset);
				FMeshDataAssetRow MeshData;
				if (ResourceToCollect)
				{
					if (MovingToActorResource)
					{
						if (TheFunctionLibrary->GetActorDistanceToCollision(this, MovingToActorResource) <= 500.0f)
						{
							if (ResourceCarriedType == EEnumResource::NONE || ResourceCarriedType == EEnumResource::TOOL)
							{
								ResourceName = TEXT("Resource");
								ResourceName += UEnumResourceTranslator::TranslateTool(ResourceToCollect->ResourceType);
								MeshData = MeshDataAssetCasted->FindInContent(ResourceName);
								ResourceCarried->SetStaticMesh(MeshData.Mesh);
								ResourceCarried->SetRelativeLocation(MeshData.RelativeLocation);
								ResourceCarried->SetRelativeRotation(MeshData.Rotation);
								ResourceCarried->SetRelativeScale3D(MeshData.Scale);
								ResourceCarriedType = EEnumResource::TOOL;
								if (PassedTime >= 5.0f)
								{
									if (ResourceToCollect->ResourcesAvailable > 0)
									{
										if (ResourceToCollect->ResourcesAvailable >= 8)
										{
											ResourceCarriedAmount = 8;
										}
										else
										{
											ResourceCarriedAmount = ResourceToCollect->ResourcesAvailable;
										}
										ResourceToCollect->ResourcesAvailable -= ResourceCarriedAmount;
										ResourceName = TEXT("Resource");
										ResourceName += UEnumResourceTranslator::TranslateResource(ResourceToCollect->ResourceType);
										MeshData = MeshDataAssetCasted->FindInContent(ResourceName);
										ResourceCarried->SetStaticMesh(MeshData.Mesh);
										ResourceCarried->SetRelativeLocation(MeshData.RelativeLocation);
										ResourceCarried->SetRelativeRotation(MeshData.Rotation);
										ResourceCarried->SetRelativeScale3D(MeshData.Scale);
										ResourceCarriedType = ResourceToCollect->ResourceType;
										PassedTime = 0.0f;
										TheFunctionLibrary->MoveToActorCollision(GetController<AAIController>(), this, ClosestTownhall, 100.0f);
									}
									if (ResourceToCollect->ResourcesAvailable <= 0)
									{
										ResourceToCollect->PerformTheDestroy();
										PassedTime = 0.0f;
									}
									PassedTime = 0.0f;
								}
							}
							else
							{
								TheFunctionLibrary->MoveToActorCollision(GetController<AAIController>(), this, ClosestTownhall);
							}
						}
						else
						{
							CurrentMission = EEnumMission::NONE;
							PassedTime = 0.0f;
						}
					}
					if (MovingToActorBuilding)
					{
						if (TheFunctionLibrary->GetActorDistanceToCollision(this, MovingToActorBuilding) <= 500.0f)
						{
							ResourceCarried->SetStaticMesh(NULL);
							switch (ResourceCarriedType)
							{
							case EEnumResource::TREE:
								CurrentPlayerState->ResourcesWood += ResourceCarriedAmount;
								break;
							case EEnumResource::ROCK:
								CurrentPlayerState->ResourcesStone += ResourceCarriedAmount;
								break;
							}
							ResourceCarriedType = EEnumResource::NONE;
							ResourceCarriedAmount = 0;
							TheFunctionLibrary->MoveToActorCollision(GetController<AAIController>(), this, ResourceToCollect, 100.0f);
							PassedTime = 0.0f;
						}
						else
						{
							CurrentMission = EEnumMission::NONE;
							PassedTime = 0.0f;
						}
					}
				}
				else
				{
					CurrentMission = EEnumMission::NONE;
					PassedTime = 0.0f;
					ResourceCarried->SetStaticMesh(nullptr);
					ResourceCarriedType = EEnumResource::NONE;
					ResourceCarriedAmount = 0;
				}
			}
		}
		else
		{
			CurrentMission = EEnumMission::NONE;
			PassedTime = 0.0f;
			ResourceCarried->SetStaticMesh(nullptr);
			ResourceCarriedType = EEnumResource::NONE;
			ResourceCarriedAmount = 0;
		}
		if (PlayerType == EEnumPlayer::AI)
		{
			FindWhatToAttackAndAttack();
		}
	break;
	case EEnumMission::MOVE:
		PassedTime = 0.0f;
		if (ResourceCarried->GetStaticMesh())
		{
			if (ResourceCarriedType == EEnumResource::NONE || ResourceCarriedType == EEnumResource::TOOL)
			{
				ResourceCarried->SetStaticMesh(nullptr);
				ResourceCarriedType = EEnumResource::NONE;
			}
		}
		if (!GetController()->IsFollowingAPath())
		{
			AStaecrafterBuilding* MovingtoActorBuilding;
			MovingtoActorBuilding = dynamic_cast<AStaecrafterBuilding*>(MovingToActor);
			if (MovingtoActorBuilding)
			{
				if (MovingtoActorBuilding->BuildingType == EEnumBuilding::TOWNHALL)
				{
					if (ResourceCarriedType != EEnumResource::NONE && ResourceCarriedType != EEnumResource::TOOL)
					{
						ResourceCarried->SetStaticMesh(NULL);
						switch (ResourceCarriedType)
						{
						case EEnumResource::TREE:
							CurrentPlayerState->ResourcesWood += ResourceCarriedAmount;
						break;
						case EEnumResource::ROCK:
							CurrentPlayerState->ResourcesStone += ResourceCarriedAmount;
						break;
						}
						ResourceCarriedType = EEnumResource::NONE;
						ResourceCarriedAmount = 0;
					}
				}
			}
			CurrentMission = EEnumMission::NONE;
		}
		if (PlayerType == EEnumPlayer::AI)
		{
			FindWhatToAttackAndAttack();
		}
	break;
	case EEnumMission::ATTACK:
		if (ResourceCarried->GetStaticMesh())
		{
			if (ResourceCarriedType == EEnumResource::TREE ||
				ResourceCarriedType == EEnumResource::ROCK)
			{
				ResourceCarried->SetStaticMesh(nullptr);
				ResourceCarriedAmount = 0;
				ResourceCarriedType = EEnumResource::NONE;
			}
		}
		if (CharacterToAttack)
		{
			if (!GetController()->IsFollowingAPath())
			{
				float DistanceToCollision;
				DistanceToCollision = TheFunctionLibrary->GetActorDistanceToCollision(this, CharacterToAttack);
				if (DistanceToCollision >= 200.0f)
				{
					CurrentMission = EEnumMission::NONE;
					ResourceCarried->SetStaticMesh(nullptr);
				}
				else
				{
					if (!ResourceCarried->GetStaticMesh())
					{
						FString ResourceName(TEXT("AttackTool"));
						UMeshDataAsset* MeshDataAssetCasted = dynamic_cast<UMeshDataAsset*>(MeshDataAsset);
						FMeshDataAssetRow MeshData = MeshDataAssetCasted->FindInContent(ResourceName);
						ResourceCarried->SetStaticMesh(MeshData.Mesh);
						ResourceCarried->SetRelativeLocation(MeshData.RelativeLocation);
						ResourceCarried->SetRelativeRotation(MeshData.Rotation);
						ResourceCarried->SetRelativeScale3D(MeshData.Scale);
						ResourceCarriedType = EEnumResource::TOOL;
						PassedTime = 0.0f;
					}
					if (PassedTime >= 1.0f)
					{
						CharacterToAttack->Health -= 7.0f;
						PassedTime = 0.0f;
						if (CharacterToAttack->Health <= 0.0f)
						{
							CurrentMission = EEnumMission::NONE;
							CharacterToAttack->PerformTheDestroy();
							ResourceCarried->SetStaticMesh(nullptr);
							CharacterToAttack = nullptr;
						}
					}
				}
			}
			else
			{
				FollowCharacterToAttack();
			}
		}
		else
		{
			if (BuildingToAttack)
			{
				if (!GetController()->IsFollowingAPath())
				{
					if (TheFunctionLibrary->GetActorDistanceToCollision(this, BuildingToAttack) <= 500.0f)
					{
						if (!ResourceCarried->GetStaticMesh())
						{
							FString ResourceName(TEXT("AttackTool"));
							UMeshDataAsset* MeshDataAssetCasted = dynamic_cast<UMeshDataAsset*>(MeshDataAsset);
							FMeshDataAssetRow MeshData = MeshDataAssetCasted->FindInContent(ResourceName);
							ResourceCarried->SetStaticMesh(MeshData.Mesh);
							ResourceCarried->SetRelativeLocation(MeshData.RelativeLocation);
							ResourceCarried->SetRelativeRotation(MeshData.Rotation);
							ResourceCarried->SetRelativeScale3D(MeshData.Scale);
							ResourceCarriedType = EEnumResource::TOOL;
							PassedTime = 0.0f;
						}
						if (PassedTime >= 1.0f)
						{
							BuildingToAttack->Health -= 7.0f;
							PassedTime = 0.0f;
							if (BuildingToAttack->Health <= 0.0f)
							{
								CurrentMission = EEnumMission::NONE;
								BuildingToAttack->PerformTheDestroy();
								ResourceCarried->SetStaticMesh(nullptr);
								BuildingToAttack = nullptr;
							}
						}
					}
					else
					{
						CurrentMission = EEnumMission::NONE;
						PassedTime = 0.0f;
					}
				}
				//To scan for rogue characters
				FindCharactersToAttackAndAttack();
			}
			else
			{
				CurrentMission = EEnumMission::NONE;
				PassedTime = 0.0f;
			}
		}
	break;
	case EEnumMission::NONE:
		PassedTime = 0.0f;
		MovingToActor = nullptr;
		if (ResourceCarriedType==EEnumResource::NONE || ResourceCarriedType == EEnumResource::TOOL)
		{
			ResourceCarried->SetStaticMesh(nullptr);
			ResourceCarriedType = EEnumResource::NONE;
		}
		if (PlayerType == EEnumPlayer::AI)
		{
			if (bAIAttacking)
			{
				AIAttackTheOtherPlayer();
			}
		}
		FindWhatToAttackAndAttack();
	break;
	}
	//AI Control
	if (PlayerType == EEnumPlayer::AI)
	{
		switch (AIMission)
		{
		case EEnumAIMission::COLLECT:
			if (CurrentMission != EEnumMission::COLLECT && CurrentMission != EEnumMission::ATTACK)
			{
				FindWhatToCollectAndCollect();
			}
		break;
		case EEnumAIMission::ATTACK:
			if (CurrentPlayerState->bStartAttacking)
			{
				if (PassedTime2 >= NextTime && !bAIAttacking)
				{
					bAIAttacking = true;
					PassedTime2 = 0.0f;
					NextTime = 1.0f;
				}
			}
		break;
		case EEnumAIMission::NONE:

		break;
		}
	}
}

void AStaecrafterCharacter::AIAttackTheOtherPlayer()
{
	AStaecrafterPlayerState* OtherPlayerState = GetWorld()->GetGameState<AStaecrafterGameState>()->GetPlayerInfoByType(EEnumPlayer::PLAYER).PlayerState;
	if (OtherPlayerState->Buildings.Num() > 0)
	{
		AStaecrafterBuilding* SelectedAttackBuilding = OtherPlayerState->Buildings[RandStream.RandRange(0, OtherPlayerState->Buildings.Num() - 1)];
		//Moves and will find what to attack by itself
		if (SelectedAttackBuilding)
		{
			CurrentMission = EEnumMission::MOVE;
			TheFunctionLibrary->MoveToActorCollision(GetController<AAIController>(), this, SelectedAttackBuilding, 100.0f);
		}
	}
}

void AStaecrafterCharacter::FindCharactersToAttackAndAttack()
{
	CharacterToAttack = nullptr;
	TArray<AStaecrafterCharacter*> CharactersInRange;
	TArray<AStaecrafterCharacter*> CharactersInRangeToAttack;
	CharactersInRange = TheFunctionLibrary->GetAllActorsInRange<AStaecrafterCharacter>(this, GetActorLocation(), 1200.0f);
	for (AStaecrafterCharacter* ASC : CharactersInRange)
	{
		if (ASC->PlayerType != PlayerType)
		{
			CharactersInRangeToAttack.Add(ASC);
		}
	}

	if (CharactersInRangeToAttack.Num() > 0)
	{
		AStaecrafterCharacter* ClosestCharacter = nullptr;
		float ClosestDistance = 0.0f;
		for (AStaecrafterCharacter* SC : CharactersInRangeToAttack)
		{
			float Distance;
			if (!ClosestCharacter)
			{
				ClosestCharacter = SC;
				ClosestDistance = TheFunctionLibrary->GetActorDistanceToCollision(this, SC);
			}
			else
			{
				Distance = TheFunctionLibrary->GetActorDistanceToCollision(this, SC);
				if (Distance < ClosestDistance)
				{
					ClosestCharacter = SC;
					ClosestDistance = Distance;
				}
			}
		}
		TheFunctionLibrary->MoveToActorCollision(GetController<AAIController>(), this, ClosestCharacter, 100.0f);
		CurrentMission = EEnumMission::ATTACK;
		CharacterToAttack = ClosestCharacter;
		BuildingToAttack = nullptr;
	}

}

void AStaecrafterCharacter::FindWhatToAttackAndAttack()
{
	CharacterToAttack = nullptr;
	BuildingToAttack = nullptr;
	FindCharactersToAttackAndAttack();

	if (!CharacterToAttack)
	{
		TArray<AStaecrafterBuilding*> BuildingsInRange;
		TArray<AStaecrafterBuilding*> BuildingsInRangeToAttack;
		BuildingsInRange = TheFunctionLibrary->GetAllActorsWithCollisionInRange<AStaecrafterBuilding>(this, GetActorLocation(), 1500.0f);
		for (AStaecrafterBuilding* ASB : BuildingsInRange)
		{
			if (ASB->PlayerType != PlayerType)
			{
				BuildingsInRangeToAttack.Add(ASB);
			}
		}
		if (BuildingsInRangeToAttack.Num() > 0)
		{
			AStaecrafterBuilding* ClosestBuilding = nullptr;
			float ClosestDistance = 0.0f;
			for (AStaecrafterBuilding* SB : BuildingsInRangeToAttack)
			{
				float Distance;
				if (!ClosestBuilding)
				{
					ClosestBuilding = SB;
					ClosestDistance = TheFunctionLibrary->GetActorDistanceToCollision(this, SB);
				}
				else
				{
					Distance = TheFunctionLibrary->GetActorDistanceToCollision(this, SB);
					if (Distance < ClosestDistance)
					{
						ClosestBuilding = SB;
						ClosestDistance = Distance;
					}
				}
			}
			TheFunctionLibrary->MoveToActorCollision(GetController<AAIController>(), this, ClosestBuilding, 100.0f);
			CurrentMission = EEnumMission::ATTACK;
			BuildingToAttack = ClosestBuilding;
		}
	}
	PassedTime3 = 0.0f;
}

void AStaecrafterCharacter::FollowCharacterToAttack()
{
	if (PassedTime3 >= 0.5f)
	{
		if (CharacterToAttack)
		{
			TheFunctionLibrary->MoveToActorCollision(GetController<AAIController>(), this, CharacterToAttack, 100.0f);
		}
		PassedTime3 = 0.0f;
	}
}

void AStaecrafterCharacter::FindWhatToCollectAndCollect()
{
	ResourceToCollect = nullptr;
	AStaecrafterResource* RandomSelectedResource;
	RandomSelectedResource = TheFunctionLibrary->GetRandomActorWithCollisionInRange<AStaecrafterResource>(this, GetActorLocation(), 5000.0f);
	if (RandomSelectedResource)
	{
		TheFunctionLibrary->MoveToActorCollision(GetController<AAIController>(), this, RandomSelectedResource, 100.0f);
		CurrentMission = EEnumMission::COLLECT;
		ResourceToCollect = RandomSelectedResource;
	}
}

void AStaecrafterCharacter::PerformTheDestroy()
{
	bWasDestroyed = true;
	CurrentPlayerState->Population--;
	Destroy();
}

void AStaecrafterCharacter::FellOutOfWorld(const UDamageType& dmgType)
{
	Super::FellOutOfWorld(dmgType);
	PerformTheDestroy();
	CurrentPlayerState->ResourcesFood += CostDataAsset->FindInContent("Unit").Food;
}
