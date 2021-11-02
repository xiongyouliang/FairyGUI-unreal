#include "UI/ScrollPanel.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "TimerManager.h"
#include "Package/FairyPackageMgr.h"
#include "UI/GList.h"
#include "UI/Controller/GController.h"
#include "UI/GScrollBar.h"
#include "Utils/ByteBuffer.h"
#include "Tween/GTween.h"
#include "Widgets/SContainer.h"
#include "FairyApplication.h"

TWeakObjectPtr<UScrollPanel> UScrollPanel::DraggingPane;

static const float TWEEN_TIME_GO = 0.5f;      //tween time for SetPos(ani)
static const float TWEEN_TIME_DEFAULT = 0.3f; //min tween time for inertial scroll
static const float PULL_RATIO = 0.5f;         //pull down/up ratio

static inline float sp_EaseFunc(float t, float d)
{
	t = t / d - 1;
	return t * t * t + 1; //cubicOut
}

UScrollPanel::UScrollPanel()
{
}

UScrollPanel::~UScrollPanel()
{
}

void UScrollPanel::AddCommonWidget(UFairyObject* InFairyObject)
{
	if (OwnerRootContainer.IsValid())
	{
		SContainer::FSlot& BarSlot = OwnerRootContainer->AddChild(InFairyObject->GetDisplayObject());
		BarSlot.PositionAttr.BindUFunction(InFairyObject, TEXT("GetRelationPos"));
		BarSlot.SizeAttr.BindUFunction(InFairyObject, TEXT("GetRelationSize"));
		BarSlot.Anchor(InFairyObject->GetAnchor());
	}
}

void UScrollPanel::Setup(FByteBuffer* Buffer)
{
	Owner = Cast<UFairyComponent>(GetOuter());
	OwnerRootContainer = Owner->GetRootContainerWidget();
	OwnerContentContainer = Owner->GetContentContainerWidget();

	ScrollStep = UFairyConfig::Config->DefaultScrollStep;
	DecelerationRate = UFairyConfig::Config->DefaultScrollDecelerationRate;
	bTouchEffect = UFairyConfig::Config->DefaultScrollTouchEffect;
	bBouncebackEffect = UFairyConfig::Config->DefaultScrollBounceEffect;
	bMouseWheelEnabled = true;
	PageSize.Set(0, 0);

	OwnerMaskContainer = Owner->GetMaskContainerWidget();
	OwnerMaskContainer->SetOpaque(false);

	Owner->On(FFairyEventNames::MouseWheel).AddUObject(this, &UScrollPanel::OnMouseWheel);
	Owner->On(FFairyEventNames::TouchBegin).AddUObject(this, &UScrollPanel::OnTouchBegin);
	Owner->On(FFairyEventNames::TouchMove).AddUObject(this, &UScrollPanel::OnTouchMove);
	Owner->On(FFairyEventNames::TouchEnd).AddUObject(this, &UScrollPanel::OnTouchEnd);
	Owner->On(FFairyEventNames::RemovedFromStage).AddLambda([this](UEventContext*) {
		if (DraggingPane.Get() == this)
		{
			DraggingPane.Reset();
		}
	});

	ScrollDirection = (EFairyScrollDirection)Buffer->ReadByte();
	EScrollBarDisplayType ScrollBarDisplayType = (EScrollBarDisplayType)Buffer->ReadByte();
	int32 flags = Buffer->ReadInt(); // 4 bytes

	if (Buffer->ReadBool())
	{
		ScrollBarMargin.Top = Buffer->ReadInt();
		ScrollBarMargin.Bottom = Buffer->ReadInt();
		ScrollBarMargin.Left = Buffer->ReadInt();
		ScrollBarMargin.Right = Buffer->ReadInt();
	}

	const FString& vtScrollBarRes = Buffer->ReadS();
	const FString& hzScrollBarRes = Buffer->ReadS();
	const FString& headerRes = Buffer->ReadS();
	const FString& footerRes = Buffer->ReadS();

	bDisplayOnLeft = (flags & 1) != 0; // 1 bit
	bSnapToItem = (flags & (1 << 1)) != 0; // 2 bit
	bDisplayInDemand = (flags & (1<<2)) != 0; // 3 bit
	bPageMode = (flags & (1<<3)) != 0;
	if ((flags & (1<<4)) != 0)
	{
		bTouchEffect = true;
	}
	else if ((flags & (1<<5)) != 0)
	{
		bTouchEffect = false;
	}

	if ((flags & (1<<6)) != 0)
	{
		bBouncebackEffect = true;
	}
	else if ((flags & (1<<7)) != 0)
	{
		bBouncebackEffect = false;
	}

	bInertiaDisabled = (flags & (1<<8)) != 0;
	if ((flags & (1<<9)) == 0)
	{
		OwnerMaskContainer->SetClipping(EWidgetClipping::ClipToBoundsAlways);
	}
	bFloating = (flags & (1<<10)) != 0;
	bDontClipMargin = (flags & (1<<11)) != 0;

	if (ScrollBarDisplayType != EScrollBarDisplayType::Hidden)
	{
		if (ScrollDirection == EFairyScrollDirection::Both || ScrollDirection == EFairyScrollDirection::Vertical)
		{
			const FString& res = vtScrollBarRes.Len() == 0 ? UFairyConfig::Config->VerticalScrollBar : vtScrollBarRes;
			if (res.Len() > 0)
			{
				VScrollBar = Cast<UGScrollBar>(UFairyPackageMgr::Get()->CreateObjectFromURL(GetOuter(), res));
				if (VScrollBar)
				{
					VScrollBar->SetScrollPane(this, true);
					AddCommonWidget(VScrollBar);
				}
			}
		}
		if (ScrollDirection == EFairyScrollDirection::Both || ScrollDirection == EFairyScrollDirection::Horizontal)
		{
			const FString& res = hzScrollBarRes.Len() == 0 ? UFairyConfig::Config->HorizontalScrollBar : hzScrollBarRes;
			if (res.Len() > 0)
			{
				HScrollBar = Cast<UGScrollBar>(UFairyPackageMgr::Get()->CreateObjectFromURL(GetOuter(), res));
				if (HScrollBar)
				{
					HScrollBar->SetScrollPane(this, false);
					AddCommonWidget(HScrollBar);
				}
			}
		}

		// Auto means show scroll bar while force on mouse, or hidden it
		bScrollBarDisplayAuto = ScrollBarDisplayType == EScrollBarDisplayType::Auto;
		if (bScrollBarDisplayAuto)
		{
			if (VScrollBar != nullptr)
			{
				VScrollBar->SetVisible(false);
			}

			if (HScrollBar != nullptr)
			{
				HScrollBar->SetVisible(false);
			}

			Owner->On(FFairyEventNames::RollOver).AddUObject(this, &UScrollPanel::OnRollOver);
			Owner->On(FFairyEventNames::RollOut).AddUObject(this, &UScrollPanel::OnRollOut);
		}
	}
	else
	{
		bMouseWheelEnabled = false;
	}

	if (headerRes.Len() > 0)
	{
		Header = Cast<UFairyComponent>(UFairyPackageMgr::Get()->CreateObjectFromURL(GetOuter(), headerRes));
		if (Header)
		{
			Header->SetVisible(false);
			AddCommonWidget(Header);
		}
	}

	if (footerRes.Len() > 0)
	{
		Footer = Cast<UFairyComponent>(UFairyPackageMgr::Get()->CreateObjectFromURL(GetOuter(), footerRes));
		if (Footer)
		{
			Footer->SetVisible(false);
			AddCommonWidget(Footer);
		}
	}

	if (Header != nullptr || Footer != nullptr)
	{
		RefreshBarAxis = (ScrollDirection == EFairyScrollDirection::Both || ScrollDirection == EFairyScrollDirection::Vertical) ? 1 : 0;
	}

	SetSize(Owner->GetSize());
}

void UScrollPanel::SetPosX(float Value, bool bAnimation)
{
	Owner->EnsureBoundsCorrect();

	if (LoopMode == 1)
	{
		LoopCheckingNewPos(Value, 0);
	}

	Value = FMath::Clamp(Value, 0.f, OverlapSize.X);
	if (Value != XPos)
	{
		XPos = Value;
		PosChanged(bAnimation);
	}
}

void UScrollPanel::SetPosY(float Value, bool bAnimation)
{
	Owner->EnsureBoundsCorrect();

	if (LoopMode == 2)
	{
		LoopCheckingNewPos(Value, 1);
	}

	Value = FMath::Clamp(Value, 0.f, OverlapSize.Y);
	if (Value != YPos)
	{
		YPos = Value;
		PosChanged(bAnimation);
	}
}

float UScrollPanel::GetPercX() const
{
	return OverlapSize.X == 0 ? 0 : XPos / OverlapSize.X;
}

void UScrollPanel::SetPercX(float Value, bool bAnimation)
{
	Owner->EnsureBoundsCorrect();
	SetPosX(OverlapSize.X * FMath::Clamp(Value, 0.f, 1.f), bAnimation);
}

float UScrollPanel::GetPercY() const
{
	return OverlapSize.Y == 0 ? 0 : YPos / OverlapSize.Y;
}

void UScrollPanel::SetPercY(float Value, bool bAnimation)
{
	Owner->EnsureBoundsCorrect();
	SetPosY(OverlapSize.Y * FMath::Clamp(Value, 0.f, 1.f), bAnimation);
}

bool UScrollPanel::IsBottomMost() const
{
	return YPos == OverlapSize.Y || OverlapSize.Y == 0;
}

bool UScrollPanel::IsRightMost() const
{
	return XPos == OverlapSize.X || OverlapSize.X == 0;
}

void UScrollPanel::ScrollLeft(float Ratio, bool bAnimation)
{
	if (bPageMode)
	{
		SetPosX(XPos - PageSize.X * Ratio, bAnimation);
	}
	else
	{
		SetPosX(XPos - ScrollStep * Ratio, bAnimation);
	}
}

void UScrollPanel::ScrollRight(float Ratio, bool bAnimation)
{
	if (bPageMode)
	{
		SetPosX(XPos + PageSize.X * Ratio, bAnimation);
	}
	else
	{
		SetPosX(XPos + ScrollStep * Ratio, bAnimation);
	}
}

void UScrollPanel::ScrollUp(float Ratio, bool bAnimation)
{
	if (bPageMode)
	{
		SetPosY(YPos - PageSize.Y * Ratio, bAnimation);
	}
	else
	{
		SetPosY(YPos - ScrollStep * Ratio, bAnimation);
	}
}

void UScrollPanel::ScrollDown(float Ratio, bool bAnimation)
{
	if (bPageMode)
	{
		SetPosY(YPos + PageSize.Y * Ratio, bAnimation);
	}
	else
	{
		SetPosY(YPos + ScrollStep * Ratio, bAnimation);
	}
}

void UScrollPanel::ScrollTop(bool bAnimation)
{
	SetPercY(0, bAnimation);
}

void UScrollPanel::ScrollBottom(bool bAnimation)
{
	SetPercY(1, bAnimation);
}

void UScrollPanel::ScrollToView(UFairyObject* Obj, bool bAnimation, bool bSetFirst)
{
	Owner->EnsureBoundsCorrect();
	if (bNeedRefresh)
	{
		Refresh();
	}

	FBox2D rect(Obj->GetPosition(), Obj->GetPosition() + Obj->GetSize());
	if (Obj->GetParent() != Owner)
	{
		rect = Obj->GetParent()->LocalToGlobalRect(rect);
		rect = Owner->GlobalToLocalRect(rect);
	}
	ScrollToView(rect, bAnimation, bSetFirst);
}

void UScrollPanel::ScrollToView(const FBox2D& Rect, bool bAnimation, bool bSetFirst)
{
	Owner->EnsureBoundsCorrect();
	if (bNeedRefresh)
	{
		Refresh();
	}

	if (OverlapSize.Y > 0)
	{
		float bottom = YPos + ViewSize.Y;
		if (bSetFirst || Rect.Min.Y <= YPos || Rect.GetSize().Y >= ViewSize.Y)
		{
			if (bPageMode)
			{
				SetPosY(FMath::FloorToFloat(Rect.Min.Y / PageSize.Y) * PageSize.Y, bAnimation);
			}
			else
			{
				SetPosY(Rect.Min.Y, bAnimation);
			}
		}
		else if (Rect.Max.Y > bottom)
		{
			if (bPageMode)
			{
				SetPosY(FMath::FloorToFloat(Rect.Min.Y / PageSize.Y) * PageSize.Y, bAnimation);
			}
			else if (Rect.GetSize().Y <= ViewSize.Y / 2)
			{
				SetPosY(Rect.Min.Y + Rect.GetSize().Y * 2 - ViewSize.Y, bAnimation);
			}
			else
			{
				SetPosY(Rect.Max.Y - ViewSize.Y, bAnimation);
			}
		}
	}
	if (OverlapSize.X > 0)
	{
		float right = XPos + ViewSize.X;
		if (bSetFirst || Rect.Min.X <= XPos || Rect.GetSize().X >= ViewSize.X)
		{
			if (bPageMode)
			{
				SetPosX(FMath::FloorToFloat(Rect.Min.X / PageSize.X) * PageSize.X, bAnimation);
			}
			SetPosX(Rect.Min.X, bAnimation);
		}
		else if (Rect.Max.X > right)
		{
			if (bPageMode)
			{
				SetPosX(FMath::FloorToFloat(Rect.Min.X / PageSize.X) * PageSize.X, bAnimation);
			}
			else if (Rect.GetSize().X <= ViewSize.X / 2)
			{
				SetPosX(Rect.Min.X + Rect.GetSize().X * 2 - ViewSize.X, bAnimation);
			}
			else
			{
				SetPosX(Rect.Max.X - ViewSize.X, bAnimation);
			}
		}
	}

	if (!bAnimation && bNeedRefresh)
	{
		Refresh();
	}
}

bool UScrollPanel::IsChildInView(UFairyObject* Obj) const
{
	//if (OverlapSize.Y > 0)
	//{
	//	float dist = Obj->GetPosition().Y + Container->GetPosition().Y;
	//	if (dist <= -Obj->GetHeight() || dist >= ViewSize.Y)
	//	{
	//		return false;
	//	}
	//}

	//if (OverlapSize.X > 0)
	//{
	//	float dist = Obj->GetPosition().X + Container->GetPosition().X;
	//	if (dist <= -Obj->GetWidth() || dist >= ViewSize.X)
	//	{
	//		return false;
	//	}
	//}

	return true;
}

int32 UScrollPanel::GetPageX() const
{
	if (!bPageMode)
	{
		return 0;
	}

	int32 page = FMath::FloorToInt(XPos / PageSize.X);
	if (XPos - page * PageSize.X > PageSize.X * 0.5f)
	{
		page++;
	}

	return page;
}

void UScrollPanel::SetPageX(int32 Value, bool bAnimation)
{
	if (!bPageMode)
	{
		return;
	}

	Owner->EnsureBoundsCorrect();

	if (OverlapSize.X > 0)
	{
		SetPosX(Value * PageSize.X, bAnimation);
	}
}

int32 UScrollPanel::GetPageY() const
{
	if (!bPageMode)
	{
		return 0;
	}

	int32 page = FMath::FloorToInt(YPos / PageSize.Y);
	if (YPos - page * PageSize.Y > PageSize.Y * 0.5f)
	{
		page++;
	}

	return page;
}

void UScrollPanel::SetPageY(int32 Value, bool bAnimation)
{
	if (!bPageMode)
	{
		return;
	}

	Owner->EnsureBoundsCorrect();

	if (OverlapSize.Y > 0)
	{
		SetPosY(Value * PageSize.Y, bAnimation);
	}
}

float UScrollPanel::GetScrollingPosX() const
{
	return ContainerPos.X;
}

float UScrollPanel::GetScrollingPosY() const
{
	return ContainerPos.Y;
}

void UScrollPanel::SetViewWidth(float Width)
{
	Width = Width + Owner->Margin.Left + Owner->Margin.Right;
	if (VScrollBar != nullptr && !bFloating)
	{
		Width += VScrollBar->GetWidth();
	}
	Owner->SetWidth(Width);
}

void UScrollPanel::SetViewHeight(float Height)
{
	Height = Height + Owner->Margin.Top + Owner->Margin.Bottom;
	if (HScrollBar != nullptr && !bFloating)
	{
		Height += HScrollBar->GetHeight();
	}
	Owner->SetHeight(Height);
}

void UScrollPanel::LockHeader(int32 Size)
{
	//if (HeaderLockedSize == Size)
	//{
	//	return;
	//}

	//const FVector2D& cpos = Container->GetPosition();

	//HeaderLockedSize = Size;
	//if (!bDispatchingPullDown && cpos.Component(RefreshBarAxis) >= 0)
	//{
	//	TweenStart = cpos;
	//	TweenChange.Set(0, 0);
	//	TweenChange[RefreshBarAxis] = HeaderLockedSize - TweenStart.Component(RefreshBarAxis);
	//	TweenDuration.Set(TWEEN_TIME_DEFAULT, TWEEN_TIME_DEFAULT);
	//	StartTween(2);
	//}
}

void UScrollPanel::LockFooter(int32 Size)
{
	//if (FooterLockedSize == Size)
	//{
	//	return;
	//}

	//const FVector2D& cpos = Container->GetPosition();

	//FooterLockedSize = Size;
	//if (!bDispatchingPullUp && cpos.Component(RefreshBarAxis) >= 0)
	//{
	//	TweenStart = cpos;
	//	TweenChange.Set(0, 0);
	//	float max = OverlapSize.Component(RefreshBarAxis);
	//	if (max == 0)
	//	{
	//		max = FMath::Max(ContentSize.Component(RefreshBarAxis) + FooterLockedSize - ViewSize.Component(RefreshBarAxis), 0.f);
	//	}
	//	else
	//	{
	//		max += FooterLockedSize;
	//	}
	//	TweenChange.Component(RefreshBarAxis) = -max - TweenStart.Component(RefreshBarAxis);
	//	TweenDuration.Set(TWEEN_TIME_DEFAULT, TWEEN_TIME_DEFAULT);
	//	StartTween(2);
	//}
}

void UScrollPanel::CancelDragging()
{
	if (DraggingPane.Get() == this)
	{
		DraggingPane.Reset();
	}

	bDragged = false;
}

void UScrollPanel::ApplyController(UGController* Controller)
{
	if (PageController == Controller)
	{
		if (ScrollDirection == EFairyScrollDirection::Horizontal)
		{
			SetPageX(Controller->GetSelectedIndex(), true);
		}
		else
		{
			SetPageY(Controller->GetSelectedIndex(), true);
		}
	}
}

void UScrollPanel::UpdatePageController()
{
	if (PageController != nullptr && !PageController->bChanging)
	{
		int32 index;
		if (ScrollDirection == EFairyScrollDirection::Horizontal)
		{
			index = GetPageX();

		}
		else
		{
			index = GetPageY();
		}
		if (index < PageController->GetPageCount())
		{
			UGController* Controller = PageController;
			PageController = nullptr; //avoid calling handleControllerChanged
			Controller->SetSelectedIndex(index);
			PageController = Controller;
		}
	}
}

void UScrollPanel::OnOwnerSizeChanged()
{
	SetSize(Owner->GetSize());
	PosChanged(false);
}

void UScrollPanel::SetSize(const FVector2D& InSize)
{
	if (HScrollBar != nullptr)
	{
		HScrollBar->SetPositionY(InSize.Y - HScrollBar->GetHeight());
		if (VScrollBar != nullptr)
		{
			HScrollBar->SetWidth(InSize.X - VScrollBar->GetWidth() - ScrollBarMargin.Left - ScrollBarMargin.Right);
			if (bDisplayOnLeft)
			{
				HScrollBar->SetPositionX(ScrollBarMargin.Left + VScrollBar->GetWidth());
			}
			else
			{
				HScrollBar->SetPositionX(ScrollBarMargin.Left);
			}
		}
		else
		{
			HScrollBar->SetWidth(InSize.X - ScrollBarMargin.Left - ScrollBarMargin.Right);
			HScrollBar->SetPositionX(ScrollBarMargin.Left);
		}
	}

	if (VScrollBar != nullptr)
	{
		if (!bDisplayOnLeft)
		{
			VScrollBar->SetPositionX(InSize.X - VScrollBar->GetWidth());
		}

		if (HScrollBar != nullptr)
		{
			VScrollBar->SetHeight(InSize.Y - HScrollBar->GetHeight() - ScrollBarMargin.Top - ScrollBarMargin.Bottom);
		}
		else
		{
			VScrollBar->SetHeight(InSize.Y - ScrollBarMargin.Top - ScrollBarMargin.Bottom);
		}
		VScrollBar->SetPositionY(ScrollBarMargin.Top);
	}

	ViewSize = InSize;
	if (HScrollBar != nullptr && !bFloating)
	{
		ViewSize.Y -= HScrollBar->GetHeight();
	}
	if (VScrollBar != nullptr && !bFloating)
	{
		ViewSize.X -= VScrollBar->GetWidth();
	}
	ViewSize.X -= (Owner->Margin.Left + Owner->Margin.Right);
	ViewSize.Y -= (Owner->Margin.Top + Owner->Margin.Bottom);

	ViewSize.X = FMath::Max(1.f, ViewSize.X);
	ViewSize.Y = FMath::Max(1.f, ViewSize.Y);
	PageSize = ViewSize;

	HandleSizeChanged();
}

void UScrollPanel::SetContentSize(const FVector2D& InSize)
{
	if (ContentSize == InSize)
	{
		return;
	}

	ContentSize = InSize;
	HandleSizeChanged();
}

void UScrollPanel::ChangeContentSizeOnScrolling(float DeltaWidth, float DeltaHeight, float DeltaPosX, float DeltaPosY)
{
	//bool isRightmost = XPos == OverlapSize.X;
	//bool isBottom = YPos == OverlapSize.Y;

	//ContentSize.X += DeltaWidth;
	//ContentSize.Y += DeltaHeight;
	//HandleSizeChanged();

	//if (Tweening == 1)
	//{
	//	if (DeltaWidth != 0 && isRightmost && TweenChange.X < 0)
	//	{
	//		XPos = OverlapSize.X;
	//		TweenChange.X = -XPos - TweenStart.X;
	//	}

	//	if (DeltaHeight != 0 && isBottom && TweenChange.Y < 0)
	//	{
	//		YPos = OverlapSize.Y;
	//		TweenChange.Y = -YPos - TweenStart.Y;
	//	}
	//}
	//else if (Tweening == 2)
	//{
	//	if (DeltaPosX != 0)
	//	{
	//		Container->SetX(Container->GetPosition().X - DeltaPosX);
	//		TweenStart.X -= DeltaPosX;
	//		XPos = -Container->GetPosition().X;
	//	}
	//	if (DeltaPosY != 0)
	//	{
	//		Container->SetY(Container->GetPosition().Y - DeltaPosY);
	//		TweenStart.Y -= DeltaPosY;
	//		YPos = -Container->GetPosition().Y;
	//	}
	//}
	//else if (bDragged)
	//{
	//	if (DeltaPosX != 0)
	//	{
	//		Container->SetX(Container->GetPosition().X - DeltaPosX);
	//		ContainerPos.X -= DeltaPosX;
	//		XPos = -Container->GetPosition().X;
	//	}
	//	if (DeltaPosY != 0)
	//	{
	//		Container->SetY(Container->GetPosition().Y - DeltaPosY);
	//		ContainerPos.Y -= DeltaPosY;
	//		YPos = -Container->GetPosition().Y;
	//	}
	//}
	//else
	//{
	//	if (DeltaWidth != 0 && isRightmost)
	//	{
	//		XPos = OverlapSize.X;
	//		Container->SetX(Container->GetPosition().X - XPos);
	//	}

	//	if (DeltaHeight != 0 && isBottom)
	//	{
	//		YPos = OverlapSize.Y;
	//		Container->SetY(Container->GetPosition().Y - YPos);
	//	}
	//}

	//if (bPageMode)
	//{
	//	UpdatePageController();
	//}
}

void UScrollPanel::HandleSizeChanged()
{
	if (bDisplayInDemand)
	{
		bVScrollNone = ContentSize.Y <= ViewSize.Y;
		bHScrollNone = ContentSize.X <= ViewSize.X;
	}

	if (VScrollBar != nullptr)
	{
		if (ContentSize.Y == 0)
		{
			VScrollBar->SetDisplayPerc(0);
		}
		else
		{
			VScrollBar->SetDisplayPerc(FMath::Min(1.f, ViewSize.Y / ContentSize.Y));
		}
	}

	if (HScrollBar != nullptr)
	{
		if (ContentSize.X == 0)
		{
			HScrollBar->SetDisplayPerc(0);
		}
		else
		{
			HScrollBar->SetDisplayPerc(FMath::Min(1.f, ViewSize.X / ContentSize.X));
		}
	}
	UpdateScrollBarVisible();

	if (ScrollDirection == EFairyScrollDirection::Horizontal || ScrollDirection == EFairyScrollDirection::Both)
	{
		OverlapSize.X = FMath::CeilToFloat(FMath::Max(0.f, ContentSize.X - ViewSize.X));
	}
	else
	{
		OverlapSize.X = 0;
	}

	if (ScrollDirection == EFairyScrollDirection::Vertical || ScrollDirection == EFairyScrollDirection::Both)
	{
		OverlapSize.Y = FMath::CeilToFloat(FMath::Max(0.f, ContentSize.Y - ViewSize.Y));
	}
	else
	{
		OverlapSize.Y = 0;
	}

	XPos = FMath::Clamp(XPos, 0.f, OverlapSize.X);
	YPos = FMath::Clamp(YPos, 0.f, OverlapSize.Y);
	float max = OverlapSize.Component(RefreshBarAxis);
	if (max == 0)
	{
		max = FMath::Max(ContentSize.Component(RefreshBarAxis) + FooterLockedSize - ViewSize.Component(RefreshBarAxis), 0.f);
	}
	else
	{
		max += FooterLockedSize;
	}

	const FVector2D& Pos = Owner->GetContentContainerPosition();
	if (RefreshBarAxis == 0)
	{
		Owner->SetContentContainerPosition(FVector2D(FMath::Clamp(Pos.X, -max, HeaderLockedSize), FMath::Clamp(Pos.Y, -OverlapSize.Y, 0.f)));
	}
	else
	{
		Owner->SetContentContainerPosition(FVector2D(FMath::Clamp(Pos.X, -OverlapSize.X, 0.f), FMath::Clamp(Pos.Y, -max, HeaderLockedSize)));
	}

	if (Header != nullptr)
	{
		if (RefreshBarAxis == 0)
		{
			Header->SetHeight(ViewSize.Y);
		}
		else
		{
			Header->SetWidth(ViewSize.X);
		}
	}

	if (Footer != nullptr)
	{
		if (RefreshBarAxis == 0)
		{
			Footer->SetHeight(ViewSize.Y);
		}
		else
		{
			Footer->SetWidth(ViewSize.X);
		}
	}

	UpdateScrollBarPos();
	if (bPageMode)
	{
		UpdatePageController();
	}
}

void UScrollPanel::PosChanged(bool bAnimation)
{
	if (AniFlag == 0)
	{
		AniFlag = bAnimation ? 1 : -1;
	}
	else if (AniFlag == 1 && !bAnimation)
	{
		AniFlag = -1;
	}

	bNeedRefresh = true;
	//DelayCall(RefreshTimerHandle, this, &UScrollPanel::Refresh);
	Refresh();
}

void UScrollPanel::Refresh()
{
	CancelDelayCall(RefreshTimerHandle);

	bNeedRefresh = false;

	if (bPageMode || bSnapToItem)
	{
		FVector2D pos(-XPos, -YPos);
		AlignPosition(pos, false);
		XPos = -pos.X;
		YPos = -pos.Y;
	}

	Refresh2();

	Owner->DispatchEvent(FFairyEventNames::Scroll);
	if (bNeedRefresh) //pos may change in onScroll
	{
		bNeedRefresh = false;
		CancelDelayCall(RefreshTimerHandle);

		Refresh2();
	}

	UpdateScrollBarPos();
	AniFlag = 0;
}

void UScrollPanel::Refresh2()
{
	//if (AniFlag == 1 && !bDragged)
	//{
	//	FVector2D pos;

	//	if (OverlapSize.X > 0)
	//	{
	//		pos.X = -(int32)XPos;
	//	}
	//	else
	//	{
	//		if (Container->GetPosition().X != 0)
	//		{
	//			Container->SetX(0);
	//		}
	//		pos.X = 0;
	//	}
	//	if (OverlapSize.Y > 0)
	//	{
	//		pos.Y = -(int32)YPos;
	//	}
	//	else
	//	{
	//		if (Container->GetPosition().Y != 0)
	//		{
	//			Container->SetY(0);
	//		}
	//		pos.Y = 0;
	//	}

	//	if (pos != Container->GetPosition())
	//	{
	//		TweenDuration.Set(TWEEN_TIME_GO, TWEEN_TIME_GO);
	//		TweenStart = Container->GetPosition();
	//		TweenChange = pos - TweenStart;
	//		StartTween(1);
	//	}
	//	else if (Tweening != 0)
	//	{
	//		KillTween();
	//	}
	//}
	//else
	//{
	//	if (Tweening != 0)
	//	{
	//		KillTween();
	//	}

	//	Container->SetPosition(FVector2D((int32)-XPos, (int32)-YPos));

	//	LoopCheckingCurrent();
	//}

	//if (bPageMode)
	//{
	//	UpdatePageController();
	//}
}

void UScrollPanel::UpdateScrollBarPos()
{
	FVector2D ContentContainerPos = Owner->GetContentContainerPosition();
	if (VScrollBar != nullptr)
	{
		VScrollBar->SetScrollPerc(OverlapSize.Y == 0 ? 0 : FMath::Clamp(-ContentContainerPos.Y, 0.f, OverlapSize.Y) / OverlapSize.Y);
	}

	if (HScrollBar != nullptr)
	{
		HScrollBar->SetScrollPerc(OverlapSize.X == 0 ? 0 : FMath::Clamp(-ContentContainerPos.X, 0.f, OverlapSize.X) / OverlapSize.X);
	}

	CheckRefreshBar();
}

void UScrollPanel::UpdateScrollBarVisible()
{
	if (VScrollBar != nullptr)
	{
		if (ViewSize.Y <= VScrollBar->GetMinSize() || bVScrollNone)
		{
			VScrollBar->SetVisible(false);
		}
		else
		{
			UpdateScrollBarVisible2(VScrollBar);
		}
	}

	if (HScrollBar != nullptr)
	{
		if (ViewSize.X <= HScrollBar->GetMinSize() || bHScrollNone)
		{
			HScrollBar->SetVisible(false);
		}
		else
		{
			UpdateScrollBarVisible2(HScrollBar);
		}
	}
}

void UScrollPanel::UpdateScrollBarVisible2(UGScrollBar* Bar)
{
	if (bScrollBarDisplayAuto)
	{
		FGTween::Kill(Bar, false);
	}

	if (bScrollBarDisplayAuto && !bHover && Tweening == 0 && !bDragged && !Bar->bGripDragging)
	{
		if (Bar->IsVisible())
		{
			FGTween::To(1, 0, 0.5f)
				->SetDelay(0.5f)
				->OnUpdate(FTweenDelegate::CreateStatic(&FGTween::Action::SetAlpha))
				->OnComplete(FTweenDelegate::CreateUObject(this, &UScrollPanel::OnBarTweenComplete))
				->SetTarget(Bar);
		}
	}
	else
	{
		Bar->SetAlpha(1);
		Bar->SetVisible(true);
	}
}

void UScrollPanel::OnBarTweenComplete(FGTweener* Tweener)
{
	UFairyObject* bar = (UFairyObject*)Tweener->GetTarget();
	bar->SetAlpha(1);
	bar->SetVisible(false);
}

float UScrollPanel::GetLoopPartSize(float Division, int32 Axis)
{
	return (ContentSize.Component(Axis) + (Axis == 0 ? ((UGList*)Owner)->GetColumnGap() : ((UGList*)Owner)->GetLineGap())) / Division;
}

bool UScrollPanel::LoopCheckingCurrent()
{
	bool changed = false;
	if (LoopMode == 1 && OverlapSize.X > 0)
	{
		if (XPos < 0.001f)
		{
			XPos += GetLoopPartSize(2, 0);
			changed = true;
		}
		else if (XPos >= OverlapSize.X)
		{
			XPos -= GetLoopPartSize(2, 0);
			changed = true;
		}
	}
	else if (LoopMode == 2 && OverlapSize.Y > 0)
	{
		if (YPos < 0.001f)
		{
			YPos += GetLoopPartSize(2, 1);
			changed = true;
		}
		else if (YPos >= OverlapSize.Y)
		{
			YPos -= GetLoopPartSize(2, 1);
			changed = true;
		}
	}

	//if (changed)
	//{
	//	Container->SetPosition(FVector2D((int32)-XPos, (int32)-YPos));
	//}

	return changed;
}

void UScrollPanel::LoopCheckingTarget(FVector2D& EndPos)
{
	if (LoopMode == 1)
	{
		LoopCheckingTarget(EndPos, 0);
	}

	if (LoopMode == 2)
	{
		LoopCheckingTarget(EndPos, 1);
	}
}

void UScrollPanel::LoopCheckingTarget(FVector2D& EndPos, int32 Axis)
{
	if (EndPos.Component(Axis) > 0)
	{
		float halfSize = GetLoopPartSize(2, Axis);
		float tmp = TweenStart.Component(Axis) - halfSize;
		if (tmp <= 0 && tmp >= -OverlapSize.Component(Axis))
		{
			EndPos.Component(Axis) = -halfSize;
			TweenStart.Component(Axis) = tmp;
		}
	}
	else if (EndPos.Component(Axis) < -OverlapSize.Component(Axis))
	{
		float halfSize = GetLoopPartSize(2, Axis);
		float tmp = TweenStart.Component(Axis) + halfSize;
		if (tmp <= 0 && tmp >= -OverlapSize.Component(Axis))
		{
			EndPos.Component(Axis) += halfSize;
			TweenStart.Component(Axis) += tmp;
		}
	}
}

void UScrollPanel::LoopCheckingNewPos(float& Value, int32 Axis)
{
	float overlapSize = OverlapSize.Component(Axis);
	if (overlapSize == 0)
		return;

	float pos = Axis == 0 ? XPos : YPos;
	bool changed = false;
	if (Value < 0.001f)
	{
		Value += GetLoopPartSize(2, Axis);
		if (Value > pos)
		{
			float v = GetLoopPartSize(6, Axis);
			v = FMath::CeilToFloat((Value - pos) / v) * v;
			pos = FMath::Clamp(pos + v, 0.f, overlapSize);
			changed = true;
		}
	}
	else if (Value >= overlapSize)
	{
		Value -= GetLoopPartSize(2, Axis);
		if (Value < pos)
		{
			float v = GetLoopPartSize(6, Axis);
			v = FMath::CeilToFloat((pos - Value) / v) * v;
			pos = FMath::Clamp(pos - v, 0.f, overlapSize);
			changed = true;
		}
	}

	//if (changed)
	//{
	//	if (Axis == 0)
	//	{
	//		Container->SetX(-(int32)pos);
	//	}
	//	else
	//	{
	//		Container->SetY(-(int32)pos);
	//	}
	//}
}

void UScrollPanel::AlignPosition(FVector2D& Pos, bool bInertialScrolling)
{
	if (bPageMode)
	{
		Pos.X = AlignByPage(Pos.X, 0, bInertialScrolling);
		Pos.Y = AlignByPage(Pos.Y, 1, bInertialScrolling);
	}
	else if (bSnapToItem)
	{
		FVector2D tmp = Owner->GetSnappingPosition(-Pos);
		if (Pos.X < 0 && Pos.X > -OverlapSize.X)
		{
			Pos.X = -tmp.X;

		}

		if (Pos.Y < 0 && Pos.Y > -OverlapSize.Y)
		{
			Pos.Y = -tmp.Y;
		}
	}
}

float UScrollPanel::AlignByPage(float Pos, int32 Axis, bool bInertialScrolling)
{
	int32 page;
	float pageSize = PageSize.Component(Axis);
	float overlapSize = OverlapSize.Component(Axis);
	float contentSize = ContentSize.Component(Axis);

	if (Pos > 0)
	{
		page = 0;
	}
	else if (Pos < -overlapSize)
	{
		page = FMath::CeilToFloat(contentSize / pageSize) - 1;
	}
	else
	{
		page = FMath::FloorToInt(-Pos / pageSize);
		float change = 0.0f;// bInertialScrolling ? (Pos - ContainerPos.Component(Axis)) : (Pos - Container->GetPosition().Component(Axis));
		float testPageSize = FMath::Min(pageSize, contentSize - (page + 1) * pageSize);
		float delta = -Pos - page * pageSize;

		if (FMath::Abs(change) > pageSize)
		{
			if (delta > testPageSize * 0.5f)
				page++;
		}
		else
		{
			if (delta > testPageSize * (change < 0 ? 0.3f : 0.7f))
				page++;
		}

		Pos = -page * pageSize;
		if (Pos < -overlapSize)
		{
			Pos = -overlapSize;
		}
	}

	if (bInertialScrolling)
	{
		float oldPos = TweenStart.Component(Axis);
		int32 oldPage;
		if (oldPos > 0)
		{
			oldPage = 0;
		}
		else if (oldPos < -overlapSize)
		{
			oldPage = FMath::CeilToInt(contentSize / pageSize) - 1;
		}
		else
		{
			oldPage = FMath::FloorToInt(-oldPos / pageSize);
		}
		int32 startPage = FMath::FloorToInt(-ContainerPos.Component(Axis) / pageSize);
		if (FMath::Abs(page - startPage) > 1 && FMath::Abs(oldPage - startPage) <= 1)
		{
			if (page > startPage)
			{
				page = startPage + 1;
			}
			else
			{
				page = startPage - 1;
			}
			Pos = -page * pageSize;
		}
	}

	return Pos;
}

FVector2D UScrollPanel::UpdateTargetAndDuration(const FVector2D& OrignPos)
{
	FVector2D ret(0, 0);
	ret.X = UpdateTargetAndDuration(OrignPos.X, 0);
	ret.Y = UpdateTargetAndDuration(OrignPos.Y, 1);
	return ret;
}

float UScrollPanel::UpdateTargetAndDuration(float Pos, int32 Axis)
{
	float v = Velocity.Component(Axis);
	float duration = 0;

	if (Pos > 0)
	{
		Pos = 0;
	}
	else if (Pos < -OverlapSize.Component(Axis))
	{
		Pos = -OverlapSize.Component(Axis);
	}
	else
	{
		float v2 = FMath::Abs(v) * VelocityScale;
		float ratio = 0;
		if (FPlatformMisc::DesktopTouchScreen())
		{
			if (v2 > 500)
			{
				ratio = FMath::Pow((v2 - 500) / 500, 2);
			}
		}
		else
		{
			FVector2D winSize;
			GWorld->GetGameViewport()->GetViewportSize(winSize);
			v2 *= 1136.0f / FMath::Max(winSize.X, winSize.Y);

			if (bPageMode)
			{
				if (v2 > 500)
				{
					ratio = FMath::Pow((v2 - 500) / 500, 2);
				}
			}
			else
			{
				if (v2 > 1000)
				{
					ratio = FMath::Pow((v2 - 1000) / 1000, 2);
				}
			}
		}

		if (ratio != 0)
		{
			if (ratio > 1)
			{
				ratio = 1;
			}

			v2 *= ratio;
			v *= ratio;
			Velocity.Component(Axis) = v;

			duration = FMath::Loge(60 / v2) / FMath::Loge(DecelerationRate) / 60;
			float change = FMath::FloorToFloat(v * duration * 0.4f);
			Pos += change;
		}
	}

	if (duration < TWEEN_TIME_DEFAULT)
	{
		duration = TWEEN_TIME_DEFAULT;
	}
	TweenDuration.Component(Axis) = duration;

	return Pos;
}

void UScrollPanel::FixDuration(int32 Axis, float OldChange)
{
	float tweenChange = TweenChange.Component(Axis);
	if (tweenChange == 0 || FMath::Abs(tweenChange) >= FMath::Abs(OldChange))
	{
		return;
	}

	float newDuration = FMath::Abs(tweenChange / OldChange) * TweenDuration.Component(Axis);
	if (newDuration < TWEEN_TIME_DEFAULT)
	{
		newDuration = TWEEN_TIME_DEFAULT;
	}

	TweenDuration.Component(Axis) = newDuration;
}

void UScrollPanel::StartTween(int32 Type)
{
	TweenTime.Set(0, 0);
	Tweening = Type;
	GWorld->GetTimerManager().SetTimer(TickTimerHandle,
		FTimerDelegate::CreateUObject(this, &UScrollPanel::TweenUpdate),
		0.016f,
		true);
	UpdateScrollBarVisible();
}

void UScrollPanel::KillTween()
{
	if (Tweening == 1)
	{
		FVector2D t = TweenStart + TweenChange;
		//Container->SetPosition(t);
		Owner->DispatchEvent(FFairyEventNames::Scroll);
	}

	Tweening = 0;
	GWorld->GetTimerManager().ClearTimer(TickTimerHandle);
	Owner->DispatchEvent(FFairyEventNames::ScrollEnd);
}

void UScrollPanel::CheckRefreshBar()
{
	if (Header == nullptr && Footer == nullptr)
	{
		return;
	}

	float pos = 0.0f;//Container->GetPosition().Component(RefreshBarAxis);
	if (Header != nullptr)
	{
		if (pos > 0)
		{
			Header->SetVisible(true);
			FVector2D vec;

			vec = Header->GetSize();
			vec.Component(RefreshBarAxis) = pos;
			Header->SetSize(vec);
		}
		else
		{
			Header->SetVisible(false);
		}
	}

	if (Footer != nullptr)
	{
		float max = OverlapSize.Component(RefreshBarAxis);
		if (pos < -max || (max == 0 && FooterLockedSize > 0))
		{
			Footer->SetVisible(true);

			FVector2D vec;

			vec = Footer->GetPosition();
			if (max > 0)
			{
				vec.Component(RefreshBarAxis) = pos + ContentSize.Component(RefreshBarAxis);
			}
			else
			{
				vec.Component(RefreshBarAxis) = FMath::Max(FMath::Min(pos + ViewSize.Component(RefreshBarAxis), ViewSize.Component(RefreshBarAxis) - FooterLockedSize), ViewSize.Component(RefreshBarAxis) - ContentSize.Component(RefreshBarAxis));
			}
			Footer->SetPosition(vec);

			vec = Footer->GetSize();
			if (max > 0)
			{
				vec.Component(RefreshBarAxis) = -max - pos;
			}
			else
			{
				vec.Component(RefreshBarAxis) = ViewSize.Component(RefreshBarAxis) - Footer->GetPosition().Component(RefreshBarAxis);
			}
			Footer->SetSize(vec);
		}
		else
		{
			Footer->SetVisible(false);
		}
	}
}

void UScrollPanel::TweenUpdate()
{
	float dt = GWorld->GetTimerManager().GetTimerElapsed(TickTimerHandle);

	float nx = RunTween(0, dt);
	float ny = RunTween(1, dt);

	//Container->SetPosition(FVector2D(nx, ny));

	if (Tweening == 2)
	{
		if (OverlapSize.X > 0)
		{
			XPos = FMath::Clamp(-nx, 0.f, OverlapSize.X);
		}
		if (OverlapSize.Y > 0)
		{
			YPos = FMath::Clamp(-ny, 0.f, OverlapSize.Y);
		}

		if (bPageMode)
		{
			UpdatePageController();
		}
	}

	if (TweenChange.X == 0 && TweenChange.Y == 0)
	{
		Tweening = 0;
		GWorld->GetTimerManager().ClearTimer(TickTimerHandle);

		LoopCheckingCurrent();

		UpdateScrollBarPos();
		UpdateScrollBarVisible();

		Owner->DispatchEvent(FFairyEventNames::Scroll);
		Owner->DispatchEvent(FFairyEventNames::ScrollEnd);
	}
	else
	{
		UpdateScrollBarPos();
		Owner->DispatchEvent(FFairyEventNames::Scroll);
	}
}

float UScrollPanel::RunTween(int32 Axis, float DeltaTime)
{
	float newValue = 0.0f;
	if (TweenChange.Component(Axis) != 0)
	{
		TweenTime.Component(Axis) += DeltaTime;
		if (TweenTime.Component(Axis) >= TweenDuration.Component(Axis))
		{
			newValue = TweenStart.Component(Axis) + TweenChange.Component(Axis);
			TweenChange.Component(Axis) = 0;
		}
		else
		{
			float ratio = sp_EaseFunc(TweenTime.Component(Axis), TweenDuration.Component(Axis));
			newValue = TweenStart.Component(Axis) + (int32)(TweenChange.Component(Axis) * ratio);
		}

		float threshold1 = 0;
		float threshold2 = -OverlapSize.Component(Axis);
		if (HeaderLockedSize > 0 && RefreshBarAxis == Axis)
		{
			threshold1 = HeaderLockedSize;
		}
		if (FooterLockedSize > 0 && RefreshBarAxis == Axis)
		{
			float max = OverlapSize.Component(RefreshBarAxis);
			if (max == 0)
			{
				max = FMath::Max(ContentSize.Component(RefreshBarAxis) + FooterLockedSize - ViewSize.Component(RefreshBarAxis), 0.f);
			}
			else
			{
				max += FooterLockedSize;
			}
			threshold2 = -max;
		}

		if (Tweening == 2 && bBouncebackEffect)
		{
			if ((newValue > 20 + threshold1 && TweenChange.Component(Axis) > 0) || (newValue > threshold1 && TweenChange.Component(Axis) == 0))
			{
				TweenTime.Component(Axis) = 0;
				TweenDuration.Component(Axis) = TWEEN_TIME_DEFAULT;
				TweenChange.Component(Axis) = -newValue + threshold1;
				TweenStart.Component(Axis) = newValue;
			}
			else if ((newValue < threshold2 - 20 && TweenChange.Component(Axis) < 0) || (newValue < threshold2 && TweenChange.Component(Axis) == 0))
			{
				TweenTime.Component(Axis) = 0;
				TweenDuration.Component(Axis) = TWEEN_TIME_DEFAULT;
				TweenChange.Component(Axis) = threshold2 - newValue;
				TweenStart.Component(Axis) = newValue;
			}
		}
		else
		{
			if (newValue > threshold1)
			{
				newValue = threshold1;
				TweenChange.Component(Axis) = 0;
			}
			else if (newValue < threshold2)
			{
				newValue = threshold2;
				TweenChange.Component(Axis) = 0;
			}
		}
	}
	//else
	//{
		//newValue = Container->GetPosition().Component(Axis);
	//}

	return newValue;
}

void UScrollPanel::OnTouchBegin(UEventContext* Context)
{
	UE_LOG(LogTemp, Warning, TEXT("UScrollPanel::OnTouchBegin(...)"));
	if (!bTouchEffect)
	{
		return;
	}

	Context->CaptureTouch();
	FVector2D curTouchPos = Owner->GlobalToLocal(Context->GetPointerPosition());

	if (Tweening != 0)
	{
		KillTween();
		UFairyApplication::Get()->CancelClick(Context->GetUserIndex(), Context->GetPointerIndex());

		bDragged = true;
	}
	else
	{
		bDragged = false;
	}

	BeginTouchPos = curTouchPos;
	LastTouchPos = curTouchPos;
	LastTouchGlobalPos = Context->GetPointerPosition();

	ContainerPosAtBegin = ContainerPos;

	bIsHoldAreaDone = false;
	Velocity.Set(0, 0);
	VelocityScale = 1;
	LastMoveTime = GWorld->GetTimeSeconds();
}

void UScrollPanel::OnTouchMove(UEventContext* Context)
{
	if (!bTouchEffect)
	{
		return;
	}

	if ((DraggingPane.IsValid() && DraggingPane.Get() != this) || UFairyObject::GetDraggingObject() != nullptr)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("UScrollPanel::OnTouchMove(...)"));

	FVector2D curTouchPos = Owner->GlobalToLocal(Context->GetPointerPosition());
	bool bScrollInVertical = false;
	bool bScrollInHorizontal = false;

	if (ScrollDirection == EFairyScrollDirection::Vertical)
	{
		bScrollInVertical = true;

		ContainerPos.Y = ContainerPosAtBegin.Y + curTouchPos.Y - BeginTouchPos.Y;
	}
	else if (ScrollDirection == EFairyScrollDirection::Horizontal)
	{
		bScrollInHorizontal = true;

		ContainerPos.X = ContainerPosAtBegin.X + curTouchPos.X - BeginTouchPos.X;
	}
	else
	{
		bScrollInVertical = true;
		bScrollInHorizontal = true;

		ContainerPos = ContainerPosAtBegin + curTouchPos - BeginTouchPos;
	}
	LimitContainerPos();
	

	float deltaTime = FSlateApplication::Get().GetDeltaTime();// GWorld->GetDeltaSeconds();
	float elapsed = GWorld->GetTimeSeconds() - LastMoveTime;
	elapsed = elapsed * 60 - 1;
	if (elapsed > 1)
	{
		Velocity *= FMath::Pow(0.833f, elapsed);
	}
	FVector2D deltaPosition = curTouchPos - LastTouchPos;
	if (!bScrollInHorizontal)
	{
		deltaPosition.X = 0;
	}
	if (!bScrollInVertical)
	{
		deltaPosition.Y = 0;
	}
	Velocity = FMath::Lerp(Velocity, deltaPosition / deltaTime, deltaTime * 10);

	FVector2D deltaGlobalPosition = LastTouchGlobalPos - Context->GetPointerPosition();
	if (deltaPosition.X != 0)
	{
		VelocityScale = FMath::Abs(deltaGlobalPosition.X / deltaPosition.X);
	}
	else if (deltaPosition.Y != 0)
	{
		VelocityScale = FMath::Abs(deltaGlobalPosition.Y / deltaPosition.Y);
	}

	LastTouchPos = curTouchPos;
	LastTouchGlobalPos = Context->GetPointerPosition();
	LastMoveTime = GWorld->GetTimeSeconds();

	DraggingPane = this;
	bIsHoldAreaDone = true;
	bDragged = true;

	UpdateScrollBarVisible();
	UpdateScrollBarPos();
	if (bPageMode)
	{
		UpdatePageController();
	}

	Owner->DispatchEvent(FFairyEventNames::Scroll);
}

void UScrollPanel::OnTouchEnd(UEventContext* Context)
{
	UE_LOG(LogTemp, Warning, TEXT("UScrollPanel::OnTouchEnd(...)"));
	if (DraggingPane.Get() == this)
	{
		DraggingPane.Reset();
	}

	if (!bDragged || !bTouchEffect)
	{
		bDragged = false;
		return;
	}

	bDragged = false;
	//TweenStart = Container->GetPosition();

	FVector2D endPos = TweenStart;
	bool flag = false;
	//if (Container->GetPosition().X > 0)
	//{
	//	endPos.X = 0;
	//	flag = true;
	//}
	//else if (Container->GetPosition().X < -OverlapSize.X)
	//{
	//	endPos.X = -OverlapSize.X;
	//	flag = true;
	//}
	//if (Container->GetPosition().Y > 0)
	//{
	//	endPos.Y = 0;
	//	flag = true;
	//}
	//else if (Container->GetPosition().Y < -OverlapSize.Y)
	//{
	//	endPos.Y = -OverlapSize.Y;
	//	flag = true;
	//}

	if (flag)
	{
		TweenChange = endPos - TweenStart;
		if (TweenChange.X < -UFairyConfig::Config->TouchDragSensitivity || TweenChange.Y < -UFairyConfig::Config->TouchDragSensitivity)
		{
			Owner->DispatchEvent(FFairyEventNames::PullDownRelease);
		}
		else if (TweenChange.X > UFairyConfig::Config->TouchDragSensitivity || TweenChange.Y > UFairyConfig::Config->TouchDragSensitivity)
		{
			Owner->DispatchEvent(FFairyEventNames::PullUpRelease);
		}

		if (HeaderLockedSize > 0 && endPos.Component(RefreshBarAxis) == 0)
		{
			endPos.Component(RefreshBarAxis) = HeaderLockedSize;
			TweenChange = endPos - TweenStart;
		}
		else if (FooterLockedSize > 0 && endPos.Component(RefreshBarAxis) == -OverlapSize.Component(RefreshBarAxis))
		{
			float max = OverlapSize.Component(RefreshBarAxis);
			if (max == 0)
			{
				max = FMath::Max(ContentSize.Component(RefreshBarAxis) + FooterLockedSize - ViewSize.Component(RefreshBarAxis), 0.f);
			}
			else
			{
				max += FooterLockedSize;
			}
			endPos.Component(RefreshBarAxis) = -max;
			TweenChange = endPos - TweenStart;
		}
		TweenDuration.Set(TWEEN_TIME_DEFAULT, TWEEN_TIME_DEFAULT);
	}
	else
	{
		if (!bInertiaDisabled)
		{
			float elapsed = GWorld->GetTimeSeconds() - LastMoveTime;
			elapsed = elapsed * 60 - 1;
			if (elapsed > 1)
			{
				Velocity *= FMath::Pow(0.833f, elapsed);
			}
			endPos = UpdateTargetAndDuration(TweenStart);
		}
		else
		{
			TweenDuration.Set(TWEEN_TIME_DEFAULT, TWEEN_TIME_DEFAULT);
		}
		FVector2D oldChange = endPos - TweenStart;

		LoopCheckingTarget(endPos);
		if (bPageMode || bSnapToItem)
		{
			AlignPosition(endPos, true);
		}

		TweenChange = endPos - TweenStart;
		if (TweenChange.X == 0 && TweenChange.Y == 0)
		{
			UpdateScrollBarVisible();
			return;
		}

		if (bPageMode || bSnapToItem)
		{
			FixDuration(0, oldChange.X);
			FixDuration(1, oldChange.Y);
		}
	}

	StartTween(2);
}

void UScrollPanel::OnMouseWheel(UEventContext* Context)
{
	UE_LOG(LogTemp, Warning, TEXT("UScrollPanel::OnMouseWheel(...)"));
	if (!bMouseWheelEnabled)
	{
		return;
	}

	float delta = -Context->GetPointerEvent().GetWheelDelta();
	if (bSnapToItem && FMath::Abs(delta) < 1)
	{
		delta = FMath::Sign(delta);
	}

	if (OverlapSize.X > 0 && OverlapSize.Y == 0)
	{
		float step = bPageMode ? PageSize.X : ScrollStep;
		SetPosX(XPos + step * delta, false);
	}
	else
	{
		float step = bPageMode ? PageSize.Y : ScrollStep;
		SetPosY(YPos + step * delta, false);
	}
}

void UScrollPanel::OnRollOver(UEventContext* Context)
{
	bHover = true;
	UpdateScrollBarVisible();
}

void UScrollPanel::OnRollOut(UEventContext* Context)
{
	bHover = false;
	UpdateScrollBarVisible();
}

void UScrollPanel::LimitContainerPos()
{
	// todo: here not add bounce effect to limit the move position, this will add sometime.
	const FVector2D OwnerPos = Owner->GetPosition();
	const FSlateRect bounds = Owner->GetBounds();
	const FVector2D maskSize = Owner->GetScrollMaskSize();

	float max_x = OwnerPos.X - bounds.Left;
	float min_x = OwnerPos.X - bounds.Right + maskSize.X;
	if (ContainerPos.X > max_x)
	{
		ContainerPos.X = max_x;
	}
	if (ContainerPos.X < min_x)
	{
		ContainerPos.X = min_x;
	}

	float max_y = OwnerPos.Y - bounds.Top;
	float min_y = OwnerPos.Y - bounds.Bottom + maskSize.Y;
	if (ContainerPos.Y > max_y)
	{
		ContainerPos.Y = max_y;
	}
	if (ContainerPos.Y < min_y)
	{
		ContainerPos.Y = min_y;
	}
}
