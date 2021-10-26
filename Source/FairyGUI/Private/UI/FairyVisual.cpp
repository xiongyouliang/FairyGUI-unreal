// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "UI/FairyVisual.h"
#include "Engine/UserInterfaceSettings.h"

/////////////////////////////////////////////////////
// UVisual

UFairyVisual::UFairyVisual(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UFairyVisual::ReleaseSlateResources(bool bReleaseChildren)
{
}

void UFairyVisual::BeginDestroy()
{
	Super::BeginDestroy();

	const bool bReleaseChildren = false;
	ReleaseSlateResources(bReleaseChildren);
}

bool UFairyVisual::NeedsLoadForServer() const
{
	const UUserInterfaceSettings* UISettings = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass());
	check(UISettings);
	return UISettings->bLoadWidgetsOnDedicatedServer;
}