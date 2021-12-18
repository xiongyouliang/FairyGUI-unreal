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

FGearBase* FGearBase::Create(UFairyObject* InOwner, EFairyGearType InType)
{
	FGearBase* Gear = nullptr;
	switch (InType)
	{
	case EFairyGearType::Display:
		Gear = new FGearDisplay(InOwner);
		break;
	case EFairyGearType::XY:
		Gear = new FGearXY(InOwner);
		break;
	case EFairyGearType::Size:
		Gear = new FGearSize(InOwner);
		break;
	case EFairyGearType::Look:
		Gear = new FGearLook(InOwner);
		break;
	case EFairyGearType::Color:
		Gear = new FGearColor(InOwner);
		break;
	case EFairyGearType::Animation:
		Gear = new FGearAnimation(InOwner);
		break;
	case EFairyGearType::Text:
		Gear = new FGearText(InOwner);
		break;
	case EFairyGearType::Icon:
		Gear = new FGearIcon(InOwner);
		break;
	case EFairyGearType::Display2:
		Gear = new FGearDisplay2(InOwner);
		break;
	case EFairyGearType::FontSize:
		Gear = new FGearFontSize(InOwner);
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

FGearBase::FGearBase(UFairyObject* InOwner) : Owner(InOwner)
{
}

FGearBase::FGearBase(UFairyObject* InOwner, EFairyGearType InType)
	: Type(InType)
	, Owner(InOwner)
{

}

FGearBase::~FGearBase()
{
	if (ControllerHandle.IsValid() && Controller.IsValid())
	{
		Controller->OnChanged().Remove(ControllerHandle);
	}
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

void FGearBase::UpdateFromRelations(const FVector2D& Delta)
{
}

void FGearBase::Setup(FairyGUI::FByteBuffer* Buffer)
{
	int32 index = Buffer->ReadShort();
	Controller = Owner->GetParent()->GetControllerAt(index);
	//Controller->OnChanged().AddRaw(this, &FGearBase::HandleControllerChanged);
	
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

