#pragma once

#include "Widgets/SDisplayObject.h"
#include "Relations.h"
#include "Package/FairyPackageItem.h"
#include "UIConfig.h"
#include "FairyCommons.h"
#include "Event/EventContext.h"
#include "Utils/NVariant.h"
#include "UI/FairyVisual.h"
#include "Slate/WidgetTransform.h"

#include "FairyObject.generated.h"

class FByteBuffer;
class FRelations;
class FGearBase;

class UGGroup;
class UFairyComponent;
class UGTreeNode;
class UGController;
class UFairyRoot;

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyObject : public UFairyVisual
{
	GENERATED_BODY()

public:
	UFairyObject();
	virtual ~UFairyObject();

	// ~ UGVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	// ~ UObject Interface
	virtual void BeginDestroy() override;

	virtual void ConstructFromResource();

	// position attribute
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	const FVector2D& GetPosition() const { return RenderTransform.Translation; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetPosition(const FVector2D& InPosition);

	float GetX() { return 0.0f; }
	float GetY() { return 0.0f; }
	void SetX(float) {};
	void SetY(float) {}

	float GetWidth() { return Size.X; }
	float GetHeight() { return Size.Y; }
	void SetWidth(float) {}
	void SetHeight(float) {}

	// scale attribute
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	const FVector2D& GetScale() const { return RenderTransform.Scale; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void UpdateScale(const FVector2D& InScale);
	void SetScale(const FVector2D& InScale){}
	void SetScaleX(float InScale) {}
	void SetScaleY(float InScale) {}
	float GetScaleX() { return 1.0f; }
	float GetScaleY() { return 1.0f; }
	

	// shear or skew attribute
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	const FVector2D& GetSkew() const { return RenderTransform.Shear; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void UpdateSkew(const FVector2D& InSkew);
	void SetSkew(const FVector2D& InSkew){}

	// rotate attribute
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	float GetRotation() const { return RenderTransform.Angle; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void UpdateRotation(float InRotation);
	void SetRotation(float InRotation) {};
	
	// Size/MinSize/MaxSize attribute
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	const FVector2D& GetSize() const { return Size; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void UpdateSize(const FVector2D& InSize, bool bIgnorePivot = false) { Size = InSize; }
	void SetSize(const FVector2D& InSize, bool bIgnorePivot = false) { UpdateSize(InSize, bIgnorePivot); }

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FVector2D& GetMinSize() { return MinSize; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetMinSize(FVector2D InMinSize) { MinSize = InMinSize; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FVector2D GetMaxSize() { return MaxSize; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetMaxSize(FVector2D InMaxSize) { MaxSize = InMaxSize; }
	// todo
	void SetXMin(float) {}
	void SetYMin(float) {}
	void SetXMax(float) {}
	void SetYMax(float) {}
	float GetXMin() { return 0.0f; };
	float GetYMin() { return 0.0f; };
	float GetXMax() { return 0.0f; };
	float GetYMax() { return 0.0f; };
	
	// pivot/anchor attribute
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	const FVector2D& GetPivot() const { return RenderTransformPivot; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void UpdatePivot(const FVector2D& InPivot, bool bAsAnchor = false);
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	bool IsPivotAsAnchor() const { return bPivotAsAnchor; }

	void SetPivot(const FVector2D& InPivot, bool bAsAnchor = false) {};

	// opacity/alpha attribute
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	float GetAlpha() const { return Alpha; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetAlpha(float InAlpha);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	bool IsGrayed() const { return bGrayed; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetGrayed(bool bInGrayed);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	bool IsVisible() const { return bVisible; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetVisible(bool bInVisible);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	bool IsTouchable() const;
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetTouchable(bool bInTouchable);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	int32 GetSortingOrder() const { return SortingOrder; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetSortingOrder(int32 InSortingOrder);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UGGroup* GetGroup() const { return Group.Get(); }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetGroup(UGGroup* InGroup);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	virtual const FString& GetText() const;
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	virtual void SetText(const FString& InText);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	virtual const FString& GetIcon() const;
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	virtual void SetIcon(const FString& InIcon);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	const FString& GetTooltips() const { return Tooltips; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetTooltips(const FString& InTooltips);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	bool IsDraggable() const { return bDraggable; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetDraggable(bool bInDraggable);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FBox2D GetDragBounds() const;
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetDragBounds(const FBox2D& InBounds);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void StartDrag(int32 UserIndex, int32 PointerIndex);
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void StopDrag();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FString GetResourceURL() const;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FString GetResourceName() const;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FString GetPackageName() const;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FVector2D GlobalToLocal(const FVector2D& InPoint);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FBox2D GlobalToLocalRect(const FBox2D& InRect);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FVector2D RootToLocal(const FVector2D& InPoint);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FBox2D RootToLocalRect(const FBox2D& InRect);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FVector2D LocalToGlobal(const FVector2D& InPoint);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FBox2D LocalToGlobalRect(const FBox2D& InRect);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FVector2D LocalToRoot(const FVector2D& InPoint);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FBox2D LocalToRootRect(const FBox2D& InRect);

	FRelations* GetRelations() { return Relations; }

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void AddRelation(UFairyObject* Obj, ERelationType RelationType, bool bUsePercent = false);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void RemoveRelation(UFairyObject* Obj, ERelationType RelationType);

	FGearBase* GetGear(int32 Index);
	bool CheckGearController(int32 Index, UGController* Controller);
	uint32 AddDisplayLock();
	void ReleaseDisplayLock(uint32 Token);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyComponent* GetParent() const { return Parent.Get(); }

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void RemoveFromParent();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	bool OnStage() const { return DisplayObject->OnStage(); }

	UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (DeterminesOutputType = "ClassType"))
	UFairyObject* CastTo(TSubclassOf<UFairyObject> ClassType) const;

	template <typename T> T* As() const;

	TSharedPtr<FFairyPackageItem> GetPackageItem() const { return PackageItem; }
	TSharedRef<SDisplayObject> GetDisplayObject() const { return DisplayObject.ToSharedRef(); }

	template <typename T> T GetProp(EObjectPropID PropID) const;
	virtual FNVariant GetProp(EObjectPropID PropID) const;
	virtual void SetProp(EObjectPropID PropID, const FNVariant& InValue);

	UGTreeNode* GetTreeNode() const { return TreeNode; }

public:
	bool DispatchEvent(const FName& EventType, const FNVariant& Data = FNVariant::Null);
	bool HasEventListener(const FName& EventType) const;
	void InvokeEventDelegate(UEventContext* Context);
	FGUIEventMDelegate& On(const FName& EventType);

	FSimpleMulticastDelegate& OnPositionChanged()
	{
		return OnPositionChangedEvent;
	}

	FSimpleMulticastDelegate& OnSizeChanged()
	{
		return OnSizeChangedEvent;
	}

	//UPROPERTY(BlueprintAssignable, Category = "FairyGUI|LifyCycle")
	//FSimpleDynDelegate OnConstructed;

	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|LifyCycle")
	FGUISimpleDynMDelegate OnEnter;

	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|LifyCycle")
	FGUISimpleDynMDelegate OnExit;

	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|Event")
	FGUIEventDynMDelegate OnClick;

	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|Event")
	FGUIEventDynMDelegate OnTouchBegin;

	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|Event")
	FGUIEventDynMDelegate OnTouchMove;

	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|Event")
	FGUIEventDynMDelegate OnTouchEnd;

	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|Event")
	FGUIEventDynMDelegate OnRollOver;

	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|Event")
	FGUIEventDynMDelegate OnRollOut;

	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|Event")
	FGUIEventDynMDelegate OnDragStart;

	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|Event")
	FGUIEventDynMDelegate OnDragMove;

	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|Event")
	FGUIEventDynMDelegate OnDragEnd;

	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|Event")
	FGUIEventDynMDelegate OnGearStop;

	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|Event")
	FGUIEventDynMDelegate OnAddedToStage;

	UPROPERTY(BlueprintAssignable, Category = "FairyGUI|Event")
	FGUIEventDynMDelegate OnRemoveFromStage;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "FairyGUI")
	FString ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
	FNVariant UserData;

	bool bUnderConstruct = false;
	bool bGearLocked = false;

	static UFairyObject* GetDraggingObject() { return DraggingObject.Get(); }

protected:
	TWeakObjectPtr<UFairyComponent> Parent;

	TSharedPtr<SDisplayObject> DisplayObject;

	TSharedPtr<FFairyPackageItem> PackageItem;

	virtual void HandleGrayedChanged();
	virtual void HandleControllerChanged(UGController* Controller);
	virtual void HandleAlphaChanged();
	virtual void HandleVisibleChanged();

	virtual void SetupBeforeAdd(FByteBuffer* Buffer, int32 BeginPos);
	virtual void SetupAfterAdd(FByteBuffer* Buffer, int32 BeginPos);

	void UpdateGear(int32 Index);
	void CheckGearDisplay();

	FVector2D Size;
	FVector2D MinSize;
	FVector2D MaxSize;

	FWidgetTransform RenderTransform;
	FVector2D RenderTransformPivot;

	FVector2D RawSize; // todo
	FVector2D LocalPosition; // local position
	FVector2D Pivot; // todo

	bool bPivotAsAnchor = false;
	float Alpha = 0.0f;
	
	bool bVisible = false;
	bool bGrayed = false;

private:
	bool InternalVisible() const;
	bool InternalVisible2() const;
	bool InternalVisible3() const;
	void UpdateGearFromRelations(int32 Index, const FVector2D& Delta);
	void UpdateRenderTransform();

	UFUNCTION()
	void OnRollOverHandler(UEventContext* Context);
	UFUNCTION()
	void OnRollOutHandler(UEventContext* Context);

	void InitDrag();
	void DragBegin(int32 UserIndex, int32 PointerIndex);
	void DragEnd();

	UFUNCTION()
	void OnTouchBeginHandler(UEventContext* Context);
	UFUNCTION()
	void OnTouchMoveHandler(UEventContext* Context);
	UFUNCTION()
	void OnTouchEndHandler(UEventContext* Context);

	bool bInternalVisible = false;
	bool bHandlingController = false;
	bool bDraggable = false;
	int32 SortingOrder = 0;
	FString Tooltips;
	TWeakObjectPtr<UGGroup> Group;
	float SizePercentInGroup = 0.0f;
	FRelations* Relations;
	FGearBase* Gears[10];
	FVector2D DragTouchStartPos;
	TOptional<FBox2D> DragBounds;
	bool bDragTesting = false;
	UGTreeNode* TreeNode;

	struct FUnifiedEventDelegate
	{
		FGUIEventMDelegate Func;
		FGUIEventDynMDelegate* DynFunc;
	};
	TMap<FName, FUnifiedEventDelegate> EventDelegates;
	FUnifiedEventDelegate& GetEventDelegate(const FName& EventType);

	FSimpleMulticastDelegate OnPositionChangedEvent;
	FSimpleMulticastDelegate OnSizeChangedEvent;

	static TWeakObjectPtr<UFairyObject> DraggingObject;
	static FVector2D GlobalDragStart;
	static FBox2D GlobalRect;
	static bool bUpdateInDragging;

	friend class UFairyComponent;
	friend class UGGroup;
	friend class FRelationItem;
	friend class FUIObjectFactory;
};

template <typename T>
inline T* UFairyObject::As() const
{
	return const_cast<T*>(Cast<T>(this));
}

template <typename T>
inline T UFairyObject::GetProp(EObjectPropID PropID) const
{
	return GetProp(PropID).As<T>();
}