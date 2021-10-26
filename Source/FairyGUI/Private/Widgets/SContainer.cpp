
#include "Widgets/SContainer.h"
#include "FairyApplication.h"

SContainer::SContainer() :
    Children(this)
{
    bCanSupportFocus = false;
}

void SContainer::Construct(const SContainer::FArguments& InArgs)
{
    SDisplayObject::Construct(SDisplayObject::FArguments().GObject(InArgs._GObject));
}

SContainer::FSlot& SContainer::AddChild(const TSharedRef<SDisplayObject>& SlotWidget)
{
    return AddChildAt(SlotWidget, Children.Num());
}

SContainer::FSlot& SContainer::AddChildAt(const TSharedRef<SDisplayObject>& SlotWidget, int32 Index)
{
    int32 Count = Children.Num();
    verifyf(Index >= 0 && Index <= Count, TEXT("Invalid child index"));
    verifyf(!SlotWidget->IsParentValid(), TEXT("Cant add a child has parent"));

    SContainer::FSlot& NewSlot = SContainer::Slot();
    NewSlot.AttachWidget(SlotWidget);
    if (Index == Count)
    {
        Children.Add(&NewSlot);
    }
    else
    {
        Children.Insert(&NewSlot, Index);
    }

    if (OnStage())
    {
        UFairyApplication::Get()->BroadcastEvent(FFairyEventNames::AddedToStage, SlotWidget);
    }

    return NewSlot;
}

void SContainer::SetChildIndex(const TSharedRef<SWidget>& SlotWidget, int32 Index)
{
    if (Index >= Children.Num())
    {
        Index = Children.Num() - 1;
    }
    int32 OldIndex = GetChildIndex(SlotWidget);
    verifyf(OldIndex != -1, TEXT("Not a child of this container"));
    if (OldIndex == Index) return;
    Children.Move(OldIndex, Index);
}

void SContainer::RemoveChild(const TSharedRef<SWidget>& SlotWidget)
{
    int32 Index = GetChildIndex(SlotWidget);
    RemoveChildAt(Index);
}

void SContainer::RemoveChildAt(int32 Index)
{
    verifyf(Index >= 0 && Index < Children.Num(), TEXT("Invalid child index"));
    TSharedRef<SWidget> SlotWidget = Children[Index].GetWidget();

    if (OnStage())
    {
        UFairyApplication::Get()->BroadcastEvent(FFairyEventNames::RemovedFromStage, SlotWidget);
    }

    Children.RemoveAt(Index);
}

int32 SContainer::GetChildIndex(const TSharedRef<SWidget>& SlotWidget) const
{
    for (int32 SlotIdx = 0; SlotIdx < Children.Num(); ++SlotIdx)
    {
        if (SlotWidget == Children[SlotIdx].GetWidget())
        {
            return SlotIdx;
        }
    }

    return -1;
}

void SContainer::RemoveChildren(int32 BeginIndex, int32 EndIndex)
{
    if (EndIndex < 0 || EndIndex >= Children.Num())
        EndIndex = Children.Num() - 1;

    UFairyApplication* Dispatcher = OnStage() ? UFairyApplication::Get() : nullptr;

    if (Dispatcher != nullptr || BeginIndex > 0 || EndIndex < Children.Num() - 1)
    {
        for (int32 i = BeginIndex; i <= EndIndex; ++i)
        {
            if (Dispatcher != nullptr)
            {
                TSharedRef<SWidget> SlotWidget = Children[BeginIndex].GetWidget();
                Dispatcher->BroadcastEvent(FFairyEventNames::RemovedFromStage, SlotWidget);
            }
            Children.RemoveAt(BeginIndex);
        }
    }
    else
    {
        Children.Empty();
    }
}

int32 SContainer::NumChildren() const
{
    return Children.Num();
}

void SContainer::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
    if (Children.Num() > 0)
    {
        for (int32 ChildIndex = 0; ChildIndex < Children.Num(); ++ChildIndex)
        {
            const SContainer::FSlot& CurChild = Children[ChildIndex];
            const TSharedRef<SWidget>& CurWidget = CurChild.GetWidget();
            const FVector2D& CurChildPos = CurChild.PositionAttr.Get();
            const FVector2D& CurChildSize = CurChild.SizeAttr.Get();
            const FVector2D& CurChildAnchor = CurChild.AnchorAttr.Get();

            FVector2D FinalPos = CurChildPos;
            if (CurChildAnchor != FVector2D::ZeroVector)
            {
                FinalPos = CurChildPos - CurChildSize * CurChildAnchor;
            }

            if (ArrangedChildren.Accepts(CurWidget->GetVisibility()))
            {
                ArrangedChildren.AddWidget(
                    AllottedGeometry.MakeChild(CurWidget, CurChildSize, FSlateLayoutTransform(FinalPos))
                );
            }
        }
    }
}

int32 SContainer::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    FArrangedChildren ArrangedChildren(EVisibility::Visible);
    ArrangeChildren(AllottedGeometry, ArrangedChildren);

    // Because we paint multiple children, we must track the maximum layer id that they produced in case one of our parents
    // wants to an overlay for all of its contents.
    int32 MaxLayerId = LayerId;

    const bool bForwardedEnabled = ShouldBeEnabled(bParentEnabled);

    const FPaintArgs NewArgs = Args.WithNewParent(this);

    for (int32 ChildIndex = 0; ChildIndex < ArrangedChildren.Num(); ++ChildIndex)
    {
        FArrangedWidget& CurWidget = ArrangedChildren[ChildIndex];

        //if (!IsChildWidgetCulled(MyCullingRect, CurWidget))
        {
            const int32 CurWidgetsMaxLayerId = CurWidget.Widget->Paint(NewArgs, CurWidget.Geometry, MyCullingRect, OutDrawElements, MaxLayerId + 1, InWidgetStyle, bForwardedEnabled);

            MaxLayerId = FMath::Max(MaxLayerId, CurWidgetsMaxLayerId);
        }
        //else
        {

        }
    }

    return MaxLayerId;
}

FChildren* SContainer::GetChildren()
{
    return &Children;
}