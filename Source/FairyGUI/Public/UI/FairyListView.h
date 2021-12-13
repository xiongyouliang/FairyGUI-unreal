#pragma once

#include "UI/FairyComponent.h"
#include "FairyListView.generated.h"

class FFairyObjectPool;

DECLARE_DELEGATE_TwoParams(FListItemRenderer, int32, UFairyObject*);
DECLARE_DELEGATE_RetVal_OneParam(FString, FListItemProvider, int32);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FDynListItemRenderer, int32, Index, UFairyObject*, Obj);
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(FString, FDynListItemProvider, int32, Index);

struct FFairyListItemInfo
{
	FVector2D Size;
	UFairyObject* Obj;
	uint32 UpdateFlag;
	bool bSelected;

	FFairyListItemInfo();
};

UCLASS(BlueprintType, NotBlueprintable)
class FAIRYGUI_API UFairyListView : public UFairyComponent
{
	GENERATED_BODY()
public:
	UFairyListView();
	virtual ~UFairyListView();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	EListLayoutType GetLayout() const { return LayoutType; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetLayout(EListLayoutType InLayout);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	int32 GetLineCount() const { return RowNum; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetLineCount(int32 InLineCount);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	int32 GetColumnCount() { return ColNum; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetColumnCount(int32 InColumnCount);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	int32 GetColumnGap() const { return ColSpacing; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetColumnGap(int32 InColumnGap);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	int32 GetLineGap() const { return RowSpacing; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetLineGap(int32 InLineGap);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	EHAlignType GetHorizontalAlign() const { return HorizontalAlign; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetHorizontalAlign(EHAlignType InAlign);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	EVAlignType GetVerticalAlign() const { return VerticalAlign; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetVerticalAlign(EVAlignType InVerticalAlign);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	bool GetAutoResizeItem() const { return bAutoResizeItem; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetAutoResizeItem(bool bFlag);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	EListSelectionMode GetSelectionMode() const { return SelectionMode; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetSelectionMode(EListSelectionMode InMode) { SelectionMode = InMode; }

	FFairyObjectPool* GetItemPool() const { return Pool; }
	UFairyObject* GetFromPool();
	UFairyObject* GetFromPool(const FName& URL);
	void ReturnToPool(UFairyObject* Obj);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyObject* AddItemFromPool(const FName& URL = NAME_None);

	virtual UFairyObject* AddChildAt(UFairyObject* Child, int32 Index) override;
	virtual void RemoveChildAt(int32 Index) override;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void RemoveChildToPoolAt(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void RemoveChildToPool(UFairyObject* Child);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void RemoveChildrenToPool(int32 BeginIndex = 0, int32 EndIndex = -1);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	int32 GetSelectedIndex() const;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetSelectedIndex(int32 Index);

	void GetSelection(TArray<int32>& OutIndice) const;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void AddSelection(int32 Index, bool bScrollItToView);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void RemoveSelection(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void ClearSelection();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SelectAll();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SelectReverse();

	void HandleArrowKey(int32 Direction);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void ResizeToFit(int32 ItemCount, int32 InMinSize = 0);

	virtual int32 GetFirstChildInView() const override;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void ScrollToView(int32 Index, bool bAnimation = false, bool bSetFirst = false);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UGController* GetSelectionController() const { return SelectionController; }

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetSelectionController(UGController* InController);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetVirtual();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetVirtualAndLoop();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	bool IsVirtual() const { return bVirtual; }

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void RefreshVirtualList();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	int32 GetNumItems() const;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetNumItems(int32 InNumItems);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	int32 ChildIndexToItemIndex(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	int32 ItemIndexToChildIndex(int32 Index) const;

	virtual FVector2D GetSnappingPosition(const FVector2D& InPoint) override;

	void SetItemRenderer(const FListItemRenderer& InItemRenderer) { ItemRenderer = InItemRenderer; }
	void SetItemProvider(const FListItemProvider& InItemProvider) { ItemProvider = InItemProvider; }

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetItemRenderer(const FDynListItemRenderer& InItemRenderer);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetItemProvider(const FDynListItemProvider& InItemProvider);

	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|Event")
	FGUIEventDynMDelegate OnClickItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
	FName DefaultItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
	bool bScrollItemToViewOnClick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
	bool bFoldInvisibleItems;

	virtual void MakeSlateWidget() override;
	virtual TSharedPtr<SContainer> GetContentContainerWidget() override;
	virtual TSharedPtr<SContainer> GetMaskContainerWidget() override;
	virtual void SetupScroll(FByteBuffer* Buffer) override;

	virtual FVector2D GetScrollMaskSize() override;
	virtual FVector2D GetScrollContentSize() override;

	virtual FVector2D GetContentContainerSize() override;
	virtual void SetContentContainerSize(FVector2D InSize) override;

	virtual FVector2D GetContentContainerPosition() override;
	virtual void SetContentContainerPosition(FVector2D InPosition) override;

protected:
	virtual void ApplyController(UGController* Controller) override;
	//virtual void HandleSizeChanged() override;
	virtual void UpdateBounds() override;
	virtual void SetBounds(float ax, float ay, float aw, float ah) override;
	virtual void SetupBeforeAdd(FByteBuffer* Buffer, int32 BeginPos) override;
	virtual void SetupAfterAdd(FByteBuffer* Buffer, int32 BeginPos) override;

	virtual void DispatchItemEvent(UFairyObject* Obj, UEventContext* Context);
	virtual void ReadItems(FByteBuffer* Buffer);
	virtual void SetupItem(FByteBuffer* Buffer, UFairyObject* Obj);

	void RefreshItemsLayout();
	void RefreshItemsLayoutForSingleCol();
	void RefreshItemsLayoutForSingleRow();
	void RefreshItemsLayoutForHorizontalFlow();
	void RefreshItemsLayoutForVerticalFlow();
	void RefreshItemsLayoutForPagination();
	
private:
	void ClearSelectionExcept(UFairyObject* Obj);
	void SetSelectionOnEvent(UFairyObject* Obj, UEventContext* Context);

	UFUNCTION()
	void OnClickItemHandler(UEventContext* Context);

	void UpdateSelectionController(int32 Index);

	void SetVirtual(bool bLoop);
	void CheckVirtualList();
	void SetVirtualListChangedFlag(bool bLayoutChanged);
	void DoRefreshVirtualList();

	void OnScrollHandler(UEventContext* Context);

	int32 GetIndexOnPos1(float& pos, bool forceUpdate);
	int32 GetIndexOnPos2(float& pos, bool forceUpdate);
	int32 GetIndexOnPos3(float& pos, bool forceUpdate);

	void HandleScroll(bool forceUpdate);
	bool HandleScroll1(bool forceUpdate);
	bool HandleScroll2(bool forceUpdate);
	void HandleScroll3(bool forceUpdate);

	void HandleArchOrder1();
	void HandleArchOrder2();

	void HandleAlign(float InContentWidth, float InContentHeight); // todo: align for a list view is not necessary. maybe remove it;

	EFairyScrollDirection GetScrollDirection();

	EListLayoutType LayoutType;
	int32 RowNum;
	int32 ColNum;
	int32 RowSpacing;
	int32 ColSpacing;
	EHAlignType HorizontalAlign;
	EVAlignType VerticalAlign;
	bool bAutoResizeItem;
	EListSelectionMode SelectionMode;
	UGController* SelectionController;
	FListItemRenderer ItemRenderer;
	FListItemProvider ItemProvider;

	FFairyObjectPool* Pool;
	int32 LastSelectedIndex;

	FVector2D ScrollContentSize;

	//Virtual List support
	bool bVirtual;
	bool bLoop;
	int32 NumItems;
	int32 RealNumItems;
	int32 FirstIndex;        //the top left index
	int32 CurLineItemCount;  //item count in one line
	int32 CurLineItemCount2; //item count in vertical direction,only pagination layout
	FVector2D ItemSize;
	int32 VirtualListChanged; //1-content changed, 2-size changed
	bool bEventLocked;
	uint32 ItemInfoVer;
	FTimerHandle RefreshTimerHandle;

	TArray<FFairyListItemInfo> VirtualItems;
};