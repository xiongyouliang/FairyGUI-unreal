#include "Tween/FairyTweener.h"
#include "Tween/EaseManager.h"
#include "Tween/GPath.h"
#include "UI/FairyObject.h"

const float K_MATH_EPSILON = 0.000001f;

FFairyTweener::FFairyTweener()
{
}

FFairyTweener::~FFairyTweener()
{
}

FFairyTweener* FFairyTweener::SetDelay(float InValue)
{
	Delay = InValue;
	return this;
}

FFairyTweener* FFairyTweener::SetDuration(float InValue)
{
	Duration = InValue;
	return this;
}

FFairyTweener* FFairyTweener::SetBreakpoint(float InValue)
{
	Breakpoint = InValue;
	return this;
}

FFairyTweener* FFairyTweener::SetEase(EEaseType InValue)
{
	EaseType = InValue;
	return this;
}

FFairyTweener* FFairyTweener::SetEasePeriod(float InValue)
{
	EasePeriod = InValue;
	return this;
}

FFairyTweener* FFairyTweener::SetEaseOvershootOrAmplitude(float InValue)
{
	EaseOvershootOrAmplitude = InValue;
	return this;
}

FFairyTweener* FFairyTweener::SetRepeat(int32 InRepeat, bool bInYoyo)
{
	Repeat = InRepeat;
	bYoyo = bInYoyo;
	return this;
}

FFairyTweener* FFairyTweener::SetTimeScale(float InValue)
{
	TimeScale = InValue;
	return this;
}

FFairyTweener* FFairyTweener::SetSnapping(bool bInValue)
{
	bSnapping = bInValue;
	return this;
}

FFairyTweener* FFairyTweener::SetTarget(UObject* InTarget)
{
	Target = InTarget;
	return this;
}

void FFairyTweener::StartWithTarget(UFairyObject* InTarget)
{
	_Target = TWeakObjectPtr<UFairyObject>(InTarget);
}

void FFairyTweener::SetTarget2(UFairyObject* InTarget)
{
	_Target = TWeakObjectPtr<UFairyObject>(InTarget);
}

UFairyObject* FFairyTweener::GetTarget2()
{
	if (_Target.IsValid())
	{
		return _Target.Get();
	}
	return nullptr;
}

bool FFairyTweener::IsTargetValid()
{
	return _Target.IsValid();
}

FFairyTweener* FFairyTweener::SetUserData(const FNVariant& InData)
{
	UserData = InData;
	return this;
}

bool FFairyTweener::IsDone() const
{
	return true;
}

FFairyTweener* FFairyTweener::SetPath(TSharedPtr<FGPath> InPath)
{
	Path = InPath;
	return this;
}

FFairyTweener* FFairyTweener::OnUpdate(FTweenDelegate Callback)
{
	OnUpdateCallback = Callback;
	return this;
}

FFairyTweener* FFairyTweener::OnStart(FTweenDelegate Callback)
{
	OnStartCallback = Callback;
	return this;
}

FFairyTweener* FFairyTweener::OnComplete(FTweenDelegate Callback)
{
	OnCompleteCallback = Callback;
	return this;
}

FFairyTweener* FFairyTweener::OnUpdate(FSimpleDelegate Callback)
{
	OnUpdateCallback.BindLambda([Callback](FFairyTweener*) {
		Callback.ExecuteIfBound();
	});
	return this;
}

FFairyTweener* FFairyTweener::OnStart(FSimpleDelegate Callback)
{
	OnStartCallback.BindLambda([Callback](FFairyTweener*) {
		Callback.ExecuteIfBound();
	});
	return this;
}

FFairyTweener* FFairyTweener::OnComplete(FSimpleDelegate Callback)
{
	OnCompleteCallback.BindLambda([Callback](FFairyTweener*) {
		Callback.ExecuteIfBound();
	});
	return this;
}

FFairyTweener* FFairyTweener::SetPaused(bool bInPaused)
{
	bPaused = bInPaused;
	return this;
}

bool FFairyTweener::IsPaused()
{
	return bPaused;
}

void FFairyTweener::Seek(float Time)
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

	DoUpdate();
}

void FFairyTweener::Kill(bool bSetComplete)
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
			DoUpdate();
		}

		OnCompleteCallback.ExecuteIfBound(this);
	}

	bKilled = true;
}

FFairyTweener* FFairyTweener::To(float InStart, float InEnd, float InDuration)
{
	ValueSize = 1;
	StartValue.X = InStart;
	EndValue.X = InEnd;
	Value.X = InStart;
	Duration = InDuration;
	return this;
}

FFairyTweener* FFairyTweener::To(const FVector2D& InStart, const FVector2D& InEnd, float InDuration)
{
	ValueSize = 2;
	StartValue.SetVec2(InStart);
	EndValue.SetVec2(InEnd);
	Value.SetVec2(InStart);
	Duration = InDuration;
	return this;
}

FFairyTweener* FFairyTweener::To(const FVector& InStart, const FVector& InEnd, float InDuration)
{
	ValueSize = 3;
	StartValue.SetVec3(InStart);
	EndValue.SetVec3(InEnd);
	Value.SetVec3(InStart);
	Duration = InDuration;
	return this;
}

FFairyTweener* FFairyTweener::To(const FVector4& InStart, const FVector4& InEnd, float InDuration)
{
	ValueSize = 4;
	StartValue.SetVec4(InStart);
	EndValue.SetVec4(InEnd);
	Value.SetVec4(InStart);
	Duration = InDuration;
	return this;
}

FFairyTweener* FFairyTweener::To(const FColor& InStart, const FColor& InEnd, float InDuration)
{
	ValueSize = 4;
	StartValue.SetColor(InStart);
	EndValue.SetColor(InEnd);
	Value.SetColor(InStart);
	Duration = InDuration;
	return this;
}

FFairyTweener* FFairyTweener::To(double InStart, double InEnd, float InDuration)
{
	ValueSize = 5;
	StartValue.D = InStart;
	EndValue.D = InEnd;
	Value.D = InStart;
	Duration = InDuration;
	return this;
}

FFairyTweener* FFairyTweener::Shake(const FVector2D& InStart, float InAmplitude, float InDuration)
{
	ValueSize = 6;
	StartValue.SetVec2(InStart);
	StartValue.W = InAmplitude;
	Duration = InDuration;
	EaseType = EEaseType::Linear;
	return this;
}

void FFairyTweener::Init()
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

void FFairyTweener::Reset()
{
	Target.Reset();
	UserData.Reset();
	Path.Reset();
	OnStartCallback.Unbind();
	OnUpdateCallback.Unbind();
	OnCompleteCallback.Unbind();
}

void FFairyTweener::Step(float DeltaTime)
{

}

void FFairyTweener::Update(float DeltaTime)
{
	if (Ended != 0) //Maybe completed by seek
	{
		OnCompleteCallback.ExecuteIfBound(this);
		bKilled = true;
		return;
	}

	if (TimeScale != 1)
	{
		DeltaTime *= TimeScale;
	}

	if (DeltaTime == 0)
	{
		return;
	}

	ElapsedTime += DeltaTime;
	DoUpdate();

	if (Ended != 0)
	{
		if (!bKilled)
		{
			OnCompleteCallback.ExecuteIfBound(this);
			bKilled = true;
		}
	}
}

void FFairyTweener::DoUpdate()
{
	Ended = 0;

	if (ValueSize == 0) //DelayedCall
	{
		if (ElapsedTime >= Delay + Duration)
		{
			Ended = 1;
		}

		return;
	}

	if (!bStarted)
	{
		if (ElapsedTime < Delay)
		{
			return;
		}

		bStarted = true;
		OnStartCallback.ExecuteIfBound(this);
		if (bKilled)
		{
			return;
		}
	}

	bool reversed = false;
	float tt = ElapsedTime - Delay;
	if (Breakpoint >= 0 && tt >= Breakpoint)
	{
		tt = Breakpoint;
		Ended = 2;
	}

	if (Repeat != 0)
	{
		int32 round = FMath::FloorToInt(tt / Duration);
		tt -= Duration * round;
		if (bYoyo)
		{
			reversed = round % 2 == 1;
		}

		if (Repeat > 0 && Repeat - round < 0)
		{
			if (bYoyo)
			{
				reversed = Repeat % 2 == 1;
			}
			tt = Duration;
			Ended = 1;
		}
	}
	else if (tt >= Duration)
	{
		tt = Duration;
		Ended = 1;
	}

	NormalizedTime = EaseManager::Evaluate(EaseType, reversed ? (Duration - tt) : tt, Duration, EaseOvershootOrAmplitude, EasePeriod);

	Value.Reset();
	DeltaValue.Reset();

	if (ValueSize == 5)
	{
		double d = StartValue.D + (EndValue.D - StartValue.D) * NormalizedTime;
		if (bSnapping)
		{
			d = round(d);
		}
		DeltaValue.D = d - Value.D;
		Value.D = d;
		Value.X = (float)d;
	}
	else if (ValueSize == 6)
	{
		if (Ended == 0)
		{
			float r = StartValue.W * (1 - NormalizedTime);
			float rx = (FMath::RandRange(0, 1) * 2 - 1) * r;
			float ry = (FMath::RandRange(0, 1) * 2 - 1) * r;
			rx = rx > 0 ? FMath::CeilToFloat(rx) : FMath::FloorToFloat(rx);
			ry = ry > 0 ? FMath::CeilToFloat(ry) : FMath::FloorToFloat(ry);

			DeltaValue.X = rx;
			DeltaValue.Y = ry;
			Value.X = StartValue.X + rx;
			Value.Y = StartValue.Y + ry;
		}
		else
		{
			Value.SetVec3(StartValue.GetVec3());
		}
	}
	else if (Path.IsValid())
	{
		FVector vec3 = Path->GetPointAt(NormalizedTime);
		if (bSnapping)
		{
			vec3.X = FMath::RoundToFloat(vec3.X);
			vec3.Y = FMath::RoundToFloat(vec3.Y);
			vec3.Z = FMath::RoundToFloat(vec3.Z);
		}
		DeltaValue.SetVec3(vec3 - Value.GetVec3());
		Value.SetVec3(vec3);
	}
	else
	{
		for (int32 i = 0; i < ValueSize; i++)
		{
			float n1 = StartValue[i];
			float n2 = EndValue[i];
			float f = n1 + (n2 - n1) * NormalizedTime;
			if (bSnapping)
			{
				f = FMath::RoundToFloat(f);
			}
			DeltaValue[i] = f - Value[i];
			Value[i] = f;
		}
		Value.D = Value.X;
	}

	OnUpdateCallback.ExecuteIfBound(this);
}