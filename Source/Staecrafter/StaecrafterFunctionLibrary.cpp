// Fill out your copyright notice in the Description page of Project Settings.


#include "StaecrafterFunctionLibrary.h"
#include "StaecrafterCharacter.h"

AStaecrafterFunctionLibrary::AStaecrafterFunctionLibrary()
{
	PrimaryActorTick.bCanEverTick = true;
	NrTicks = 0;
}

void AStaecrafterFunctionLibrary::Tick(float DeltaTime)
{
	if (NrTicks >= 2)
	{
		int32 TimeToExecuteTrueIndex = -1;

		for (int32 i = 0; i < ConsideredActorsToExecution.Num(); i++)
		{
			if (ConsideredActorsToExecution[i].TimeToExecute == true)
			{
				TimeToExecuteTrueIndex = i;
			}
		}

		for (int32 i = 0; i < ConsideredActorsToExecution.Num(); i++)
		{
			ConsideredActorsToExecution[i].TimeToExecute = false;
		}

		for (int32 i = TimeToExecuteTrueIndex + 1; i < TimeToExecuteTrueIndex + 21 && i < ConsideredActorsToExecution.Num(); i++)
		{
			ConsideredActorsToExecution[i].TimeToExecute = true;
		}
		NrTicks = 0;
	}
	NrTicks++;
}

void AStaecrafterFunctionLibrary::MoveToActorCollision(AAIController* AIController, AActor* AIControllerOwner, AActor* Destination)
{
	UWorld* World = Destination->GetWorld();
	FVector CollisionDestination = Destination->GetActorLocation();
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Emplace(ECC_WorldDynamic);
	TArray<AActor*> ActorsToIgnore;
	TArray<FHitResult> Hits;
	UKismetSystemLibrary::LineTraceMultiForObjects(
		World, 
		AIControllerOwner->GetActorLocation(), 
		Destination->GetActorLocation(), 
		ObjectTypes, 
		false, 
		ActorsToIgnore, 
		EDrawDebugTrace::None, 
		Hits, 
		true
	);
	for (FHitResult H : Hits)
	{
		if (H.Actor.Get() == Destination)
		{
			CollisionDestination = H.ImpactPoint;
			break;
		}
	}
	AStaecrafterCharacter* OwnerCharacter;
	OwnerCharacter = dynamic_cast<AStaecrafterCharacter*>(AIControllerOwner);
	if (OwnerCharacter)
	{
		OwnerCharacter->MovingToActor = Destination;
	}
	AIController->MoveToLocation(CollisionDestination);
}

void AStaecrafterFunctionLibrary::MoveToActorCollision(AAIController* AIController, AActor* AIControllerOwner, AActor* Destination, float AcceptanceRadius)
{
	UWorld* World = Destination->GetWorld();
	FVector CollisionDestination = Destination->GetActorLocation();
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Emplace(ECC_WorldDynamic);
	TArray<AActor*> ActorsToIgnore;
	TArray<FHitResult> Hits;
	UKismetSystemLibrary::LineTraceMultiForObjects(
		World, 
		AIControllerOwner->GetActorLocation(), 
		Destination->GetActorLocation(), 
		ObjectTypes, 
		false, 
		ActorsToIgnore, 
		EDrawDebugTrace::None, 
		Hits, 
		true
	);
	for (FHitResult H : Hits)
	{
		if (H.Actor.Get() == Destination)
		{
			CollisionDestination = H.ImpactPoint;
			break;
		}
	}
	AStaecrafterCharacter* OwnerCharacter;
	OwnerCharacter = dynamic_cast<AStaecrafterCharacter*>(AIControllerOwner);
	if (OwnerCharacter)
	{
		OwnerCharacter->MovingToActor = Destination;
	}
	AIController->MoveToLocation(CollisionDestination, AcceptanceRadius);
}

float AStaecrafterFunctionLibrary::GetActorDistanceToCollision(AActor* ActorStart, AActor* ActorEnd)
{
	float DistanceToCollision;
	UWorld* World = ActorStart->GetWorld();
	FVector CollisionDestination = ActorEnd->GetActorLocation();
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Emplace(ECC_WorldDynamic);
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(ActorStart);
	TArray<FHitResult> Hits;
	UKismetSystemLibrary::LineTraceMultiForObjects(
		World, 
		ActorStart->GetActorLocation(), 
		ActorEnd->GetActorLocation(), 
		ObjectTypes, 
		false, 
		ActorsToIgnore, 
		EDrawDebugTrace::None, 
		Hits, 
		true
	);
	for (FHitResult H : Hits)
	{
		if (H.Actor.Get() == ActorEnd)
		{
			CollisionDestination = H.ImpactPoint;
			break;
		}
	}
	DistanceToCollision = (CollisionDestination - ActorStart->GetActorLocation()).Size();
	return DistanceToCollision;
}
