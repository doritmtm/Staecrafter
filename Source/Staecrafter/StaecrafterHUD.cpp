// Fill out your copyright notice in the Description page of Project Settings.


#include "StaecrafterHUD.h"

AStaecrafterHUD::AStaecrafterHUD()
{
	bDragEnabled = false;
}

void AStaecrafterHUD::DrawHUD()
{
	Super::DrawHUD();
	if (bDragEnabled)
	{
		SelectedPawns.Empty();
		GetActorsInSelectionRectangle<AStaecrafterCharacter>(
			FVector2D(StartDragX, StartDragY), 
			FVector2D(ContinueDragX, ContinueDragY), 
			SelectedPawns, 
			false, 
			true
		);
		DrawRect(
			FLinearColor(0.0, 1.0, 1.0, 0.3), 
			StartDragX, 
			StartDragY, 
			ContinueDragX - StartDragX, 
			ContinueDragY - StartDragY
		);
	}
}
