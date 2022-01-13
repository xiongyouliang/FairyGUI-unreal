#pragma once

#include "Widgets/SDisplayObject.h"
#include "Widgets/SContainer.h"
#include "Slate/WidgetTransform.h"

#include "Package/FairyPackageItem.h"
#include "UI/FairyVisual.h"
#include "UI/Relation/Relations.h"
#include "UI/FairyConfig.h"
#include "UI/Controller/Gears/FairyGearType.h"
#include "FairyCommons.h"
#include "Event/EventContext.h"
#include "Utils/NVariant.h"
#include "Utils/ByteBuffer.h"


#include "FairyObject.generated.h"

class FByteBuffer;
class FRelations;

class UGGroup;
class UFairyComponent;
class UGTreeNode;
class UFairyRoot;

class UFairyController;

/**
* TODO:
* UFairyObject is base object as FairyGUI Object, I want to refactor it only handle some base attribute and some optional attribute:
* Base attributes: @Name, @Position, @Size, @MinSize @MaxSize, @bUseOriginSize, @bLockAspectRatio, @Scale, @Rotation, @Skew @Pivot, @bPivotAsAnchor @Opacity, @Visibility
* Optional attributes: @Touchable, @bGray, @AttributeController, @Relations, @Effect, @Misc(others)
* 
* All these attribute are abstraction come from FairyGUI Editor;
*/

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyObject : public UFairyVisual
{
	GENERATED_BODY()

public:
	UFairyObject();
	virtual ~UFairyObject();

public:
	// ~ UGVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	// ~ UObject Interface
	virtual void BeginDestroy() override;

	// position attribute
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	const FVector2D& GetPosition() const { return Position; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetPosition(const FVector2D& InPosition);
	void SetPositionX(const float InPositionX);
	void SetPositionY(const float InPositionY);

	// scale attribute
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	const FVector2D& GetScale() const { return RenderTransform.Scale; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetScale(const FVector2D& InScale);
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetScaleX(float InScale);
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetScaleY(float InScale);
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	float GetScaleX();
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	float GetScaleY();
	
	// shear or skew attribute
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	const FVector2D& GetSkew() const;
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetSkew(const FVector2D& InSkew);

	// rotate attribute
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	float GetRotation() const { return RenderTransform.Angle; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetRotation(float InRotation);
	
	// pivot/anchor attribute
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	const FVector2D& GetPivot() const { return RenderTransformPivot; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetPivot(const FVector2D& InPivot, bool bAsAnchor = false);
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	bool IsPivotAsAnchor() const { return bPivotAsAnchor; }
	const FVector2D& GetAnchor() const { return Anchor; }

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
	FColor GetColor() const { return NativeGetColor(); }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetColor(const FColor& InColor) { NativeSetColor(InColor); }
	virtual FColor NativeGetColor() const { return FColor(); };
	virtual void NativeSetColor(const FColor& IColor) {};

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
	FName GetResourceURL() const;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FName GetResourceName() const;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FName GetPackageName() const;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
	FNVariant UserData;

	bool bUnderConstruct = false;
	bool bGearLocked = false;

	static UFairyObject* GetDraggingObject() { return DraggingObject.Get(); }
public:
	//****************************
	// UFairyObject interface
	//****************************

	virtual void ConstructFromResource() {}; // todo: Unknow 
	virtual void MakeSlateWidget() {} // subclass use this method to create Slate Widget Object
	virtual void SetupBeforeAdd(FairyGUI::FByteBuffer* Buffer, int32 BeginPos);
	virtual void SetupAfterAdd(FairyGUI::FByteBuffer* Buffer, int32 BeginPos);

	inline void SetPackageItem(const TSharedPtr<FFairyPackageItem>& InPackageItem) { PackageItem = InPackageItem; }

	// Relations feature
	FRelations& GetRelations();
	void AddRelation(UFairyObject* Obj, ERelationType RelationType, bool bUsePercent = false);
	void RemoveRelation(UFairyObject* Obj, ERelationType RelationType);

	// ID and Name
	const FString& GetID() { return ID; }
	void SetID(const FString& InID) { ID = InID; }
	const FString& GetName() { return Name; }
	void SetName(const FString& InName) { Name = InName; }

	// Size/MinSize/MaxSize
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	const FVector2D& GetSize() const { return Size; }

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetSize(const FVector2D& InSize);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	float GetWidth();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	float GetHeight();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetWidth(float InSizeWidth);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetHeight(float InSizeHeight);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FVector2D& GetMinSize() { return MinSize; }

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetMinSize(FVector2D InMinSize) { MinSize = InMinSize; }

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FVector2D GetMaxSize() { return MaxSize; }

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetMaxSize(FVector2D InMaxSize) { MaxSize = InMaxSize; }

	void SetMinWidth(float InMinWidth) { MinSize.X = InMinWidth; }
	void SetMinHeight(float InMinHeight) { MinSize.Y = InMinHeight; }
	void SetMaxWidth(float InMaxWidth) { MaxSize.X = InMaxWidth; }
	void SetMaxHeight(float InMaxHeight) { MaxSize.Y = InMaxHeight; }
	float GetMinWidth() { return MinSize.X; };
	float GetMinHeight() { return MinSize.Y; };
	float GetMaxWidth() { return MaxSize.X; };
	float GetMaxHeight() { return MaxSize.Y; };

	/**
	* @Desc:GetBounds() all value is in parent local coordinate.
	*/
	virtual FSlateRect GetBounds();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	const FVector2D& GetRelationSize() const;
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	const FVector2D& GetRelationPos() const;

	// Parent
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	bool HasParent() { return Parent ? true : false; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyComponent* GetParent() { return Parent; }
	const UFairyComponent* GetParent() const { return Parent; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetParent(UFairyComponent* InParent) { Parent = InParent; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void RemoveFromParent();

	void SetSlot(SContainer::FSlot* InSlot) { WidgetSlot = InSlot; };

	// todo: remove
	bool InternalVisible() const;
	bool InternalVisible2() const;
	bool InternalVisible3() const;
	void UpdateGearFromRelations(int32 Index, const FVector2D& Delta);

	virtual void HandleGrayedChanged();
	virtual void HandleAlphaChanged();
	virtual void HandleVisibleChanged();

	void UpdateGear(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	virtual void RunTween(UFairyTweener* InAction);
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	virtual UFairyTweener* GetTweenerByTag(int InTag);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	virtual void StopTween(UFairyTweener* InAction);
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	virtual void StopTweenByTag(int InTag);
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	virtual void StopAllTweens();
	

protected:
	FString ID;
	FString Name;
	
	FVector2D Size;
	FVector2D MinSize;
	FVector2D MaxSize;
	FVector2D Anchor;
	FVector2D Position;

	FWidgetTransform RenderTransform; // Describes the standard transformation of a widget: Translation, Scale, Shear, Angle
	FVector2D RenderTransformPivot;
	bool bPivotAsAnchor;

	TOptional<FRelations> Relations;

	float Alpha = 0.0f;
	
	bool bVisible = false;
	bool bGrayed = false;

	class UFairyComponent* Parent; // parent component object
	TSharedPtr<SDisplayObject> DisplayObject; // the underlying SWidget
	SContainer::FSlot* WidgetSlot;
	TSharedPtr<FFairyPackageItem> PackageItem; // A Fairy Editor export data

	// *************** Controller property start ******************
public:
	bool CheckGearController(int32 Index, UFairyController* Controller);
	uint32 AddDisplayLock();
	void ReleaseDisplayLock(uint32 Token);

	/**
	 * Return FGearDisplay and FGearDisplay2 combine result:
	 * 
	 * @return boolean result, means GearDisplay effect; if no GearDisplay, return true;
	 */
	virtual void ApplyController(UFairyController* Controller);
	// *************** Controller property end ******************
private:
	void UpdateRenderTransform();

	// !!!DEPRECATED FUNCTION !!!
	UFUNCTION()
	void OnRollOverHandler(UEventContext* Context);
	UFUNCTION()
	void OnRollOutHandler(UEventContext* Context);

	// !!!DEPRECATED FUNCTION !!!
	void InitDrag() {};
	void DragBegin(int32 UserIndex, int32 PointerIndex) {};
	void DragEnd() {};

	UFUNCTION()
	void OnTouchBeginHandler(UEventContext* Context) {};
	UFUNCTION()
	void OnTouchMoveHandler(UEventContext* Context) {};
	UFUNCTION()
	void OnTouchEndHandler(UEventContext* Context) {};

	bool bInternalVisible = false;
	bool bDraggable = false;
	int32 SortingOrder = 0;
	FString Tooltips;
	TWeakObjectPtr<UGGroup> Group;
	float SizePercentInGroup = 0.0f;

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

	static TWeakObjectPtr<UFairyObject> DraggingObject;
	static FVector2D GlobalDragStart;
	static FBox2D GlobalRect;
	static bool bUpdateInDragging;
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