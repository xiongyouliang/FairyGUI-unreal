// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UI/Transition/FairyTransition.h"
#include "FairyTransitionMgr.generated.h"

/**
 * 
 */
UCLASS()
class FAIRYGUI_API UFairyTransitionMgr : public UObject
{
	GENERATED_BODY()

public:
	void AddTransition(UFairyTransition* InTransition);
	void RemoveTransition(UFairyTransition* InTransition);
	void RemoveAllTransition();

	int32 GetTransitionNum();
	int32 GetTransitionNumInComponent(UFairyComponent* InTragetComponent);
private:
	UFairyTransitionMgr() {};
	friend class UFairyApplication;

	UPROPERTY()
	TArray<UFairyTransition*> TransitionList;	
};
