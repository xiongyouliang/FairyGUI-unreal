#include "Tween/FairyTweenHelper.h"
#include "Tween/TweenManager.h"
#include "FairyApplication.h"
#include "UI/GProgressBar.h"

FGTweener* FFairyTweenHelper::To(float StartValue, float EndValue, float Duration)
{
    return UFairyApplication::Get()->TweenManager.CreateTween()->To(StartValue, EndValue, Duration);
}

FGTweener* FFairyTweenHelper::To(const FVector2D& StartValue, const FVector2D & EndValue, float Duration)
{
    return UFairyApplication::Get()->TweenManager.CreateTween()->To(StartValue, EndValue, Duration);
}

FGTweener* FFairyTweenHelper::To(const FVector& StartValue, const FVector & EndValue, float Duration)
{
    return UFairyApplication::Get()->TweenManager.CreateTween()->To(StartValue, EndValue, Duration);
}

FGTweener* FFairyTweenHelper::To(const FVector4& StartValue, const FVector4 & EndValue, float Duration)
{
    return UFairyApplication::Get()->TweenManager.CreateTween()->To(StartValue, EndValue, Duration);
}

FGTweener* FFairyTweenHelper::To(const FColor& StartValue, const FColor & EndValue, float Duration)
{
    return UFairyApplication::Get()->TweenManager.CreateTween()->To(StartValue, EndValue, Duration);
}

FGTweener* FFairyTweenHelper::ToDouble(double StartValue, double EndValue, float Duration)
{
    return UFairyApplication::Get()->TweenManager.CreateTween()->To(StartValue, EndValue, Duration);
}

FGTweener* FFairyTweenHelper::DelayedCall(float Delay)
{
    return UFairyApplication::Get()->TweenManager.CreateTween()->SetDelay(Delay);
}

FGTweener* FFairyTweenHelper::Shake(const FVector2D& StartValue, float Amplitude, float Duration)
{
    return UFairyApplication::Get()->TweenManager.CreateTween()->Shake(StartValue, Amplitude, Duration);
}

bool FFairyTweenHelper::IsTweening(const FTweenerHandle& Handle)
{
    if (!UFairyApplication::IsStarted())
        return false;

    return UFairyApplication::Get()->TweenManager.IsTweening(Handle);
}

bool FFairyTweenHelper::IsTweening(UObject* Target)
{
    if (!UFairyApplication::IsStarted())
        return false;

    return UFairyApplication::Get()->TweenManager.IsTweening(Target);
}

void FFairyTweenHelper::Kill(FTweenerHandle& Handle, bool bSetComplete)
{
    if (!UFairyApplication::IsStarted())
        return;

    UFairyApplication::Get()->TweenManager.KillTween(Handle, bSetComplete);
}

void FFairyTweenHelper::Kill(UObject* Target, bool bSetComplete)
{
    if (!UFairyApplication::IsStarted())
        return;

    UFairyApplication::Get()->TweenManager.KillTweens(Target, bSetComplete);
}

FGTweener* FFairyTweenHelper::GetTween(const FTweenerHandle& Handle)
{
    if (!UFairyApplication::IsStarted())
        return nullptr;

    return UFairyApplication::Get()->TweenManager.GetTween(Handle);
}

FGTweener* FFairyTweenHelper::GetTween(UObject * Target)
{
    if (!UFairyApplication::IsStarted())
        return nullptr;

    return UFairyApplication::Get()->TweenManager.GetTween(Target);
}

void FFairyTweenHelper::Action::MoveX(FGTweener* Tweener)
{
    UFairyObject * target = Cast<UFairyObject>(Tweener->GetTarget());
    //target->SetX(Tweener->Value.X);
    FVector2D Pos = target->GetPosition();
    target->SetPosition(FVector2D(Tweener->Value.X, Pos.Y));
}

void FFairyTweenHelper::Action::MoveY(FGTweener* Tweener)
{
    UFairyObject * target = Cast<UFairyObject>(Tweener->GetTarget());
    //target->SetY(Tweener->Value.X);
}

void FFairyTweenHelper::Action::Move(FGTweener* Tweener)
{
    UFairyObject * target = Cast<UFairyObject>(Tweener->GetTarget());
    //target->SetPosition(Tweener->Value.GetVec2());
}

void FFairyTweenHelper::Action::SetWidth(FGTweener* Tweener)
{
    UFairyObject * target = Cast<UFairyObject>(Tweener->GetTarget());
    //target->SetWidth(Tweener->Value.X);
}

void FFairyTweenHelper::Action::SetHeight(FGTweener* Tweener)
{
    UFairyObject * target = Cast<UFairyObject>(Tweener->GetTarget());
    //target->SetHeight(Tweener->Value.X);
}

void FFairyTweenHelper::Action::SetSize(FGTweener* Tweener)
{
    UFairyObject * target = Cast<UFairyObject>(Tweener->GetTarget());
    //target->SetSize(Tweener->Value.GetVec2());
}

void FFairyTweenHelper::Action::ScaleX(FGTweener* Tweener)
{
    UFairyObject * target = Cast<UFairyObject>(Tweener->GetTarget());
    //target->SetScaleX(Tweener->Value.X);
}

void FFairyTweenHelper::Action::ScaleY(FGTweener* Tweener)
{
    UFairyObject * target = Cast<UFairyObject>(Tweener->GetTarget());
    //target->SetScaleY(Tweener->Value.X);
}

void FFairyTweenHelper::Action::ScaleXY(FGTweener* Tweener)
{
    UFairyObject * target = Cast<UFairyObject>(Tweener->GetTarget());
    //target->SetScale(Tweener->Value.GetVec2());
}

void FFairyTweenHelper::Action::Rotate(FGTweener* Tweener)
{
    UFairyObject * target = Cast<UFairyObject>(Tweener->GetTarget());
    //target->SetRotation(Tweener->Value.X);
}

void FFairyTweenHelper::Action::SetAlpha(FGTweener* Tweener)
{
    UFairyObject * target = Cast<UFairyObject>(Tweener->GetTarget());
    target->SetAlpha(Tweener->Value.X);
}

void FFairyTweenHelper::Action::SetProgress(FGTweener* Tweener)
{
    UGProgressBar * target = Cast<UGProgressBar>(Tweener->GetTarget());
    target->Update(Tweener->Value.D);
}
