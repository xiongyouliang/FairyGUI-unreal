#include "UI/GScrollBar.h"
#include "Utils/ByteBuffer.h"

UGScrollBar::UGScrollBar()
{
}

UGScrollBar::~UGScrollBar()
{
}


void UGScrollBar::SetScrollPane(UScrollPanel* InTarget, bool bInVertical)
{
    Target = InTarget;
    bVertical = bInVertical;
}

void UGScrollBar::SetDisplayPerc(float Value)
{
    const FVector2D& OldPos = GripObject->GetPosition();
    if (bVertical)
    {
        if (!bFixedGripSize)
        {
            GripObject->SetHeight(FMath::FloorToFloat(Value * BarObject->GetHeight()));
        }
        const float NewPosY = round(BarObject->GetPosition().Y + (BarObject->GetHeight() - GripObject->GetHeight()) * ScrollPerc);
        GripObject->SetPosition(FVector2D(OldPos.X, NewPosY));
    }
    else
    {
        if (!bFixedGripSize)
        {
            GripObject->SetWidth(FMath::FloorToFloat(Value * BarObject->GetWidth()));
        }
        const float NewPosX = round(BarObject->GetPosition().X + (BarObject->GetWidth() - GripObject->GetWidth()) * ScrollPerc);
        GripObject->SetPosition(FVector2D(NewPosX, OldPos.Y));
    }

    GripObject->SetVisible(Value != 0 && Value != 1);
}

void UGScrollBar::SetScrollPerc(float Value)
{
    FVector2D NewPos = GripObject->GetPosition();
    ScrollPerc = Value;
    if (bVertical)
    {
        NewPos.Y = round(BarObject->GetPosition().Y + (BarObject->GetHeight() - GripObject->GetHeight()) * ScrollPerc);
    }
    else
    {
        NewPos.X = round(BarObject->GetPosition().X + (BarObject->GetWidth() - GripObject->GetWidth()) * ScrollPerc);
    }
    GripObject->SetPosition(NewPos);
}

float UGScrollBar::GetMinSize()
{
    if (bVertical)
    {
        return (ArrowButton1 != nullptr ? ArrowButton1->GetHeight() : 0) + (ArrowButton2 != nullptr ? ArrowButton2->GetHeight() : 0);

    }
    else
    {
        return (ArrowButton1 != nullptr ? ArrowButton1->GetWidth() : 0) + (ArrowButton2 != nullptr ? ArrowButton2->GetWidth() : 0);
    }
}

void UGScrollBar::ConstructExtension(FairyGUI::FByteBuffer* buffer)
{
    buffer->Seek(0, 6);

    bFixedGripSize = buffer->ReadBool();

    GripObject = GetChild("grip");
    verifyf(GripObject != nullptr, TEXT("FairyGUI: should define grip"));
    BarObject = GetChild("bar");
    verifyf(BarObject != nullptr, TEXT("FairyGUI: should define bar"));

    ArrowButton1 = GetChild("arrow1");
    ArrowButton2 = GetChild("arrow2");

    GripObject->On(FFairyEventNames::TouchBegin).AddUObject(this, &UGScrollBar::OnGripTouchBegin);
    GripObject->On(FFairyEventNames::TouchMove).AddUObject(this, &UGScrollBar::OnGripTouchMove);
    GripObject->On(FFairyEventNames::TouchEnd).AddUObject(this, &UGScrollBar::OnGripTouchEnd);

    On(FFairyEventNames::TouchBegin).AddUObject(this, &UGScrollBar::OnTouchBeginHandler);

    if (ArrowButton1 != nullptr)
    {
        ArrowButton1->On(FFairyEventNames::TouchBegin).AddUObject(this, &UGScrollBar::OnArrowButton1Click);
    }

    if (ArrowButton2 != nullptr)
    {
        ArrowButton2->On(FFairyEventNames::TouchBegin).AddUObject(this, &UGScrollBar::OnArrowButton2Click);
    }
}

void UGScrollBar::OnTouchBeginHandler(UEventContext* Context)
{
    Context->StopPropagation();

    FVector2D pt = GripObject->GlobalToLocal(Context->GetPointerPosition());
    if (bVertical)
    {
        if (pt.Y < 0)
        {
            //Target->ScrollUp(4, false);
        }
        else
        {
            //Target->ScrollDown(4, false);
        }
    }
    else
    {
        if (pt.X < 0)
        {
            //Target->ScrollLeft(4, false);
        }
        else
        {
            //Target->ScrollRight(4, false);
        }
    }
}

void UGScrollBar::OnGripTouchBegin(UEventContext* Context)
{
    if (BarObject == nullptr)
    {
        return;
    }

    Context->StopPropagation();
    Context->CaptureTouch();

    bGripDragging = true;
    //Target->UpdateScrollBarVisible();

    FVector2D pt = GlobalToLocal(Context->GetPointerPosition());
    DragOffset = pt - GripObject->GetPosition();
}

void UGScrollBar::OnGripTouchMove(UEventContext* Context)
{
    FVector2D pt = GlobalToLocal(Context->GetPointerPosition());
    if (bVertical)
    {
        float curY = pt.Y - DragOffset.Y;
        float diff = BarObject->GetHeight() - GripObject->GetHeight();
        if (diff == 0)
        {
            //Target->SetPercY(0);
        }
        else
        {
            //Target->SetPercY((curY - BarObject->GetPosition().Y) / diff);
        }
    }
    else
    {
        float curX = pt.X - DragOffset.X;
        float diff = BarObject->GetWidth() - GripObject->GetWidth();
        if (diff == 0)
        {
            //Target->SetPercX(0);
        }
        else
        {
            //Target->SetPercX((curX - BarObject->GetPosition().X) / diff);
        }
    }
}

void UGScrollBar::OnGripTouchEnd(UEventContext* Context)
{
    bGripDragging = false;
    Target->UpdateScrollBarVisible();
}

void UGScrollBar::OnArrowButton1Click(UEventContext* Context)
{
    Context->StopPropagation();

    if (bVertical)
    {
        //Target->ScrollUp();
    }
    else
    {
        //Target->ScrollLeft();
    }
}

void UGScrollBar::OnArrowButton2Click(UEventContext* Context)
{
    Context->StopPropagation();

    if (bVertical)
    {
        //Target->ScrollDown();
    }
    else
    {
        //Target->ScrollRight();
    }
}
