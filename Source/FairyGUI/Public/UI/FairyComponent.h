#pragma once

#include "UI/FairyObject.h"
#include "UI/ScrollPanel.h"

#include "FairyComponent.generated.h"

class UFairyController;
class UFairyTransition;
class SContainer;

UCLASS(BlueprintType, Blueprintable)
class FAIRYGUI_API UFairyComponent : public UFairyObject
{
	GENERATED_BODY()

public:
	UFairyComponent();
	virtual ~UFairyComponent();

	// ~ UGVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	// ~ child operation start
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyObject* AddChild(UFairyObject* Child);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	virtual UFairyObject* AddChildAt(UFairyObject* Child, int32 Index);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void RemoveChild(UFairyObject* Child);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	virtual void RemoveChildAt(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void RemoveChildren(int32 BeginIndex = 0, int32 EndIndex = -1);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (DeterminesOutputType = "ClassType"))
	UFairyObject* GetChildAt(int32 Index, TSubclassOf<UFairyObject> ClassType = nullptr) const;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (DeterminesOutputType = "ClassType"))
	UFairyObject* GetChild(const FString& ChildName, TSubclassOf<UFairyObject> ClassType = nullptr) const;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (DeterminesOutputType = "ClassType"))
	UFairyObject* GetChildByPath(const FString& Path, TSubclassOf<UFairyObject> ClassType = nullptr) const;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (DeterminesOutputType = "ClassType"))
	UFairyObject* GetChildInGroup(const UGGroup* Group, const FString& ChildName, TSubclassOf<UFairyObject> ClassType = nullptr) const;

	UFairyObject* GetChildByID(const FString& ChildID) const;
	const TArray<UFairyObject*>& GetChildren() const { return Children; }

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	int32 GetChildIndex(const UFairyObject* Child) const;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetChildIndex(UFairyObject* Child, int32 Index);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	int32 SetChildIndexBefore(UFairyObject* Child, int32 Index);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SwapChildren(UFairyObject* Child1, UFairyObject* Child2);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SwapChildrenAt(int32 Index1, int32 Index2);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	int32 NumChildren() const;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	bool IsAncestorOf(const UFairyObject* Obj) const;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	virtual bool IsChildInView(UFairyObject* Child) const;
	
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	virtual int32 GetFirstChildInView() const;
	// ~ child operation end

	// *********** Controller start ***********
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyController* GetControllerAt(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyController* GetController(const FName& ControllerName) const;

	const TArray<UFairyController*>& GetControllers() const;

	void AddController(UFairyController* Controller);

	void RemoveController(UFairyController* Controller);

	virtual void ApplyController(UFairyController* Controller) override;

	void ApplyAllControllers();
	// *********** Controller end ***********

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	bool IsOpaque() const;
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetOpaque(bool bInOpaque);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	const FMargin& GetMargin() { return Margin; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetMargin(const FMargin& InMargin);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	EChildrenRenderOrder GetChildrenRenderOrder() const { return ChildrenRenderOrder; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetChildrenRenderOrder(EChildrenRenderOrder InChildrenRenderOrder);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	int32 GetApexIndex() const { return ApexIndex; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetApexIndex(int32 InApedIndex);

	//UFairyObject* getMask() const;
	//void setMask(UFairyObject* value, bool inverted = false);

	//IHitTest* getHitArea() const { return _hitArea; }
	//void setHitArea(IHitTest* value);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UScrollPanel* GetScrollPane() const { return ScrollPanel; }

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	float GetViewWidth() const;
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetViewWidth(float InViewWidth);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	float GetViewHeight() const;
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetViewHeight(float InViewHeight);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetBoundsChangedFlag();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void EnsureBoundsCorrect();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void OnClickChild(const FString& ChildName, const FGUIEventDynDelegate& Delegate)
	{
		GetChild(ChildName)->OnClick.AddUnique(Delegate);
	}

	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|Event")
	FGUIEventDynMDelegate OnDrop;

	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|Event")
	FGUIEventDynMDelegate OnScroll;
   
	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|Event")
	FGUIEventDynMDelegate OnScrollEnd;

	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|Event")
	FGUIEventDynMDelegate OnPullUpRelease;

	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|Event")
	FGUIEventDynMDelegate OnPullDownRelease;

	virtual FVector2D GetSnappingPosition(const FVector2D& InPoint);

	//internal use
	void ChildSortingOrderChanged(UFairyObject* Child, int32 OldValue, int32 NewValue);
	void ChildStateChanged(UFairyObject* Child);
	void AdjustRadioGroupDepth(UFairyObject* Child, UFairyController* Controller);

	virtual void ConstructFromResource() override;
	void ConstructFromResource(TArray<UFairyObject*>* ObjectPool, int32 PoolIndex);

protected:
	// Scroll view property
	TSharedPtr<SContainer> Container;
	TSharedPtr<SContainer> MaskWidget;
	TSharedPtr<SContainer> ScrollWidget;
	SContainer::FSlot* ScrollWidgetSlot;
	SContainer::FSlot* MaskWidgetSlot;
public:
	virtual FSlateRect GetBounds() override;
	bool bBuildingDisplayList;
	virtual void MakeSlateWidget() override;
	virtual TSharedPtr<SContainer> GetRootContainerWidget();
	virtual TSharedPtr<SContainer> GetContentContainerWidget();
	virtual TSharedPtr<SContainer> GetMaskContainerWidget();

	virtual void SetupOverflow(EOverflowType InOverflow);

	// ************** ScrollView API Start *******************
	virtual void SetupScroll(FairyGUI::FByteBuffer* Buffer);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FVector2D GetScrollMaskSizeForBind();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FVector2D GetScrollContentSizeForBind();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FVector2D GetScrollContentPositionForBind();

	virtual FVector2D GetScrollMaskSize();
	virtual FVector2D GetScrollContentSize();
	virtual void SetScrollContentSize(FVector2D InSize);
	virtual FVector2D GetScrollContentPosition();
	virtual void SetScrollContentPosition(FVector2D InPosition);

	[[deprecated("Will removed; Use GetScrollContentSize and GetSize instead.")]]
	virtual FVector2D GetContentContainerSize() { return FVector2D::ZeroVector; };
	[[deprecated("Will removed; Use SetScrollContentSize and SetSize instead.")]]
	virtual void SetContentContainerSize(FVector2D InSize) {};
	[[deprecated("Will removed; Use GetScrollContentPosition and GetPosition instead.")]]
	virtual FVector2D GetContentContainerPosition() { return FVector2D::ZeroVector; };
	[[deprecated("Will removed; Use SetScrollContentPosition and SetPosition instead.")]]
	virtual void SetContentContainerPosition(FVector2D InPosition) {};
	// ************** ScrollView API End *******************
protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "FairyGUI", meta = (DisplayName = "OnConstruct"))
	void K2_OnConstruct();

protected:
	virtual void ConstructExtension(FairyGUI::FByteBuffer* Buffer);
	virtual void OnConstruct();
	virtual void SetupAfterAdd(FairyGUI::FByteBuffer* Buffer, int32 BeginPos) override;

	//virtual void HandleSizeChanged() override;
	virtual void HandleGrayedChanged() override;

	virtual void UpdateBounds();
	virtual void SetBounds(float ax, float ay, float aw, float ah);

	UPROPERTY(Transient)
	TArray<UFairyObject*> Children; // Child FairyObject
	
	UPROPERTY(Transient)
	UScrollPanel* ScrollPanel;

	// Component attribute
	FMargin Margin;
	FVector2D AlignOffset;
	EChildrenRenderOrder ChildrenRenderOrder;
	int32 ApexIndex;
	uint8 bBoundsChanged : 1;
	uint8 bTrackBounds : 1;

private:
	int32 GetInsertPosForSortingChild(UFairyObject* Child);
	int32 MoveChild(UFairyObject* Child, int32 OldIndex, int32 NewIndex);

	void BuildNativeDisplayList(bool bImmediatelly = false);
	void AddWidget(UFairyObject* InChild, int32 index);

	void OnAddedToStageHandler(UEventContext* Context);
	void OnRemovedFromStageHandler(UEventContext* Context);

	int32 SortingChildCount;
	UFairyController* ApplyingController;

	FTimerHandle UpdateBoundsTimerHandle;
	FTimerHandle BuildDisplayListTimerHandle;

	friend class UScrollPanel;
};