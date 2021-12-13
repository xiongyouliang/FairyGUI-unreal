#pragma once

#include "Misc/Attribute.h"
#include "SDisplayObject.h"

class FAIRYGUI_API SContainer : public SDisplayObject
{
public:
    class FSlot : public TSlotBase<FSlot>
    {
    public:
        FSlot& Position(const TAttribute<FVector2D>& InPosition) 
        {
            PositionAttr = InPosition;
            return *this;
        }

        FSlot& Size(const TAttribute<FVector2D>& InSize)
        {
            SizeAttr = InSize;
            return *this;
        }

        FSlot& Anchor(const TAttribute<FVector2D>& InAnchor)
        {
            AnchorAttr = InAnchor;
            return *this;
        }

        TAttribute<FVector2D> PositionAttr;
        TAttribute<FVector2D> SizeAttr;
        TAttribute<FVector2D> AnchorAttr;

        FSlot()
            :TSlotBase<FSlot>()
            , PositionAttr(FVector2D::ZeroVector)
            , SizeAttr(FVector2D(1.0f, 1.0f))
            , AnchorAttr(FVector2D::ZeroVector)
        {

        }
    };

    SLATE_BEGIN_ARGS(SContainer) :
        _GObject(nullptr)
    {}
    SLATE_ARGUMENT(UFairyObject*, GObject) // todo: rename to FairyObject
    SLATE_END_ARGS()

    SContainer();

    void Construct(const FArguments& InArgs);

    SContainer::FSlot& AddChild(const TSharedRef<SDisplayObject>& SlotWidget);
    SContainer::FSlot& AddChildAt(const TSharedRef<SDisplayObject>& SlotWidget, int32 Index);

    int32 GetChildIndex(const TSharedRef<SWidget>& SlotWidget) const;
    void SetChildIndex(const TSharedRef<SWidget>& SlotWidget, int32 Index);

    void RemoveChild(const TSharedRef<SWidget>& SlotWidget);
    void RemoveChildAt(int32 Index);
    void RemoveChildren(int32 BeginIndex = 0, int32 EndIndex = -1);
    int32 NumChildren() const;

    static FSlot& Slot()
    {
        return *(new FSlot());
    }

    FSlot& AddSlot()
    {
        SContainer::FSlot& NewSlot = SContainer::Slot();
        Children.Add(&NewSlot);
        return NewSlot;
    }

public:
    virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
    virtual FChildren* GetChildren() override;

protected:
    TPanelChildren<SContainer::FSlot> Children;
};