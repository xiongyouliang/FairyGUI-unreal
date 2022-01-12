#include "UI/FairyComponent.h"
#include "UI/FairyButton.h"
#include "UI/GGroup.h"
#include "UI/Relation/Relations.h"
#include "UI/UIObjectFactory.h"
#include "Package/FairyPackage.h"
#include "Package/FairyPackageMgr.h"
#include "UI/Controller/FairyController.h"
#include "UI/Controller/FairyControllerMgr.h"
#include "UI/Transition/FairyTransition.h"
#include "UI/FairyRoot.h"
#include "Utils/ByteBuffer.h"
#include "Widgets/SContainer.h"

#include "FairyApplication.h"

static FThreadSafeCounter FairyComponentCount;

UFairyComponent::UFairyComponent() :
	bBuildingDisplayList(false),
	AlignOffset(ForceInit),
	ApexIndex(0),
	bBoundsChanged(false),
	bTrackBounds(false),
	SortingChildCount(0)
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		//UE_LOG(LogFairyGUI, Warning, TEXT("UFairyComponent::UFairyComponent(...), Count:%d"), FairyComponentCount.GetValue());
		FairyComponentCount.Increment();
	}
}

UFairyComponent::~UFairyComponent()
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		FairyComponentCount.Decrement();
		//UE_LOG(LogFairyGUI, Warning, TEXT("UFairyComponent::~UFairyComponent(...), Count:%d"), FairyComponentCount.GetValue());
	}
}

void UFairyComponent::ReleaseSlateResources(bool bReleaseChildren)
{
	if (Container.IsValid())
	{
		Container.Reset();
	}

	Super::ReleaseSlateResources(bReleaseChildren);
}

FSlateRect UFairyComponent::GetBounds()
{
	// Calculate child combine bounds
	FSlateRect TargetRect = Super::GetBounds();
	const FVector2D ChildOffset = FVector2D(TargetRect.Left, TargetRect.Top);
	if (Children.Num() > 0)
	{
		int32 ChildNum = Children.Num();
		for (int32 i = 0; i < ChildNum; ++i)
		{
			UFairyObject* Child = Children[i];
			FSlateRect ChildRect = Child->GetBounds();
			TargetRect = TargetRect.Expand(ChildRect.OffsetBy(ChildOffset));
		}
	}
	return TargetRect;
}

void UFairyComponent::MakeSlateWidget()
{
	if (!DisplayObject.IsValid())
	{
		DisplayObject = Container = SNew(SContainer).GObject(this);
		DisplayObject->SetOpaque(false);
	}
}

TSharedPtr<SContainer> UFairyComponent::GetRootContainerWidget()
{
	return Container;
}

TSharedPtr<SContainer> UFairyComponent::GetContentContainerWidget()
{
	if (ScrollPanel)
	{
		return ScrollWidget;
	}
	else
	{
		return Container;
	}
}

TSharedPtr<SContainer> UFairyComponent::GetMaskContainerWidget()
{
	return MaskWidget;
}

void UFairyComponent::AddWidget(UFairyObject* InChild, int32 index)
{
	TSharedPtr<SContainer> ContentContainer = GetContentContainerWidget();
	int num = ContentContainer->GetChildren()->Num();
	if (index > num)
	{
		index = num;
	}
	
	SContainer::FSlot& Slot = ContentContainer->AddChildAt(InChild->GetDisplayObject(), index);
	Slot.PositionAttr.BindUFunction(InChild, TEXT("GetRelationPos"));
	Slot.SizeAttr.BindUFunction(InChild, TEXT("GetRelationSize"));

	Slot.Anchor(InChild->GetAnchor());
	InChild->SetSlot(&Slot);
}

UFairyObject* UFairyComponent::AddChild(UFairyObject* Child)
{
	AddChildAt(Child, Children.Num());
	return Child;
}

UFairyObject* UFairyComponent::AddChildAt(UFairyObject* Child, int32 Index)
{
	verifyf(Child != nullptr, TEXT("Argument must be non-nil"));
	verifyf(Index >= 0 && Index <= Children.Num(), TEXT("Invalid child index"));

	if (Child->GetParent() == this)
	{
		SetChildIndex(Child, Index);
	}
	else
	{
		Child->RemoveFromParent();
		Child->SetParent(this);

		int32 cnt = Children.Num();
		if ( Child->GetSortingOrder() != 0 )
		{
			SortingChildCount++;
			Index = GetInsertPosForSortingChild(Child);
		}
		else if (SortingChildCount > 0)
		{
			if ( Index > (cnt - SortingChildCount) )
			{
				Index = cnt - SortingChildCount;
			}
		}

		if (Index == cnt)
		{
			Children.Add(Child);
		}
		else 
		{
			Children.Insert(Child, Index);
		}

		ChildStateChanged(Child);
		//SetBoundsChangedFlag();
	}
	return Child;
}

int32 UFairyComponent::GetInsertPosForSortingChild(UFairyObject* Child)
{
	int32 cnt = Children.Num();
	int32 i;
	for (i = 0; i < cnt; i++)
	{
		UFairyObject* Obj = Children[i];
		if (Obj == Child)
		{
			continue;
		}

		if (Child->GetSortingOrder() < Obj->GetSortingOrder())
		{
			break;
		}
	}
	return i;
}

void UFairyComponent::RemoveChild(UFairyObject* Child)
{
	verifyf(Child != nullptr, TEXT("Argument must be non-nil"));

	int32 ChildIndex = Children.Find(Child);
	if (ChildIndex != INDEX_NONE)
	{
		RemoveChildAt(ChildIndex);
	}
}

void UFairyComponent::RemoveChildAt(int32 Index)
{
	verifyf(Index >= 0 && Index < Children.Num(), TEXT("Invalid child index"));

	UFairyObject* Child = Children[Index];

	Child->SetParent(nullptr);

	if (Child->GetSortingOrder() != 0)
	{
		SortingChildCount--;
	}

	Child->SetGroup(nullptr);
	if (Child->GetDisplayObject()->GetParentWidget().IsValid())
	{
		GetContentContainerWidget()->RemoveChild(Child->GetDisplayObject());
		Child->SetSlot(nullptr);
		if (ChildrenRenderOrder == EChildrenRenderOrder::Arch)
		{
			BuildNativeDisplayList();
		}
	}

	Children.RemoveAt(Index);
	SetBoundsChangedFlag();
}

void UFairyComponent::RemoveChildren(int32 BeginIndex, int32 EndIndex)
{
	if (EndIndex < 0 || EndIndex >= Children.Num())
	{
		EndIndex = Children.Num() - 1;
	}

	for (int32 i = BeginIndex; i <= EndIndex; ++i)
	{
		RemoveChildAt(BeginIndex);
	}
}

UFairyObject* UFairyComponent::GetChildAt(int32 Index, TSubclassOf<UFairyObject> ClassType) const
{
	verifyf(Index >= 0 && Index < Children.Num(), TEXT("Invalid child index"));

	return Children[Index];
}

UFairyObject* UFairyComponent::GetChild(const FString& ChildName, TSubclassOf<UFairyObject> ClassType) const
{
	for (const auto& Child : Children)
	{
		if (Child->GetName().Compare(ChildName) == 0)
		{
			return Child;
		}
	}

	return nullptr;
}

UFairyObject* UFairyComponent::GetChildByPath(const FString& Path, TSubclassOf<UFairyObject> ClassType) const
{
	const UFairyComponent* Com = this;
	UFairyObject* Obj = nullptr;

	int32 Index1 = 0, Index2 = -1;
	while ((Index2 = Path.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromStart, Index1)) != -1
		|| Index1 == 0)
	{
		if (Index2 == -1)
		{
			Index2 = Path.Len();
		}

		if (Com == nullptr)
		{
			Com = Cast<UFairyComponent>(Obj);
			if (Com == nullptr)
			{
				Obj = nullptr;
				break;
			}
		}

		Obj = Com->GetChild(Path.Mid(Index1, Index2 - Index1));
		if (!Obj)
		{
			break;
		}

		Com = nullptr;
		Index1 = Index2 + 1;
	}

	return Obj;
}

UFairyObject* UFairyComponent::GetChildInGroup(const UGGroup* InGroup, const FString& ChildName, TSubclassOf<UFairyObject> ClassType) const
{
	verifyf(InGroup != nullptr, TEXT("Argument must be non-nil"));

	for (const auto& Obj : Children)
	{
		if (Obj->GetGroup() == InGroup && Obj->GetName().Compare(ChildName) == 0)
		{
			return Obj;
		}
	}

	return nullptr;
}

UFairyObject* UFairyComponent::GetChildByID(const FString& ChildID) const
{
	for (const auto& Obj : Children)
	{
		if (Obj->GetID().Compare(ChildID) == 0)
		{
			return Obj;
		}
	}

	return nullptr;
}

int32 UFairyComponent::GetChildIndex(const UFairyObject* Child) const
{
	verifyf(Child != nullptr, TEXT("Argument must be non-nil"));

	return Children.IndexOfByKey(Child);
}

void UFairyComponent::SetChildIndex(UFairyObject* Child, int32 Index)
{
	verifyf(Child != nullptr, TEXT("Argument must be non-nil"));

	int32 OldIndex = Children.IndexOfByKey(Child);
	verifyf(OldIndex != -1, TEXT("Not a child of this container"));

	if (Child->GetSortingOrder() != 0) //no effect
	{
		return;
	}

	int32 cnt = Children.Num();
	if (SortingChildCount > 0)
	{
		if (Index > (cnt - SortingChildCount - 1))
		{
			Index = cnt - SortingChildCount - 1;
		}
	}

	MoveChild(Child, OldIndex, Index);
}

int UFairyComponent::SetChildIndexBefore(UFairyObject* Child, int32 Index)
{
	verifyf(Child != nullptr, TEXT("Argument must be non-nil"));

	int32 OldIndex = Children.IndexOfByKey(Child);
	verifyf(OldIndex != -1, TEXT("Not a child of this container"));

	if (Child->GetSortingOrder() != 0) //no effect
	{
		return OldIndex;
	}

	int32 cnt = Children.Num();
	if (SortingChildCount > 0)
	{
		if (Index > (cnt - SortingChildCount - 1))
		{
			Index = cnt - SortingChildCount - 1;
		}
	}

	if (OldIndex < Index)
	{
		return MoveChild(Child, OldIndex, Index - 1);
	}
	else
	{
		return MoveChild(Child, OldIndex, Index);
	}
}

int32 UFairyComponent::MoveChild(UFairyObject* Child, int32 OldIndex, int32 Index)
{
	int32 cnt = Children.Num();
	if (Index > cnt)
	{
		Index = cnt;
	}

	if (OldIndex == Index)
	{
		return OldIndex;
	}

	Children.RemoveAt(OldIndex);
	if (Index >= cnt)
	{
		Children.Add(Child);
	}
	else
	{
		Children.Insert(Child, Index);
	}

	if (Child->GetDisplayObject()->IsParentValid())
	{
		int32 DisplayIndex = 0;
		if (ChildrenRenderOrder == EChildrenRenderOrder::Ascent)
		{
			for (int32 i = 0; i < Index; i++)
			{
				UFairyObject* Obj = Children[i];
				if (Obj->GetDisplayObject()->IsParentValid())
				{
					DisplayIndex++;
				}
			}
			GetContentContainerWidget()->SetChildIndex(Child->GetDisplayObject(), DisplayIndex);
		}
		else if (ChildrenRenderOrder == EChildrenRenderOrder::Descent)
		{
			for (int32 i = cnt - 1; i > Index; i--)
			{
				UFairyObject* Obj = Children[i];
				if (Obj->GetDisplayObject()->IsParentValid())
				{
					DisplayIndex++;
				}
			}
			GetContentContainerWidget()->SetChildIndex(Child->GetDisplayObject(), DisplayIndex);
		}
		else
		{
			BuildNativeDisplayList();
		}

		SetBoundsChangedFlag();
	}

	return Index;
}

void UFairyComponent::SwapChildren(UFairyObject* Child1, UFairyObject* Child2)
{
	verifyf(Child1 != nullptr, TEXT("Argument1 must be non-nil"));
	verifyf(Child2 != nullptr, TEXT("Argument2 must be non-nil"));

	int32 Index1 = Children.IndexOfByKey(Child1);
	int32 Index2 = Children.IndexOfByKey(Child2);

	verifyf(Index1 != -1, TEXT("Not a child of this container"));
	verifyf(Index2 != -1, TEXT("Not a child of this container"));

	SwapChildrenAt(Index1, Index2);
}

void UFairyComponent::SwapChildrenAt(int32 Index1, int32 Index2)
{
	UFairyObject* Child1 = Children[Index1];
	UFairyObject* Child2 = Children[Index2];

	SetChildIndex(Child1, Index2);
	SetChildIndex(Child2, Index1);
}

int32 UFairyComponent::NumChildren() const
{
	return Children.Num();
}

bool UFairyComponent::IsAncestorOf(const UFairyObject* Obj) const
{
	if (Obj == nullptr)
	{
		return false;
	}

	const UFairyComponent* Com = Obj->GetParent();
	while (Com != nullptr)
	{
		if (Com == this)
		{
			return true;
		}

		Com = Com->GetParent();
	}
	return false;
}

bool UFairyComponent::IsChildInView(UFairyObject* Child) const
{
	if (ScrollPanel != nullptr)
	{
		return ScrollPanel->IsChildInView(Child);
	}
	else if (GetDisplayObject()->GetClipping() != EWidgetClipping::Inherit)
	{
		// this calculation is NOT a rigorous way, it is not consider shear(or skew) effect;
		FVector2D ChildPos = Child->GetPosition();
		FVector2D ChildSize = Child->GetSize();
		FVector2D ChildScale = Child->GetScale();

		FVector2D ContainerSize = this->GetSize();
		FVector2D ContainerScale = this->GetScale();
 
		return (ChildPos.X + ChildSize.X*ChildScale.X >= 0 && ChildPos.X < ContainerSize.X*ContainerScale.X) && (ChildPos.Y + ChildSize.Y*ChildScale.Y >= 0 && ChildSize.Y <= ContainerSize.Y*ContainerScale.Y);
		//return Child->GetX() + Child->GetWidth() >= 0 && Child->GetX() <= GetWidth() && Child->GetY() + Child->GetHeight() >= 0 && Child->GetY() <= GetHeight();
	}
	else
	{
		return true;
	}
}

int32 UFairyComponent::GetFirstChildInView() const
{
	int32 i = 0;
	for (auto& Obj : Children)
	{
		if (IsChildInView(Obj))
		{
			return i;
		}
		i++;
	}
	return -1;
}

// *************************************************************************
// *********************** Component Controller start **********************
UFairyController* UFairyComponent::GetController(const FName& ControllerName) const
{
	UFairyControllerMgr* ControllerMgr = UFairyApplication::Get()->GetControllerMgr();
	return ControllerMgr->GetController(this, ControllerName);
}

const TArray<UFairyController*>& UFairyComponent::GetControllers() const
{
	return UFairyApplication::Get()->GetControllerMgr()->GetControllerList(this);
}

void UFairyComponent::AddController(UFairyController* Controller)
{
	UFairyControllerMgr* ControllerMgr = UFairyApplication::Get()->GetControllerMgr();
	ControllerMgr->AddController(this, Controller);
}

UFairyController* UFairyComponent::GetControllerAt(int32 Index) const
{
	return UFairyApplication::Get()->GetControllerMgr()->GetController(this, Index);
}

void UFairyComponent::RemoveController(UFairyController* Controller)
{
	UFairyApplication::Get()->GetControllerMgr()->RemoveController(this, Controller);
}

void UFairyComponent::ApplyController(UFairyController* Controller)
{
	UFairyApplication::Get()->GetControllerMgr()->ApplyController(this, Controller);
}

void UFairyComponent::ApplyAllControllers()
{
	UFairyApplication::Get()->GetControllerMgr()->ApplyAllController(this);
}
// *********************** Component Controller end **********************
// *************************************************************************

void UFairyComponent::AdjustRadioGroupDepth(UFairyObject* Obj, UFairyController* Controller)
{
	int32 cnt = Children.Num();
	int32 i;
	UFairyObject* Child;
	int32 myIndex = -1, maxIndex = -1;
	for (i = 0; i < cnt; i++)
	{
		Child = Children[i];
		if (Child == Obj)
		{
			myIndex = i;
		}
		else if (Child->IsA<UFairyButton>() && ((UFairyButton*)Child)->GetRelatedController() == Controller)
		{
			if (i > maxIndex)
			{
				maxIndex = i;
			}
		}
	}
	if (myIndex < maxIndex)
	{
		SwapChildrenAt(myIndex, maxIndex);
	}
}

bool UFairyComponent::IsOpaque() const
{
	return GetDisplayObject()->IsOpaque();
}

void UFairyComponent::SetOpaque(bool bInOpaque)
{
	GetDisplayObject()->SetOpaque(bInOpaque);
}

void UFairyComponent::SetMargin(const FMargin& InMargin)
{
	Margin = InMargin;
}

void UFairyComponent::SetChildrenRenderOrder(EChildrenRenderOrder InRenderOrder)
{
	if (ChildrenRenderOrder != InRenderOrder)
	{
		ChildrenRenderOrder = InRenderOrder;
		BuildNativeDisplayList();
	}
}

void UFairyComponent::SetApexIndex(int32 InApexIndex)
{
	if (ApexIndex != InApexIndex)
	{
		ApexIndex = InApexIndex;

		if (ChildrenRenderOrder == EChildrenRenderOrder::Arch)
		{
			BuildNativeDisplayList();
		}
	}
}

float UFairyComponent::GetViewWidth() const
{
	if (ScrollPanel != nullptr)
	{
		return ScrollPanel->GetViewSize().X;
	}
	else
	{
		return Size.X - Margin.Left - Margin.Right;
	}
}

void UFairyComponent::SetViewWidth(float InViewWidth)
{
	if (ScrollPanel != nullptr)
	{
		ScrollPanel->SetViewWidth(InViewWidth);
	}
	else
	{
		SetWidth(InViewWidth + Margin.Left + Margin.Right);
	}	
}

float UFairyComponent::GetViewHeight() const
{
	if (ScrollPanel != nullptr)
	{
		return ScrollPanel->GetViewSize().Y;
	}
	else
	{
		return Size.Y - Margin.Top - Margin.Bottom;
	}
}

void UFairyComponent::SetViewHeight(float InViewHeight)
{
	if (ScrollPanel != nullptr)
	{
		ScrollPanel->SetViewHeight(InViewHeight);
	}
	else
	{
		SetHeight(InViewHeight + Margin.Top + Margin.Bottom);
	}
}

void UFairyComponent::SetBoundsChangedFlag()
{
	if (bBoundsChanged)
	{
		return;
	}

	bBoundsChanged = true;

	EnsureBoundsCorrect();
}

void UFairyComponent::EnsureBoundsCorrect()
{
	if (bBoundsChanged)
	{
		UpdateBounds();
	}
}

void UFairyComponent::UpdateBounds()
{
	float left = FLT_MAX;
	float top = FLT_MAX;
	float right = FLT_MIN;
	float bottom = FLT_MIN;
	if (Children.Num() > 0)
	{
		int32 ChildNum = Children.Num();
		for (int32 i = 0; i < ChildNum; ++i)
		{
			UFairyObject* child = Children[i];
			const FVector2D ChildPos = child->GetPosition();
			const FVector2D ChildSize = child->GetSize(); // todo: if child is a Component, and its bound not equal its size, will not get right bound for this component;
			const FVector2D ChildAnchor = child->GetAnchor();

			const float ChildLeftPos = ChildPos.X - ChildSize.X * ChildAnchor.X;
			if (ChildLeftPos < left)
			{
				left = ChildLeftPos;
			}

			const float ChildTopPos = ChildPos.Y - ChildSize.Y * ChildAnchor.Y;
			if (ChildTopPos < top)
			{
				top = ChildTopPos;
			}

			const float ChildRightPos = ChildPos.X + ChildSize.X * ChildAnchor.X;
			if (ChildRightPos > right)
			{
				right = ChildRightPos;
			}

			const float ChildBottomPos = ChildPos.Y + ChildSize.Y * ChildAnchor.Y;
			if (ChildBottomPos > bottom)
			{
				bottom = ChildBottomPos;
			}
		}
	}
	SetBounds(left, top, right-left, bottom-top);
}

void UFairyComponent::SetBounds(float ax, float ay, float aw, float ah)
{
	bBoundsChanged = false;
}

void UFairyComponent::ChildStateChanged(UFairyObject* Child)
{
	if (bBuildingDisplayList)
	{
		return;
	}

	int32 ChildNum = Children.Num();

	if (Child->InternalVisible())
	{
		if (!Child->GetDisplayObject()->IsParentValid())
		{
			if (ChildrenRenderOrder == EChildrenRenderOrder::Ascent)
			{
				int32 index = 0;
				for (int32 i = 0; i < ChildNum; i++)
				{
					UFairyObject* Obj = Children[i];
					if (Obj == Child)
					{
						break;
					}

					if (Obj->GetDisplayObject()->IsParentValid())
					{
						index++;
					}
				}
				AddWidget(Child, index);
			}
			else if (ChildrenRenderOrder == EChildrenRenderOrder::Descent)
			{
				int32 index = 0;
				for (int32 i = ChildNum - 1; i >= 0; i--)
				{
					UFairyObject* Obj = Children[i];
					if (Obj == Child)
					{
						break;
					}

					if (Obj->GetDisplayObject()->IsParentValid())
					{
						index++;
					}
				}
				AddWidget(Child, index);
			}
			else
			{
				BuildNativeDisplayList();
			}
		}
	}
	else
	{
		if (Child->GetDisplayObject()->IsParentValid())
		{
			GetContentContainerWidget()->RemoveChild(Child->GetDisplayObject());
			Child->SetSlot(nullptr);
			if (ChildrenRenderOrder == EChildrenRenderOrder::Arch)
			{
				BuildNativeDisplayList();
			}
		}
	}
}

void UFairyComponent::ChildSortingOrderChanged(UFairyObject* Child, int32 OldValue, int32 NewValue)
{
	if (NewValue == 0)
	{
		SortingChildCount--;
		SetChildIndex(Child, Children.Num());
	}
	else
	{
		if (OldValue == 0)
		{
			SortingChildCount++;
		}

		int32 OldIndex = Children.IndexOfByKey(Child);
		int32 Index = GetInsertPosForSortingChild(Child);
		if (OldIndex < Index)
		{
			MoveChild(Child, OldIndex, Index - 1);

		}
		else
		{
			MoveChild(Child, OldIndex, Index);
		}
	}
}

void UFairyComponent::BuildNativeDisplayList(bool bImmediatelly)
{
	int32 cnt = Children.Num();
	if (cnt == 0)
	{
		return;
	}

	switch (ChildrenRenderOrder)
	{
	case EChildrenRenderOrder::Ascent:
		{
			for (int32 i = 0; i < cnt; i++)
			{
				UFairyObject* Child = Children[i];
				if (Child->InternalVisible())
				{
					AddWidget(Child, i);
				}
			}
		}
		break;
	case EChildrenRenderOrder::Descent:
		{
			for (int32 i = 0; i < cnt; i++)
			{
				UFairyObject* Child = Children[i];
				if (Child->InternalVisible())
				{
					AddWidget(Child, i);
				}
			}
		}
		break;
	case EChildrenRenderOrder::Arch:
		{
			int32 ai = FMath::Min(ApexIndex, cnt);
			for (int32 i = 0; i < ai; i++)
			{
				UFairyObject* Child = Children[i];
				if (Child->InternalVisible())
				{
					AddWidget(Child, i);
				}
			}
			for (int32 i = cnt - 1; i >= ai; i--)
			{
				UFairyObject* Child = Children[i];
				if (Child->InternalVisible())
				{
					AddWidget(Child, i);
				}
			}
		}
		break;
	default:
		break;
	}
}

FVector2D UFairyComponent::GetSnappingPosition(const FVector2D& InPoint)
{
	int32 ChildNum = Children.Num();
	if (ChildNum == 0)
	{
		return InPoint;
	}

	EnsureBoundsCorrect();

	UFairyObject* Obj = nullptr;

	FVector2D ret = InPoint;

	int32 i = 0;
	if (ret.Y != 0)
	{
		for (; i < ChildNum; i++)
		{
			Obj = Children[i];
			const FVector2D& CurChildPos = Obj->GetPosition();
			if (ret.Y < CurChildPos.Y)
			{
				if (i == 0)
				{
					ret.Y = 0;
					break;
				}
				else
				{
					UFairyObject* prev = Children[i - 1];
					const FVector2D& PreChildPos = prev->GetPosition();
					const FVector2D& PreChildSize = prev->GetSize();
					if (ret.Y < PreChildPos.Y + PreChildSize.Y / 2) //top half part
					{
						ret.Y = PreChildPos.Y;
					}
					else //bottom half part
					{
						ret.Y = CurChildPos.Y;
					}
					break;
				}
			}
		}

		if (i == ChildNum)
		{
			const FVector2D& Pos = Obj->GetPosition();
			ret.Y = Pos.Y;
		}
	}

	if (ret.X != 0)
	{
		if (i > 0)
		{
			i--;
		}
		for (; i < ChildNum; i++)
		{
			Obj = Children[i];
			const FVector2D& CurChildPos = Obj->GetPosition();
			if (ret.X < CurChildPos.X)
			{
				if (i == 0)
				{
					ret.X = 0;
					break;
				}
				else
				{
					UFairyObject* prev = Children[i - 1];
					const FVector2D& PreChildPos = prev->GetPosition();
					const FVector2D& PreChildSize = prev->GetSize();
					if (ret.X < PreChildPos.X + PreChildSize.X / 2) // top half part
					{
						ret.X = PreChildPos.X;
					}
					else //bottom half part
					{
						ret.X = CurChildPos.X;
					}
					break;
				}
			}
		}
		if (i == ChildNum)
		{
			const FVector2D& Pos = Obj->GetPosition();
			ret.X = Pos.X;
		}
	}

	return ret;
}

void UFairyComponent::SetupOverflow(EOverflowType InOverflow)
{
	if (InOverflow == EOverflowType::Hidden)
	{
		GetDisplayObject()->SetClipping(EWidgetClipping::ClipToBoundsAlways);
		GetDisplayObject()->SetCullingBoundsExtension(Margin);
	}
}

void UFairyComponent::SetupScroll(FairyGUI::FByteBuffer* Buffer)
{
	if (!MaskWidget.IsValid() &&Container.IsValid())
	{
		const TSharedPtr<SContainer> RootContainer = SNew(SContainer).GObject(this);
		const TSharedRef<SContainer> MaskContainer = SNew(SContainer).GObject(this);
		const TSharedRef<SContainer> ScrollContainer = Container.ToSharedRef();

		SContainer::FSlot& MaskContainerSlot = RootContainer->AddChild(MaskContainer);
		MaskContainerSlot.SizeAttr.BindUObject(this, &UFairyComponent::GetScrollMaskSize);
		MaskWidgetSlot = &MaskContainerSlot;

		SContainer::FSlot& ContentContainerSlot = MaskContainer->AddChild(ScrollContainer);
		ContentContainerSlot.SizeAttr.BindUObject(this, &UFairyComponent::GetScrollContentSize);
		ContentContainerSlot.PositionAttr.BindUObject(this, &UFairyComponent::GetScrollContentPosition);
		ScrollWidgetSlot = &ContentContainerSlot;

		DisplayObject = Container = RootContainer;

		ScrollWidget = ScrollContainer;
		MaskWidget = MaskContainer;
	}

	ScrollPanel = NewObject<UScrollPanel>(this);
	ScrollPanel->Setup(Buffer);
}

//FVector2D UFairyComponent::GetScrollMaskSizeForBind()
//{
//	return GetScrollMaskSize();
//}
//
//FVector2D UFairyComponent::GetScrollContentSizeForBind()
//{
//	return GetScrollContentSize();
//}
//
//FVector2D UFairyComponent::GetScrollContentPositionForBind()
//{
//	return GetScrollContentPosition();
//}

FVector2D UFairyComponent::GetScrollMaskSize() const
{
	if (ScrollPanel)
	{
		return ScrollPanel->GetViewSize();
	}
	return FVector2D::ZeroVector;
}

FVector2D UFairyComponent::GetScrollContentSize() const
{
	if (ScrollPanel)
	{
		return ScrollPanel->GetContentSize();
	}
	return FVector2D::ZeroVector;
}

void UFairyComponent::SetScrollContentSize(FVector2D InSize)
{
	if (ScrollPanel)
	{
		ScrollPanel->ContentSize = InSize;
	}
}

FVector2D UFairyComponent::GetScrollContentPosition() const
{
	if (ScrollPanel)
	{
		return ScrollPanel->ContainerPos;
	}
	return FVector2D::ZeroVector;
}

void UFairyComponent::SetScrollContentPosition(FVector2D InPosition)
{
	if (ScrollPanel)
	{
		ScrollPanel->ContainerPos = InPosition;
	}
}

void UFairyComponent::HandleGrayedChanged()
{
	UFairyObject::HandleGrayedChanged();

	UFairyController* Controller = GetController("grayed");
	if (Controller != nullptr)
	{
		Controller->SetSelectedIndex(IsGrayed() ? 1 : 0);
	}
	else
	{
		for (auto& Child : Children)
		{
			Child->HandleGrayedChanged();
		}
	}
}

void UFairyComponent::OnAddedToStageHandler(UEventContext* Context)
{
	UFairyApplication::Get()->GetTransitionMgr()->OnComponentEnter(this);
}

void UFairyComponent::OnRemovedFromStageHandler(UEventContext* Context)
{
	UFairyApplication::Get()->GetTransitionMgr()->OnComponentExit(this);
}

// ***************************** Parse Component start ******************************************
void UFairyComponent::ConstructFromResource()
{
	MakeSlateWidget();
	ConstructFromResource(nullptr, 0);
}

void UFairyComponent::ConstructFromResource(TArray<UFairyObject*>* ObjectPool, int32 PoolIndex)
{
	TSharedPtr<FFairyPackageItem> CurPackageItem = PackageItem->GetBranch();

	if (!CurPackageItem->bTranslated)
	{
		CurPackageItem->bTranslated = true;
	}

	FairyGUI::FByteBuffer* CurPackageItemBuffer = CurPackageItem->RawData.Get();
	CurPackageItemBuffer->Seek(0, 0);

	bUnderConstruct = true;

	// Set this Fairy Object Size attribute
	float SizeX = CurPackageItemBuffer->ReadInt();
	float SizeY = CurPackageItemBuffer->ReadInt();
	this->SetSize(FVector2D(SizeX, SizeY));

	if (CurPackageItemBuffer->ReadBool())
	{
		this->MinSize.X = CurPackageItemBuffer->ReadInt();
		this->MaxSize.X = CurPackageItemBuffer->ReadInt();
		this->MinSize.Y = CurPackageItemBuffer->ReadInt();
		this->MaxSize.Y = CurPackageItemBuffer->ReadInt();
	}

	// Set this Object Pivot attribute
	if (CurPackageItemBuffer->ReadBool())
	{
		float PivotX = CurPackageItemBuffer->ReadFloat();
		float PivotY = CurPackageItemBuffer->ReadFloat();
		bool PivotAsAnchor = CurPackageItemBuffer->ReadBool();
		SetPivot(FVector2D(PivotX, PivotY), PivotAsAnchor);
	}

	// Set this Component Margin attribute
	if (CurPackageItemBuffer->ReadBool())
	{
		Margin.Top = CurPackageItemBuffer->ReadInt();
		Margin.Bottom = CurPackageItemBuffer->ReadInt();
		Margin.Left = CurPackageItemBuffer->ReadInt();
		Margin.Right = CurPackageItemBuffer->ReadInt();
	}

	// Setup overflow
	int32 cursorPos = CurPackageItemBuffer->GetPos();
	EOverflowType overflow = (EOverflowType)CurPackageItemBuffer->ReadByte();
	if (overflow == EOverflowType::Scroll)
	{
		int32 savedPos = CurPackageItemBuffer->GetPos();
		CurPackageItemBuffer->Seek(0, 7);

		SetupScroll(CurPackageItemBuffer);

		CurPackageItemBuffer->SetPos(savedPos);
	}
	else
	{
		SetupOverflow(overflow);
	}

	if (CurPackageItemBuffer->ReadBool()) //clipsoft
	{
		CurPackageItemBuffer->Skip(8);
	}

	bBuildingDisplayList = true;

	// Parse all controller for this UFairyComponent
	CurPackageItemBuffer->Seek(0, 1);
	int32 controllerCount = CurPackageItemBuffer->ReadShort();
	for (int32 i = 0; i < controllerCount; i++)
	{
		int32 nextPos = CurPackageItemBuffer->ReadShort();
		nextPos += CurPackageItemBuffer->GetPos();

		UFairyController* Controller = NewObject<UFairyController>(this);
		Controller->Setup(CurPackageItemBuffer);

		UFairyApplication::Get()->GetControllerMgr()->AddController(this, Controller);

		CurPackageItemBuffer->SetPos(nextPos);
	}

	// Parse Children
	CurPackageItemBuffer->Seek(0, 2);
	UFairyObject* Child = nullptr;
	int32 childCount = CurPackageItemBuffer->ReadShort();
	for (int32 i = 0; i < childCount; i++)
	{
		int32 dataLen = CurPackageItemBuffer->ReadShort();
		int32 curPos = CurPackageItemBuffer->GetPos();

		if (ObjectPool != nullptr) 
		{
			Child = (*ObjectPool)[PoolIndex + i];
		}
		else
		{
			CurPackageItemBuffer->Seek(curPos, 0);

			EObjectType ObjectType = (EObjectType)CurPackageItemBuffer->ReadByte();
			//const FString& src = CurPackageItemBuffer->ReadS();
			//const FString& PackageID = CurPackageItemBuffer->ReadS();
			FName src = CurPackageItemBuffer->ReadFNameFromCache();
			FName PackageID = CurPackageItemBuffer->ReadFNameFromCache();

			TSharedPtr<FFairyPackageItem> ChildPackageItem = nullptr;
			if (!src.IsNone())
			{
				UFairyPackage* Package;
				if (!PackageID.IsNone())
				{
					Package = UFairyPackageMgr::Get()->GetPackageByID(PackageID);
				}
				else 
				{
					Package = CurPackageItem->OwnerPackage;
				}

				if (Package != nullptr)
				{
					ChildPackageItem = Package->GetItem(src);
				}
			}

			if (ChildPackageItem.IsValid())
			{
				Child = FUIObjectFactory::NewObject(GetOuter(), ChildPackageItem);
				Child->MakeSlateWidget();
				Child->ConstructFromResource();
			}
			else
			{
				Child = FUIObjectFactory::NewObject(GetOuter(), ObjectType);
				Child->MakeSlateWidget();
			}
		}

		Child->bUnderConstruct = true;
		Child->SetupBeforeAdd(CurPackageItemBuffer, curPos);

		AddChild(Child);

		CurPackageItemBuffer->SetPos(curPos + dataLen);
	}

	CurPackageItemBuffer->Seek(0, 3);
	GetRelations().Setup(CurPackageItemBuffer, true); // setup the Component relation info

	CurPackageItemBuffer->Seek(0, 2);
	CurPackageItemBuffer->Skip(2);

	// setup the children relation info
	for (int32 i = 0; i < childCount; i++)
	{
		int32 nextPos = CurPackageItemBuffer->ReadShort();
		nextPos += CurPackageItemBuffer->GetPos();

		CurPackageItemBuffer->Seek(CurPackageItemBuffer->GetPos(), 3);
		Children[i]->GetRelations().Setup(CurPackageItemBuffer, false);

		CurPackageItemBuffer->SetPos(nextPos);
	}

	CurPackageItemBuffer->Seek(0, 2);
	CurPackageItemBuffer->Skip(2);

	for (int32 i = 0; i < childCount; i++)
	{
		int32 nextPos = CurPackageItemBuffer->ReadShort();
		nextPos += CurPackageItemBuffer->GetPos();

		Child = Children[i];
		Child->SetupAfterAdd(CurPackageItemBuffer, CurPackageItemBuffer->GetPos());
		Child->bUnderConstruct = false;

		CurPackageItemBuffer->SetPos(nextPos);
	}

	CurPackageItemBuffer->Seek(0, 4);

	CurPackageItemBuffer->Skip(2); //customData
	SetOpaque(CurPackageItemBuffer->ReadBool());
	int32 maskId = CurPackageItemBuffer->ReadShort();
	if (maskId != -1)
	{
		bool inverted = CurPackageItemBuffer->ReadBool();
		//setMask(getChildAt(maskId)->displayObject(), inverted);
	}

	//const FString& hitTestId = Buffer->ReadS();
	FName hitTestId = CurPackageItemBuffer->ReadFNameFromCache();
	int32 i1 = CurPackageItemBuffer->ReadInt();
	int32 i2 = CurPackageItemBuffer->ReadInt();
	if (!hitTestId.IsNone())
	{
		TSharedPtr<FFairyPackageItem> pi = CurPackageItem->OwnerPackage->GetItem(hitTestId);
		/*if (pi != nullptr && pi->pixelHitTestData != nullptr)
			setHitArea(new PixelHitTest(pi->pixelHitTestData, i1, i2));*/
	}
	else if (i1 != 0 && i2 != -1)
	{
		//setHitArea(new ChildHitArea(getChildAt(i2)));
	}

	CurPackageItemBuffer->Seek(0, 5);

	// *** parse transitions start ***
	int32 transitionCount = CurPackageItemBuffer->ReadShort();
	for (int32 i = 0; i < transitionCount; i++)
	{
		int32 nextPos = CurPackageItemBuffer->ReadShort();
		nextPos += CurPackageItemBuffer->GetPos();

		UFairyTransition* Transition = NewObject<UFairyTransition>(this);
		Transition->SetTargetComponent(this);
		Transition->Setup(CurPackageItemBuffer);
		UFairyApplication::Get()->GetTransitionMgr()->AddTransition(Transition);

		CurPackageItemBuffer->SetPos(nextPos);
	}

	// todo: This code will remove when all FairyObject has OnEnter/OnExit Event Callback method.
	if (transitionCount > 0)
	{
		On(FFairyEventNames::AddedToStage).AddUObject(this, &UFairyComponent::OnAddedToStageHandler);
		On(FFairyEventNames::RemovedFromStage).AddUObject(this, &UFairyComponent::OnRemovedFromStageHandler);
	}
	// *** parse transitions end ***

	ApplyAllControllers();

	bBuildingDisplayList = false;
	bUnderConstruct = false;

	BuildNativeDisplayList();
	SetBoundsChangedFlag();
	ConstructExtension(CurPackageItemBuffer);

	OnConstruct();

	if (ScrollPanel)
	{
		ScrollPanel->SetContentSize(this->GetBounds().GetSize());
	}
}

void UFairyComponent::ConstructExtension(FairyGUI::FByteBuffer* Buffer)
{
}

void UFairyComponent::OnConstruct()
{
	K2_OnConstruct();
}

void UFairyComponent::SetupAfterAdd(FairyGUI::FByteBuffer* Buffer, int32 BeginPos)
{
	Super::SetupAfterAdd(Buffer, BeginPos);

	Buffer->Seek(BeginPos, 4);

	int32 pageController = Buffer->ReadShort();
	if (pageController != -1 && ScrollPanel != nullptr && ScrollPanel->bPageMode)
	{
		ScrollPanel->PageController = Parent->GetControllerAt(pageController);
	}

	int32 cnt = Buffer->ReadShort();
	for (int32 i = 0; i < cnt; i++)
	{
		UFairyController* Controller = GetController(Buffer->ReadFNameFromCache());
		const FName& PageID = Buffer->ReadFNameFromCache();
		if (Controller != nullptr)
		{
			Controller->SetSelectedPageID(PageID);
		}
	}

	if (Buffer->Version >= 2)
	{
		cnt = Buffer->ReadShort();
		for (int32 i = 0; i < cnt; i++)
		{
			FString Target = Buffer->ReadStringFromCache();
			EObjectPropID PropID = (EObjectPropID)Buffer->ReadShort();
			FString Value = Buffer->ReadStringFromCache();
			UFairyObject* Obj = GetChildByPath(Target);
			if (Obj != nullptr)
			{
				Obj->SetProp(PropID, FNVariant(Value));
			}
		}
	}

	if (ScrollPanel)
	{
		ScrollPanel->ContentSize = GetBounds().GetSize();
	}
}
// ***************************** Parse Component end ******************************************
