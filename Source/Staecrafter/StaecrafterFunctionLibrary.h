// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AIController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "StaecrafterFunctionLibrary.generated.h"

USTRUCT(BlueprintType)
struct FConsideredActorExecutionInfo
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* Owner;

	UPROPERTY()
	bool TimeToExecute = false;
};

UCLASS(Blueprintable)
class STAECRAFTER_API AStaecrafterFunctionLibrary : public AActor
{
	GENERATED_BODY()

public:
	AStaecrafterFunctionLibrary();

	TArray<FConsideredActorExecutionInfo> ConsideredActorsToExecution;

	virtual void Tick(float DeltaTime);

	template <class T>
	T* GetRandomActorInRange(AActor* Owner, FVector Point, float Range)
	{
		bool Ok = true;
		FConsideredActorExecutionInfo ExecutionInfo;
		ExecutionInfo.Owner = Owner;
		int32 Index = 0;
		for (FConsideredActorExecutionInfo I : ConsideredActorsToExecution)
		{
			if (I.Owner == Owner)
			{
				Ok = false;
				ExecutionInfo = I;
				break;
			}
			Index++;
		}
		if (Ok)
		{
			ConsideredActorsToExecution.Add(ExecutionInfo);
		}
		else
		{
			if (ExecutionInfo.TimeToExecute)
			{
				return GetRandomActorInRangeNoExecutionPlanning<T>(Owner, Point, Range);
			}
		}
		return nullptr;
	}

	template <class T>
	T* GetRandomActorInRangeNoExecutionPlanning(AActor* Owner, FVector Point, float Range)
	{
		TArray<T*> ActorsInRange;
		UWorld* World = Owner->GetWorld();
		ActorsInRange = GetAllActorsInRangeNoExecutionPlanning<T>(Owner, Point, Range);
		if (ActorsInRange.Num() > 0)
		{
			T* RandomSelectedActor;
			RandomSelectedActor = ActorsInRange[FMath::RandRange(0, ActorsInRange.Num() - 1)];
			return RandomSelectedActor;
		}
		else
		{
			return nullptr;
		}
	}

	template <class T>
	T* GetRandomActorWithCollisionInRange(AActor* Owner, FVector Point, float Range)
	{
		bool Ok = true;
		FConsideredActorExecutionInfo ExecutionInfo;
		ExecutionInfo.Owner = Owner;
		int32 Index = 0;
		for (FConsideredActorExecutionInfo I : ConsideredActorsToExecution)
		{
			if (I.Owner == Owner)
			{
				Ok = false;
				ExecutionInfo = I;
				break;
			}
			Index++;
		}
		if (Ok)
		{
			ConsideredActorsToExecution.Add(ExecutionInfo);
		}
		else
		{
			if (ExecutionInfo.TimeToExecute)
			{
				return GetRandomActorWithCollisionInRangeNoExecutionPlanning<T>(Owner, Point, Range);
			}
		}
		return nullptr;
	}

	template <class T>
	T* GetRandomActorWithCollisionInRangeNoExecutionPlanning(AActor* Owner, FVector Point, float Range)
	{
		TArray<T*> ActorsInRange;
		ActorsInRange = GetAllActorsWithCollisionInRangeNoExecutionPlanning<T>(Owner, Point, Range);
		if (ActorsInRange.Num() > 0)
		{
			T* RandomSelectedActor;
			RandomSelectedActor = ActorsInRange[FMath::RandRange(0, ActorsInRange.Num() - 1)];
			return RandomSelectedActor;
		}
		else
		{
			return nullptr;
		}
	}

	template <class T>
	TArray<T*> GetAllActorsInRange(AActor* Owner, FVector Point, float Range)
	{
		TArray<T*> ActorsInRange;
		bool Ok = true;
		FConsideredActorExecutionInfo ExecutionInfo;
		int32 Index = 0;
		ExecutionInfo.Owner = Owner;
		for (FConsideredActorExecutionInfo I : ConsideredActorsToExecution)
		{
			if (I.Owner == Owner)
			{
				Ok = false;
				ExecutionInfo = I;
				break;
			}
			Index++;
		}
		if (Ok)
		{
			ConsideredActorsToExecution.Add(ExecutionInfo);
		}
		else
		{
			if (ExecutionInfo.TimeToExecute)
			{
				ActorsInRange = GetAllActorsInRangeNoExecutionPlanning<T>(Owner, Point, Range);
			}
		}
		return ActorsInRange;
	}

	template <class T>
	TArray<T*> GetAllActorsInRangeNoExecutionPlanning(AActor* Owner, FVector Point, float Range)
	{
		TArray<AActor*> AllActors;
		TArray<T*> ActorsInRange;
		UWorld* World = Owner->GetWorld();
		UGameplayStatics::GetAllActorsOfClass(World, T::StaticClass(), AllActors);
		AllActors.Remove(Owner);
		FVector DistanceVector;
		for (AActor* A : AllActors)
		{
			T* AT = dynamic_cast<T*>(A);
			DistanceVector = A->GetActorLocation() - Point;
			if (DistanceVector.Size() <= Range)
			{
				ActorsInRange.Add(AT);
			}
		}
		return ActorsInRange;
	}

	template <class T>
	TArray<T*> GetAllActorsWithCollisionInRange(AActor* Owner, FVector Point, float Range)
	{
		TArray<T*> ActorsInRange;
		bool Ok = true;
		FConsideredActorExecutionInfo ExecutionInfo;
		ExecutionInfo.Owner = Owner;
		int32 Index = 0;
		for (FConsideredActorExecutionInfo I : ConsideredActorsToExecution)
		{
			if (I.Owner == Owner)
			{
				Ok = false;
				ExecutionInfo = I;
				break;
			}
			Index++;
		}
		if (Ok)
		{
			ConsideredActorsToExecution.Add(ExecutionInfo);
		}
		else
		{
			if (ExecutionInfo.TimeToExecute)
			{
				ActorsInRange = GetAllActorsWithCollisionInRangeNoExecutionPlanning<T>(Owner, Point, Range);
			}
		}
		return ActorsInRange;
	}

	template <class T>
	TArray<T*> GetAllActorsWithCollisionInRangeNoExecutionPlanning(AActor* Owner, FVector Point, float Range)
	{
		TArray<AActor*> AllActors;
		TArray<T*> ActorsInRange;
		UWorld* World = Owner->GetWorld();
		UGameplayStatics::GetAllActorsOfClass(World, T::StaticClass(), AllActors);
		AllActors.Remove(Owner);
		FVector CollisionPoint;
		float DistanceToCollision;
		for (AActor* A : AllActors)
		{
			T* AT = dynamic_cast<T*>(A);
			World = A->GetWorld();
			FVector CollisionDestination = A->GetActorLocation();
			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
			ObjectTypes.Emplace(ECC_WorldDynamic);
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(Owner);
			TArray<FHitResult> Hits;
			UKismetSystemLibrary::LineTraceMultiForObjects(
				World,
				Point,
				A->GetActorLocation(),
				ObjectTypes,
				false,
				ActorsToIgnore,
				EDrawDebugTrace::None,
				Hits,
				true
			);
			for (FHitResult H : Hits)
			{
				if (H.Actor.Get() == A)
				{
					CollisionDestination = H.ImpactPoint;
					break;
				}
			}
			DistanceToCollision = (CollisionDestination - Point).Size();
			if (DistanceToCollision <= Range && DistanceToCollision > 0.0f)
			{
				ActorsInRange.Add(AT);
			}
		}
		return ActorsInRange;
	}

	void MoveToActorCollision(AAIController* AIController, AActor* AIControllerOwner, AActor* Destination);
	
	void MoveToActorCollision(AAIController* AIController, AActor* AIControllerOwner, AActor* Destination, float AcceptanceRadius);
	
	UFUNCTION(BlueprintCallable)
	float GetActorDistanceToCollision(AActor* ActorStart, AActor* ActorEnd);

private:
	int32 NrTicks;
	
};
