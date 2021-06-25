#include "UI/FairyObject.h"
#include "UI/GList.h"
#include "UI/GGroup.h"
#include "UI/GController.h"
#include "Package/FairyPackage.h"
#include "UI/FairyRoot.h"
#include "UI/Gears/GearBase.h"
#include "UI/Gears/GearDisplay.h"
#include "UI/Gears/GearDisplay2.h"
#include "Widgets/SDisplayObject.h"
#include "Utils/ByteBuffer.h"
#include "FairyApplication.h"

TWeakObjectPtr<UFairyObject> UFairyObject::DraggingObject;
FVector2D UFairyObject::GlobalDragStart;
FBox2D UFairyObject::GlobalRect;
bool UFairyObject::bUpdateInDragging = false;

UFairyObject::UFairyObject() :
	SourceSize(ForceInit),
	InitSize(ForceInit),
	MinSize(ForceInit),
	MaxSize(ForceInit),
	Position(ForceInit),
	Size(ForceInit),
	RawSize(ForceInit),
	Pivot(ForceInit),
	Scale{ 1, 1 },
	Skew(ForceInit),
	Alpha(1.0f),
	bVisible(true),
	bInternalVisible(true),
	SortingOrder(0)
{
	static int32 _gInstanceCounter = 1;
	ID.AppendInt(_gInstanceCounter);

	for (int32 i = 0; i < 10; i++)
	{
		Gears[i] = nullptr;
	}
	Relations = new FRelations(this);
}

UFairyObject::~UFairyObject()
{
	for (int32 i = 0; i < 10; i++)
	{
		if (Gears[i])
		{
			delete Gears[i];
			Gears[i] = nullptr;
		}
	}
		
	//delete Relations;
	if (Relations)
	{
		delete Relations;
		Relations = nullptr;
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
	if (Parent.IsValid())
	{
		Parent.Reset();
	}

	if (Group.IsValid())
	{
		Group.Reset();
	}
	Super::BeginDestroy();
}

void UFairyObject::SetX(float InX)
{
	SetPosition(FVector2D(InX, Position.Y));
}

void UFairyObject::SetY(float InY)
{
	SetPosition(FVector2D(Position.X, InY));
}

void UFairyObject::SetPosition(const FVector2D& InPosition)
{
	if (Position != InPosition)
	{
		FVector2D delta = InPosition - Position;
		Position = InPosition;

		HandlePositionChanged();

		UGGroup* g = Cast<UGGroup>(this);
		if (g != nullptr)
			g->MoveChildren(delta);

		UpdateGear(1);

		if (Parent.IsValid() && Parent->IsA<UGList>())
		{
			Parent->SetBoundsChangedFlag();
			if (Group.IsValid())
				Group->SetBoundsChangedFlag(true);

			OnPositionChangedEvent.Broadcast();
		}

		if (DraggingObject.Get() == this && !bUpdateInDragging)
			GlobalRect = LocalToGlobalRect(FBox2D(FVector2D(), Size));
	}
}

float UFairyObject::GetXMin() const
{
	return bPivotAsAnchor ? (Position.X - Size.X * Pivot.X) : Position.X;
}

void UFairyObject::SetXMin(float InXMin)
{
	if (bPivotAsAnchor)
	{
		SetPosition(FVector2D(InXMin + Size.X * Pivot.X, Position.Y));
	}
	else
	{
		SetPosition(FVector2D(InXMin, Position.Y));
	}
}

float UFairyObject::GetYMin() const
{
	return bPivotAsAnchor ? (Position.Y - Size.Y * Pivot.Y) : Position.Y;
}

void UFairyObject::SetYMin(float InYMin)
{
	if (bPivotAsAnchor)
	{
		SetPosition(FVector2D(Position.X, InYMin + Size.Y * Pivot.Y));
	}
	else
	{
		SetPosition(FVector2D(Position.X, InYMin));
	}
}

void UFairyObject::SetSize(const FVector2D& InSize, bool bIgnorePivot)
{
	if (RawSize != InSize)
	{
		RawSize = InSize;
		FVector2D ClampSize = InSize;
		if (ClampSize.X < MinSize.X)
		{
			ClampSize.X = MinSize.X;
		}
		else if (MaxSize.X > 0 && ClampSize.X > MaxSize.X)
		{
			ClampSize.X = MaxSize.X;
		}
			
		if (ClampSize.Y < MinSize.Y)
		{
			ClampSize.Y = MinSize.Y;
		}
		else if (MaxSize.Y > 0 && ClampSize.Y > MaxSize.Y)
		{
			ClampSize.Y = MaxSize.Y;
		}
		FVector2D Delta = ClampSize - Size;
		Size = ClampSize;

		HandleSizeChanged();

		if (Pivot.X != 0 || Pivot.Y != 0)
		{
			if (!bPivotAsAnchor)
			{
				if (!bIgnorePivot)
				{
					SetPosition(Position - Pivot * Delta);
				}
				else
				{
					HandlePositionChanged();
				}
			}
			else
			{
				HandlePositionChanged();
			}
		}
		else
		{
			HandlePositionChanged();
		}

		UGGroup* g = Cast<UGGroup>(this);
		if (g != nullptr)
		{
			g->ResizeChildren(Delta);
		}

		UpdateGear(2);

		if (Parent.IsValid())
		{
			Relations->OnOwnerSizeChanged(Delta, bPivotAsAnchor || !bIgnorePivot);
			Parent->SetBoundsChangedFlag();
			if (Group.IsValid())
			{
				Group->SetBoundsChangedFlag();
			}
		}
		OnSizeChangedEvent.Broadcast();
	}
}

void UFairyObject::SetSizeDirectly(const FVector2D& InSize)
{
	RawSize = InSize;
	Size = InSize;
	if (Size.X < 0)
	{
		Size.X = 0;
	}
	if (Size.Y < 0)
	{
		Size.Y = 0;
	}
}

void UFairyObject::Center(bool bRestraint)
{
	UFairyComponent* Root = nullptr;
	if (Parent.IsValid())
	{
		Root = Parent.Get();
	}
	else
	{
		Root = UFairyApplication::Get()->GetUIRoot(this);
	}

	SetPosition(((Root->Size - Size) / 2).RoundToVector());
	if (bRestraint)
	{
		AddRelation(Root, ERelationType::Center_Center);
		AddRelation(Root, ERelationType::Middle_Middle);
	}
}

void UFairyObject::MakeFullScreen(bool bRestraint)
{
	SetSize(UFairyApplication::Get()->GetUIRoot(this)->GetSize());
	if (bRestraint)
	{
		AddRelation(UFairyApplication::Get()->GetUIRoot(this), ERelationType::Size);
	}
}

void UFairyObject::SetPivot(const FVector2D& InPivot, bool bAsAnchor)
{
	if (Pivot != InPivot || bPivotAsAnchor != bAsAnchor)
	{
		Pivot = InPivot;
		bPivotAsAnchor = bAsAnchor;
		DisplayObject->SetRenderTransformPivot(FVector2D(Pivot.X, Pivot.Y));
		HandlePositionChanged();
	}
}

void UFairyObject::SetScale(const FVector2D& InScale)
{
	if (Scale != InScale)
	{
		Scale = InScale;
		UpdateTransform();
		UpdateGear(2);
	}
}

void UFairyObject::SetSkew(const FVector2D& InSkew)
{
	Skew = InSkew;
}

void UFairyObject::SetRotation(float InRotation)
{
	if (Rotation != InRotation)
	{
		Rotation = InRotation;
		UpdateTransform();
		UpdateGear(3);
	}
}

void UFairyObject::UpdateTransform()
{
	FScale2D Scale2D = FScale2D(Scale);
	FQuat2D Quat2D = FQuat2D(FMath::DegreesToRadians(Rotation));
	FMatrix2x2 Matrix = Concatenate(Quat2D, Scale2D);
	DisplayObject->SetRenderTransform(FSlateRenderTransform(Matrix, Position));
}

void UFairyObject::SetAlpha(float InAlpha)
{
	if (Alpha != InAlpha)
	{
		Alpha = InAlpha;
		HandleAlphaChanged();
		UpdateGear(3);
	}
}

void UFairyObject::SetGrayed(bool InBGrayed)
{
	if (bGrayed != InBGrayed)
	{
		bGrayed = InBGrayed;
		HandleGrayedChanged();
		UpdateGear(3);
	}
}

void UFairyObject::SetVisible(bool bInVisible)
{
	if (bVisible != bInVisible)
	{
		bVisible = bInVisible;
		HandleVisibleChanged();
		if (Parent.IsValid())
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
		if (Parent.IsValid())
			Parent->ChildSortingOrderChanged(this, old, SortingOrder);
	}
}

void UFairyObject::SetGroup(UGGroup* InGroup)
{
	if (Group.Get() != InGroup)
	{
		if (Group.IsValid())
			Group->SetBoundsChangedFlag();
		Group = InGroup;
		if (Group.IsValid())
			Group->SetBoundsChangedFlag();
		HandleVisibleChanged();
		if (Parent.IsValid())
			Parent->ChildStateChanged(this);
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
		On(FUIEvents::RollOver).AddUObject(this, &UFairyObject::OnRollOverHandler);
		On(FUIEvents::RollOut).AddUObject(this, &UFairyObject::OnRollOutHandler);
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
		return DragBounds.GetValue();
	else
		return FBox2D(FVector2D::ZeroVector, FVector2D::ZeroVector);
}

void UFairyObject::SetDragBounds(const FBox2D& InBounds)
{
	if (InBounds.Min == InBounds.Max && InBounds.Min == FVector2D::ZeroVector)
		DragBounds.Reset();
	else
		DragBounds = InBounds;
}

void UFairyObject::StartDrag(int32 UserIndex, int32 PointerIndex)
{
	DragBegin(UserIndex, PointerIndex);
}

void UFairyObject::StopDrag()
{
	DragEnd();
}

FString UFairyObject::GetResourceURL() const
{
	if (PackageItem.IsValid())
		return "ui://" + PackageItem->Owner->GetID() + PackageItem->ID;
	else
		return G_EMPTY_STRING;
}

FString UFairyObject::GetResourceName() const
{
	if (PackageItem.IsValid())
		return PackageItem->Name;
	else
		return G_EMPTY_STRING;
}

FString UFairyObject::GetPackageName() const
{
	if (PackageItem.IsValid())
		return PackageItem->Owner->GetName();
	else
		return G_EMPTY_STRING;
}

FVector2D UFairyObject::LocalToGlobal(const FVector2D& InPoint)
{
	FVector2D Point = InPoint;
	if (bPivotAsAnchor)
		Point += Size * Pivot;

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
		Point -= Size * Pivot;
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

void UFairyObject::AddRelation(UFairyObject* Obj, ERelationType RelationType, bool bUsePercent)
{
	Relations->Add(Obj, RelationType, bUsePercent);
}

void UFairyObject::RemoveRelation(UFairyObject* Obj, ERelationType RelationType)
{
	Relations->Remove(Obj, RelationType);
}

FGearBase* UFairyObject::GetGear(int32 Index)
{
	FGearBase* gear = Gears[Index];
	if (gear == nullptr)
	{
		gear = FGearBase::Create(this, static_cast<FGearBase::EType>(Index));
		Gears[Index] = gear;
	}
	return gear;
}

void UFairyObject::UpdateGear(int32 Index)
{
	if (bUnderConstruct || bGearLocked)
		return;

	FGearBase* gear = Gears[Index];
	if (gear != nullptr && gear->GetController() != nullptr)
		gear->UpdateState();
}

bool UFairyObject::CheckGearController(int32 Index, UGController* Controller)
{
	return Gears[Index] != nullptr && Gears[Index]->GetController() == Controller;
}

void UFairyObject::UpdateGearFromRelations(int32 Index, const FVector2D& Delta)
{
	if (Gears[Index] != nullptr)
		Gears[Index]->UpdateFromRelations(Delta);
}

uint32 UFairyObject::AddDisplayLock()
{
	FGearDisplay* gearDisplay = (FGearDisplay*)Gears[0];
	if (gearDisplay != nullptr && gearDisplay->GetController() != nullptr)
	{
		uint32 ret = gearDisplay->AddLock();
		CheckGearDisplay();

		return ret;
	}
	else
		return 0;
}

void UFairyObject::ReleaseDisplayLock(uint32 Token)
{
	FGearDisplay* gearDisplay = (FGearDisplay*)Gears[0];
	if (gearDisplay != nullptr && gearDisplay->GetController() != nullptr)
	{
		gearDisplay->ReleaseLock(Token);
		CheckGearDisplay();
	}
}

void UFairyObject::CheckGearDisplay()
{
	if (bHandlingController)
		return;

	bool connected = Gears[0] == nullptr || ((FGearDisplay*)Gears[0])->IsConnected();
	if (Gears[8] != nullptr && Gears[8]->GetType() == FGearBase::EType::Display2)
		connected = static_cast<FGearDisplay2*>(Gears[8])->Evaluate(connected);

	if (connected != bInternalVisible)
	{
		bInternalVisible = connected;
		if (Parent.IsValid())
			Parent->ChildStateChanged(this);
		if (Group.IsValid() && Group->IsExcludeInvisibles())
			Group->SetBoundsChangedFlag();
	}
}

void UFairyObject::RemoveFromParent()
{
	if (Parent.IsValid())
		Parent->RemoveChild(this);
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
		Delegate.DynFunc->Broadcast(Context);
}

UFairyObject::FUnifiedEventDelegate& UFairyObject::GetEventDelegate(const FName& EventType)
{
	FUnifiedEventDelegate* Delegate = EventDelegates.Find(EventType);
	if (Delegate == nullptr)
	{
		Delegate = &EventDelegates.Add(EventType);

		UProperty* Property = GetClass()->FindPropertyByName(FName(*FString("On").Append(EventType.ToString())));
		if (Property != nullptr)
			Delegate->DynFunc = Property->ContainerPtrToValuePtr<FGUIEventDynMDelegate>(this);
		else
			Delegate->DynFunc = nullptr;
	}

	return *Delegate;
}

FGUIEventMDelegate& UFairyObject::On(const FName& EventType)
{
	return GetEventDelegate(EventType).Func;
}

void UFairyObject::ConstructFromResource()
{
}

void UFairyObject::HandlePositionChanged()
{
	DisplayObject->SetPosition(bPivotAsAnchor ? (Position - Size * Pivot) : Position);
}

void UFairyObject::HandleSizeChanged()
{
	DisplayObject->SetSize(Size);
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

void UFairyObject::HandleControllerChanged(UGController* Controller)
{
	bHandlingController = true;
	for (int32 i = 0; i < 10; i++)
	{
		FGearBase* gear = Gears[i];
		if (gear != nullptr && gear->GetController() == Controller)
			gear->Apply();
	}
	bHandlingController = false;


	CheckGearDisplay();
}

void UFairyObject::SetupBeforeAdd(FByteBuffer* Buffer, int32 BeginPos)
{
	Buffer->Seek(BeginPos, 0);
	Buffer->Skip(5);

	ID = Buffer->ReadS();
	Name = Buffer->ReadS();
	float f1 = Buffer->ReadInt();
	float f2 = Buffer->ReadInt();
	SetPosition(FVector2D(f1, f2));

	if (Buffer->ReadBool())
	{
		InitSize.X = Buffer->ReadInt();
		InitSize.Y = Buffer->ReadInt();
		SetSize(InitSize, true);
	}

	if (Buffer->ReadBool())
	{
		MinSize.X = Buffer->ReadInt();
		MaxSize.X = Buffer->ReadInt();
		MinSize.Y = Buffer->ReadInt();
		MaxSize.Y = Buffer->ReadInt();
	}

	if (Buffer->ReadBool())
	{
		f1 = Buffer->ReadFloat();
		f2 = Buffer->ReadFloat();
		SetScale(FVector2D(f1, f2));
	}

	if (Buffer->ReadBool())
	{
		f1 = Buffer->ReadFloat();
		f2 = Buffer->ReadFloat();
		SetSkew(FVector2D(f1, f2));
	}

	if (Buffer->ReadBool())
	{
		f1 = Buffer->ReadFloat();
		f2 = Buffer->ReadFloat();
		SetPivot(FVector2D(f1, f2), Buffer->ReadBool());
	}

	f1 = Buffer->ReadFloat();
	if (f1 != 1)
	{
		SetAlpha(f1);
	}
		

	f1 = Buffer->ReadFloat();
	if (f1 != 0)
	{
		SetRotation(f1);
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

	const FString& str = Buffer->ReadS();
	if (!str.IsEmpty())
	{
		UserData = str;
	}
}

void UFairyObject::SetupAfterAdd(FByteBuffer* Buffer, int32 BeginPos)
{
	Buffer->Seek(BeginPos, 1);

	const FString& str = Buffer->ReadS();
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

	int16 cnt = Buffer->ReadShort();
	for (int32 i = 0; i < cnt; i++)
	{
		int16 nextPos = Buffer->ReadShort();
		nextPos += Buffer->GetPos();

		FGearBase* gear = GetGear(Buffer->ReadByte());
		gear->Setup(Buffer);

		Buffer->SetPos(nextPos);
	}
}

void UFairyObject::InitDrag()
{
	if (bDraggable)
	{
		OnTouchBegin.AddUniqueDynamic(this, &UFairyObject::OnTouchBeginHandler);
		OnTouchMove.AddUniqueDynamic(this, &UFairyObject::OnTouchMoveHandler);
		OnTouchEnd.AddUniqueDynamic(this, &UFairyObject::OnTouchEndHandler);
	}
	else
	{
		OnTouchBegin.RemoveDynamic(this, &UFairyObject::OnTouchBeginHandler);
		OnTouchMove.RemoveDynamic(this, &UFairyObject::OnTouchMoveHandler);
		OnTouchEnd.RemoveDynamic(this, &UFairyObject::OnTouchEndHandler);
	}
}

void UFairyObject::DragBegin(int32 UserIndex, int32 PointerIndex)
{
	if (DispatchEvent(FUIEvents::DragStart))
		return;

	if (DraggingObject.IsValid())
	{
		UFairyObject* tmp = DraggingObject.Get();
		DraggingObject->StopDrag();
		DraggingObject.Reset();
		tmp->DispatchEvent(FUIEvents::DragEnd);
	}

	GlobalDragStart = UFairyApplication::Get()->GetTouchPosition(UserIndex, PointerIndex);
	GlobalRect = LocalToGlobalRect(FBox2D(FVector2D::ZeroVector, Size));
	DraggingObject = this;
	bDragTesting = false;

	UFairyApplication::Get()->AddMouseCaptor(UserIndex, PointerIndex, this);

	OnTouchMove.AddUniqueDynamic(this, &UFairyObject::OnTouchMoveHandler);
	OnTouchEnd.AddUniqueDynamic(this, &UFairyObject::OnTouchEndHandler);
}

void UFairyObject::DragEnd()
{
	if (DraggingObject.Get() == this)
	{
		bDragTesting = false;
		DraggingObject.Reset();
	}
}

void UFairyObject::OnTouchBeginHandler(UEventContext* Context)
{
	DragTouchStartPos = Context->GetPointerPosition();
	bDragTesting = true;
	Context->CaptureTouch();
}

void UFairyObject::OnTouchMoveHandler(UEventContext* Context)
{
	if (DraggingObject.Get() != this && bDragTesting)
	{
		int32 sensitivity;
		if (FPlatformMisc::DesktopTouchScreen())
			sensitivity = FUIConfig::Config.ClickDragSensitivity;
		else
			sensitivity = FUIConfig::Config.TouchDragSensitivity;
		if (FMath::Abs(DragTouchStartPos.X - Context->GetPointerPosition().X) < sensitivity
			&& FMath::Abs(DragTouchStartPos.Y - Context->GetPointerPosition().Y) < sensitivity)
			return;

		bDragTesting = false;
		DragBegin(Context->GetUserIndex(), Context->GetPointerIndex());
	}

	if (DraggingObject.Get() == this)
	{
		FVector2D Pos = Context->GetPointerPosition() - GlobalDragStart + GlobalRect.Min;
		if (DragBounds.IsSet())
		{
			FBox2D rect = UFairyApplication::Get()->GetUIRoot(this)->LocalToGlobalRect(DragBounds.GetValue());
			if (Pos.X < rect.Min.X)
				Pos.X = rect.Min.X;
			else if (Pos.X + GlobalRect.GetSize().X > rect.Max.X)
			{
				Pos.X = rect.Max.X - GlobalRect.GetSize().X;
				if (Pos.X < rect.Min.X)
					Pos.X = rect.Min.X;
			}

			if (Pos.Y < rect.Min.Y)
				Pos.Y = rect.Min.Y;
			else if (Pos.Y + GlobalRect.GetSize().Y > rect.Max.Y)
			{
				Pos.Y = rect.Max.Y - GlobalRect.GetSize().Y;
				if (Pos.Y < rect.Min.Y)
					Pos.Y = rect.Min.Y;
			}
		}

		Pos = Parent->GlobalToLocal(Pos);

		bUpdateInDragging = true;
		SetPosition(Pos.RoundToVector());
		bUpdateInDragging = false;

		DispatchEvent(FUIEvents::DragMove);
	}
}

void UFairyObject::OnTouchEndHandler(UEventContext* Context)
{
	if (DraggingObject.Get() == this)
	{
		DraggingObject.Reset();
		DispatchEvent(FUIEvents::DragEnd);
	}
}