#include "Widgets/SDisplayObject.h"
#include "Framework/Application/SlateApplication.h"
#include "FairyApplication.h"
#include "UI/FairyObject.h"
//#include "Engine/GameViewportClient.h" // Slate Widget shouldn't reference Engine module class

FNoChildren SDisplayObject::NoChildrenInstance;
FName SDisplayObject::SDisplayObjectTag("SDisplayObjectTag");

SDisplayObject::SDisplayObject() :
	bVisible(true),
	bInteractable(true),
	bTouchable(true),
	bOpaque(true)
{
	SetCanTick(false);
	bCanSupportFocus = false;
}

void SDisplayObject::Construct(const SDisplayObject::FArguments& InArgs)
{
	FairyObject = InArgs._GObject;
	SetTag(InArgs._Tag);
}

FVector2D SDisplayObject::GetInViewSize()
{
	FVector2D InViewSize(ForceInit);
	if (FairyObject.IsValid())
	{
		InViewSize = FairyObject->GetRelationSize();
	}
	return InViewSize;
}

FVector2D SDisplayObject::GetInViewSize() const
{
	FVector2D InViewSize(ForceInit);
	if (FairyObject.IsValid())
	{
		InViewSize = FairyObject->GetRelationSize();
	}
	return InViewSize;
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
	{
		SetVisibility(EVisibility::Collapsed);
	}
	else if (!HitTestFlag)
	{
		SetVisibility(EVisibility::HitTestInvisible);
	}
	else if (!bOpaque)
	{
		SetVisibility(EVisibility::SelfHitTestInvisible);
	}
	else
	{
		SetVisibility(EVisibility::All);
	}
}

FVector2D SDisplayObject::ComputeDesiredSize(float) const
{
	FVector2D InViewSize = FVector2D::ZeroVector;
	if (FairyObject.IsValid())
	{
		InViewSize = FairyObject->GetRelationSize();
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
	//UE_LOG(LogTemp, Warning, TEXT("SDisplayObject::OnMouseEnter()"));
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