#pragma once

#include "CoreMinimal.h"
#include "Delegates/DelegateCombinations.h"

#include "UI/FairyObject.h"
#include "Tween/EaseType.h"

#include "FairyTweener.generated.h"

class FGPath;
class UFairyTweener;

extern const float K_MATH_EPSILON;

DECLARE_DELEGATE_OneParam(FTweenDelegate, UFairyTweener*);
DECLARE_DYNAMIC_DELEGATE_OneParam(FTweenDynamicDelegate, UFairyTweener*, FairyTweener);

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweener : public UObject
{
	GENERATED_BODY()
public:
	UFairyTweener();
	virtual ~UFairyTweener();
	
	[[deprecated("Will remove in next refactor.")]]
	UFairyTweener* SetEase(EEaseType InValue);
	[[deprecated("Will remove in next refactor.")]]
	UFairyTweener* SetEasePeriod(float InValue);
	[[deprecated("Will remove in next refactor.")]]
	UFairyTweener* SetEaseOvershootOrAmplitude(float InValue);
	[[deprecated("Will remove in next refactor.")]]
	UFairyTweener* SetPath(TSharedPtr<FGPath> InPath);

	virtual void StartWithTarget(UFairyObject* InTarget);
	void SetTarget(UFairyObject* InTarget);
	UFairyObject* GetTarget();
	bool IsTargetValid();

	virtual void Step(float InDeltaTime);
	virtual void Update(float InTime);
	virtual bool IsDone() const;

	UFairyTweener* SetPaused(bool bInPaused);
	virtual bool IsPaused();

	int GetTag();
	void SetTag(int InTag);

protected:
	UPROPERTY()
	UFairyObject* _Target;
	int _tag;
	
	bool bPaused;

	EEaseType EaseType;
	float EaseOvershootOrAmplitude;
	float EasePeriod;

	bool bYoyo;
	TSharedPtr<FGPath> Path;
};
