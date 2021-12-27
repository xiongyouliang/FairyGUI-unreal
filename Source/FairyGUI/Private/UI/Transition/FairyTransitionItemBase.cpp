// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/FairyTransitionItemBase.h"
#include "UI/Transition/FairyTransitionItemPos.h"
#include "UI/Transition/FairyTransition.h"
#include "UI/FairyComponent.h"
#include "UI/FieldTypes.h"
#include "Utils/ByteBuffer.h"

FFairyTransitionItemBase::FFairyTransitionItemBase(EFairyTransitionItemType InItemType)
	:ItemType(InItemType)
{
	
}

FFairyTransitionItemBase::~FFairyTransitionItemBase()
{

}

void FFairyTransitionItemBase::SetOwner(UFairyTransition* InOwner)
{
	OwnerPtr = InOwner;
}

UFairyTransition* FFairyTransitionItemBase::GetOwner()
{
	if (OwnerPtr.IsValid())
	{
		return OwnerPtr.Get();
	}
	return nullptr;
}

void FFairyTransitionItemBase::SetTarget(UFairyObject* InTargetObject)
{
	TargetPtr = InTargetObject;
}

UFairyObject* FFairyTransitionItemBase::GetTarget()
{
	return TargetPtr.Get();
}

bool FFairyTransitionItemBase::IsTargetValid()
{
	return TargetPtr.IsValid();
}

bool FFairyTransitionItemBase::IsHasTween()
{
	return bHasTween;
}

void FFairyTransitionItemBase::ParseBaseData(FairyGUI::FByteBuffer* InBuffer, int32 curPos)
{
	StartTime = InBuffer->ReadFloat();
	TargetIndex = InBuffer->ReadShort();
	Label = InBuffer->ReadFNameFromCache();
	bHasTween = InBuffer->ReadBool();

	UE_LOG(LogFairyGUI, Log, TEXT("FairyTransitionItemBase::ParseBaseData(...), StartTime:%f-%d"), StartTime, sizeof(TOptional<FFariyTransitionTweenConfig>));

	if (OwnerPtr.IsValid())
	{
		UFairyComponent* TargetComponent = OwnerPtr->GetTargetComponent();

		if (TargetIndex == -1)
		{
			TargetPtr = TargetComponent;
		}
		else
		{
			UFairyObject* TargetObject = TargetComponent->GetChildAt(TargetIndex);
			TargetPtr = TargetObject;
		}
	}
}

void FFairyTransitionItemBase::Setup(FairyGUI::FByteBuffer* InBuffer, int32 curPos)
{
	ParseBaseData(InBuffer, curPos);

	if (bHasTween)
	{
		ParseTweenKeyFrameData(InBuffer, curPos);
	}
	else
	{
		ParseNoTweenKeyFrameData(InBuffer, curPos);
	}
}

FFairyTransitionTweenableItem::FFairyTransitionTweenableItem(EFairyTransitionItemType InItemType)
	:FFairyTransitionItemBase(InItemType)
{

}

void FFairyTransitionTweenableItem::ParseTweenKeyFrameData(FairyGUI::FByteBuffer* InBuffer, int32 curPos)
{
	TweenConfigPtr = MakeShareable(new FFariyTransitionTweenConfig);

	InBuffer->Seek(curPos, 1);
	TweenConfigPtr->Duration = InBuffer->ReadFloat();
	TweenConfigPtr->EaseType = (EFairyEaseType)InBuffer->ReadByte();
	TweenConfigPtr->Repeat = InBuffer->ReadInt();
	TweenConfigPtr->bYoyo = InBuffer->ReadBool();

	
	//TweenConfigPtr->EndLabel = InBuffer->ReadStringFromCache();
	InBuffer->ReadStringFromCache();

	InBuffer->Seek(curPos, 2);
	ParseTweenStartData(InBuffer);

	InBuffer->Seek(curPos, 3);
	ParseTweenEndData(InBuffer);

	ParsePathData(InBuffer);
}

void FFairyTransitionTweenableItem::ParseNoTweenKeyFrameData(FairyGUI::FByteBuffer* InBuffer, int32 curPos)
{
	InBuffer->Seek(curPos, 2);
	ParseTweenStartData(InBuffer);
}

FFairyTransitionItemBase* CreateTransitionItemWithType(const EFairyTransitionItemType InActionType)
{
	FFairyTransitionItemBase *item = nullptr;
	switch (InActionType)
	{
	case EFairyTransitionItemType::XY:
		item = new FFairyTransitionItemPos();
		break;
	case EFairyTransitionItemType::Size:
		break;
	case EFairyTransitionItemType::Scale:
		break;
	case EFairyTransitionItemType::Pivot:
		break;
	case EFairyTransitionItemType::Alpha:
		break;
	case EFairyTransitionItemType::Rotation:
		break;
	case EFairyTransitionItemType::Color:
		break;
	case EFairyTransitionItemType::Animation:
		break;
	case EFairyTransitionItemType::Visible:
		break;
	case EFairyTransitionItemType::Sound:
		break;
	case EFairyTransitionItemType::Transition:
		break;
	case EFairyTransitionItemType::Shake:
		break;
	case EFairyTransitionItemType::ColorFilter:
		break;
	case EFairyTransitionItemType::Skew:
		break;
	case EFairyTransitionItemType::Text:
		break;
	case EFairyTransitionItemType::Icon:
		break;
	case EFairyTransitionItemType::Unknown:
		break;
	default:
		break;
	}

	return item;
}




