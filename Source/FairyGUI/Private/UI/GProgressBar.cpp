#include "UI/GProgressBar.h"
#include "UI/GImage.h"
#include "UI/GLoader.h"
#include "Tween/FairyTweenHelper.h"
#include "Utils/ByteBuffer.h"

UGProgressBar::UGProgressBar() :
    Max(100)
{
}

UGProgressBar::~UGProgressBar()
{
}

void UGProgressBar::SetTitleType(EProgressTitleType InTitleType)
{
    if (TitleType != InTitleType)
    {
        TitleType = InTitleType;
        Update(Value);
    }
}

void UGProgressBar::SetMin(float InMin)
{
    if (Min != InMin)
    {
        Min = InMin;
        Update(Value);
    }
}

void UGProgressBar::SetMax(float InMax)
{
    if (Max != InMax)
    {
        Max = InMax;
        Update(Value);
    }
}

void UGProgressBar::SetValue(float InValue)
{
    if (Value != InValue)
    {
        FFairyTweenHelper::Kill(TweenHandle, false);

        Value = InValue;
        Update(Value);
    }
}

void UGProgressBar::TweenValue(float InValue, float Duration)
{
    float oldValule;

    UFairyTweener* tweener = FFairyTweenHelper::GetTween(TweenHandle);
    if (tweener != nullptr)
    {
        oldValule = tweener->Value.D;
        tweener->Kill(false);
    }
    else
        oldValule = Value;

    Value = InValue;
    TweenHandle = FFairyTweenHelper::To(oldValule, Value, Duration)
        ->SetEase(EEaseType::Linear)
        ->SetTarget(this)
        ->GetHandle();
}

void UGProgressBar::Update(float InValue)
{
    float percent;
    if (Max == Min)
    {
        percent = 0;
    }
    else
    {
        percent = FMath::Clamp<float>((InValue - Min) / (Max - Min), 0, 1);
    }

    if (TitleObject != nullptr)
    {
        FString NewTitle;
        switch (TitleType)
        {
        case EProgressTitleType::Percent:
            NewTitle.AppendInt(FMath::FloorToInt(percent * 100));
            NewTitle.AppendChar('%');
            break;

        case EProgressTitleType::ValueMax:
            NewTitle.AppendInt(FMath::FloorToInt(InValue));
            NewTitle.AppendChar('/');
            NewTitle.AppendInt(FMath::FloorToInt(Max));
            break;

        case EProgressTitleType::Value:
            NewTitle.AppendInt(FMath::FloorToInt(InValue));
            break;

        case EProgressTitleType::Max:
            NewTitle.AppendInt(FMath::FloorToInt(Max));
            break;
        default:
            break;
        }
        TitleObject->SetText(NewTitle);
    }

    FVector2D FullSize = GetSize() - BarMaxSizeDelta;;
    if (!bReverse)
    {
        if (BarObjectH != nullptr)
        {
            if (!SetFillAmount(BarObjectH, percent))
            {
                BarObjectH->SetWidth(FMath::RoundToFloat(FullSize.X * percent));
            }
        }
        if (BarObjectV != nullptr)
        {
            if (!SetFillAmount(BarObjectV, percent))
            {
                BarObjectV->SetHeight(FMath::RoundToFloat(FullSize.Y * percent));
            }
        }
    }
    else
    {
        if (BarObjectH != nullptr)
        {
            if (!SetFillAmount(BarObjectH, 1 - percent))
            {
                BarObjectH->SetWidth(FMath::RoundToFloat(FullSize.X * percent));
                const FVector2D Pos = BarObjectH->GetPosition();
                float PosX = BarStartPosition.X + (FullSize.X - BarObjectH->GetWidth());
                BarObjectH->SetPosition(FVector2D(PosX, Pos.Y));
            }
        }
        if (BarObjectV != nullptr)
        {
            if (!SetFillAmount(BarObjectV, 1 - percent))
            {
                const FVector2D OldPos = BarObjectH->GetPosition();
                float NewPosY = BarStartPosition.Y + (FullSize.Y - BarObjectV->GetHeight());
                BarObjectV->SetPosition(FVector2D(OldPos.X, NewPosY));
            }
        }
    }
}

bool UGProgressBar::SetFillAmount(UFairyObject* Bar, float Amount)
{
    UGImage* image = nullptr;
    UGLoader* loader = nullptr;

    if ((image = Cast<UGImage>(Bar)) != nullptr && image->GetFillMethod() != EFillMethod::None)
        image->SetFillAmount(Amount);
    else if ((loader = Cast<UGLoader>(Bar)) != nullptr && loader->GetFillMethod() != EFillMethod::None)
        loader->SetFillAmount(Amount);
    else
        return false;

    return true;
}

//void UGProgressBar::HandleSizeChanged()
//{
//    UFairyComponent::HandleSizeChanged();
//
//    BarMaxSize = GetSize() - BarMaxSizeDelta;
//
//    if (!bUnderConstruct)
//    {
//        Update(Value);
//    }
//}

void UGProgressBar::ConstructExtension(FByteBuffer* Buffer)
{
    Buffer->Seek(0, 6);

    TitleType = (EProgressTitleType)Buffer->ReadByte();
    bReverse = Buffer->ReadBool();

    TitleObject = GetChild("title");
    BarObjectH = GetChild("bar");
    BarObjectV = GetChild("bar_v");

    if (BarObjectH != nullptr)
    {
        BarMaxSize.X = BarObjectH->GetWidth();
        BarMaxSizeDelta.X = GetWidth() - BarMaxSize.X;
        BarStartPosition.X = BarObjectH->GetPosition().X;
    }
    if (BarObjectV != nullptr)
    {
        BarMaxSize.Y = BarObjectV->GetHeight();
        BarMaxSizeDelta.Y = GetHeight() - BarMaxSize.Y;
        BarStartPosition.Y = BarObjectV->GetPosition().Y;
    }
}

void UGProgressBar::SetupAfterAdd(FByteBuffer* Buffer, int32 BeginPos)
{
    UFairyComponent::SetupAfterAdd(Buffer, BeginPos);

    if (!Buffer->Seek(BeginPos, 6))
    {
        Update(Value);
        return;
    }

    if ((EObjectType)Buffer->ReadByte() != PackageItem->ObjectType)
    {
        Update(Value);
        return;
    }

    Value = Buffer->ReadInt();
    Max = Buffer->ReadInt();
    if (Buffer->Version >= 2)
        Min = Buffer->ReadInt();

    Update(Value);
}

