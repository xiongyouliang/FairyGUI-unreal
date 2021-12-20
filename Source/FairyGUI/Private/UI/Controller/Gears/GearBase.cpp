#include "UI/Controller/Gears/GearBase.h"
#include "UI/Controller/Gears/GearDisplay.h"
#include "UI/Controller/Gears/GearAnimation.h"
#include "UI/Controller/Gears/GearColor.h"
#include "UI/Controller/Gears/GearDisplay2.h"
#include "UI/Controller/Gears/GearFontSize.h"
#include "UI/Controller/Gears/GearIcon.h"
#include "UI/Controller/Gears/GearLook.h"
#include "UI/Controller/Gears/GearSize.h"
#include "UI/Controller/Gears/GearText.h"
#include "UI/Controller/Gears/GearXY.h"

#include "UI/Controller/FairyController.h"

#include "UI/FairyObject.h"
#include "UI/FairyComponent.h"
#include "Utils/ByteBuffer.h"

bool FGearBase::bDisableAllTweenEffect = false;

FGearBase* FGearBase::Create(UFairyObject* InTarget, EFairyGearType InType)
{
	FGearBase* Gear = nullptr;
	switch (InType)
	{
	case EFairyGearType::Display:
		Gear = new FGearDisplay(InTarget);
		break;
	case EFairyGearType::XY:
		Gear = new FGearXY(InTarget);
		break;
	case EFairyGearType::Size:
		Gear = new FGearSize(InTarget);
		break;
	case EFairyGearType::Look:
		Gear = new FGearLook(InTarget);
		break;
	case EFairyGearType::Color:
		Gear = new FGearColor(InTarget);
		break;
	case EFairyGearType::Animation:
		Gear = new FGearAnimation(InTarget);
		break;
	case EFairyGearType::Text:
		Gear = new FGearText(InTarget);
		break;
	case EFairyGearType::Icon:
		Gear = new FGearIcon(InTarget);
		break;
	case EFairyGearType::Display2:
		Gear = new FGearDisplay2(InTarget);
		break;
	case EFairyGearType::FontSize:
		Gear = new FGearFontSize(InTarget);
		break;
	}

	return Gear;
}

FGearTweenConfig::FGearTweenConfig():
	bTween(true),
	EaseType(EFairyEaseType::QuadOut),
	Duration(0.3f),
	Delay(0),
	DisplayLockToken(0)
{
}

FGearBase::FGearBase(UFairyObject* InTarget) : TargetObject(InTarget)
{
}

FGearBase::FGearBase(UFairyObject* InTarget, EFairyGearType InType)
	: Type(InType)
	, TargetObject(InTarget)
{

}

FGearBase::~FGearBase()
{
}

UFairyController* FGearBase::GetController() const
{
	return Controller.Get();
}

void FGearBase::SetController(UFairyController* InController)
{
	if (!Controller.IsValid() || Controller.Get() != InController)
	{
		Controller = InController;
		if (Controller != nullptr)
		{
			Init();
		}
	}
}

void FGearBase::HandleControllerChanged(UFairyController* InController)
{
	this->Apply();
}

UFairyObject* FGearBase::GetTarget()
{
	if (TargetObject.IsValid())
	{
		return TargetObject.Get();
	}
	return nullptr;
}

FGearTweenConfig& FGearBase::GetTweenConfig()
{
	if (!TweenConfig.IsSet())
	{
		TweenConfig.Emplace();
	}

	return TweenConfig.GetValue();
}

void FGearBase::Init()
{
}

void FGearBase::AddStatus(const FString& PageID, FairyGUI::FByteBuffer* Buffer)
{
}

void FGearBase::Apply()
{
}

void FGearBase::UpdateState()
{
}

bool FGearBase::IsActived()
{
	return false;
}

void FGearBase::UpdateFromRelations(const FVector2D& Delta)
{
}


/**
* todo: this is not a good desgin, buffer parsing need remove to inherited class.
*/
void FGearBase::Setup(FairyGUI::FByteBuffer* Buffer)
{
	int32 index = Buffer->ReadShort();
	Controller = TargetObject->GetParent()->GetControllerAt(index);
	
	Init();

	int32 Count = Buffer->ReadShort();
	FGearDisplay* g0 = Type == EFairyGearType::Display ? static_cast<FGearDisplay*>(this) : nullptr;
	FGearDisplay2* g1 = Type == EFairyGearType::Display2 ? static_cast<FGearDisplay2*>(this) : nullptr;
	FGearXY* g2 = nullptr;
	if (g0)
	{
		Buffer->ReadSArray(g0->Pages, Count);
	}
	else if (g1)
	{
		Buffer->ReadSArray(g1->Pages, Count);
	}
	else
	{
		for (int32 i = 0; i < Count; i++)
		{
			const FString& page = Buffer->ReadStringFromCache();
			if (page.IsEmpty())
			{
				continue;
			}
			AddStatus(page, Buffer);
		}

		if (Buffer->ReadBool())
		{
			AddStatus(G_EMPTY_STRING, Buffer);
		}
	}

	if (Buffer->ReadBool())
	{
		TweenConfig.Emplace();
		TweenConfig->bTween = true;
		TweenConfig->EaseType = (EFairyEaseType)Buffer->ReadByte();
		TweenConfig->Duration = Buffer->ReadFloat();
		TweenConfig->Delay = Buffer->ReadFloat();
	}

	if (Buffer->Version >= 2)
	{
		g2 = Type == EFairyGearType::XY ? static_cast<FGearXY*>(this) : nullptr;
		if (g2)
		{
			if (Buffer->ReadBool())
			{
				g2->bPositionsInPercent = true;
				for (int32 i = 0; i < Count; i++)
				{
					const FString& page = Buffer->ReadStringFromCache();
					if (page.IsEmpty())
					{
						continue;
					}

					g2->AddExtStatus(page, Buffer);
				}

				if (Buffer->ReadBool())
				{
					g2->AddExtStatus(G_EMPTY_STRING, Buffer);
				}
			}
		}
		else if (g1 != nullptr)
		{
			g1->Condition = (FGearDisplay2::EConditionType)Buffer->ReadByte();
		}
	}
}

