#pragma once

#include "CoreMinimal.h"
#include "Tween/EaseType.h"
#include "Tween/TweenValue.h"
#include "Tween/TweenerHandle.h"
#include "Tween/FairyTweener.h"

class UFairyTweener;

class FAIRYGUI_API FFairyTweenHelper
{
public:
    static UFairyTweener* To(float StartValue, float EndValue, float Duration);
    static UFairyTweener* To(const FVector2D& StartValue, const FVector2D& EndValue, float Duration);
    static UFairyTweener* To(const FVector& StartValue, const FVector& EndValue, float Duration);
    static UFairyTweener* To(const FVector4& StartValue, const FVector4& EndValue, float Duration);
    static UFairyTweener* To(const FColor& StartValue, const FColor& EndValue, float Duration);
    static UFairyTweener* ToDouble(double StartValue, double EndValue, float Duration);
    static UFairyTweener* DelayedCall(float Delay);
    static UFairyTweener* Shake(const FVector2D& StartValue, float Amplitude, float Duration);
    static bool IsTweening(const FTweenerHandle& Handle);
    static bool IsTweening(UObject* Target);
    static UFairyTweener* GetTween(const FTweenerHandle& Handle);
    static UFairyTweener* GetTween(UObject* Target);
    static void Kill(FTweenerHandle& Handle, bool bSetComplete = false);
    static void Kill(UObject* Target, bool bSetComplete = false);
    struct Action
    {
        static void MoveX(UFairyTweener* Tweener);
        static void MoveY(UFairyTweener* Tweener);
        static void Move(UFairyTweener* Tweener);
        static void SetWidth(UFairyTweener* Tweener);
        static void SetHeight(UFairyTweener* Tweener);
        static void SetSize(UFairyTweener* Tweener);
        static void ScaleX(UFairyTweener* Tweener);
        static void ScaleY(UFairyTweener* Tweener);
        static void ScaleXY(UFairyTweener* Tweener);
        static void Rotate(UFairyTweener* Tweener);
        static void SetAlpha(UFairyTweener* Tweener);
        static void SetProgress(UFairyTweener* Tweener);
    };
};
