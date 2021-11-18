#include "Tween/FairyTweenHelper.h"
#include "Tween/TweenManager.h"
#include "Tween/FairyTweener.h"

#include "FairyApplication.h"
#include "UI/GProgressBar.h"

UFairyTweener* FFairyTweenHelper::To(float StartValue, float EndValue, float Duration)
{
    return UTweenManager::Get()->CreateTweener()->To(StartValue, EndValue, Duration);
}

UFairyTweener* FFairyTweenHelper::To(const FVector2D& StartValue, const FVector2D & EndValue, float Duration)
{
    return UTweenManager::Get()->CreateTweener()->To(StartValue, EndValue, Duration);
}

UFairyTweener* FFairyTweenHelper::To(const FVector& StartValue, const FVector & EndValue, float Duration)
{
    return UTweenManager::Get()->CreateTweener()->To(StartValue, EndValue, Duration);
}

UFairyTweener* FFairyTweenHelper::To(const FVector4& StartValue, const FVector4 & EndValue, float Duration)
{
    return UTweenManager::Get()->CreateTweener()->To(StartValue, EndValue, Duration);
}

UFairyTweener* FFairyTweenHelper::To(const FColor& StartValue, const FColor & EndValue, float Duration)
{
    return UTweenManager::Get()->CreateTweener()->To(StartValue, EndValue, Duration);
}

UFairyTweener* FFairyTweenHelper::ToDouble(double StartValue, double EndValue, float Duration)
{
    return UTweenManager::Get()->CreateTweener()->To(StartValue, EndValue, Duration);
}

UFairyTweener* FFairyTweenHelper::DelayedCall(float Delay)
{
    return UTweenManager::Get()->CreateTweener()->SetDelay(Delay);
}

UFairyTweener* FFairyTweenHelper::Shake(const FVector2D& StartValue, float Amplitude, float Duration)
{
    return UTweenManager::Get()->CreateTweener()->Shake(StartValue, Amplitude, Duration);
}

bool FFairyTweenHelper::IsTweening(const FTweenerHandle& Handle)
{
    if (!UFairyApplication::IsStarted())
        return false;

    return UTweenManager::Get()->IsTweening(Handle);
}

bool FFairyTweenHelper::IsTweening(UObject* Target)
{
    if (!UFairyApplication::IsStarted())
        return false;

    return UTweenManager::Get()->IsTweening(Target);
}

void FFairyTweenHelper::Kill(FTweenerHandle& Handle, bool bSetComplete)
{
    if (!UFairyApplication::IsStarted())
        return;

    UTweenManager::Get()->KillTween(Handle, bSetComplete);
}

void FFairyTweenHelper::Kill(UObject* Target, bool bSetComplete)
{
    if (!UFairyApplication::IsStarted())
        return;

    UTweenManager::Get()->KillTweens(Target, bSetComplete);
}

UFairyTweener* FFairyTweenHelper::GetTween(const FTweenerHandle& Handle)
{
    if (!UFairyApplication::IsStarted())
        return nullptr;

    return UTweenManager::Get()->GetTween(Handle);
}

UFairyTweener* FFairyTweenHelper::GetTween(UObject * Target)
{
    if (!UFairyApplication::IsStarted())
        return nullptr;

    return UTweenManager::Get()->GetTween(Target);
}

void FFairyTweenHelper::Action::MoveX(UFairyTweener* Tweener)
{
    UFairyObject * target = Cast<UFairyObject>(Tweener->GetTarget());
    //target->SetX(Tweener->Value.X);
    FVector2D Pos = target->GetPosition();
    target->SetPosition(FVector2D(Tweener->Value.X, Pos.Y));
}

void FFairyTweenHelper::Action::MoveY(UFairyTweener* Tweener)
{
    UFairyObject * target = Cast<UFairyObject>(Tweener->GetTarget());
    //target->SetY(Tweener->Value.X);
}

void FFairyTweenHelper::Action::Move(UFairyTweener* Tweener)
{
    UFairyObject * target = Cast<UFairyObject>(Tweener->GetTarget());
    //target->SetPosition(Tweener->Value.GetVec2());
}

void FFairyTweenHelper::Action::SetWidth(UFairyTweener* Tweener)
{
    UFairyObject * target = Cast<UFairyObject>(Tweener->GetTarget());
    //target->SetWidth(Tweener->Value.X);
}

void FFairyTweenHelper::Action::SetHeight(UFairyTweener* Tweener)
{
    UFairyObject * target = Cast<UFairyObject>(Tweener->GetTarget());
    //target->SetHeight(Tweener->Value.X);
}

void FFairyTweenHelper::Action::SetSize(UFairyTweener* Tweener)
{
    UFairyObject * target = Cast<UFairyObject>(Tweener->GetTarget());
    //target->SetSize(Tweener->Value.GetVec2());
}

void FFairyTweenHelper::Action::ScaleX(UFairyTweener* Tweener)
{
    UFairyObject * target = Cast<UFairyObject>(Tweener->GetTarget());
    //target->SetScaleX(Tweener->Value.X);
}

void FFairyTweenHelper::Action::ScaleY(UFairyTweener* Tweener)
{
    UFairyObject * target = Cast<UFairyObject>(Tweener->GetTarget());
    //target->SetScaleY(Tweener->Value.X);
}

void FFairyTweenHelper::Action::ScaleXY(UFairyTweener* Tweener)
{
    UFairyObject * target = Cast<UFairyObject>(Tweener->GetTarget());
    //target->SetScale(Tweener->Value.GetVec2());
}

void FFairyTweenHelper::Action::Rotate(UFairyTweener* Tweener)
{
    UFairyObject * target = Cast<UFairyObject>(Tweener->GetTarget());
    //target->SetRotation(Tweener->Value.X);
}

void FFairyTweenHelper::Action::SetAlpha(UFairyTweener* Tweener)
{
    UFairyObject * target = Cast<UFairyObject>(Tweener->GetTarget());
    target->SetAlpha(Tweener->Value.X);
}

void FFairyTweenHelper::Action::SetProgress(UFairyTweener* Tweener)
{
    UGProgressBar * target = Cast<UGProgressBar>(Tweener->GetTarget());
    target->Update(Tweener->Value.D);
}
