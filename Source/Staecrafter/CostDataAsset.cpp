// Fill out your copyright notice in the Description page of Project Settings.


#include "CostDataAsset.h"

FCostDataAssetRow UCostDataAsset::FindInContent(FString Name)
{
	for (FCostDataAssetRow R : Content)
	{
		if (R.Name.Compare(Name) == 0)
		{
			return R;
		}
	}
	return FCostDataAssetRow();
}
