#pragma once

#include "UI/FairyObject.h"
#include "UI/ScrollPane.h"

#include "FairyComponent.generated.h"

class UGController;
class UTransition;
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
	// ~ child operation end

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	virtual int32 GetFirstChildInView() const;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UGController* GetControllerAt(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UGController* GetController(const FString& ControllerName) const;

	const TArray<UGController*>& GetControllers() const { return Controllers; }
	void AddController(UGController* Controller);
	void RemoveController(UGController* Controller);
	void ApplyController(UGController* Controller);
	void ApplyAllControllers();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UTransition* GetTransition(const FString& TransitionName) const;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UTransition* GetTransitionAt(int32 Index) const;

	const TArray<UTransition*>& GetTransitions() const { return Transitions; }

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
	UScrollPane* GetScrollPane() const { return ScrollPane; }

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
	void AdjustRadioGroupDepth(UFairyObject* Child, UGController* Controller);

	virtual void ConstructFromResource() override;
	void ConstructFromResource(TArray<UFairyObject*>* ObjectPool, int32 PoolIndex);

	bool bBuildingDisplayList;

protected:
	virtual void ConstructExtension(FByteBuffer* Buffer);
	virtual void OnConstruct();
	UFUNCTION(BlueprintImplementableEvent, Category = "FairyGUI",  meta = ( DisplayName = "OnConstruct"))
	void K2_OnConstruct();
	virtual void SetupAfterAdd(FByteBuffer* Buffer, int32 BeginPos) override;
	//virtual void HandleSizeChanged() override;
	virtual void HandleGrayedChanged() override;
	virtual void HandleControllerChanged(UGController* Controller) override;

	virtual void UpdateBounds();
	void SetBounds(float ax, float ay, float aw, float ah);

	void SetupOverflow(EOverflowType InOverflow);
	void SetupScroll(FByteBuffer* Buffer);

	UPROPERTY(Transient)
	TArray<UFairyObject*> Children;
	UPROPERTY(Transient)
	TArray<UGController*> Controllers;
	UPROPERTY(Transient)
	TArray<UTransition*> Transitions;
	UPROPERTY(Transient)
	UScrollPane* ScrollPane;

	// Slate Resource
	//TSharedPtr<SContainer> RootContainer;
	TSharedPtr<SContainer> Container;
	void MakeSlateWidget();

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

	void OnAddedToStageHandler(UEventContext* Context);
	void OnRemovedFromStageHandler(UEventContext* Context);

	int32 SortingChildCount;
	UGController* ApplyingController;

	FTimerHandle UpdateBoundsTimerHandle;
	FTimerHandle BuildDisplayListTimerHandle;

	friend class UScrollPane;
};