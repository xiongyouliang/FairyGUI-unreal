#pragma once

#include "CoreMinimal.h"
#include "GTweener.h"
#include "EaseType.h"
#include "TweenValue.h"

class FAIRYGUI_API FFairyTweenHelper
{
public:
    static FFairyTweener* To(float StartValue, float EndValue, float Duration);
    static FFairyTweener* To(const FVector2D& StartValue, const FVector2D& EndValue, float Duration);
    static FFairyTweener* To(const FVector& StartValue, const FVector& EndValue, float Duration);
    static FFairyTweener* To(const FVector4& StartValue, const FVector4& EndValue, float Duration);
    static FFairyTweener* To(const FColor& StartValue, const FColor& EndValue, float Duration);
    static FFairyTweener* ToDouble(double StartValue, double EndValue, float Duration);
    static FFairyTweener* DelayedCall(float Delay);
    static FFairyTweener* Shake(const FVector2D& StartValue, float Amplitude, float Duration);

    static bool IsTweening(const FTweenerHandle& Handle);
    static bool IsTweening(UObject* Target);

    static FFairyTweener* GetTween(const FTweenerHandle& Handle);
    static FFairyTweener* GetTween(UObject* Target);

    static void Kill(FTweenerHandle& Handle, bool bSetComplete = false);
    static void Kill(UObject* Target, bool bSetComplete = false);

    struct Action
    {
        static void MoveX(FFairyTweener* Tweener);
        static void MoveY(FFairyTweener* Tweener);
        static void Move(FFairyTweener* Tweener);
        static void SetWidth(FFairyTweener* Tweener);
        static void SetHeight(FFairyTweener* Tweener);
        static void SetSize(FFairyTweener* Tweener);
        static void ScaleX(FFairyTweener* Tweener);
        static void ScaleY(FFairyTweener* Tweener);
        static void ScaleXY(FFairyTweener* Tweener);
        static void Rotate(FFairyTweener* Tweener);
        static void SetAlpha(FFairyTweener* Tweener);
        static void SetProgress(FFairyTweener* Tweener);
    };
};
