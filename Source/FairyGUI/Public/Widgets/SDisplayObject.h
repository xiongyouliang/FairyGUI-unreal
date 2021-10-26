#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Slate.h"

class UFairyObject;

/*
* This is FairyGUI basic Slate widget;
* Maybe need to rename this class to SFairyWidget
*/
class FAIRYGUI_API SDisplayObject : public SWidget
{
public:
	SLATE_BEGIN_ARGS(SDisplayObject) :
		_GObject(nullptr),
		_Tag(SDisplayObject::SDisplayObjectTag)
	{}
		SLATE_ARGUMENT(class UFairyObject*, GObject)
		SLATE_ARGUMENT(FName, Tag)
	SLATE_END_ARGS()

public:
	static FName SDisplayObjectTag;

	SDisplayObject();
	void Construct(const FArguments& InArgs);

	void SetVisible(bool bInVisible);
	bool IsVisible() const { return bVisible; }

	void SetTouchable(bool bInTouchable);
	bool IsTouchable() const { return bTouchable; }

	void SetOpaque(bool bInOpaque);
	bool IsOpaque() const { return bOpaque; }

	void SetInteractable(bool bInInteractable);
	virtual bool IsInteractable() const override { return bInteractable; }

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	bool OnStage() const;

	FVector2D GetInViewSize();
	FVector2D GetInViewSize() const;
	TWeakObjectPtr<class UFairyObject> GetFairyObject() const { return FairyObject; };
	TWeakObjectPtr<class UFairyObject> GetFairyObject() { return FairyObject; };

private:
	TWeakObjectPtr<class UFairyObject> FairyObject;

protected:
	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;
	virtual FChildren* GetChildren() override;
	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual void EnsureSizeCorrect();

	void UpdateVisibilityFlags();

protected:
	bool bVisible : 1;
	bool bInteractable : 1;
	bool bTouchable : 1;
	bool bOpaque : 1;

private:
	static FNoChildren NoChildrenInstance;
};