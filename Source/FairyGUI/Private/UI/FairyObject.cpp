#include "UI/FairyObject.h"
#include "Package/FairyPackage.h"
#include "UI/FairyListView.h"
#include "UI/GGroup.h"
#include "UI/FairyRoot.h"
#include "UI/Controller/Gears/GearDisplay.h"
#include "UI/Controller/Gears/GearDisplay2.h"
#include "Tween/TweenManager.h"
#include "Widgets/SDisplayObject.h"
#include "Utils/ByteBuffer.h"
#include "FairyApplication.h"

TWeakObjectPtr<UFairyObject> UFairyObject::DraggingObject;
FVector2D UFairyObject::GlobalDragStart;
FBox2D UFairyObject::GlobalRect;
bool UFairyObject::bUpdateInDragging = false;

UFairyObject::UFairyObject() :
	Size(ForceInit),
	MinSize(ForceInit),
	MaxSize(ForceInit),
	Anchor(ForceInit),
	RenderTransformPivot(ForceInit),
	bPivotAsAnchor(false),
	Alpha(1.0f),
	bVisible(true),
	WidgetSlot(nullptr),
	bInternalVisible(true),
	SortingOrder(0)
{
	static int32 _gInstanceCounter = 1;
	ID.AppendInt(_gInstanceCounter);

	for (int32 i = 0; i < 10; i++)
	{
		Gears[i] = nullptr;
	}
}

UFairyObject::~UFairyObject()
{
	WidgetSlot = nullptr;

	for (int32 i = 0; i < 10; i++)
	{
		if (Gears[i])
		{
			delete Gears[i];
			Gears[i] = nullptr;
		}
	}
}

void UFairyObject::ReleaseSlateResources(bool bReleaseChildren)
{
	if (DisplayObject.IsValid())
	{
		DisplayObject.Reset();
	}

	Super::ReleaseSlateResources(bReleaseChildren);
}

void UFairyObject::BeginDestroy()
{

	if (Group.IsValid())
	{
		Group.Reset();
	}
	Super::BeginDestroy();
}

void UFairyObject::SetPosition(const FVector2D& InPosition)
{
	Position = InPosition;
	if (WidgetSlot)
	{
		WidgetSlot->Position(InPosition);
	}
}

void UFairyObject::SetPositionX(const float InPositionX)
{
	FVector2D NewPos = FVector2D(InPositionX, Position.Y);
	SetPosition(NewPos);
}

void UFairyObject::SetPositionY(const float InPositionY)
{
	FVector2D NewPos = FVector2D(Position.X, InPositionY);
	SetPosition(NewPos);
}

void UFairyObject::SetSize(const FVector2D& InSize)
{
	Size = InSize;

	if (WidgetSlot)
	{
		WidgetSlot->Size(InSize);
	}
}

void UFairyObject::SetWidth(float InSizeWidth)
{
	FVector2D NewSize(InSizeWidth, Size.Y);
	SetSize(NewSize);
}

void UFairyObject::SetHeight(float InSizeHeight)
{
	FVector2D NewSize(Size.X, InSizeHeight);
	SetSize(NewSize);
}

float UFairyObject::GetWidth()
{
	return Size.X;
}

float UFairyObject::GetHeight()
{
	return Size.Y;
}

void UFairyObject::SetPivot(const FVector2D& InPivot, bool bAsAnchor)
{
	if (RenderTransformPivot != InPivot || bPivotAsAnchor != bAsAnchor)
	{
		RenderTransformPivot = InPivot;
		bPivotAsAnchor = bAsAnchor;
		if (bAsAnchor)
		{
			Anchor = InPivot;
		}

		if (DisplayObject.IsValid())
		{
			DisplayObject->SetRenderTransformPivot(RenderTransformPivot);
		}
	}
}

void UFairyObject::SetScale(const FVector2D& InScale)
{
	RenderTransform.Scale = InScale;
	UpdateRenderTransform();
}

void UFairyObject::SetScaleX(float InScaleX)
{
	RenderTransform.Scale.X = InScaleX;
	UpdateRenderTransform();
}

void UFairyObject::SetScaleY(float InScaleY)
{
	RenderTransform.Scale.Y = InScaleY;
	UpdateRenderTransform();
}

float UFairyObject::GetScaleX()
{
	return RenderTransform.Scale.X;
}

float UFairyObject::GetScaleY()
{
	return RenderTransform.Scale.Y;
}

FSlateRect UFairyObject::GetBounds()
{
	float Left = Position.X - Size.X * Anchor.X * GetScaleX();
	float Top = Position.Y - Size.Y * Anchor.Y * GetScaleY();
	float Right = Position.X + Size.X * (1 - Anchor.X) * GetScaleX();
	float Bottom = Position.Y + Size.Y * (1 - Anchor.Y) * GetScaleY();

	return FSlateRect(Left, Top, Right, Bottom);
}

void UFairyObject::SetSkew(const FVector2D& InSkew)
{
	RenderTransform.Shear = -InSkew;
	UpdateRenderTransform();
}

const FVector2D& UFairyObject::GetSkew() const
{
	return RenderTransform.Shear;
}

void UFairyObject::SetRotation(float InRotation)
{
	RenderTransform.Angle = InRotation;
	UpdateRenderTransform();
}

void UFairyObject::UpdateRenderTransform()
{
	if (DisplayObject.IsValid())
	{
		if (RenderTransform.IsIdentity())
		{
			DisplayObject->SetRenderTransform(TOptional<FSlateRenderTransform>());
		}
		else
		{
			DisplayObject->SetRenderTransform(RenderTransform.ToSlateRenderTransform());
		}

		DisplayObject->SetRenderTransformPivot(RenderTransformPivot);
	}
}

const FVector2D& UFairyObject::GetRelationSize() const
{
	if (Relations.IsSet() && !Relations->IsEmpty())
	{
		const FRelations& RelationsObj = Relations.GetValue();
		RelationsObj.ApplyRelation();
	}

	return GetSize();
}

const FVector2D& UFairyObject::GetRelationPos() const
{
	// todo: calculate final relation position
	if (Relations.IsSet() && !Relations->IsEmpty())
	{
		const FRelations& RelationsObj = Relations.GetValue();
		RelationsObj.ApplyRelation();
	}

	return GetPosition();
}

void UFairyObject::SetAlpha(float InAlpha)
{
	if (Alpha != InAlpha)
	{
		Alpha = InAlpha;
		HandleAlphaChanged();
		UpdateGear(EFairyGearType::Look);
	}
}

void UFairyObject::SetGrayed(bool InBGrayed)
{
	if (bGrayed != InBGrayed)
	{
		bGrayed = InBGrayed;
		HandleGrayedChanged();
		UpdateGear(EFairyGearType::Look);
	}
}

void UFairyObject::SetVisible(bool bInVisible)
{
	if (bVisible != bInVisible)
	{
		bVisible = bInVisible;
		HandleVisibleChanged();
		if (Parent)
		{
			Parent->SetBoundsChangedFlag();
		}

		if (Group.IsValid() && Group->IsExcludeInvisibles())
		{
			Group->SetBoundsChangedFlag();
		}
	}
}

bool UFairyObject::InternalVisible() const
{
	return bInternalVisible && (!Group.IsValid() || Group->InternalVisible());
}

bool UFairyObject::InternalVisible2() const
{
	return bVisible && (!Group.IsValid() || Group->InternalVisible2());
}

bool UFairyObject::InternalVisible3() const
{
	return bVisible && bInternalVisible;
}

bool UFairyObject::IsTouchable() const
{
	return DisplayObject->IsTouchable();
}

void UFairyObject::SetTouchable(bool bInTouchable)
{
	DisplayObject->SetTouchable(bInTouchable);
}

void UFairyObject::SetSortingOrder(int32 InSortingOrder)
{
	if (InSortingOrder < 0)
		InSortingOrder = 0;
	if (SortingOrder != InSortingOrder)
	{
		int32 old = SortingOrder;
		SortingOrder = InSortingOrder;
		if (Parent)
		{
			Parent->ChildSortingOrderChanged(this, old, SortingOrder);
		}
	}
}

void UFairyObject::SetGroup(UGGroup* InGroup)
{
	if (Group.Get() != InGroup)
	{
		if (Group.IsValid())
		{
			Group->SetBoundsChangedFlag();
		}

		Group = InGroup;

		if (Group.IsValid())
		{
			Group->SetBoundsChangedFlag();
		}

		HandleVisibleChanged();

		if (Parent)
		{
			Parent->ChildStateChanged(this);
		}
	}
}

const FString& UFairyObject::GetText() const
{
	return G_EMPTY_STRING;
}

void UFairyObject::SetText(const FString& InText)
{
}

const FString& UFairyObject::GetIcon() const
{
	return G_EMPTY_STRING;
}

void UFairyObject::SetIcon(const FString& InIcon)
{
}

void UFairyObject::SetTooltips(const FString& InTooltips)
{
	Tooltips = InTooltips;
	if (!Tooltips.IsEmpty())
	{
		On(FFairyEventNames::RollOver).AddUObject(this, &UFairyObject::OnRollOverHandler);
		On(FFairyEventNames::RollOut).AddUObject(this, &UFairyObject::OnRollOutHandler);
	}
}

void UFairyObject::OnRollOverHandler(UEventContext* Context)
{
	UFairyApplication::Get()->GetUIRoot(this)->ShowTooltips(Tooltips);
}

void UFairyObject::OnRollOutHandler(UEventContext* Context)
{
	UFairyApplication::Get()->GetUIRoot(this)->HideTooltips();
}

void UFairyObject::SetDraggable(bool bInDraggable)
{
	if (bDraggable != bInDraggable)
	{
		bDraggable = bInDraggable;
		InitDrag();
	}
}

FBox2D UFairyObject::GetDragBounds() const
{
	if (DragBounds.IsSet())
	{
		return DragBounds.GetValue();
	}
	else
	{
		return FBox2D(FVector2D::ZeroVector, FVector2D::ZeroVector);
	}
}

void UFairyObject::SetDragBounds(const FBox2D& InBounds)
{
	if (InBounds.Min == InBounds.Max && InBounds.Min == FVector2D::ZeroVector)
	{
		DragBounds.Reset();
	}
	else
	{
		DragBounds = InBounds;
	}
}

void UFairyObject::StartDrag(int32 UserIndex, int32 PointerIndex)
{
	DragBegin(UserIndex, PointerIndex);
}

void UFairyObject::StopDrag()
{
	DragEnd();
}

FName UFairyObject::GetResourceURL() const
{
	if (PackageItem.IsValid())
	{
		FString TempUrl = FString(TEXT("ui://")) + PackageItem->OwnerPackage->GetID().ToString() + PackageItem->ID.ToString();
		return FName(TempUrl);
	}
	else
	{
		return NAME_None;
	}
}

FName UFairyObject::GetResourceName() const
{
	if (PackageItem.IsValid())
	{
		return PackageItem->Name;
	}
	else
	{
		return NAME_None;
	}
}

FName UFairyObject::GetPackageName() const
{
	if (PackageItem.IsValid())
	{
		return PackageItem->OwnerPackage->GetName();
	}
	else
	{
		return NAME_None;
	}
}

FVector2D UFairyObject::LocalToGlobal(const FVector2D& InPoint)
{
	FVector2D Point = InPoint;
	if (bPivotAsAnchor)
	{
		Point += Size * RenderTransformPivot;
	}

	return DisplayObject->GetCachedGeometry().LocalToAbsolute(Point);
}

FBox2D UFairyObject::LocalToGlobalRect(const FBox2D& InRect)
{
	FVector2D v0 = LocalToGlobal(InRect.Min);
	FVector2D v1 = LocalToGlobal(InRect.Max);
	return FBox2D(v0, v1);
}

FVector2D UFairyObject::LocalToRoot(const FVector2D& InPoint)
{
	return UFairyApplication::Get()->GetUIRoot(this)->GlobalToLocal(LocalToGlobal(InPoint));
}

FBox2D UFairyObject::LocalToRootRect(const FBox2D& InRect)
{
	return UFairyApplication::Get()->GetUIRoot(this)->GlobalToLocalRect(LocalToGlobalRect(InRect));
}

FVector2D UFairyObject::GlobalToLocal(const FVector2D& InPoint)
{
	FVector2D Point = DisplayObject->GetCachedGeometry().AbsoluteToLocal(InPoint);
	if (bPivotAsAnchor)
	{
		Point -= Size * RenderTransformPivot;
	}
	return Point;
}

FBox2D UFairyObject::GlobalToLocalRect(const FBox2D& InRect)
{
	FVector2D v0 = GlobalToLocal(InRect.Min);
	FVector2D v1 = GlobalToLocal(InRect.Max);
	return FBox2D(v0, v1);
}

FVector2D UFairyObject::RootToLocal(const FVector2D& InPoint)
{
	return GlobalToLocal(UFairyApplication::Get()->GetUIRoot(this)->LocalToGlobal(InPoint));
}

FBox2D UFairyObject::RootToLocalRect(const FBox2D& InRect)
{
	return GlobalToLocalRect(UFairyApplication::Get()->GetUIRoot(this)->LocalToGlobalRect(InRect));
}

FRelations& UFairyObject::GetRelations()
{
	if (!Relations.IsSet())
	{
		Relations = FRelations(this);
	}

	return Relations.GetValue();
}

void UFairyObject::AddRelation(UFairyObject* Obj, ERelationType RelationType, bool bUsePercent)
{
	GetRelations().Add(Obj, RelationType, bUsePercent);
}

void UFairyObject::RemoveRelation(UFairyObject* Obj, ERelationType RelationType)
{
	GetRelations().Remove(Obj, RelationType);
}

// ********************* Controller start *******************

void UFairyObject::UpdateGear(int32 Index)
{
	//if (bUnderConstruct || bGearLocked)
	//{
	//	return;
	//}

	//FGearBase* gear = Gears[Index];
	//if (gear != nullptr && gear->GetController() != nullptr)
	//{
	//	gear->UpdateState();
	//}
}

bool UFairyObject::CheckGearController(int32 Index, UFairyController* Controller)
{
	return Gears[Index] != nullptr && Gears[Index]->GetController() == Controller;
}

bool UFairyObject::IsGearVisible()
{
	bool bGearVisible = false;

	FGearDisplay* DisplayGear = (FGearDisplay*)Gears[EFairyGearType::Display];
	FGearDisplay2* Display2Gear = (FGearDisplay2*)Gears[EFairyGearType::Display2];

	// no display control option
	if (DisplayGear == nullptr)
	{
		bGearVisible = true;
	}
	else
	{
		bGearVisible = DisplayGear->IsConnected(); // get DisplayGear1 option value
		if (Display2Gear != nullptr)
		{
			bGearVisible = Display2Gear->Evaluate(bGearVisible); // evaluate with DisplayGear2 option value
		}
	}
	return bGearVisible;
}

void UFairyObject::UpdateGearFromRelations(int32 Index, const FVector2D& Delta)
{
	if (Gears[Index] != nullptr)
	{
		Gears[Index]->UpdateFromRelations(Delta);
	}
}

uint32 UFairyObject::AddDisplayLock()
{
	FGearDisplay* gearDisplay = (FGearDisplay*)Gears[EFairyGearType::Display];
	if (gearDisplay != nullptr && gearDisplay->GetController() != nullptr)
	{
		uint32 ret = gearDisplay->AddLock();
		CheckGearDisplay();

		return ret;
	}
	else
	{
		return 0;
	}
}

void UFairyObject::ReleaseDisplayLock(uint32 Token)
{
	FGearDisplay* gearDisplay = (FGearDisplay*)Gears[EFairyGearType::Display];
	if (gearDisplay != nullptr && gearDisplay->GetController() != nullptr)
	{
		gearDisplay->ReleaseLock(Token);
		CheckGearDisplay();
	}
}

void UFairyObject::CheckGearDisplay()
{
	if (bHandlingController)
	{
		return;
	}

	bool bGearVisible = IsGearVisible();
	if (bInternalVisible != bGearVisible)
	{
		bInternalVisible = bGearVisible;
		if (Parent)
		{
			Parent->ChildStateChanged(this);
		}

		if (Group.IsValid() && Group->IsExcludeInvisibles())
		{
			Group->SetBoundsChangedFlag();
		}
	}
}

void UFairyObject::ApplyController(UFairyController* Controller)
{
	bHandlingController = true;
	for (int32 i = 0; i < 10; i++)
	{
		FGearBase* gear = Gears[i];
		if (gear != nullptr && gear->GetController() == Controller)
		{
			gear->Apply();
		}
	}
	bHandlingController = false;

	CheckGearDisplay();
}
// ********************* Controller end *******************

void UFairyObject::RunTween(UFairyTweener* InAction)
{
	bool bOnStage = OnStage();
	UTweenManager::Get()->AddTweener(InAction, this, !bOnStage);
}

UFairyTweener* UFairyObject::GetTweenerByTag(int InTag)
{
	return UTweenManager::Get()->GetTweenerByTag(InTag, this);
}

void UFairyObject::StopTween(UFairyTweener* InAction)
{
	UTweenManager::Get()->RemoveTweener(InAction);
}

void UFairyObject::StopTweenByTag(int InTag)
{
	UTweenManager::Get()->RemoveTweenerByTag(InTag, this);
}

void UFairyObject::StopAllTweens()
{
	UTweenManager::Get()->RemoveAllTweenerWithTarget(this);
}

void UFairyObject::RemoveFromParent()
{
	if (Parent)
	{
		Parent->RemoveChild(this);
	}
}

UFairyObject* UFairyObject::CastTo(TSubclassOf<UFairyObject> ClassType) const
{
	return const_cast<UFairyObject*>(this);
}

FNVariant UFairyObject::GetProp(EObjectPropID PropID) const
{
	switch (PropID)
	{
	case EObjectPropID::Text:
		return FNVariant(GetText());
	case EObjectPropID::Icon:
		return FNVariant(GetIcon());
	default:
		return FNVariant(0);
	}
}

void UFairyObject::SetProp(EObjectPropID PropID, const FNVariant& InValue)
{
	switch (PropID)
	{
	case EObjectPropID::Text:
		return SetText(InValue.AsString());
	case EObjectPropID::Icon:
		return SetIcon(InValue.AsString());
	default:
		break;
	}
}

bool UFairyObject::DispatchEvent(const FName& EventType, const FNVariant& Data)
{
	return UFairyApplication::Get()->DispatchEvent(EventType, DisplayObject.ToSharedRef(), Data);
}

bool UFairyObject::HasEventListener(const FName& EventType) const
{
	const FUnifiedEventDelegate& Delegate = const_cast<UFairyObject*>(this)->GetEventDelegate(EventType);
	return Delegate.Func.IsBound() || ( Delegate.DynFunc != nullptr && Delegate.DynFunc->IsBound() );
}

void UFairyObject::InvokeEventDelegate(UEventContext* Context)
{
	FUnifiedEventDelegate& Delegate = GetEventDelegate(Context->GetType());
	Delegate.Func.Broadcast(Context);
	if (Delegate.DynFunc != nullptr)
	{
		Delegate.DynFunc->Broadcast(Context);
	}
}

UFairyObject::FUnifiedEventDelegate& UFairyObject::GetEventDelegate(const FName& EventType)
{
	FUnifiedEventDelegate* Delegate = EventDelegates.Find(EventType);
	if (Delegate == nullptr)
	{
		Delegate = &EventDelegates.Add(EventType);

		FProperty* Property = GetClass()->FindPropertyByName(FName(*FString("On").Append(EventType.ToString())));
		if (Property != nullptr)
		{
			Delegate->DynFunc = Property->ContainerPtrToValuePtr<FGUIEventDynMDelegate>(this);
		}
		else
		{
			Delegate->DynFunc = nullptr;
		}
	}

	return *Delegate;
}

FGUIEventMDelegate& UFairyObject::On(const FName& EventType)
{
	return GetEventDelegate(EventType).Func;
}

void UFairyObject::HandleAlphaChanged()
{
	DisplayObject->SetRenderOpacity(Alpha);
}

void UFairyObject::HandleGrayedChanged()
{
	DisplayObject->SetEnabled(!bGrayed);
}

void UFairyObject::HandleVisibleChanged()
{
	DisplayObject->SetVisible(InternalVisible2());
}

/**
* setup base property for this FairyObject
*/
void UFairyObject::SetupBeforeAdd(FairyGUI::FByteBuffer* Buffer, int32 BeginPos)
{
	Buffer->Seek(BeginPos, 0);
	Buffer->Skip(5);

	this->ID = Buffer->ReadStringFromCache();
	this->Name = Buffer->ReadStringFromCache();

	// position attribute, delay setting as we need know the anchor attribute
	float PosX = Buffer->ReadInt();
	float PosY = Buffer->ReadInt();
	SetPosition(FVector2D(PosX, PosY));

	if (Buffer->ReadBool())
	{
		this->Size.X = Buffer->ReadInt();
		this->Size.Y = Buffer->ReadInt();
	}

	if (Buffer->ReadBool())
	{
		this->MinSize.X = Buffer->ReadInt();
		this->MaxSize.X = Buffer->ReadInt();
		this->MinSize.Y = Buffer->ReadInt();
		this->MaxSize.Y = Buffer->ReadInt();
	}

	if (Buffer->ReadBool())
	{
		float ScaleX = Buffer->ReadFloat();
		float ScaleY = Buffer->ReadFloat();
		RenderTransform.Scale = FVector2D(ScaleX, ScaleY);
	}

	if (Buffer->ReadBool())
	{
		float SkewX = Buffer->ReadFloat();
		float SkewY = Buffer->ReadFloat();
		RenderTransform.Shear = -FVector2D(SkewX, SkewY);
	}

	if (Buffer->ReadBool())
	{
		float PivotX = Buffer->ReadFloat();
		float PivotY = Buffer->ReadFloat();
		this->RenderTransformPivot = FVector2D(PivotX, PivotY);
		this->bPivotAsAnchor = Buffer->ReadBool();

		if (this->bPivotAsAnchor)
		{
			this->Anchor = this->RenderTransformPivot;
		}
	}

	float AlphaValue = Buffer->ReadFloat();
	if (AlphaValue != 1)
	{
		SetAlpha(AlphaValue);
	}
		

	float RotationValue = Buffer->ReadFloat();
	if (RotationValue != 0)
	{
		RenderTransform.Angle = RotationValue;
	}

	if (!Buffer->ReadBool())
	{
		SetVisible(false);
	}

	if (!Buffer->ReadBool())
	{
		SetTouchable(false);
	}
		
	if (Buffer->ReadBool())
	{
		SetGrayed(true);
	}
		
	Buffer->ReadByte(); //blendMode
	Buffer->ReadByte(); //filter

	const FString& str = Buffer->ReadStringFromCache();
	if (!str.IsEmpty())
	{
		UserData = str;
	}
}

void UFairyObject::SetupAfterAdd(FairyGUI::FByteBuffer* Buffer, int32 BeginPos)
{
	Buffer->Seek(BeginPos, 1);

	const FString& str = Buffer->ReadStringFromCache();
	if (!str.IsEmpty())
	{
		SetTooltips(str);
	}

	int16 groupId = Buffer->ReadShort();
	if (groupId >= 0)
	{
		Group = Cast<UGGroup>(Parent->GetChildAt(groupId));
	}

	Buffer->Seek(BeginPos, 2);

	// Parse the controller property
	int16 cnt = Buffer->ReadShort(); // gear number
	for (int32 i = 0; i < cnt; i++)
	{
		int16 nextPos = Buffer->ReadShort();
		nextPos += Buffer->GetPos();

		EFairyGearType GearType = (EFairyGearType)Buffer->ReadByte();
		FGearBase* gear = FGearBase::Create(this, GearType);
		gear->Setup(Buffer);
		gear->GetController()->AddAbserver(MakeShareable<FGearBase>(gear));

		Buffer->SetPos(nextPos);
	}

	UpdateRenderTransform();
}
