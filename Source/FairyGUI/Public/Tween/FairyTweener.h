#pragma once

#include "CoreMinimal.h"
#include "Delegates/DelegateCombinations.h"

#include "UI/FairyObject.h"
#include "Tween/EaseType.h"
#include "Tween/TweenValue.h"
#include "Tween/TweenerHandle.h"
#include "Utils/NVariant.h"

#include "FairyTweener.generated.h"

class FGPath;
class UFairyTweener;

extern const float K_MATH_EPSILON;

DECLARE_DELEGATE_OneParam(FTweenDelegate, UFairyTweener*);

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweener : public UObject
{
	GENERATED_BODY()
public:
	UFairyTweener();
	virtual ~UFairyTweener();

	const FTweenerHandle& GetHandle() {
		return Handle;
	}

	UFairyTweener* SetDelay(float InValue);
	float GetDelay() const { return Delay; }

	[[deprecated("Mark to remove in next refactor.")]]
	UFairyTweener* SetDuration(float InValue);
	[[deprecated("Mark to remove in next refactor.")]]
	float GetDuration() const { return Duration; }

	UFairyTweener* SetBreakpoint(float InValue);

	UFairyTweener* SetEase(EEaseType InValue);
	UFairyTweener* SetEasePeriod(float InValue);
	UFairyTweener* SetEaseOvershootOrAmplitude(float InValue);

	UFairyTweener* SetRepeat(int32 InRepeat, bool bInYoyo = false);
	int32 GetRepeat() const { return Repeat; }

	UFairyTweener* SetTimeScale(float InValue);
	UFairyTweener* SetSnapping(bool InValue);

	UFairyTweener* SetTarget(UObject* InTarget);
	UObject* GetTarget() const { return Target.Get(); }

	virtual void StartWithTarget(UFairyObject* InTarget);
	void SetTarget2(UFairyObject* InTarget);
	UFairyObject* GetTarget2();
	bool IsTargetValid();

	const FNVariant& GetUserData() const { return UserData; }
	UFairyTweener* SetUserData(const FNVariant& InData);

	UFairyTweener* SetPath(TSharedPtr<FGPath> InPath);

	UFairyTweener* OnUpdate(FTweenDelegate Callback);
	UFairyTweener* OnStart(FTweenDelegate Callback);
	UFairyTweener* OnComplete(FTweenDelegate Callback);
	UFairyTweener* OnUpdate(FSimpleDelegate Callback);
	UFairyTweener* OnStart(FSimpleDelegate Callback);
	UFairyTweener* OnComplete(FSimpleDelegate Callback);

	float GetNormalizedTime() const { return NormalizedTime; }

	virtual bool IsDone() const;
	bool IsCompleted() const { return Ended != 0; }
	bool AllCompleted() const { return Ended == 1; }
	
	
	UFairyTweener* SetPaused(bool bInPaused);
	virtual bool IsPaused();

	void Seek(float Time);
	void Kill(bool bSetComplete = false);

	FTweenValue StartValue;
	FTweenValue EndValue;
	FTweenValue Value;
	FTweenValue DeltaValue;

	UFairyTweener* To(float InStart, float InEnd, float InDuration);
	UFairyTweener* To(const FVector2D& InStart, const FVector2D& InEnd, float InDuration);
	UFairyTweener* To(const FVector& InStart, const FVector& InEnd, float InDuration);
	UFairyTweener* To(const FVector4& InStart, const FVector4& InEnd, float InDuration);
	UFairyTweener* To(const FColor& InStart, const FColor& InEnd, float InDuration);
	UFairyTweener* To(double InStart, double InEnd, float InDuration);
	UFairyTweener* Shake(const FVector2D& InStart, float InAmplitude, float InDuration);
	
	void Init();
	void Reset();
	virtual void Step(float InDeltaTime);
	virtual void Update(float InTime);

protected:
	virtual void DoUpdate();

protected:
	TWeakObjectPtr<UObject> Target;
	TWeakObjectPtr<UFairyObject> _Target;

	bool bKilled;
	bool bPaused;

	float Delay;
	float Duration;
	float Breakpoint;

	EEaseType EaseType;
	float EaseOvershootOrAmplitude;
	float EasePeriod;

	int32 Repeat;
	bool bYoyo;
	float TimeScale;
	bool bSnapping;

	FNVariant UserData;
	int32 ValueSize;
	TSharedPtr<FGPath> Path;
	
	FTweenerHandle Handle;

	FTweenDelegate OnStartCallback;
	FTweenDelegate OnUpdateCallback;
	FTweenDelegate OnCompleteCallback;

	bool bStarted;
	int32 Ended;
	float ElapsedTime;
	float NormalizedTime;

	friend class FFairyTweenHelper;
	friend class UTweenManager;
};
