// Fill out your copyright notice in the Description page of Project Settings.


#include "StaecrafterResource.h"
#include "Components/CapsuleComponent.h"

AStaecrafterResource::AStaecrafterResource()
{
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

void AStaecrafterResource::BeginPlay()
{
	Super::BeginPlay();
	SelectedDecal->SetVisibility(false);
	RandStream.Initialize(FDateTime::Now().ToUnixTimestamp());
	ResourcesAvailable = RandStream.RandRange(330, 1180);
}

void AStaecrafterResource::PerformTheDestroy()
{
	Destroy();
}
