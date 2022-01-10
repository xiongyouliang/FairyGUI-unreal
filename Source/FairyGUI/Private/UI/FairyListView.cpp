#include "UI/FairyListView.h"
#include "UI/FairyButton.h"
#include "UI/FairyObjectPool.h"
#include "UI/Controller/FairyController.h"
#include "UI/GScrollBar.h"
#include "Package/FairyPackage.h"
#include "Package/FairyPackageMgr.h"
#include "Utils/ByteBuffer.h"
#include "Widgets/SContainer.h"

FFairyListItemInfo::FFairyListItemInfo() :
	Obj(nullptr),
	UpdateFlag(0),
	bSelected(false)
{
}

UFairyListView::UFairyListView() :
	bScrollItemToViewOnClick(true),
	bAutoResizeItem(true),
	LastSelectedIndex(-1),
	FirstIndex(-1)
{
	if (!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
	{
		bTrackBounds = true;
		Pool = new FFairyObjectPool();
	}
}

UFairyListView::~UFairyListView()
{
	delete Pool;

	SelectionController = nullptr;
	bScrollItemToViewOnClick = false;

	ScrollWidgetSlot = nullptr;
	MaskWidgetSlot = nullptr;
}

void UFairyListView::MakeSlateWidget()
{
	if (!DisplayObject.IsValid())
	{
		const TSharedPtr<SContainer> RootContainer = SNew(SContainer).GObject(this);
		const TSharedRef<SContainer> MaskContainer = SNew(SContainer).GObject(this);
		const TSharedRef<SContainer> ScrollContainer = SNew(SContainer).GObject(this);

		SContainer::FSlot& MaskContainerSlot = RootContainer->AddChild(MaskContainer);
		MaskContainerSlot.SizeAttr.BindUObject(this, &UFairyListView::GetScrollMaskSize);
		MaskWidgetSlot = &MaskContainerSlot;

		SContainer::FSlot& ContentContainerSlot = MaskContainer->AddChild(ScrollContainer);
		ContentContainerSlot.SizeAttr.BindUObject(this, &UFairyListView::GetScrollContentSize);
		ScrollWidgetSlot = &ContentContainerSlot;

		DisplayObject = Container = RootContainer;

		ScrollWidget = ScrollContainer;
		MaskWidget = MaskContainer;
	}
}

TSharedPtr<SContainer> UFairyListView::GetContentContainerWidget()
{
	return ScrollWidget;
}

TSharedPtr<SContainer> UFairyListView::GetMaskContainerWidget()
{
	return MaskWidget;
}

void UFairyListView::SetupScroll(FairyGUI::FByteBuffer* Buffer)
{
	Super::SetupScroll(Buffer);
}

FVector2D UFairyListView::GetScrollMaskSize() const
{
	// scroll area size is equal FVector2D(Size.X - VerticalScrollBarWith, Size.Y - HorizontalScrollBarHeight)
	const FVector2D& ComponentSize = GetSize();

	FVector2D FinalSize = ComponentSize;

	if (ScrollPanel && ScrollPanel->GetVtScrollBar())
	{
		UGScrollBar* ScrollBar = ScrollPanel->GetVtScrollBar();
		FinalSize.X -= ScrollBar->GetWidth();
	}

	if (ScrollPanel && ScrollPanel->GetHzScrollBar())
	{
		UGScrollBar* ScrollBar = ScrollPanel->GetHzScrollBar();
		FinalSize.Y -= ScrollBar->GetHeight();
	}

	return FinalSize;
}

FVector2D UFairyListView::GetScrollContentSize() const
{
	if (ScrollContentSize.Equals(FVector2D::ZeroVector))
	{
		return GetScrollMaskSize();
	}
	return ScrollContentSize;
}

void UFairyListView::ApplyController(UFairyController* Controller)
{
	UFairyComponent::ApplyController(Controller);

	if (SelectionController == Controller)
	{
		SetSelectedIndex(Controller->GetSelectedIndex());
	}
}

void UFairyListView::UpdateBounds()
{
	UE_LOG(LogTemp, Warning, TEXT("UFairyListView::UpdateBounds()"));
	Super::UpdateBounds();
}

void UFairyListView::SetBounds(float ax, float ay, float aw, float ah)
{
	Super::SetBounds(ax, ay, aw, ah);

	if (ScrollPanel)
	{
		ScrollPanel->SetContentSize(FVector2D(FMath::CeilToFloat(ax + aw), FMath::CeilToFloat(ay + ah)));
	}
}

void UFairyListView::SetupBeforeAdd(FairyGUI::FByteBuffer* Buffer, int32 BeginPos)
{
	Super::SetupBeforeAdd(Buffer, BeginPos);

	Buffer->Seek(BeginPos, 5);

	LayoutType = (EListLayoutType)Buffer->ReadByte();
	SelectionMode = (EListSelectionMode)Buffer->ReadByte();
	HorizontalAlign = (EHAlignType)Buffer->ReadByte();
	VerticalAlign = (EVAlignType)Buffer->ReadByte();
	RowSpacing = Buffer->ReadShort();
	ColSpacing = Buffer->ReadShort();
	RowNum = Buffer->ReadShort();
	ColNum = Buffer->ReadShort();

	bAutoResizeItem = Buffer->ReadBool();
	ChildrenRenderOrder = (EChildrenRenderOrder)Buffer->ReadByte();
	ApexIndex = Buffer->ReadShort();

	if (Buffer->ReadBool())
	{
		Margin.Top = Buffer->ReadInt();
		Margin.Bottom = Buffer->ReadInt();
		Margin.Left = Buffer->ReadInt();
		Margin.Right = Buffer->ReadInt();
	}

	EOverflowType overflow = (EOverflowType)Buffer->ReadByte();
	if (overflow == EOverflowType::Scroll)
	{
		int32 savedPos = Buffer->GetPos();
		Buffer->Seek(BeginPos, 7);
		SetupScroll(Buffer);
		Buffer->SetPos(savedPos);
	}
	else
	{
		SetupOverflow(overflow);
	}

	if (Buffer->ReadBool()) //clipSoftness
	{
		Buffer->Skip(8);
	}

	if (Buffer->Version >= 2)
	{
		bScrollItemToViewOnClick = Buffer->ReadBool();
		bFoldInvisibleItems = Buffer->ReadBool();
	}

	Buffer->Seek(BeginPos, 8);

	DefaultItem = Buffer->ReadFNameFromCache();
	ReadItems(Buffer);
}

void UFairyListView::SetupAfterAdd(FairyGUI::FByteBuffer* Buffer, int32 BeginPos)
{
	Super::SetupAfterAdd(Buffer, BeginPos);

	Buffer->Seek(BeginPos, 6);

	int32 i = Buffer->ReadShort();
	if (i != -1)
	{
		SelectionController = Parent->GetControllerAt(i);
	}
}

void UFairyListView::ReadItems(FairyGUI::FByteBuffer* Buffer)
{
	const FString* str = nullptr;
	FName url;

	int32 itemCount = Buffer->ReadShort();
	for (int32 i = 0; i < itemCount; i++)
	{
		int32 nextPos = Buffer->ReadShort();
		nextPos += Buffer->GetPos();

		str = Buffer->ReadSP();
		if (!str || (*str).IsEmpty())
		{
			url = DefaultItem;
			if (url.IsNone())
			{
				Buffer->SetPos(nextPos);
				continue;
			}
		}
		else
		{
			url = FName(*str);
		}

		UFairyObject* obj = GetFromPool(url);
		if (obj != nullptr)
		{
			AddChild(obj);
			SetupItem(Buffer, obj);
		}

		Buffer->SetPos(nextPos);
	}

	RefreshItemsLayout();
}

void UFairyListView::SetupItem(FairyGUI::FByteBuffer* Buffer, UFairyObject* Obj)
{
	const FString* str;
	UFairyButton* btn = Cast<UFairyButton>(Obj);

	if ((str = Buffer->ReadSP()) != nullptr)
	{
		Obj->SetText(*str);
	}

	if ((str = Buffer->ReadSP()) != nullptr && btn)
	{
		btn->SetSelectedTitle(*str);
	}

	if ((str = Buffer->ReadSP()) != nullptr)
	{
		Obj->SetIcon(*str);
	}

	if ((str = Buffer->ReadSP()) != nullptr && btn)
	{
		btn->SetSelectedIcon(*str);
	}

	if ((str = Buffer->ReadSP()) != nullptr)
	{
		Obj->SetName(*str);
	}

	UFairyComponent* gcom = Cast<UFairyComponent>(Obj);
	if (gcom != nullptr)
	{
		int32 cnt = Buffer->ReadShort();
		for (int32 i = 0; i < cnt; i++)
		{
			UFairyController* cc = gcom->GetController(Buffer->ReadFNameFromCache());
			const FName& PageID = Buffer->ReadFNameFromCache();
			cc->SetSelectedPageID(PageID);
		}

		if (Buffer->Version >= 2)
		{
			cnt = Buffer->ReadShort();
			for (int32 i = 0; i < cnt; i++)
			{
				const FString& target = Buffer->ReadStringFromCache();
				EObjectPropID PropID = (EObjectPropID)Buffer->ReadShort();
				FString value = Buffer->ReadStringFromCache();
				UFairyObject* obj2 = gcom->GetChildByPath(target);
				if (obj2 != nullptr)
				{
					obj2->SetProp(PropID, FNVariant(value));
				}
			}
		}
	}
}

// *********************** item layout start *********************************
void UFairyListView::RefreshItemsLayout()
{
	if (LayoutType == EListLayoutType::SingleCol)
	{
		RefreshItemsLayoutForSingleCol();
	}
	else if (LayoutType == EListLayoutType::SingleRow)
	{
		RefreshItemsLayoutForSingleRow();
	}
	else if (LayoutType == EListLayoutType::HorizontalFlow)
	{
		RefreshItemsLayoutForHorizontalFlow();
	}
	else if (LayoutType == EListLayoutType::VerticalFlow)
	{
		RefreshItemsLayoutForVerticalFlow();
	}
	else if (LayoutType == EListLayoutType::Pagination)
	{
		RefreshItemsLayoutForPagination();
	}
}

void UFairyListView::RefreshItemsLayoutForSingleCol()
{
	const int ChildNum = Children.Num();
	int curPosY = 0;
	FVector2D ContainerSize = GetScrollContentSize();
	for (int i = 0; i < ChildNum; i++)
	{
		UFairyObject* child = GetChildAt(i);
		const FVector2D curPos = child->GetPosition();
		if (bFoldInvisibleItems && !child->IsVisible())
		{
			continue;
		}

		if (i != 0)
		{
			curPosY += RowSpacing;
		}

		child->SetPosition(FVector2D(curPos.X, curPosY));
		if (bAutoResizeItem)
		{
			child->SetSize(FVector2D(ContainerSize.X, child->GetHeight()));
		}
		curPosY += FMath::CeilToFloat(child->GetHeight());
	}
}

void UFairyListView::RefreshItemsLayoutForSingleRow()
{
	const int ChildNum = Children.Num();
	int curPosX = 0;
	FVector2D ContainerSize = GetScrollContentSize();
	for (int i = 0; i < ChildNum; i++)
	{
		UFairyObject* child = GetChildAt(i);
		const FVector2D curPos = child->GetPosition();
		if (bFoldInvisibleItems && !child->IsVisible())
		{
			continue;
		}

		if (i != 0)
		{
			curPosX += ColSpacing;
		}

		child->SetPosition(FVector2D(curPosX, curPos.Y));
		if (bAutoResizeItem)
		{
			child->SetSize(FVector2D(ContainerSize.X, child->GetHeight()));
		}
		curPosX += FMath::CeilToFloat(child->GetHeight());
	}
}

void UFairyListView::RefreshItemsLayoutForHorizontalFlow()
{
	const int ChildNum = Children.Num();
	if (ChildNum == 0)
	{
		return;
	}

	int curPosX = 0;
	int curPosY = 0;
	FVector2D ContainerSize = GetScrollContentSize();
	FVector2D ChildOriginSize = GetChildAt(0)->GetSize();

	FVector2D ChildTargetSize = ChildOriginSize;
	// Calculate the child size first
	int32 TargetColNum = ColNum;
	if (bAutoResizeItem && TargetColNum > 0)
	{
		ChildTargetSize.X = (ContainerSize.X - (TargetColNum - 1) * ColSpacing) / TargetColNum;
	}
	else
	{
		TargetColNum = FMath::FloorToInt(ContainerSize.X / ChildOriginSize.X);
	}

	for (int i = 0; i < ChildNum; i++)
	{
		UFairyObject* child = GetChildAt(i);
		child->SetSize(ChildTargetSize);

		const int colIndex = i % TargetColNum;
		const int rowIndex = FMath::FloorToInt(i / TargetColNum);

		curPosX = colIndex * (ChildTargetSize.X + ColSpacing);
		curPosY = rowIndex * (ChildTargetSize.Y + RowSpacing);

		child->SetPosition(FVector2D(curPosX, curPosY));
	}
}

void UFairyListView::RefreshItemsLayoutForVerticalFlow()
{
	const int ChildNum = Children.Num();
	if (ChildNum == 0)
	{
		return;
	}

	int curPosX = 0;
	int curPosY = 0;
	FVector2D ContainerSize = GetScrollContentSize();
	FVector2D ChildOriginSize = GetChildAt(0)->GetSize();

	FVector2D ChildTargetSize = ChildOriginSize;
	// Calculate the child size first
	int32 TargetRowNum = RowNum; // RowNum is member variable, do not change it's value.
	if (bAutoResizeItem && TargetRowNum > 0)
	{
		ChildTargetSize.Y = (ContainerSize.Y - (TargetRowNum - 1) * RowSpacing) / TargetRowNum;
	}
	else
	{
		TargetRowNum = FMath::FloorToInt(ContainerSize.X / ChildOriginSize.X);
	}

	for (int i = 0; i < ChildNum; i++)
	{
		UFairyObject* child = GetChildAt(i);
		child->SetSize(ChildTargetSize);

		const int colIndex = FMath::FloorToInt(i / TargetRowNum);
		const int rowIndex = i % TargetRowNum;

		curPosX = colIndex * (ChildTargetSize.X + ColSpacing);
		curPosY = rowIndex * (ChildTargetSize.Y + RowSpacing);

		child->SetPosition(FVector2D(curPosX, curPosY));
	}
}

void UFairyListView::RefreshItemsLayoutForPagination()
{
	const int ChildNum = Children.Num();
	if (ChildNum == 0)
	{
		return;
	}

	FVector2D ContainerSize = GetScrollContentSize();
	FVector2D ChildOriginSize = GetChildAt(0)->GetSize();

	FVector2D ChildTargetSize = ChildOriginSize;
	int TargetRowNum = RowNum;
	int TargetColNum = ColNum;
	if (bAutoResizeItem)
	{
		if (TargetRowNum > 0)
		{
			ChildTargetSize.Y = (ContainerSize.Y - (TargetRowNum - 1) * RowSpacing) / TargetRowNum;
		}
		else
		{
			TargetRowNum = FMath::FloorToInt((ContainerSize.Y+RowSpacing) / (ChildOriginSize.Y+RowSpacing));
			if (TargetRowNum == 0)
			{
				TargetRowNum = 1;
				ChildTargetSize.Y = ContainerSize.Y;
			}
		}

		if (TargetColNum > 0)
		{
			ChildTargetSize.X = (ContainerSize.X - (TargetColNum - 1) * ColSpacing) / TargetColNum;
		}
		else
		{
			TargetColNum = FMath::FloorToInt((ContainerSize.X+ColSpacing)/(ChildTargetSize.X+ColSpacing));
			if (TargetColNum == 0)
			{
				TargetColNum = 1;
				ChildTargetSize.X = ContainerSize.X;
			}
		}
	}

	const int NumInOnePage = TargetRowNum * TargetColNum;
	for (int i = 0; i < ChildNum; i++)
	{
		UFairyObject* child = GetChildAt(i);
		child->SetSize(ChildTargetSize);

		bool bNewPage = i % NumInOnePage == 0 ? true : false;

		const int pageIndex = FMath::FloorToInt(i / NumInOnePage);

		const int itemIndexInPage = i - pageIndex * NumInOnePage;
		const int colIndexInPage = itemIndexInPage % TargetColNum;
		const int rowIndexInPage = FMath::FloorToInt(itemIndexInPage / TargetColNum);

		const float curPosX = pageIndex * ContainerSize.X + colIndexInPage * (ChildTargetSize.X + ColSpacing);
		const float curPosY = rowIndexInPage * (ChildTargetSize.Y + RowSpacing);

		child->SetPosition(FVector2D(curPosX, curPosY));
	}
}
// *********************** item layout end *********************************

void UFairyListView::SetLayout(EListLayoutType InLayout)
{
	if (LayoutType != InLayout)
	{
		LayoutType = InLayout;
		SetBoundsChangedFlag();
		if (bVirtual)
		{
			SetVirtualListChangedFlag(true);
		}
	}
}

void UFairyListView::SetLineCount(int32 InLineCount)
{
	if (RowNum != InLineCount)
	{
		RowNum = InLineCount;
		if (LayoutType == EListLayoutType::VerticalFlow || LayoutType == EListLayoutType::Pagination)
		{
			SetBoundsChangedFlag();
			if (bVirtual)
			{
				SetVirtualListChangedFlag(true);
			}
		}
	}
}

void UFairyListView::SetColumnCount(int32 InColumnCount)
{
	if (ColNum != InColumnCount)
	{
		ColNum = InColumnCount;
		if (LayoutType == EListLayoutType::HorizontalFlow || LayoutType == EListLayoutType::Pagination)
		{
			SetBoundsChangedFlag();
			if (bVirtual)
			{
				SetVirtualListChangedFlag(true);
			}
		}
	}
}

void UFairyListView::SetLineGap(int32 InLineGap)
{
	if (RowSpacing != InLineGap)
	{
		RowSpacing = InLineGap;
		SetBoundsChangedFlag();
		if (bVirtual)
		{
			SetVirtualListChangedFlag(true);
		}
	}
}

void UFairyListView::SetColumnGap(int32 InColumnGap)
{
	if (ColSpacing != InColumnGap)
	{
		ColSpacing = InColumnGap;
		SetBoundsChangedFlag();
		if (bVirtual)
		{
			SetVirtualListChangedFlag(true);
		}
	}
}

void UFairyListView::SetHorizontalAlign(EHAlignType InAlign)
{
	if (HorizontalAlign != InAlign)
	{
		HorizontalAlign = InAlign;
		SetBoundsChangedFlag();
		if (bVirtual)
		{
			SetVirtualListChangedFlag(true);
		}
	}
}

void UFairyListView::SetVerticalAlign(EVAlignType InVerticalAlign)
{
	if (VerticalAlign != InVerticalAlign)
	{
		VerticalAlign = InVerticalAlign;
		SetBoundsChangedFlag();
		if (bVirtual)
		{
			SetVirtualListChangedFlag(true);
		}
	}
}

void UFairyListView::SetAutoResizeItem(bool bFlag)
{
	if (bAutoResizeItem != bFlag)
	{
		bAutoResizeItem = bFlag;
		SetBoundsChangedFlag();
		if (bVirtual)
		{
			SetVirtualListChangedFlag(true);
		}
	}
}

UFairyObject* UFairyListView::GetFromPool()
{
	return GetFromPool(NAME_None);
}

UFairyObject* UFairyListView::GetFromPool(const FName& URL)
{
	UFairyObject* ret = nullptr;
	if (URL.IsNone())
	{
		ret = Pool->GetOrCreateObject(this, DefaultItem);
	}
	else
	{
		ret = Pool->GetOrCreateObject(this, URL);
	}

	if (ret != nullptr)
	{
		ret->SetVisible(true);
	}
	return ret;
}

void UFairyListView::ReturnToPool(UFairyObject* Obj)
{
	Pool->ReturnObject(Obj);
}

UFairyObject* UFairyListView::AddItemFromPool(const FName& URL)
{
	UFairyObject* Obj = GetFromPool(URL);

	return AddChild(Obj);
}

UFairyObject* UFairyListView::AddChildAt(UFairyObject* Child, int32 Index)
{
	Super::AddChildAt(Child, Index); // Replace UFairyComponent with Super; maybe we need change inherit to de: UFairyComponent -> UFairyScrollPanel -> UFairyList

	if (Child->IsA<UFairyButton>())
	{
		UFairyButton* Button = (UFairyButton*)Child;
		Button->SetSelected(false);
		Button->bChangeStateOnClick = false;
	}

	Child->OnClick.AddUniqueDynamic(this, &UFairyListView::OnClickItemHandler);

	return Child;
}

void UFairyListView::RemoveChildAt(int32 Index)
{
	UFairyObject* Child = Children[Index];
	Child->OnClick.RemoveDynamic(this, &UFairyListView::OnClickItemHandler);

	Super::RemoveChildAt(Index);
}

void UFairyListView::RemoveChildToPoolAt(int32 Index)
{
	ReturnToPool(GetChildAt(Index));
	RemoveChildAt(Index);
}

void UFairyListView::RemoveChildToPool(UFairyObject* Child)
{
	ReturnToPool(Child);
	RemoveChild(Child);
}

void UFairyListView::RemoveChildrenToPool(int32 BeginIndex, int32 EndIndex)
{
	if (EndIndex < 0 || EndIndex >= Children.Num())
	{
		EndIndex = Children.Num() - 1;
	}

	for (int32 i = BeginIndex; i <= EndIndex; ++i)
	{
		RemoveChildToPoolAt(BeginIndex);
	}
}

int32 UFairyListView::GetSelectedIndex() const
{
	if (bVirtual)
	{
		int32 cnt = RealNumItems;
		for (int32 i = 0; i < cnt; i++)
		{
			const FFairyListItemInfo& ii = VirtualItems[i];
			if ((Cast<UFairyButton>(ii.Obj) && ((UFairyButton*)ii.Obj)->IsSelected()) || (ii.Obj == nullptr && ii.bSelected))
			{
				if (bLoop)
				{
					return i % NumItems;
				}
				else
				{
					return i;
				}
			}
		}
	}
	else
	{
		int32 cnt = Children.Num();
		for (int32 i = 0; i < cnt; i++)
		{
			UFairyButton* Obj = Cast<UFairyButton>(Children[i]);
			if (Obj != nullptr && Obj->IsSelected())
			{
				return i;
			}
		}
	}
	return -1;
}

void UFairyListView::SetSelectedIndex(int32 Index)
{
	if (Index >= 0 && Index < GetNumItems())
	{
		if (SelectionMode != EListSelectionMode::Single)
		{
			ClearSelection();
		}
		AddSelection(Index, false);
	}
	else
	{
		ClearSelection();
	}
}

void UFairyListView::SetSelectionController(UFairyController* InController)
{
	SelectionController = InController;
}

void UFairyListView::GetSelection(TArray<int32>& OutIndice) const
{
	OutIndice.Reset();
	if (bVirtual)
	{
		int32 cnt = RealNumItems;
		for (int32 i = 0; i < cnt; i++)
		{
			const FFairyListItemInfo& ii = VirtualItems[i];
			if ((Cast<UFairyButton>(ii.Obj) && ((UFairyButton*)ii.Obj)->IsSelected()) || (ii.Obj == nullptr && ii.bSelected))
			{
				int32 j = i;
				if (bLoop)
				{
					j = i % NumItems;
					if (OutIndice.Contains(j))
					{
						continue;
					}
				}
				OutIndice.Add(j);
			}
		}
	}
	else
	{
		int32 cnt = Children.Num();
		for (int32 i = 0; i < cnt; i++)
		{
			UFairyButton* Obj = Cast<UFairyButton>(Children[i]);
			if (Obj != nullptr && Obj->IsSelected())
			{
				OutIndice.Add(i);
			}
		}
	}
}

void UFairyListView::AddSelection(int32 Index, bool bScrollItToView)
{
	if (SelectionMode == EListSelectionMode::None)
	{
		return;
	}

	CheckVirtualList();

	if (SelectionMode == EListSelectionMode::Single)
	{
		ClearSelection();
	}

	if (bScrollItToView)
	{
		ScrollToView(Index);
	}

	LastSelectedIndex = Index;
	UFairyButton* Obj = nullptr;
	if (bVirtual)
	{
		FFairyListItemInfo& ii = VirtualItems[Index];
		if (ii.Obj != nullptr)
		{
			Obj = ii.Obj->As<UFairyButton>();
		}
		ii.bSelected = true;
	}
	else
	{
		Obj = GetChildAt(Index)->As<UFairyButton>();
	}

	if (Obj != nullptr && !Obj->IsSelected())
	{
		Obj->SetSelected(true);
		UpdateSelectionController(Index);
	}
}

void UFairyListView::RemoveSelection(int32 Index)
{
	if (SelectionMode == EListSelectionMode::None)
	{
		return;
	}

	UFairyButton* Obj = nullptr;
	if (bVirtual)
	{
		FFairyListItemInfo& ii = VirtualItems[Index];
		if (ii.Obj != nullptr)
		{
			Obj = ii.Obj->As<UFairyButton>();
		}
		ii.bSelected = false;
	}
	else
	{
		Obj = GetChildAt(Index)->As<UFairyButton>();
	}

	if (Obj != nullptr)
	{
		Obj->SetSelected(false);
	}
}

void UFairyListView::ClearSelection()
{
	if (bVirtual)
	{
		int32 cnt = RealNumItems;
		for (int32 i = 0; i < cnt; i++)
		{
			FFairyListItemInfo& ii = VirtualItems[i];
			if (Cast<UFairyButton>(ii.Obj))
			{
				((UFairyButton*)ii.Obj)->SetSelected(false);
			}
			ii.bSelected = false;
		}
	}
	else
	{
		int32 cnt = Children.Num();
		for (int32 i = 0; i < cnt; i++)
		{
			UFairyButton* Obj = Children[i]->As<UFairyButton>();
			if (Obj != nullptr)
			{
				Obj->SetSelected(false);
			}
		}
	}
}

void UFairyListView::ClearSelectionExcept(UFairyObject* Obj)
{
	if (bVirtual)
	{
		int32 cnt = RealNumItems;
		for (int32 i = 0; i < cnt; i++)
		{
			FFairyListItemInfo& ii = VirtualItems[i];
			if (ii.Obj != Obj)
			{
				if (Cast<UFairyButton>(ii.Obj))
				{
					((UFairyButton*)ii.Obj)->SetSelected(false);
				}
				ii.bSelected = false;
			}
		}
	}
	else
	{
		int32 cnt = Children.Num();
		for (int32 i = 0; i < cnt; i++)
		{
			UFairyButton* Child = Children[i]->As<UFairyButton>();
			if (Child != nullptr && Child != Obj)
			{
				Child->SetSelected(false);
			}
		}
	}
}

void UFairyListView::SelectAll()
{
	CheckVirtualList();

	int32 last = -1;
	if (bVirtual)
	{
		int32 cnt = RealNumItems;
		for (int32 i = 0; i < cnt; i++)
		{
			FFairyListItemInfo& ii = VirtualItems[i];
			if (Cast<UFairyButton>(ii.Obj) && !((UFairyButton*)ii.Obj)->IsSelected())
			{
				((UFairyButton*)ii.Obj)->SetSelected(true);
				last = i;
			}
			ii.bSelected = true;
		}
	}
	else
	{
		int32 cnt = Children.Num();
		for (int32 i = 0; i < cnt; i++)
		{
			UFairyButton* Obj = Children[i]->As<UFairyButton>();
			if (Obj != nullptr && !Obj->IsSelected())
			{
				Obj->SetSelected(true);
				last = i;
			}
		}
	}

	if (last != -1)
	{
		UpdateSelectionController(last);
	}
}

void UFairyListView::SelectReverse()
{
	CheckVirtualList();

	int32 last = -1;
	if (bVirtual)
	{
		int32 cnt = RealNumItems;
		for (int32 i = 0; i < cnt; i++)
		{
			FFairyListItemInfo& ii = VirtualItems[i];
			if (Cast<UFairyButton>(ii.Obj))
			{
				((UFairyButton*)ii.Obj)->SetSelected(!((UFairyButton*)ii.Obj)->IsSelected());
				if (((UFairyButton*)ii.Obj)->IsSelected())
				{
					last = i;
				}
			}
			ii.bSelected = !ii.bSelected;
		}
	}
	else
	{
		int32 cnt = Children.Num();
		for (int32 i = 0; i < cnt; i++)
		{
			UFairyButton* Obj = Children[i]->As<UFairyButton>();
			if (Obj != nullptr)
			{
				Obj->SetSelected(!Obj->IsSelected());
				if (Obj->IsSelected())
				{
					last = i;
				}
			}
		}
	}

	if (last != -1)
	{
		UpdateSelectionController(last);
	}
}

void UFairyListView::HandleArrowKey(int32 Direction)
{
	int32 index = GetSelectedIndex();
	if (index == -1)
	{
		return;
	}

	switch (Direction)
	{
	case 1: //up
		if (LayoutType == EListLayoutType::SingleCol || LayoutType == EListLayoutType::VerticalFlow)
		{
			index--;
			if (index >= 0)
			{
				ClearSelection();
				AddSelection(index, true);
			}
		}
		else if (LayoutType == EListLayoutType::HorizontalFlow || LayoutType == EListLayoutType::Pagination)
		{
			UFairyObject* current = Children[index];
			int32 k = 0;
			int32 i;
			for (i = index - 1; i >= 0; i--)
			{
				UFairyObject* obj = Children[i];
				if (obj->GetPosition().Y != current->GetPosition().Y)
				{
					current = obj;
					break;
				}
				k++;
			}
			for (; i >= 0; i--)
			{
				UFairyObject* obj = Children[i];
				if (obj->GetPosition().Y != current->GetPosition().Y)
				{
					ClearSelection();
					AddSelection(i + k + 1, true);
					break;
				}
			}
		}
		break;

	case 3: //right
		if (LayoutType == EListLayoutType::SingleRow || LayoutType == EListLayoutType::HorizontalFlow || LayoutType == EListLayoutType::Pagination)
		{
			index++;
			if (index < Children.Num())
			{
				ClearSelection();
				AddSelection(index, true);
			}
		}
		else if (LayoutType == EListLayoutType::VerticalFlow)
		{
			UFairyObject* current = Children[index];
			int32 k = 0;
			int32 cnt = Children.Num();
			int32 i;
			for (i = index + 1; i < cnt; i++)
			{
				UFairyObject* obj = Children[i];
				if (obj->GetPosition().X != current->GetPosition().X)
				{
					current = obj;
					break;
				}
				k++;
			}
			for (; i < cnt; i++)
			{
				UFairyObject* obj = Children[i];
				if (obj->GetPosition().X != current->GetPosition().X)
				{
					ClearSelection();
					AddSelection(i - k - 1, true);
					break;
				}
			}
		}
		break;

	case 5: //down
		if (LayoutType == EListLayoutType::SingleCol || LayoutType == EListLayoutType::VerticalFlow)
		{
			index++;
			if (index < Children.Num())
			{
				ClearSelection();
				AddSelection(index, true);
			}
		}
		else if (LayoutType == EListLayoutType::HorizontalFlow || LayoutType == EListLayoutType::Pagination)
		{
			UFairyObject* current = Children[index];
			int32 k = 0;
			int32 cnt = Children.Num();
			int32 i;
			for (i = index + 1; i < cnt; i++)
			{
				UFairyObject* obj = Children[i];
				if (obj->GetPosition().Y != current->GetPosition().Y)
				{
					current = obj;
					break;
				}
				k++;
			}
			for (; i < cnt; i++)
			{
				UFairyObject* obj = Children[i];
				if (obj->GetPosition().Y != current->GetPosition().Y)
				{
					ClearSelection();
					AddSelection(i - k - 1, true);
					break;
				}
			}
		}
		break;

	case 7: //left
		if (LayoutType == EListLayoutType::SingleRow || LayoutType == EListLayoutType::HorizontalFlow || LayoutType == EListLayoutType::Pagination)
		{
			index--;
			if (index >= 0)
			{
				ClearSelection();
				AddSelection(index, true);
			}
		}
		else if (LayoutType == EListLayoutType::VerticalFlow)
		{
			UFairyObject* current = Children[index];
			int32 k = 0;
			int32 i;
			for (i = index - 1; i >= 0; i--)
			{
				UFairyObject* obj = Children[i];
				if (obj->GetPosition().X != current->GetPosition().X)
				{
					current = obj;
					break;
				}
				k++;
			}
			for (; i >= 0; i--)
			{
				UFairyObject* obj = Children[i];
				if (obj->GetPosition().X != current->GetPosition().X)
				{
					ClearSelection();
					AddSelection(i + k + 1, true);
					break;
				}
			}
		}
		break;
	}
}

void UFairyListView::OnClickItemHandler(UEventContext* Context)
{
	UFairyObject* Obj = Context->GetSender();
	if (Obj->IsA<UFairyButton>() && SelectionMode != EListSelectionMode::None)
	{
		SetSelectionOnEvent(Obj, Context);
	}

	if (ScrollPanel != nullptr && bScrollItemToViewOnClick)
	{
		ScrollPanel->ScrollToView(Obj, true);
	}

	DispatchItemEvent(Obj, Context);
}

void UFairyListView::DispatchItemEvent(UFairyObject* Obj, UEventContext* Context)
{
	DispatchEvent(FFairyEventNames::ClickItem, FNVariant(Obj));
}

void UFairyListView::SetSelectionOnEvent(UFairyObject* Obj, UEventContext* Context)
{
	bool bDontChangeLastIndex = false;
	UFairyButton* Button = Cast<UFairyButton>(Obj);
	int32 Index = ChildIndexToItemIndex(GetChildIndex(Obj));

	if (SelectionMode == EListSelectionMode::Single)
	{
		if (!Button->IsSelected())
		{
			ClearSelectionExcept(Button);
			Button->SetSelected(true);
		}
	}
	else
	{
		if (Context->GetPointerEvent().IsShiftDown())
		{
			if (!Button->IsSelected())
			{
				if (LastSelectedIndex != -1)
				{
					int32 min = FMath::Min(LastSelectedIndex, Index);
					int32 max = FMath::Max(LastSelectedIndex, Index);
					max = FMath::Min(max, GetNumItems() - 1);
					if (bVirtual)
					{
						for (int32 i = min; i <= max; i++)
						{
							FFairyListItemInfo& ii = VirtualItems[i];
							if (ii.Obj != nullptr && ii.Obj->IsA<UFairyButton>())
							{
								Cast<UFairyButton>(ii.Obj)->SetSelected(true);
							}
							ii.bSelected = true;
						}
					}
					else
					{
						for (int32 i = min; i <= max; i++)
						{
							UFairyButton* Child = GetChildAt(i)->As<UFairyButton>();
							if (Child != nullptr && !Child->IsSelected())
							{
								Child->SetSelected(true);
							}
						}
					}

					bDontChangeLastIndex = true;
				}
				else
				{
					Button->SetSelected(true);
				}
			}
		}
		else if (Context->GetPointerEvent().IsControlDown() || SelectionMode == EListSelectionMode::MultipleSingleclick)
		{
			Button->SetSelected(!Button->IsSelected());
		}
		else
		{
			if (!Button->IsSelected())
			{
				ClearSelectionExcept(Button);
				Button->SetSelected(true);
			}
			else if (Context->GetMouseButton() == EKeys::LeftMouseButton)
			{
				ClearSelectionExcept(Button);
			}
		}
	}

	if (!bDontChangeLastIndex)
	{
		LastSelectedIndex = Index;
	}

	if (Button->IsSelected())
	{
		UpdateSelectionController(Index);
	}
}

void UFairyListView::ResizeToFit(int32 ItemCount, int32 InMinSize)
{
	EnsureBoundsCorrect();

	int32 curCount = GetNumItems();
	if (ItemCount > curCount)
	{
		ItemCount = curCount;
	}

	if (bVirtual)
	{
		int32 lineCount = FMath::CeilToInt((float)ItemCount / CurLineItemCount);
		if (LayoutType == EListLayoutType::SingleCol || LayoutType == EListLayoutType::HorizontalFlow)
		{
			SetViewHeight(lineCount * ItemSize.Y + FMath::Max(0, lineCount - 1) * RowSpacing);
		}
		else
		{
			SetViewWidth(lineCount * ItemSize.X + FMath::Max(0, lineCount - 1) * ColSpacing);
		}
	}
	else if (ItemCount == 0)
	{
		if (LayoutType == EListLayoutType::SingleCol || LayoutType == EListLayoutType::HorizontalFlow)
		{
			SetViewHeight(InMinSize);
		}
		else
		{
			SetViewWidth(InMinSize);
		}
	}
	else
	{
		int32 i = ItemCount - 1;
		UFairyObject* obj = nullptr;
		while (i >= 0)
		{
			obj = GetChildAt(i);
			if (!bFoldInvisibleItems || obj->IsVisible())
			{
				break;
			}
			i--;
		}
		if (i < 0)
		{
			if (LayoutType == EListLayoutType::SingleCol || LayoutType == EListLayoutType::HorizontalFlow)
			{
				SetViewHeight(InMinSize);
			}
			else
			{
				SetViewWidth(InMinSize);
			}
		}
		else
		{
			float size;
			if (LayoutType == EListLayoutType::SingleCol || LayoutType == EListLayoutType::HorizontalFlow)
			{
				size = obj->GetPosition().Y + obj->GetHeight();
				if (size < InMinSize)
				{
					size = InMinSize;
				}
				SetViewHeight(size);
			}
			else
			{
				size = obj->GetPosition().X + obj->GetWidth();
				if (size < InMinSize)
				{
					size = InMinSize;
				}
				SetViewWidth(size);
			}
		}
	}
}

int32 UFairyListView::GetFirstChildInView() const
{
	return ChildIndexToItemIndex(UFairyComponent::GetFirstChildInView());
}

//void UFairyListView::HandleSizeChanged()
//{
//    UFairyComponent::HandleSizeChanged();
//
//    SetBoundsChangedFlag();
//    if (bVirtual)
//    {
//        SetVirtualListChangedFlag(true);
//    }
//}



void UFairyListView::UpdateSelectionController(int32 Index)
{
	if (SelectionController != nullptr && !SelectionController->bChanging && Index < SelectionController->GetPageCount())
	{
		UFairyController* Controller = SelectionController;
		SelectionController = nullptr;
		Controller->SetSelectedIndex(Index);
		SelectionController = Controller;
	}
}

void UFairyListView::ScrollToView(int32 Index, bool bAnimation, bool bSetFirst)
{
	if (bVirtual)
	{
		if (NumItems == 0)
		{
			return;
		}

		CheckVirtualList();

		verifyf(Index >= 0 && Index < VirtualItems.Num(), TEXT("Invalid child index"));

		if (bLoop)
		{
			Index = FMath::FloorToFloat(FirstIndex / NumItems) * NumItems + Index;
		}

		FBox2D rect;
		FFairyListItemInfo& ii = VirtualItems[Index];
		if (LayoutType == EListLayoutType::SingleCol || LayoutType == EListLayoutType::HorizontalFlow)
		{
			float pos = 0;
			for (int32 i = CurLineItemCount - 1; i < Index; i += CurLineItemCount)
			{
				pos += VirtualItems[i].Size.Y + RowSpacing;
			}
			rect.Min.Set(0, pos);
			rect.Max = rect.Min + FVector2D(ItemSize.X, ii.Size.Y);
		}
		else if (LayoutType == EListLayoutType::SingleRow || LayoutType == EListLayoutType::VerticalFlow)
		{
			float pos = 0;
			for (int32 i = CurLineItemCount - 1; i < Index; i += CurLineItemCount)
			{
				pos += VirtualItems[i].Size.X + ColSpacing;
			}
			rect.Min.Set(pos, 0);
			rect.Max = rect.Min + FVector2D(ii.Size.X, ItemSize.Y);
		}
		else
		{
			int32 page = Index / (CurLineItemCount * CurLineItemCount2);
			rect.Min.Set(page * GetViewWidth() + (Index % CurLineItemCount) * (ii.Size.X + ColSpacing),
				(Index / CurLineItemCount) % CurLineItemCount2 * (ii.Size.Y + RowSpacing));
			rect.Max = rect.Min + ii.Size;
		}

		if (ScrollPanel != nullptr)
		{
			ScrollPanel->ScrollToView(rect, bAnimation, bSetFirst);
		}
		else if (Parent && Parent->GetScrollPane() != nullptr)
		{
			FBox2D rect2 = LocalToGlobalRect(rect);
			rect2 = Parent->GlobalToLocalRect(rect2);
			Parent->GetScrollPane()->ScrollToView(rect2, bAnimation, bSetFirst);
		}
	}
	else
	{
		UFairyObject* obj = GetChildAt(Index);
		if (ScrollPanel != nullptr)
		{
			ScrollPanel->ScrollToView(obj, bAnimation, bSetFirst);
		}
		else if (Parent && Parent->GetScrollPane() != nullptr)
		{
			Parent->GetScrollPane()->ScrollToView(obj, bAnimation, bSetFirst);
		}
	}
}

int32 UFairyListView::ChildIndexToItemIndex(int32 Index) const
{
	if (!bVirtual)
	{
		return Index;
	}

	if (LayoutType == EListLayoutType::Pagination)
	{
		for (int32 i = FirstIndex; i < RealNumItems; i++)
		{
			if (VirtualItems[i].Obj != nullptr)
			{
				Index--;
				if (Index < 0)
				{
					return i;
				}
			}
		}

		return Index;
	}
	else
	{
		Index += FirstIndex;
		if (bLoop && NumItems > 0)
		{
			Index = Index % NumItems;
		}

		return Index;
	}
}

int32 UFairyListView::ItemIndexToChildIndex(int32 Index) const
{
	if (!bVirtual)
		return Index;

	if (LayoutType == EListLayoutType::Pagination)
	{
		return GetChildIndex(VirtualItems[Index].Obj);
	}
	else
	{
		if (bLoop && NumItems > 0)
		{
			int32 j = FirstIndex % NumItems;
			if (Index >= j)
			{
				Index = Index - j;

			}
			else
			{
				Index = NumItems - j + Index;
			}
		}
		else
		{
			Index -= FirstIndex;
		}

		return Index;
	}
}

void UFairyListView::SetVirtual()
{
	SetVirtual(false);
}

void UFairyListView::SetVirtualAndLoop()
{
	SetVirtual(true);
}

void UFairyListView::SetVirtual(bool bInLoop)
{
	if (!bVirtual)
	{
		verifyf(ScrollPanel != nullptr, TEXT("FairyGUI: Virtual list must be scrollable!"));

		if (bInLoop)
		{
			verifyf(LayoutType != EListLayoutType::HorizontalFlow && LayoutType != EListLayoutType::VerticalFlow,
				TEXT("Loop list is not supported for HorizontalFlow or VerticalFlow layout!"));

			ScrollPanel->bBouncebackEffect = false;
		}

		bVirtual = true;
		bLoop = bInLoop;
		RemoveChildrenToPool();

		if (ItemSize.X == 0 || ItemSize.Y == 0)
		{
			UFairyObject* obj = GetFromPool();
			verifyf(obj != nullptr, TEXT("Virtual List must have a default list item resource."));
			ItemSize = obj->GetSize();
			ItemSize.X = FMath::CeilToFloat(ItemSize.X);
			ItemSize.Y = FMath::CeilToFloat(ItemSize.Y);
			ReturnToPool(obj);
		}

		if (LayoutType == EListLayoutType::SingleCol || LayoutType == EListLayoutType::HorizontalFlow)
		{
			ScrollPanel->ScrollStep = ItemSize.Y;
			if (bLoop)
			{
				ScrollPanel->LoopMode = 2;
			}
		}
		else
		{
			ScrollPanel->ScrollStep = ItemSize.X;
			if (bLoop)
			{
				ScrollPanel->LoopMode = 1;
			}
		}

		On(FFairyEventNames::Scroll).AddUObject(this, &UFairyListView::OnScrollHandler);
		SetVirtualListChangedFlag(true);
	}
}

int32 UFairyListView::GetNumItems() const
{
	return bVirtual ? NumItems : Children.Num();
}

void UFairyListView::SetNumItems(int32 InNumItems)
{
	if (bVirtual)
	{
		verifyf(ItemRenderer.IsBound(), TEXT("Set itemRenderer first!"));

		NumItems = InNumItems;
		RealNumItems = bLoop ? NumItems * 6 : NumItems;

		int32 oldCount = VirtualItems.Num();
		if (RealNumItems > oldCount)
		{
			for (int32 i = oldCount; i < RealNumItems; i++)
			{
				FFairyListItemInfo ii;
				ii.Size = ItemSize;

				VirtualItems.Add(MoveTemp(ii));
			}
		}
		else
		{
			for (int32 i = RealNumItems; i < oldCount; i++)
			{
				VirtualItems[i].bSelected = false;
			}
		}

		if (VirtualListChanged != 0)
		{
			CancelDelayCall(RefreshTimerHandle);
		}

		DoRefreshVirtualList();
	}
	else
	{
		int32 cnt = Children.Num();
		if (InNumItems > cnt)
		{
			for (int32 i = cnt; i < InNumItems; i++)
			{
				if (!ItemProvider.IsBound())
				{
					AddItemFromPool();
				}
				else
				{
					AddItemFromPool(FName(ItemProvider.Execute(i)));
				}
			}
		}
		else
		{
			RemoveChildrenToPool(InNumItems, cnt);
		}

		if (ItemRenderer.IsBound())
		{
			for (int32 i = 0; i < InNumItems; i++)
			{
				ItemRenderer.Execute(i, GetChildAt(i));
			}
		}
	}
}

void UFairyListView::RefreshVirtualList()
{
	verifyf(bVirtual, TEXT("not virtual list"));

	SetVirtualListChangedFlag(false);
}

FVector2D UFairyListView::GetSnappingPosition(const FVector2D& InPoint)
{
	if (bVirtual)
	{
		FVector2D ret = InPoint;
		if (LayoutType == EListLayoutType::SingleCol || LayoutType == EListLayoutType::HorizontalFlow)
		{
			int32 index = GetIndexOnPos1(ret.Y, false);
			if (index < VirtualItems.Num() && InPoint.Y - ret.Y > VirtualItems[index].Size.Y / 2 && index < RealNumItems)
			{
				ret.Y += VirtualItems[index].Size.Y + RowSpacing;
			}
		}
		else if (LayoutType == EListLayoutType::SingleRow || LayoutType == EListLayoutType::VerticalFlow)
		{
			int32 index = GetIndexOnPos2(ret.X, false);
			if (index < VirtualItems.Num() && InPoint.X - ret.X > VirtualItems[index].Size.X / 2 && index < RealNumItems)
			{
				ret.X += VirtualItems[index].Size.X + ColSpacing;
			}
		}
		else
		{
			int32 index = GetIndexOnPos3(ret.X, false);
			if (index < VirtualItems.Num() && InPoint.X - ret.X > VirtualItems[index].Size.X / 2 && index < RealNumItems)
			{
				ret.X += VirtualItems[index].Size.X + ColSpacing;
			}
		}

		return ret;
	}
	else
	{
		return UFairyComponent::GetSnappingPosition(InPoint);
	}
}

void UFairyListView::CheckVirtualList()
{
	if (VirtualListChanged != 0)
	{
		DoRefreshVirtualList();
		CancelDelayCall(RefreshTimerHandle);
	}
}

void UFairyListView::SetVirtualListChangedFlag(bool bLayoutChanged)
{
	if (bLayoutChanged)
	{
		VirtualListChanged = 2;
	}
	else if (VirtualListChanged == 0)
	{
		VirtualListChanged = 1;
	}

	//DelayCall(RefreshTimerHandle, this, &UFairyListView::DoRefreshVirtualList);
	DoRefreshVirtualList();
}

void UFairyListView::DoRefreshVirtualList()
{
	bool bLayoutChanged = VirtualListChanged == 2;
	VirtualListChanged = 0;
	bEventLocked = true;

	if (bLayoutChanged)
	{
		if (LayoutType == EListLayoutType::SingleCol || LayoutType == EListLayoutType::SingleRow)
		{
			CurLineItemCount = 1;
		}
		else if (LayoutType == EListLayoutType::HorizontalFlow)
		{
			if (ColNum > 0)
			{
				CurLineItemCount = ColNum;
			}
			else
			{
				CurLineItemCount = FMath::FloorToInt((ScrollPanel->GetViewSize().X + ColSpacing) / (ItemSize.X + ColSpacing));
				if (CurLineItemCount <= 0)
				{
					CurLineItemCount = 1;
				}
			}
		}
		else if (LayoutType == EListLayoutType::VerticalFlow)
		{
			if (RowNum > 0)
			{
				CurLineItemCount = RowNum;
			}
			else
			{
				CurLineItemCount = FMath::FloorToInt((ScrollPanel->GetViewSize().Y + RowSpacing) / (ItemSize.Y + RowSpacing));
				if (CurLineItemCount <= 0)
				{
					CurLineItemCount = 1;
				}
			}
		}
		else //Pagination
		{
			if (ColNum > 0)
			{
				CurLineItemCount = ColNum;
			}
			else
			{
				CurLineItemCount = FMath::FloorToInt((ScrollPanel->GetViewSize().X + ColSpacing) / (ItemSize.X + ColSpacing));
				if (CurLineItemCount <= 0)
				{
					CurLineItemCount = 1;
				}
			}

			if (RowNum > 0)
			{
				CurLineItemCount2 = RowNum;
			}
			else
			{
				CurLineItemCount2 = FMath::FloorToInt((ScrollPanel->GetViewSize().Y + RowSpacing) / (ItemSize.Y + RowSpacing));
				if (CurLineItemCount2 <= 0)
				{
					CurLineItemCount2 = 1;
				}
			}
		}
	}
	float ch = 0, cw = 0;
	if (RealNumItems > 0)
	{
		int32 len = FMath::FloorToInt((float)RealNumItems / CurLineItemCount) * CurLineItemCount;
		int32 len2 = FMath::Min(CurLineItemCount, RealNumItems);
		if (LayoutType == EListLayoutType::SingleCol || LayoutType == EListLayoutType::HorizontalFlow)
		{
			for (int32 i = 0; i < len; i += CurLineItemCount)
			{
				ch += VirtualItems[i].Size.Y + RowSpacing;
			}
			if (ch > 0)
			{
				ch -= RowSpacing;
			}

			if (bAutoResizeItem)
			{
				cw = ScrollPanel->GetViewSize().X;
			}
			else
			{
				for (int32 i = 0; i < len2; i++)
				{
					cw += VirtualItems[i].Size.X + ColSpacing;
				}
				if (cw > 0)
				{
					cw -= ColSpacing;
				}
			}
		}
		else if (LayoutType == EListLayoutType::SingleRow || LayoutType == EListLayoutType::VerticalFlow)
		{
			for (int32 i = 0; i < len; i += CurLineItemCount)
			{
				cw += VirtualItems[i].Size.X + ColSpacing;
			}
			if (cw > 0)
			{
				cw -= ColSpacing;
			}

			if (bAutoResizeItem)
			{
				ch = ScrollPanel->GetViewSize().Y;
			}
			else
			{
				for (int32 i = 0; i < len2; i++)
				{
					ch += VirtualItems[i].Size.Y + RowSpacing;
				}
				if (ch > 0)
				{
					ch -= RowSpacing;
				}
			}
		}
		else
		{
			int32 pageCount = FMath::CeilToInt((float)len / (CurLineItemCount * CurLineItemCount2));
			cw = pageCount * GetViewWidth();
			ch = GetViewHeight();
		}
	}

	HandleAlign(cw, ch);
	ScrollPanel->SetContentSize(FVector2D(cw, ch));

	bEventLocked = false;

	HandleScroll(true);
}

void UFairyListView::OnScrollHandler(UEventContext* Context)
{
	HandleScroll(false);
}

int32 UFairyListView::GetIndexOnPos1(float& pos, bool forceUpdate)
{
	if (RealNumItems < CurLineItemCount)
	{
		pos = 0;
		return 0;
	}

	if (NumChildren() > 0 && !forceUpdate)
	{
		float pos2 = GetChildAt(0)->GetPosition().Y;
		if (pos2 + (RowSpacing > 0 ? 0 : -RowSpacing) > pos)
		{
			for (int32 i = FirstIndex - CurLineItemCount; i >= 0; i -= CurLineItemCount)
			{
				pos2 -= (VirtualItems[i].Size.Y + RowSpacing);
				if (pos2 <= pos)
				{
					pos = pos2;
					return i;
				}
			}

			pos = 0;
			return 0;
		}
		else
		{
			float testGap = RowSpacing > 0 ? RowSpacing : 0;
			for (int32 i = FirstIndex; i < RealNumItems; i += CurLineItemCount)
			{
				float pos3 = pos2 + VirtualItems[i].Size.Y;
				if (pos3 + testGap > pos)
				{
					pos = pos2;
					return i;
				}
				pos2 = pos3 + RowSpacing;
			}

			pos = pos2;
			return RealNumItems - CurLineItemCount;
		}
	}
	else
	{
		float pos2 = 0;
		float testGap = RowSpacing > 0 ? RowSpacing : 0;
		for (int32 i = 0; i < RealNumItems; i += CurLineItemCount)
		{
			float pos3 = pos2 + VirtualItems[i].Size.Y;
			if (pos3 + testGap > pos)
			{
				pos = pos2;
				return i;
			}
			pos2 = pos3 + RowSpacing;
		}

		pos = pos2;
		return RealNumItems - CurLineItemCount;
	}
}

int32 UFairyListView::GetIndexOnPos2(float& pos, bool forceUpdate)
{
	if (RealNumItems < CurLineItemCount)
	{
		pos = 0;
		return 0;
	}

	if (NumChildren() > 0 && !forceUpdate)
	{
		float pos2 = GetChildAt(0)->GetPosition().X;
		if (pos2 + (ColSpacing > 0 ? 0 : -ColSpacing) > pos)
		{
			for (int32 i = FirstIndex - CurLineItemCount; i >= 0; i -= CurLineItemCount)
			{
				pos2 -= (VirtualItems[i].Size.X + ColSpacing);
				if (pos2 <= pos)
				{
					pos = pos2;
					return i;
				}
			}

			pos = 0;
			return 0;
		}
		else
		{
			float testGap = ColSpacing > 0 ? ColSpacing : 0;
			for (int32 i = FirstIndex; i < RealNumItems; i += CurLineItemCount)
			{
				float pos3 = pos2 + VirtualItems[i].Size.X;
				if (pos3 + testGap > pos)
				{
					pos = pos2;
					return i;
				}
				pos2 = pos3 + ColSpacing;
			}

			pos = pos2;
			return RealNumItems - CurLineItemCount;
		}
	}
	else
	{
		float pos2 = 0;
		float testGap = ColSpacing > 0 ? ColSpacing : 0;
		for (int32 i = 0; i < RealNumItems; i += CurLineItemCount)
		{
			float pos3 = pos2 + VirtualItems[i].Size.X;
			if (pos3 + testGap > pos)
			{
				pos = pos2;
				return i;
			}
			pos2 = pos3 + ColSpacing;
		}

		pos = pos2;
		return RealNumItems - CurLineItemCount;
	}
}

int32 UFairyListView::GetIndexOnPos3(float& pos, bool forceUpdate)
{
	if (RealNumItems < CurLineItemCount)
	{
		pos = 0;
		return 0;
	}

	float viewWidth = GetViewWidth();
	int32 page = FMath::FloorToInt(pos / viewWidth);
	int32 startIndex = page * (CurLineItemCount * CurLineItemCount2);
	float pos2 = page * viewWidth;
	float testGap = ColSpacing > 0 ? ColSpacing : 0;
	for (int32 i = 0; i < CurLineItemCount; i++)
	{
		float pos3 = pos2 + VirtualItems[startIndex + i].Size.X;
		if (pos3 + testGap > pos)
		{
			pos = pos2;
			return startIndex + i;
		}
		pos2 = pos3 + ColSpacing;
	}

	pos = pos2;
	return startIndex + CurLineItemCount - 1;
}

void UFairyListView::HandleScroll(bool forceUpdate)
{
	if (bEventLocked)
	{
		return;
	}

	if (LayoutType == EListLayoutType::SingleCol || LayoutType == EListLayoutType::HorizontalFlow)
	{
		int32 enterCounter = 0;
		while (HandleScroll1(forceUpdate))
		{
			enterCounter++;
			forceUpdate = false;
			if (enterCounter > 20)
			{
				UE_LOG(LogFairyGUI, Warning, TEXT("list will never be filled as the item renderer function always returns a different size."));
				break;
			}
		}
		HandleArchOrder1();
	}
	else if (LayoutType == EListLayoutType::SingleRow || LayoutType == EListLayoutType::VerticalFlow)
	{
		int32 enterCounter = 0;
		while (HandleScroll2(forceUpdate))
		{
			enterCounter++;
			forceUpdate = false;
			if (enterCounter > 20)
			{
				UE_LOG(LogFairyGUI, Warning, TEXT("list will never be filled as the item renderer function always returns a different size."));
				break;
			}
		}
		HandleArchOrder2();
	}
	else
	{
		HandleScroll3(forceUpdate);
	}

	bBoundsChanged = false;
}

bool UFairyListView::HandleScroll1(bool forceUpdate)
{
	float pos = ScrollPanel->GetScrollingPosY();
	float max = pos + ScrollPanel->GetViewSize().Y;
	bool end = max == ScrollPanel->GetContentSize().Y;

	int32 newFirstIndex = GetIndexOnPos1(pos, forceUpdate);
	if (newFirstIndex == FirstIndex && !forceUpdate)
		return false;

	int32 oldFirstIndex = FirstIndex;
	FirstIndex = newFirstIndex;
	int32 curIndex = newFirstIndex;
	bool forward = oldFirstIndex > newFirstIndex;
	int32 childCount = NumChildren();
	int32 lastIndex = oldFirstIndex + childCount - 1;
	int32 reuseIndex = forward ? lastIndex : oldFirstIndex;
	float curX = 0, curY = pos;
	bool needRender;
	float deltaSize = 0;
	float firstItemDeltaSize = 0;
	FName url(DefaultItem);
	int32 partSize = (int32)((ScrollPanel->GetViewSize().X - ColSpacing * (CurLineItemCount - 1)) / CurLineItemCount);

	ItemInfoVer++;
	while (curIndex < RealNumItems && (end || curY < max))
	{
		FFairyListItemInfo& ii = VirtualItems[curIndex];

		if (ii.Obj == nullptr || forceUpdate)
		{
			if (ItemProvider.IsBound())
			{
				url = FName(ItemProvider.Execute(curIndex % NumItems));
				if (url.IsNone())
				{
					url = DefaultItem;
				}
				url = UFairyPackageMgr::Get()->NormalizeURL(url);
			}

			if (ii.Obj != nullptr && ii.Obj->GetResourceURL().Compare(url) != 0)
			{
				if (Cast<UFairyButton>(ii.Obj))
				{
					ii.bSelected = ((UFairyButton*)ii.Obj)->IsSelected();
				}
				RemoveChildToPool(ii.Obj);
				ii.Obj = nullptr;
			}
		}

		if (ii.Obj == nullptr)
		{
			if (forward)
			{
				for (int32 j = reuseIndex; j >= oldFirstIndex; j--)
				{
					FFairyListItemInfo& ii2 = VirtualItems[j];
					if (ii2.Obj != nullptr && ii2.UpdateFlag != ItemInfoVer && ii2.Obj->GetResourceURL().Compare(url) == 0)
					{
						if (Cast<UFairyButton>(ii2.Obj))
						{
							ii2.bSelected = ((UFairyButton*)ii2.Obj)->IsSelected();
						}
						ii.Obj = ii2.Obj;
						ii2.Obj = nullptr;
						if (j == reuseIndex)
						{
							reuseIndex--;
						}
						break;
					}
				}
			}
			else
			{
				for (int32 j = reuseIndex; j <= lastIndex; j++)
				{
					FFairyListItemInfo& ii2 = VirtualItems[j];
					if (ii2.Obj != nullptr && ii2.UpdateFlag != ItemInfoVer && ii2.Obj->GetResourceURL().Compare(url) == 0)
					{
						if (Cast<UFairyButton>(ii2.Obj))
						{
							ii2.bSelected = ((UFairyButton*)ii2.Obj)->IsSelected();
						}
						ii.Obj = ii2.Obj;
						ii2.Obj = nullptr;
						if (j == reuseIndex)
						{
							reuseIndex++;
						}
						break;
					}
				}
			}

			if (ii.Obj != nullptr)
			{
				SetChildIndex(ii.Obj, forward ? curIndex - newFirstIndex : NumChildren());
			}
			else
			{
				ii.Obj = Pool->GetOrCreateObject(this, url);
				if (forward)
				{
					AddChildAt(ii.Obj, curIndex - newFirstIndex);
				}
				else
				{
					AddChild(ii.Obj);
				}
			}
			if (Cast<UFairyButton>(ii.Obj))
			{
				((UFairyButton*)ii.Obj)->SetSelected(ii.bSelected);
			}

			needRender = true;
		}
		else
		{
			needRender = forceUpdate;
		}

		if (needRender)
		{
			if (bAutoResizeItem && (LayoutType == EListLayoutType::SingleCol || ColNum > 0))
			{
				ii.Obj->SetSize(FVector2D(partSize, ii.Obj->GetHeight()));
				ii.Obj->SetPivot(ii.Obj->GetPivot(), true);
			}

			ItemRenderer.Execute(curIndex % NumItems, ii.Obj);
			if (curIndex % CurLineItemCount == 0)
			{
				deltaSize += FMath::CeilToFloat(ii.Obj->GetHeight()) - ii.Size.Y;
				if (curIndex == newFirstIndex && oldFirstIndex > newFirstIndex)
				{
					firstItemDeltaSize = FMath::CeilToFloat(ii.Obj->GetHeight()) - ii.Size.Y;
				}
			}
			ii.Size.X = FMath::CeilToFloat(ii.Obj->GetWidth());
			ii.Size.Y = FMath::CeilToFloat(ii.Obj->GetHeight());
		}

		ii.UpdateFlag = ItemInfoVer;
		ii.Obj->SetPosition(FVector2D(curX, curY));
		if (curIndex == newFirstIndex)
		{
			max += ii.Size.Y;
		}

		curX += ii.Size.X + ColSpacing;

		if (curIndex % CurLineItemCount == CurLineItemCount - 1)
		{
			curX = 0;
			curY += ii.Size.Y + RowSpacing;
		}
		curIndex++;
	}

	for (int32 i = 0; i < childCount; i++)
	{
		FFairyListItemInfo& ii = VirtualItems[oldFirstIndex + i];
		if (ii.UpdateFlag != ItemInfoVer && ii.Obj != nullptr)
		{
			if (Cast<UFairyButton>(ii.Obj))
			{
				ii.bSelected = ((UFairyButton*)ii.Obj)->IsSelected();
			}
			RemoveChildToPool(ii.Obj);
			ii.Obj = nullptr;
		}
	}

	childCount = Children.Num();
	for (int32 i = 0; i < childCount; i++)
	{
		UFairyObject* obj = VirtualItems[newFirstIndex + i].Obj;
		if (Children[i] != obj)
		{
			SetChildIndex(obj, i);
		}
	}

	if (deltaSize != 0 || firstItemDeltaSize != 0)
	{
		ScrollPanel->ChangeContentSizeOnScrolling(0, deltaSize, 0, firstItemDeltaSize);
	}

	//if (curIndex > 0 && NumChildren() > 0 && Container->GetPosition().Y <= 0 && GetChildAt(0)->GetPosition().Y > -Container->GetPosition().Y)
	//{
	//	return true;
	//}
	//else
	//{
	//	return false;
	//}
	return true;
}

bool UFairyListView::HandleScroll2(bool forceUpdate)
{
	float pos = ScrollPanel->GetScrollingPosX();
	float max = pos + ScrollPanel->GetViewSize().X;
	bool end = pos == ScrollPanel->GetContentSize().X;

	int32 newFirstIndex = GetIndexOnPos2(pos, forceUpdate);
	if (newFirstIndex == FirstIndex && !forceUpdate)
	{
		return false;
	}

	int32 oldFirstIndex = FirstIndex;
	FirstIndex = newFirstIndex;
	int32 curIndex = newFirstIndex;
	bool forward = oldFirstIndex > newFirstIndex;
	int32 childCount = NumChildren();
	int32 lastIndex = oldFirstIndex + childCount - 1;
	int32 reuseIndex = forward ? lastIndex : oldFirstIndex;
	float curX = pos, curY = 0;
	bool needRender;
	float deltaSize = 0;
	float firstItemDeltaSize = 0;
	FName url(DefaultItem);
	int32 partSize = (int32)((ScrollPanel->GetViewSize().Y - RowSpacing * (CurLineItemCount - 1)) / CurLineItemCount);

	ItemInfoVer++;
	while (curIndex < RealNumItems && (end || curX < max))
	{
		FFairyListItemInfo& ii = VirtualItems[curIndex];

		if (ii.Obj == nullptr || forceUpdate)
		{
			if (ItemProvider.IsBound())
			{
				url = FName(ItemProvider.Execute(curIndex % NumItems));
				if (url.IsNone())
				{
					url = DefaultItem;
				}
				url = UFairyPackageMgr::Get()->NormalizeURL(url);
			}

			if (ii.Obj != nullptr && ii.Obj->GetResourceURL().Compare(url) != 0)
			{
				if (Cast<UFairyButton>(ii.Obj))
				{
					ii.bSelected = ((UFairyButton*)ii.Obj)->IsSelected();
				}
				RemoveChildToPool(ii.Obj);
				ii.Obj = nullptr;
			}
		}

		if (ii.Obj == nullptr)
		{
			if (forward)
			{
				for (int32 j = reuseIndex; j >= oldFirstIndex; j--)
				{
					FFairyListItemInfo& ii2 = VirtualItems[j];
					if (ii2.Obj != nullptr && ii2.UpdateFlag != ItemInfoVer && ii2.Obj->GetResourceURL().Compare(url) == 0)
					{
						if (Cast<UFairyButton>(ii2.Obj))
						{
							ii2.bSelected = ((UFairyButton*)ii2.Obj)->IsSelected();
						}
						ii.Obj = ii2.Obj;
						ii2.Obj = nullptr;
						if (j == reuseIndex)
						{
							reuseIndex--;
						}
						break;
					}
				}
			}
			else
			{
				for (int32 j = reuseIndex; j <= lastIndex; j++)
				{
					FFairyListItemInfo& ii2 = VirtualItems[j];
					if (ii2.Obj != nullptr && ii2.UpdateFlag != ItemInfoVer && ii2.Obj->GetResourceURL().Compare(url) == 0)
					{
						if (Cast<UFairyButton>(ii2.Obj))
						{
							ii2.bSelected = ((UFairyButton*)ii2.Obj)->IsSelected();
						}
						ii.Obj = ii2.Obj;
						ii2.Obj = nullptr;
						if (j == reuseIndex)
						{
							reuseIndex++;
						}
						break;
					}
				}
			}

			if (ii.Obj != nullptr)
			{
				SetChildIndex(ii.Obj, forward ? curIndex - newFirstIndex : NumChildren());
			}
			else
			{
				ii.Obj = Pool->GetOrCreateObject(this, url);
				if (forward)
				{
					AddChildAt(ii.Obj, curIndex - newFirstIndex);
				}
				else
				{
					AddChild(ii.Obj);
				}
			}
			if (Cast<UFairyButton>(ii.Obj))
			{
				((UFairyButton*)ii.Obj)->SetSelected(ii.bSelected);
			}

			needRender = true;
		}
		else
		{
			needRender = forceUpdate;
		}

		if (needRender)
		{
			if (bAutoResizeItem && (LayoutType == EListLayoutType::SingleRow || RowNum > 0))
			{
				ii.Obj->SetSize(FVector2D(ii.Obj->GetWidth(), partSize));
				ii.Obj->SetPivot(ii.Obj->GetPivot(), true);
			}

			ItemRenderer.Execute(curIndex % NumItems, ii.Obj);
			if (curIndex % CurLineItemCount == 0)
			{
				deltaSize += FMath::CeilToFloat(ii.Obj->GetWidth()) - ii.Size.X;
				if (curIndex == newFirstIndex && oldFirstIndex > newFirstIndex)
				{
					firstItemDeltaSize = FMath::CeilToFloat(ii.Obj->GetWidth()) - ii.Size.X;
				}
			}
			ii.Size.X = FMath::CeilToFloat(ii.Obj->GetWidth());
			ii.Size.Y = FMath::CeilToFloat(ii.Obj->GetHeight());
		}

		ii.UpdateFlag = ItemInfoVer;
		ii.Obj->SetPosition(FVector2D(curX, curY));
		if (curIndex == newFirstIndex)
		{
			max += ii.Size.X;
		}

		curY += ii.Size.Y + RowSpacing;

		if (curIndex % CurLineItemCount == CurLineItemCount - 1)
		{
			curY = 0;
			curX += ii.Size.X + ColSpacing;
		}
		curIndex++;
	}

	for (int32 i = 0; i < childCount; i++)
	{
		FFairyListItemInfo& ii = VirtualItems[oldFirstIndex + i];
		if (ii.UpdateFlag != ItemInfoVer && ii.Obj != nullptr)
		{
			if (Cast<UFairyButton>(ii.Obj))
				ii.bSelected = ((UFairyButton*)ii.Obj)->IsSelected();
			RemoveChildToPool(ii.Obj);
			ii.Obj = nullptr;
		}
	}

	childCount = Children.Num();
	for (int32 i = 0; i < childCount; i++)
	{
		UFairyObject* obj = VirtualItems[newFirstIndex + i].Obj;
		if (Children[i] != obj)
			SetChildIndex(obj, i);
	}

	if (deltaSize != 0 || firstItemDeltaSize != 0)
	{
		ScrollPanel->ChangeContentSizeOnScrolling(deltaSize, 0, firstItemDeltaSize, 0);
	}

	//if (curIndex > 0 && NumChildren() > 0 && Container->GetPosition().X <= 0 && GetChildAt(0)->GetPosition().X > -Container->GetPosition().X)
	//	return true;
	//else
	//	return false;
	return true;
}

void UFairyListView::HandleScroll3(bool forceUpdate)
{
	float pos = ScrollPanel->GetScrollingPosX();

	int32 newFirstIndex = GetIndexOnPos3(pos, forceUpdate);
	if (newFirstIndex == FirstIndex && !forceUpdate)
		return;

	int32 oldFirstIndex = FirstIndex;
	FirstIndex = newFirstIndex;

	int32 reuseIndex = oldFirstIndex;
	int32 virtualItemCount = VirtualItems.Num();
	int32 pageSize = CurLineItemCount * CurLineItemCount2;
	int32 startCol = newFirstIndex % CurLineItemCount;
	float viewWidth = GetViewWidth();
	int32 page = (int32)(newFirstIndex / pageSize);
	int32 startIndex = page * pageSize;
	int32 lastIndex = startIndex + pageSize * 2;
	bool needRender;
	FName url(DefaultItem);
	int32 partWidth = (int32)((ScrollPanel->GetViewSize().X - ColSpacing * (CurLineItemCount - 1)) / CurLineItemCount);
	int32 partHeight = (int32)((ScrollPanel->GetViewSize().Y - RowSpacing * (CurLineItemCount2 - 1)) / CurLineItemCount2);
	ItemInfoVer++;

	for (int32 i = startIndex; i < lastIndex; i++)
	{
		if (i >= RealNumItems)
			continue;

		int32 col = i % CurLineItemCount;
		if (i - startIndex < pageSize)
		{
			if (col < startCol)
				continue;
		}
		else
		{
			if (col > startCol)
				continue;
		}

		FFairyListItemInfo& ii = VirtualItems[i];
		ii.UpdateFlag = ItemInfoVer;
	}

	UFairyObject* lastObj = nullptr;
	int32 insertIndex = 0;
	for (int32 i = startIndex; i < lastIndex; i++)
	{
		if (i >= RealNumItems)
			continue;

		FFairyListItemInfo& ii = VirtualItems[i];
		if (ii.UpdateFlag != ItemInfoVer)
			continue;

		if (ii.Obj == nullptr)
		{
			reuseIndex = reuseIndex < 0 ? 0 : reuseIndex;
			while (reuseIndex < virtualItemCount)
			{
				FFairyListItemInfo& ii2 = VirtualItems[reuseIndex];
				if (ii2.Obj != nullptr && ii2.UpdateFlag != ItemInfoVer)
				{
					if (Cast<UFairyButton>(ii2.Obj))
						ii2.bSelected = ((UFairyButton*)ii2.Obj)->IsSelected();
					ii.Obj = ii2.Obj;
					ii2.Obj = nullptr;
					break;
				}
				reuseIndex++;
			}

			if (insertIndex == -1)
				insertIndex = GetChildIndex(lastObj) + 1;

			if (ii.Obj == nullptr)
			{
				if (ItemProvider.IsBound())
				{
					url = FName(ItemProvider.Execute(i % NumItems));
					if (url.IsNone())
					{
						url = DefaultItem;
					}
					url = UFairyPackageMgr::Get()->NormalizeURL(url);
				}

				ii.Obj = Pool->GetOrCreateObject(this, url);
				AddChildAt(ii.Obj, insertIndex);
			}
			else
			{
				insertIndex = SetChildIndexBefore(ii.Obj, insertIndex);
			}
			insertIndex++;

			if (Cast<UFairyButton>(ii.Obj))
				((UFairyButton*)ii.Obj)->SetSelected(ii.bSelected);

			needRender = true;
		}
		else
		{
			needRender = forceUpdate;
			insertIndex = -1;
			lastObj = ii.Obj;
		}

		if (needRender)
		{
			if (bAutoResizeItem)
			{
				UFairyObject* Object = ii.Obj;
				if (CurLineItemCount == ColNum && CurLineItemCount2 == RowNum)
				{
					Object->SetSize(FVector2D(partWidth, partHeight));
				}
				else if (CurLineItemCount == ColNum)
				{
					Object->SetSize(FVector2D(partWidth, ii.Obj->GetHeight()));
				}
				else if (CurLineItemCount2 == RowNum)
				{
					Object->SetSize(FVector2D(ii.Obj->GetWidth(), partHeight));
				}
				Object->SetPivot(Object->GetPivot(), true);
			}

			ItemRenderer.Execute(i % NumItems, ii.Obj);
			ii.Size.X = FMath::CeilToFloat(ii.Obj->GetWidth());
			ii.Size.Y = FMath::CeilToFloat(ii.Obj->GetHeight());
		}
	}

	float borderX = (startIndex / pageSize) * viewWidth;
	float xx = borderX;
	float yy = 0;
	float lineHeight = 0;
	for (int32 i = startIndex; i < lastIndex; i++)
	{
		if (i >= RealNumItems)
			continue;

		FFairyListItemInfo& ii = VirtualItems[i];
		if (ii.UpdateFlag == ItemInfoVer)
			ii.Obj->SetPosition(FVector2D(xx, yy));

		if (ii.Size.Y > lineHeight)
			lineHeight = ii.Size.Y;
		if (i % CurLineItemCount == CurLineItemCount - 1)
		{
			xx = borderX;
			yy += lineHeight + RowSpacing;
			lineHeight = 0;

			if (i == startIndex + pageSize - 1)
			{
				borderX += viewWidth;
				xx = borderX;
				yy = 0;
			}
		}
		else
		{
			xx += ii.Size.X + ColSpacing;
		}
	}

	for (int32 i = reuseIndex; i < virtualItemCount; i++)
	{
		FFairyListItemInfo& ii = VirtualItems[i];
		if (ii.UpdateFlag != ItemInfoVer && ii.Obj != nullptr)
		{
			if (Cast<UFairyButton>(ii.Obj))
			{
				ii.bSelected = ((UFairyButton*)ii.Obj)->IsSelected();
			}
			RemoveChildToPool(ii.Obj);
			ii.Obj = nullptr;
		}
	}
}

void UFairyListView::HandleArchOrder1()
{
	if (ChildrenRenderOrder == EChildrenRenderOrder::Arch)
	{
		float mid = ScrollPanel->GetPosY() + GetViewHeight() / 2;
		float minDist = FLT_MAX, dist;
		int32 apexIndex = 0;
		int32 cnt = NumChildren();
		for (int32 i = 0; i < cnt; i++)
		{
			UFairyObject* obj = GetChildAt(i);
			if (!bFoldInvisibleItems || obj->IsVisible())
			{
				dist = FMath::Abs(mid - obj->GetPosition().Y - obj->GetHeight() / 2);
				if (dist < minDist)
				{
					minDist = dist;
					apexIndex = i;
				}
			}
		}
		SetApexIndex(apexIndex);
	}
}

void UFairyListView::HandleArchOrder2()
{
	if (ChildrenRenderOrder == EChildrenRenderOrder::Arch)
	{
		float mid = ScrollPanel->GetPosX() + GetViewWidth() / 2;
		float minDist = FLT_MAX, dist;
		int32 apexIndex = 0;
		int32 cnt = NumChildren();
		for (int32 i = 0; i < cnt; i++)
		{
			UFairyObject* obj = GetChildAt(i);
			if (!bFoldInvisibleItems || obj->IsVisible())
			{
				dist = FMath::Abs(mid - obj->GetPosition().X - obj->GetWidth() / 2);
				if (dist < minDist)
				{
					minDist = dist;
					apexIndex = i;
				}
			}
		}
		SetApexIndex(apexIndex);
	}
}

void UFairyListView::HandleAlign(float contentWidth, float contentHeight)
{
	FVector2D newOffset(0, 0);

	float viewHeight = GetViewHeight();
	float viewWidth = GetViewWidth();
	
	if (contentWidth < viewWidth)
	{
		if (HorizontalAlign == EHAlignType::Center)
		{
			newOffset.X = (int32)((viewWidth - contentWidth) / 2);
		}
		else if (HorizontalAlign == EHAlignType::Right)
		{
			newOffset.X = viewWidth - contentWidth;
		}
	}

	if (contentHeight < viewHeight)
	{
		if (VerticalAlign == EVAlignType::Middle)
		{
			newOffset.Y = (int32)((viewHeight - contentHeight) / 2);
		}
		else if (VerticalAlign == EVAlignType::Bottom)
		{
			newOffset.Y = viewHeight - contentHeight;
		}
	}

	if (newOffset != AlignOffset)
	{
		AlignOffset = newOffset;
		SetScrollContentPosition(newOffset);
	}
}

void UFairyListView::SetItemRenderer(const FDynListItemRenderer& InItemRenderer)
{
	if (InItemRenderer.IsBound())
	{
		ItemRenderer = FListItemRenderer::CreateUFunction(const_cast<UObject*>(InItemRenderer.GetUObject()), InItemRenderer.GetFunctionName());
	}
	else
	{
		ItemRenderer.Unbind();
	}
}

void UFairyListView::SetItemProvider(const FDynListItemProvider& InItemProvider)
{
	if (InItemProvider.IsBound())
	{
		ItemProvider = FListItemProvider::CreateUFunction(const_cast<UObject*>(InItemProvider.GetUObject()), InItemProvider.GetFunctionName());
	}
	else
	{
		ItemProvider.Unbind();
	}
}


EFairyScrollDirection UFairyListView::GetScrollDirection()
{
	if (LayoutType == EListLayoutType::SingleCol || LayoutType == EListLayoutType::HorizontalFlow)
	{
		return EFairyScrollDirection::Horizontal;
	}
	else if (LayoutType == EListLayoutType::SingleRow || LayoutType == EListLayoutType::VerticalFlow)
	{
		return EFairyScrollDirection::Vertical;
	}
	else
	{
		return EFairyScrollDirection::Both;
	}
}

