// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UI/Controller/FairyController.h"
#include "UI/FairyComponent.h"
#include "FairyControllerMgr.generated.h"

UCLASS()
class FAIRYGUI_API UFairyControllerArrayWrapper : public UObject
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TArray<UFairyController*> ControllerList;

public:
	void AddController(UFairyController* InFairyController);
	void RemoveAllController();

	TArray<UFairyController*>& GetControllerList();
};

/**
 * 
 */
UCLASS()
class FAIRYGUI_API UFairyControllerMgr : public UObject
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	TMap<UFairyComponent*, UFairyControllerArrayWrapper*> EntryTable;
public:
	void AddController(UFairyComponent* InTarget, UFairyController* InFairyController);
	void RemoveController(UFairyComponent* InTarge, UFairyController* InPenddingRemoveController);
	void RemoveAllController(UFairyComponent* InTarget);

	void ApplyAllController(UFairyComponent* InTarget);
	void ApplyController(UFairyComponent* InTarget, UFairyController* InFairyController);
	void ApplyController(UFairyComponent* InTarget, const FName& InName);

	bool HasController(const UFairyComponent* InTarget);
	UFairyController* GetController(const UFairyComponent* InTarget, FName InName);
	UFairyController* GetController(const UFairyComponent* InTarget, int32 InIndex);
	
	/**
	* Making sure there is controller for target component;
	*/
	const TArray<UFairyController*>& GetControllerList(const UFairyComponent* InTarget);
};


