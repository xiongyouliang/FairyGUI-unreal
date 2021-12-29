#pragma once

#include "CoreMinimal.h"
#include "Delegates/DelegateCombinations.h"

#include "UI/FairyObject.h"
#include "Tween/FairyEaseType.h"

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
	UFairyTweener* SetPath(TSharedPtr<FGPath> InPath);

	virtual void SetTarget(UFairyObject* InTarget);
	UFairyObject* GetTarget();
	bool IsTargetValid();

	virtual void Step(float InDeltaTime);
	virtual void Update(float InTime);
	virtual bool IsDone() const;
	virtual void Reset();

	UFairyTweener* SetPaused(bool bInPaused);
	virtual bool IsPaused();

	int GetTag();
	void SetTag(int InTag);

protected:
	UPROPERTY()
	UFairyObject* _Target;
	int _tag;
	bool bPaused;

	bool bYoyo;
	TSharedPtr<FGPath> Path;
};
