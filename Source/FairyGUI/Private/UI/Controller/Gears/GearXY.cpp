#include "UI/Controller/Gears/GearXY.h"
#include "UI/FairyComponent.h"
#include "Package/FairyPackage.h"
#include "UI/Controller/FairyController.h"
#include "Utils/ByteBuffer.h"

FGearXY::FGearXY(UFairyObject* InOwner) :
	FGearBase(InOwner),
	bPositionsInPercent(false)
{
	Type = EFairyGearType::XY;
}

FGearXY::~FGearXY()
{
}

void FGearXY::Init()
{
	const FVector2D OwnerPos = TargetObject->GetPosition();
	Default = FVector4(OwnerPos.X, OwnerPos.Y,
		OwnerPos.X / TargetObject->GetParent()->GetWidth(),
		OwnerPos.Y / TargetObject->GetParent()->GetHeight());
	Storage.Reset();
}

void FGearXY::AddStatus(const FName& PageID, FairyGUI::FByteBuffer* Buffer)
{
	FVector4 Value;
	Value.X = Buffer->ReadInt();
	Value.Y = Buffer->ReadInt();

	if (PageID.IsNone())
	{
		Default = Value;
	}
	else
	{
		Storage.Add(PageID, MoveTemp(Value));
	}
}

void FGearXY::AddExtStatus(const FName& PageID, FairyGUI::FByteBuffer* Buffer)
{
	FVector4* Value = PageID.IsNone() ? &Default : Storage.Find(Controller->GetSelectedPageID());
	Value->Z = Buffer->ReadFloat();
	Value->W = Buffer->ReadFloat();
}

void FGearXY::Apply()
{
	FVector4* Value = Storage.Find(Controller->GetSelectedPageID());
	if (Value == nullptr)
	{
		Value = &Default;
	}

	FVector2D EndPt;

	if (bPositionsInPercent && TargetObject->GetParent())
	{
		EndPt.X = Value->Z * TargetObject->GetParent()->GetWidth();
		EndPt.Y = Value->W * TargetObject->GetParent()->GetHeight();
	}
	else
	{
		EndPt.X = Value->X;
		EndPt.Y = Value->Y;
	}

	if (TweenConfig.IsSet() && TweenConfig->bTween && UFairyPackage::Constructing == 0 && !bDisableAllTweenEffect)
	{
		// todo: use new tween system.
	}
	else
	{
		TargetObject->bGearLocked = true;
		TargetObject->SetPosition(EndPt);
		TargetObject->bGearLocked = false;
	}
}

void FGearXY::OnTweenComplete()
{
	//if (TweenConfig->DisplayLockToken != 0)
	//{
	//    TargetObject->ReleaseDisplayLock(TweenConfig->DisplayLockToken);
	//    TweenConfig->DisplayLockToken = 0;
	//}
	//TweenConfig->Handle.Invalidate();
	//TargetObject->DispatchEvent(FFairyEventNames::GearStop);
}

void FGearXY::UpdateState()
{
	const FVector2D& OwnerPos = TargetObject->GetPosition();
	Storage.Add(Controller->GetSelectedPageID(), FVector4(
		OwnerPos.X, OwnerPos.Y,
		OwnerPos.X / TargetObject->GetParent()->GetWidth(),
		OwnerPos.Y / TargetObject->GetParent()->GetHeight()));
}

void FGearXY::UpdateFromRelations(const FVector2D& Delta)
{
	if (Controller.Get() != nullptr && Storage.Num() > 0 && !bPositionsInPercent)
	{
		for (auto It = Storage.CreateIterator(); It; ++It)
		{
			It->Value = FVector4(It->Value.X + Delta.X, It->Value.Y + Delta.Y, It->Value.Z, It->Value.W);
		}
		Default.X += Delta.X;
		Default.Y += Delta.Y;

		UpdateState();
	}
}
