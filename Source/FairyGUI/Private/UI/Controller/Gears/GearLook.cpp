#include "UI/Controller/Gears/GearLook.h"
#include "UI/FairyObject.h"
#include "Package/FairyPackage.h"
#include "UI/Controller/FairyController.h"
#include "Utils/ByteBuffer.h"

FGearLook::FValue::FValue() :
	Alpha(0),
	Rotation(0),
	bGrayed(false),
	bTouchable(false)
{
}

FGearLook::FGearLook(UFairyObject* InOwner) : FGearBase(InOwner)
{
	Type = EFairyGearType::Look;
}

FGearLook::~FGearLook()
{
}

void FGearLook::Init()
{
	Default.Alpha = TargetObject->GetAlpha();
	Default.Rotation = TargetObject->GetRotation();
	Default.bGrayed = TargetObject->IsGrayed();
	Default.bTouchable = TargetObject->IsTouchable();
	Storage.Reset();
}

void FGearLook::AddStatus(const FName& PageID, FairyGUI::FByteBuffer* Buffer)
{
	FValue Value;
	Value.Alpha = Buffer->ReadFloat();
	Value.Rotation = Buffer->ReadFloat();
	Value.bGrayed = Buffer->ReadBool();
	Value.bTouchable = Buffer->ReadBool();

	if (PageID.IsNone())
	{
		Default = Value;
	}
	else
	{
		Storage.Add(PageID, MoveTemp(Value));
	}
}

void FGearLook::Apply()
{
	FValue* Value = Storage.Find(Controller->GetSelectedPageID());
	if (Value == nullptr)
	{
		Value = &Default;
	}

	if (TweenConfig.IsSet() && TweenConfig->bTween && UFairyPackage::Constructing == 0 && !bDisableAllTweenEffect)
	{
		// todo: use new tween system
	}
	else
	{
		TargetObject->bGearLocked = true;
		TargetObject->SetAlpha(Value->Alpha);
		TargetObject->SetRotation(Value->Rotation);
		TargetObject->SetGrayed(Value->bGrayed);
		TargetObject->SetTouchable(Value->bTouchable);
		TargetObject->bGearLocked = false;
	}
}

void FGearLook::OnTweenUpdate(UFairyTweener* Tweener)
{
  //  int32 flag = Tweener->GetUserData().AsInt();
  //  TargetObject->bGearLocked = true;

  //  if ((flag & 1) != 0)
  //  {
		//TargetObject->SetAlpha(Tweener->Value.X);
  //  }

  //  if ((flag & 2) != 0)
  //  {
		//TargetObject->SetRotation(Tweener->Value.Y);
  //  }
  //  TargetObject->bGearLocked = false;
}

void FGearLook::OnTweenComplete()
{
	//if (TweenConfig->DisplayLockToken != 0)
	//{
	//    TargetObject->ReleaseDisplayLock(TweenConfig->DisplayLockToken);
	//    TweenConfig->DisplayLockToken = 0;
	//}
	//TweenConfig->Handle.Invalidate();
	//TargetObject->DispatchEvent(FFairyEventNames::GearStop);
}

void FGearLook::UpdateState()
{
	FValue Value;
	Value.Alpha = TargetObject->GetAlpha();
	Value.Rotation = TargetObject->GetRotation();
	Value.bGrayed = TargetObject->IsGrayed();
	Value.bTouchable = TargetObject->IsTouchable();
	Storage.Add(Controller->GetSelectedPageID(), MoveTemp(Value));
}
