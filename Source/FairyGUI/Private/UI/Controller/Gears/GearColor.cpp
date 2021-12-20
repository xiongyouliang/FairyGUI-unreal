#include "UI/Controller/Gears/GearColor.h"
#include "UI/FairyObject.h"
#include "Package/FairyPackage.h"
#include "UI/Controller/FairyController.h"
#include "Utils/ByteBuffer.h"

FGearColor::FValue::FValue()
{
}

FGearColor::FGearColor(UFairyObject* InOwner) : FGearBase(InOwner)
{
	Type = EFairyGearType::Color;
}

FGearColor::~FGearColor()
{
}

void FGearColor::Init()
{
	Default.Color = TargetObject->GetProp<FColor>(EObjectPropID::Color);
	Default.OutlineColor = TargetObject->GetProp<FColor>(EObjectPropID::OutlineColor);
	Storage.Reset();
}

void FGearColor::AddStatus(const FString& PageID, FairyGUI::FByteBuffer* Buffer)
{
	FValue Value;
	Value.Color = Buffer->ReadColor();
	Value.OutlineColor = Buffer->ReadColor();
	if (PageID.IsEmpty())
	{
		Default = Value;
	}
	else
	{
		Storage.Add(PageID, MoveTemp(Value));
	}
}

void FGearColor::Apply()
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
		TargetObject->SetProp(EObjectPropID::Color, FNVariant(Value->Color));
		TargetObject->SetProp(EObjectPropID::OutlineColor, FNVariant(Value->OutlineColor));
		TargetObject->bGearLocked = false;
	}
}

void FGearColor::OnTweenUpdate(UFairyTweener* Tweener)
{
	//TargetObject->bGearLocked = true;
	//TargetObject->SetProp(EObjectPropID::Color, FNVariant(Tweener->Value.GetColor()));
	//TargetObject->bGearLocked = false;
}

void FGearColor::OnTweenComplete()
{
	//if (TweenConfig->DisplayLockToken != 0)
	//{
	//    TargetObject->ReleaseDisplayLock(TweenConfig->DisplayLockToken);
	//    TweenConfig->DisplayLockToken = 0;
	//}
	//TweenConfig->Handle.Invalidate();
	//TargetObject->DispatchEvent(FFairyEventNames::GearStop);
}

void FGearColor::UpdateState()
{
	FValue Value;
	Value.Color = TargetObject->GetProp<FColor>(EObjectPropID::Color);
	Value.OutlineColor = TargetObject->GetProp<FColor>(EObjectPropID::OutlineColor);
	Storage.Add(Controller->GetSelectedPageID(), MoveTemp(Value));
}
