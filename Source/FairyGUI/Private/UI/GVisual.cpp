// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "UI/GVisual.h"
#include "Engine/UserInterfaceSettings.h"

/////////////////////////////////////////////////////
// UVisual

UGVisual::UGVisual(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGVisual::ReleaseSlateResources(bool bReleaseChildren)
{
}

void UGVisual::BeginDestroy()
{
	Super::BeginDestroy();

	const bool bReleaseChildren = false;
	ReleaseSlateResources(bReleaseChildren);
}

bool UGVisual::NeedsLoadForServer() const
{
	const UUserInterfaceSettings* UISettings = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass());
	check(UISettings);
	return UISettings->bLoadWidgetsOnDedicatedServer;
}