#pragma once

#include "CoreMinimal.h"
#include "Event/EventContext.h"
#include "UI/Transition/FairyTransitionItemBase.h"
#include "UI/Transition/FairyTransitionTweenConfig.h"
#include "FairyTransition.generated.h"

class UFairyObject;
class UFairyComponent;
class UFairyController;
class FByteBuffer;
class UFairyTweener;

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTransition : public UObject
{
	GENERATED_BODY()
public:
	UFairyTransition();
	virtual ~UFairyTransition();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI | Transition")
	FName GetName();

	UFairyComponent* GetTargetComponent() { return TargetComponent; }
	void SetTargetComponent(UFairyComponent* InTargetComponent) { TargetComponent = InTargetComponent; }
	
	void OnTargetComponentEnter();
	void OnTargetComponentExit();

	void Setup(FairyGUI::FByteBuffer* Buffer);

private:
	FName Name;

	UPROPERTY()
	UFairyComponent* TargetComponent;

	TArray<TSharedPtr<FFairyTransitionItemBase>> Items;

	bool bPlaying;
	bool bPaused;
	bool bAutoPlay;

	int32 Options;
	int32 AutoPlayTimes;
	float AutoPlayDelay;
};