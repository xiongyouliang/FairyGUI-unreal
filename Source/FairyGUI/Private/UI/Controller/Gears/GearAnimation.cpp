#include "UI/Controller/Gears/GearAnimation.h"
#include "UI/FairyObject.h"
#include "UI/Controller/FairyController.h"
#include "Utils/ByteBuffer.h"

FGearAnimation::FValue::FValue() :
	bPlaying(false), Frame(0)
{
}

FGearAnimation::FGearAnimation(UFairyObject* InOwner) : FGearBase(InOwner)
{
	Type = EFairyGearType::Animation;
}

FGearAnimation::~FGearAnimation()
{
}

void FGearAnimation::Init()
{
	Default.bPlaying = TargetObject->GetProp<bool>(EObjectPropID::Playing);
	Default.Frame = TargetObject->GetProp<int32>(EObjectPropID::Frame);
	Storage.Reset();
}

void FGearAnimation::AddStatus(const FName& PageID, FairyGUI::FByteBuffer* Buffer)
{
	FValue Value;
	Value.bPlaying = Buffer->ReadBool();
	Value.Frame = Buffer->ReadInt();
	if (PageID.IsNone())
	{
		Default = Value;
	}
	else
	{
		Storage.Add(PageID, MoveTemp(Value));
	}
}

void FGearAnimation::Apply()
{
	TargetObject->bGearLocked = true;

	FValue* Value = Storage.Find(Controller->GetSelectedPageID());
	if (Value == nullptr)
	{
		Value = &Default;
	}

	TargetObject->SetProp(EObjectPropID::Playing, FNVariant(Value->bPlaying));
	TargetObject->SetProp(EObjectPropID::Frame, FNVariant(Value->Frame));

	TargetObject->bGearLocked = false;
}

void FGearAnimation::UpdateState()
{
	FValue Value;
	Value.bPlaying = TargetObject->GetProp<bool>(EObjectPropID::Playing);
	Value.Frame = TargetObject->GetProp<int32>(EObjectPropID::Frame);
	Storage.Add(Controller->GetSelectedPageID(), MoveTemp(Value));
}