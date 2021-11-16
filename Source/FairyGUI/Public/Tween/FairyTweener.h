#pragma once

#include "CoreMinimal.h"
#include "EaseType.h"
#include "TweenValue.h"
#include "TweenerHandle.h"
#include "Utils/NVariant.h"

class FGPath;
class FFairyTweener;

DECLARE_DELEGATE_OneParam(FTweenDelegate, FFairyTweener*);

class FAIRYGUI_API FFairyTweener
{
public:
    FFairyTweener();
    virtual ~FFairyTweener();

    const FTweenerHandle& GetHandle() {
        return Handle;
    }

    FFairyTweener* SetDelay(float InValue);
    float GetDelay() const { return Delay; }

    FFairyTweener* SetDuration(float InValue);
    float GetDuration() const { return Duration; }

    FFairyTweener* SetBreakpoint(float InValue);

    FFairyTweener* SetEase(EEaseType InValue);
    FFairyTweener* SetEasePeriod(float InValue);
    FFairyTweener* SetEaseOvershootOrAmplitude(float InValue);

    FFairyTweener* SetRepeat(int32 InRepeat, bool bInYoyo = false);
    int32 GetRepeat() const { return Repeat; }

    FFairyTweener* SetTimeScale(float InValue);
    FFairyTweener* SetSnapping(bool InValue);

    FFairyTweener* SetTarget(UObject* InTarget);
    UObject* GetTarget() const { return Target.Get(); }

    const FNVariant& GetUserData() const { return UserData; }
    FFairyTweener* SetUserData(const FNVariant& InData);

    FFairyTweener* SetPath(TSharedPtr<FGPath> InPath);

    FFairyTweener* OnUpdate(FTweenDelegate Callback);
    FFairyTweener* OnStart(FTweenDelegate Callback);
    FFairyTweener* OnComplete(FTweenDelegate Callback);
    FFairyTweener* OnUpdate(FSimpleDelegate Callback);
    FFairyTweener* OnStart(FSimpleDelegate Callback);
    FFairyTweener* OnComplete(FSimpleDelegate Callback);

    float GetNormalizedTime() const { return NormalizedTime; }
    bool IsCompleted() const { return Ended != 0; }
    bool AllCompleted() const { return Ended == 1; }
    FFairyTweener* SetPaused(bool bInPaused);
    void Seek(float Time);
    void Kill(bool bSetComplete = false);

    FTweenValue StartValue;
    FTweenValue EndValue;
    FTweenValue Value;
    FTweenValue DeltaValue;

    FFairyTweener* To(float InStart, float InEnd, float InDuration);
    FFairyTweener* To(const FVector2D& InStart, const FVector2D& InEnd, float InDuration);
    FFairyTweener* To(const FVector& InStart, const FVector& InEnd, float InDuration);
    FFairyTweener* To(const FVector4& InStart, const FVector4& InEnd, float InDuration);
    FFairyTweener* To(const FColor& InStart, const FColor& InEnd, float InDuration);
    FFairyTweener* To(double InStart, double InEnd, float InDuration);
    FFairyTweener* Shake(const FVector2D& InStart, float InAmplitude, float InDuration);
    
    void Init();
    void Reset();
    virtual void Update(float DeltaTime);
    virtual void Update();

protected:
    TWeakObjectPtr<UObject> Target;
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
    friend class FTweenManager;
};
