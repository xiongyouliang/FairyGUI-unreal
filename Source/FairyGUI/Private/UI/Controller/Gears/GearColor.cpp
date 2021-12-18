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
    Default.Color = Owner->GetProp<FColor>(EObjectPropID::Color);
    Default.OutlineColor = Owner->GetProp<FColor>(EObjectPropID::OutlineColor);
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
        Owner->bGearLocked = true;
        Owner->SetProp(EObjectPropID::Color, FNVariant(Value->Color));
        Owner->SetProp(EObjectPropID::OutlineColor, FNVariant(Value->OutlineColor));
        Owner->bGearLocked = false;
    }
}

void FGearColor::OnTweenUpdate(UFairyTweener* Tweener)
{
	//Owner->bGearLocked = true;
	//Owner->SetProp(EObjectPropID::Color, FNVariant(Tweener->Value.GetColor()));
	//Owner->bGearLocked = false;
}

void FGearColor::OnTweenComplete()
{
    //if (TweenConfig->DisplayLockToken != 0)
    //{
    //    Owner->ReleaseDisplayLock(TweenConfig->DisplayLockToken);
    //    TweenConfig->DisplayLockToken = 0;
    //}
    //TweenConfig->Handle.Invalidate();
    //Owner->DispatchEvent(FFairyEventNames::GearStop);
}

void FGearColor::UpdateState()
{
    FValue Value;
    Value.Color = Owner->GetProp<FColor>(EObjectPropID::Color);
    Value.OutlineColor = Owner->GetProp<FColor>(EObjectPropID::OutlineColor);
    Storage.Add(Controller->GetSelectedPageID(), MoveTemp(Value));
}
