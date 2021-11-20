#include "Tween/FairyTweener.h"
#include "Tween/EaseManager.h"
#include "Tween/GPath.h"
#include "UI/FairyObject.h"

const float K_MATH_EPSILON = 0.000001f;

UFairyTweener::UFairyTweener()
	:_Target(nullptr)
	,_tag(0)
{
}

UFairyTweener::~UFairyTweener()
{
}

UFairyTweener* UFairyTweener::SetDelay(float InValue)
{
	Delay = InValue;
	return this;
}

UFairyTweener* UFairyTweener::SetBreakpoint(float InValue)
{
	Breakpoint = InValue;
	return this;
}

UFairyTweener* UFairyTweener::SetEase(EEaseType InValue)
{
	EaseType = InValue;
	return this;
}

UFairyTweener* UFairyTweener::SetEasePeriod(float InValue)
{
	EasePeriod = InValue;
	return this;
}

UFairyTweener* UFairyTweener::SetEaseOvershootOrAmplitude(float InValue)
{
	EaseOvershootOrAmplitude = InValue;
	return this;
}

UFairyTweener* UFairyTweener::SetRepeat(int32 InRepeat, bool bInYoyo)
{
	Repeat = InRepeat;
	bYoyo = bInYoyo;
	return this;
}

UFairyTweener* UFairyTweener::SetTimeScale(float InValue)
{
	TimeScale = InValue;
	return this;
}

UFairyTweener* UFairyTweener::SetSnapping(bool bInValue)
{
	bSnapping = bInValue;
	return this;
}

UFairyTweener* UFairyTweener::SetTarget(UObject* InTarget)
{
	Target = InTarget;
	return this;
}

void UFairyTweener::StartWithTarget(UFairyObject* InTarget)
{
	_Target = InTarget;
}

void UFairyTweener::SetTarget2(UFairyObject* InTarget)
{
	_Target = InTarget;
}

UFairyObject* UFairyTweener::GetTarget2()
{
	return _Target;
}

bool UFairyTweener::IsTargetValid()
{
	return _Target != nullptr;
}

UFairyTweener* UFairyTweener::SetUserData(const FNVariant& InData)
{
	UserData = InData;
	return this;
}

bool UFairyTweener::IsDone() const
{
	return true;
}

UFairyTweener* UFairyTweener::SetPath(TSharedPtr<FGPath> InPath)
{
	Path = InPath;
	return this;
}

UFairyTweener* UFairyTweener::OnUpdate(FTweenDelegate Callback)
{
	OnUpdateCallback = Callback;
	return this;
}

UFairyTweener* UFairyTweener::OnStart(FTweenDelegate Callback)
{
	OnStartCallback = Callback;
	return this;
}

UFairyTweener* UFairyTweener::OnComplete(FTweenDelegate Callback)
{
	OnCompleteCallback = Callback;
	return this;
}

UFairyTweener* UFairyTweener::OnUpdate(FSimpleDelegate Callback)
{
	OnUpdateCallback.BindLambda([Callback](UFairyTweener*) {
		Callback.ExecuteIfBound();
	});
	return this;
}

UFairyTweener* UFairyTweener::OnStart(FSimpleDelegate Callback)
{
	OnStartCallback.BindLambda([Callback](UFairyTweener*) {
		Callback.ExecuteIfBound();
	});
	return this;
}

UFairyTweener* UFairyTweener::OnComplete(FSimpleDelegate Callback)
{
	OnCompleteCallback.BindLambda([Callback](UFairyTweener*) {
		Callback.ExecuteIfBound();
	});
	return this;
}

UFairyTweener* UFairyTweener::SetPaused(bool bInPaused)
{
	bPaused = bInPaused;
	return this;
}

bool UFairyTweener::IsPaused()
{
	return bPaused;
}

void UFairyTweener::Seek(float Time)
{
	if (bKilled)
	{
		return;
	}

	ElapsedTime = Time;
	if (ElapsedTime < Delay)
	{
		if (bStarted)
		{
			ElapsedTime = Delay;
		}
		else
		{
			return;
		}
	}
}

void UFairyTweener::Kill(bool bSetComplete)
{
	if (bKilled)
	{
		return;
	}

	if (bSetComplete)
	{
		if (Ended == 0)
		{
			if (Breakpoint >= 0)
			{
				ElapsedTime = Delay + Breakpoint;
			}
			else if (Repeat >= 0)
			{
				ElapsedTime = Delay + Duration * (Repeat + 1);
			}
			else
			{
				ElapsedTime = Delay + Duration * 2;
			}
		}

		OnCompleteCallback.ExecuteIfBound(this);
	}

	bKilled = true;
}

UFairyTweener* UFairyTweener::To(float InStart, float InEnd, float InDuration)
{
	ValueSize = 1;
	StartValue.X = InStart;
	EndValue.X = InEnd;
	Value.X = InStart;
	Duration = InDuration;
	return this;
}

UFairyTweener* UFairyTweener::To(const FVector2D& InStart, const FVector2D& InEnd, float InDuration)
{
	ValueSize = 2;
	StartValue.SetVec2(InStart);
	EndValue.SetVec2(InEnd);
	Value.SetVec2(InStart);
	Duration = InDuration;
	return this;
}

UFairyTweener* UFairyTweener::To(const FVector& InStart, const FVector& InEnd, float InDuration)
{
	ValueSize = 3;
	StartValue.SetVec3(InStart);
	EndValue.SetVec3(InEnd);
	Value.SetVec3(InStart);
	Duration = InDuration;
	return this;
}

UFairyTweener* UFairyTweener::To(const FVector4& InStart, const FVector4& InEnd, float InDuration)
{
	ValueSize = 4;
	StartValue.SetVec4(InStart);
	EndValue.SetVec4(InEnd);
	Value.SetVec4(InStart);
	Duration = InDuration;
	return this;
}

UFairyTweener* UFairyTweener::To(const FColor& InStart, const FColor& InEnd, float InDuration)
{
	ValueSize = 4;
	StartValue.SetColor(InStart);
	EndValue.SetColor(InEnd);
	Value.SetColor(InStart);
	Duration = InDuration;
	return this;
}

UFairyTweener* UFairyTweener::To(double InStart, double InEnd, float InDuration)
{
	ValueSize = 5;
	StartValue.D = InStart;
	EndValue.D = InEnd;
	Value.D = InStart;
	Duration = InDuration;
	return this;
}

UFairyTweener* UFairyTweener::Shake(const FVector2D& InStart, float InAmplitude, float InDuration)
{
	ValueSize = 6;
	StartValue.SetVec2(InStart);
	StartValue.W = InAmplitude;
	Duration = InDuration;
	EaseType = EEaseType::Linear;
	return this;
}

void UFairyTweener::Init()
{
	Delay = 0;
	Duration = 0;
	Breakpoint = -1;
	EaseType = EEaseType::QuadOut;
	TimeScale = 1;
	EasePeriod = 0;
	EaseOvershootOrAmplitude = 1.70158f;
	bSnapping = false;
	Repeat = 0;
	bYoyo = false;
	ValueSize = 0;
	bStarted = false;
	bPaused = false;
	bKilled = false;
	ElapsedTime = 0;
	NormalizedTime = 0;
	Ended = 0;
	StartValue.Reset();
	EndValue.Reset();
	Value.Reset();
	DeltaValue.Reset();
}

void UFairyTweener::Reset()
{
	Target.Reset();
	UserData.Reset();
	Path.Reset();
	OnStartCallback.Unbind();
	OnUpdateCallback.Unbind();
	OnCompleteCallback.Unbind();
}

void UFairyTweener::Step(float DeltaTime)
{

}

void UFairyTweener::Update(float DeltaTime)
{
}

void UFairyTweener::SetTag(int InTag)
{
	_tag = InTag;
}

int UFairyTweener::GetTag()
{
	return _tag;
}