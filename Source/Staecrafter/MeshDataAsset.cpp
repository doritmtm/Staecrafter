// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshDataAsset.h"

FMeshDataAssetRow UMeshDataAsset::FindInContent(FString Name)
{
	for (int32 i = 0; i < Content.Num(); i++)
	{
		if (Content[i].Name.Compare(Name) == 0)
		{
			return Content[i];
		}
	}
	return FMeshDataAssetRow();
}
