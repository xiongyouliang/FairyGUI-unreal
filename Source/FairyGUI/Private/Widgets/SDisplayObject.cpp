#include "Widgets/SDisplayObject.h"
#include "Framework/Application/SlateApplication.h"
#include "FairyApplication.h"
//#include "Engine/GameViewportClient.h" // Slate Widget shouldn't reference Engine module class

FNoChildren SDisplayObject::NoChildrenInstance;
FName SDisplayObject::SDisplayObjectTag("SDisplayObjectTag");

SDisplayObject::SDisplayObject() :
    bVisible(true),
    bInteractable(true),
    bTouchable(true),
    bOpaque(true),
    Size(ForceInit)
{
    SetCanTick(false);
    bCanSupportFocus = false;
}

void SDisplayObject::Construct(const SDisplayObject::FArguments& InArgs)
{
    GObject = InArgs._GObject;
    SetTag(InArgs._Tag);
}

const FVector2D& SDisplayObject::GetPosition() const
{
    return LocalPosition;
}

void SDisplayObject::SetPosition(const FVector2D& InPosition)
{
    LocalPosition = InPosition;
    UpdateRenderTransform();
}

void SDisplayObject::SetX(float InX)
{
    LocalPosition.X = InX;
    UpdateRenderTransform();
}

void SDisplayObject::SetY(float InY)
{
    LocalPosition.Y = InY;
    UpdateRenderTransform();
}

void SDisplayObject::UpdateRenderTransform()
{
    float DPIScale = UFairyApplication::Get()->GetDPIScale();
    FVector2D FinalPosition = LocalPosition / DPIScale;
    //if (!GetRenderTransform().IsSet())
    //{
    //    SetRenderTransform(FSlateRenderTransform(FinalPosition));
    //}
    //else
    //{
    //    SetRenderTransform(FSlateRenderTransform(GetRenderTransform()->GetMatrix(), FinalPosition));
    //}
}

void SDisplayObject::SetSize(const FVector2D& InSize)
{
    if (Size != InSize)
    {
        Size = InSize;
        Invalidate(EInvalidateWidget::LayoutAndVolatility);
    }
}

FVector2D SDisplayObject::GetSize()
{
    EnsureSizeCorrect();
    return Size;
}

void SDisplayObject::EnsureSizeCorrect()
{

}

void SDisplayObject::SetVisible(bool bInVisible)
{
    if (bVisible != bInVisible)
    {
        bVisible = bInVisible;
        UpdateVisibilityFlags();
    }
}

void SDisplayObject::SetTouchable(bool bInTouchable)
{
    if (bTouchable != bInTouchable)
    {
        bTouchable = bInTouchable;
        UpdateVisibilityFlags();
    }
}

void SDisplayObject::SetOpaque(bool bInOpaque)
{
    if (bOpaque != bInOpaque)
    {
        bOpaque = bInOpaque;
        UpdateVisibilityFlags();
    }
}

void SDisplayObject::SetInteractable(bool bInInteractable)
{
    if (bInteractable != bInInteractable)
    {
        bInteractable = bInInteractable;
        UpdateVisibilityFlags();
    }
}

void SDisplayObject::UpdateVisibilityFlags()
{
    bool HitTestFlag = bInteractable && bTouchable;
    if (!bVisible)
        SetVisibility(EVisibility::Collapsed);
    else if (!HitTestFlag)
        SetVisibility(EVisibility::HitTestInvisible);
    else if (!bOpaque)
        SetVisibility(EVisibility::SelfHitTestInvisible);
    else
        SetVisibility(EVisibility::All);
}

FVector2D SDisplayObject::ComputeDesiredSize(float) const
{
    FVector2D InViewSize = FVector2D::ZeroVector;
    if (GObject.IsValid())
    {
        InViewSize = GObject->GetRelationSize();
    }
    return InViewSize;
}

FChildren* SDisplayObject::GetChildren()
{
    return &NoChildrenInstance;
}

void SDisplayObject::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
}

int32 SDisplayObject::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    return LayerId;
}

bool SDisplayObject::OnStage() const
{
    if (!IsParentValid())
    {
        return false;
    }

    TSharedPtr<SWindow> Window = FSlateApplication::Get().FindWidgetWindow(AsShared());
    if (Window.IsValid())
    {
        return true;
    }
    else
    {
        return false;
    }
}

FReply SDisplayObject::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    return UFairyApplication::Get()->OnWidgetMouseButtonDown(AsShared(), MyGeometry, MouseEvent);
}

FReply SDisplayObject::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    return UFairyApplication::Get()->OnWidgetMouseButtonUp(AsShared(), MyGeometry, MouseEvent);
}

FReply SDisplayObject::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    return UFairyApplication::Get()->OnWidgetMouseMove(AsShared(), MyGeometry, MouseEvent);
}

FReply SDisplayObject::OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    return UFairyApplication::Get()->OnWidgetMouseButtonDoubleClick(AsShared(), MyGeometry, MouseEvent);
}

void SDisplayObject::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    return UFairyApplication::Get()->OnWidgetMouseEnter(AsShared(), MyGeometry, MouseEvent);
}

void SDisplayObject::OnMouseLeave(const FPointerEvent& MouseEvent)
{
    return UFairyApplication::Get()->OnWidgetMouseLeave(AsShared(), MouseEvent);
}

FReply SDisplayObject::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    return UFairyApplication::Get()->OnWidgetMouseWheel(AsShared(), MyGeometry, MouseEvent);
}