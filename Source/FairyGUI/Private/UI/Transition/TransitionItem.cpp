// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/TransitionItem.h"
#include "UI/FieldTypes.h"
#include "Utils/ByteBuffer.h"

FTransitionItem::FTransitionItem(ETransitionActionType InType) :
	Time(0),
	Type(InType),
	Tweener(nullptr),
	Target(nullptr),
	DisplayLockToken(0)
{
	switch (InType)
	{
	case ETransitionActionType::XY:
	case ETransitionActionType::Size:
	case ETransitionActionType::Scale:
	case ETransitionActionType::Pivot:
	case ETransitionActionType::Skew:
	case ETransitionActionType::Alpha:
	case ETransitionActionType::Rotation:
	case ETransitionActionType::Color:
	case ETransitionActionType::ColorFilter:
		Data.Emplace();
		break;

	case ETransitionActionType::Animation:
		AniData.Emplace();
		break;

	case ETransitionActionType::Shake:
		ShakeData.Emplace();
		break;

	case ETransitionActionType::Sound:
		SoundData.Emplace();
		break;

	case ETransitionActionType::Transition:
		TransData.Emplace();
		break;

	case ETransitionActionType::Visible:
		VisibleData.Emplace();
		break;

	case ETransitionActionType::Text:
	case ETransitionActionType::Icon:
		TextData.Emplace();
		break;

	default:
		break;
	}
}

FTransitionItem::~FTransitionItem()
{
	if (Tweener != nullptr)
	{
		Tweener->Kill();
	}
}

FTransitionItemBase::FTransitionItemBase(ETransitionActionType InActionType)
	:ActionType(InActionType)
{
	
}

FTransitionItemBase::~FTransitionItemBase()
{

}

void FTransitionItemBase::ParseBaseData(FByteBuffer* InBuffer, int32 curPos)
{
	startTime = InBuffer->ReadFloat();
	targetID = InBuffer->ReadShort();
	label = InBuffer->ReadS();
	bHasTween = InBuffer->ReadBool();
}

void FTransitionItemBase::ParseTweenData(FByteBuffer* InBuffer, int32 curPos)
{
	TweenConfigPtr = MakeShareable(CreateTwencConfigWithType(ActionType));

	InBuffer->Seek(curPos, 1);
	TweenConfigPtr->Duration = InBuffer->ReadFloat();
	TweenConfigPtr->EaseType = (EEaseType)InBuffer->ReadByte();
	TweenConfigPtr->Repeat = InBuffer->ReadInt();
	TweenConfigPtr->bYoyo = InBuffer->ReadBool();
	TweenConfigPtr->EndLabel = InBuffer->ReadS();

	InBuffer->Seek(curPos, 2);
	TweenConfigPtr->SetupStartData(InBuffer);

	InBuffer->Seek(curPos, 3);
	TweenConfigPtr->SetupEndData(InBuffer);

	if (InBuffer->Version >= 2)
	{
		int32 pathLen = InBuffer->ReadInt();
		if (pathLen > 0)
		{
			TweenConfigPtr->Path = MakeShareable(new FGPath());
			TArray<FGPathPoint> pts;

			FVector v0(ForceInit), v1(ForceInit), v2(ForceInit);

			for (int32 j = 0; j < pathLen; j++)
			{
				FGPathPoint::ECurveType curveType = (FGPathPoint::ECurveType)InBuffer->ReadByte();
				switch (curveType)
				{
				case FGPathPoint::ECurveType::Bezier:
					v0.X = InBuffer->ReadFloat();
					v0.Y = InBuffer->ReadFloat();
					v1.X = InBuffer->ReadFloat();
					v1.Y = InBuffer->ReadFloat();
					pts.Add(FGPathPoint(v0, v1));
					break;
				case FGPathPoint::ECurveType::CubicBezier:
					v0.X = InBuffer->ReadFloat();
					v0.Y = InBuffer->ReadFloat();
					v1.X = InBuffer->ReadFloat();
					v1.Y = InBuffer->ReadFloat();
					v2.X = InBuffer->ReadFloat();
					v2.Y = InBuffer->ReadFloat();
					pts.Add(FGPathPoint(v0, v1, v2));
					break;
				default:
					v0.X = InBuffer->ReadFloat();
					v0.Y = InBuffer->ReadFloat();
					pts.Add(FGPathPoint(v0, curveType));
					break;
				}
			}
			TweenConfigPtr->Path->Create(pts.GetData(), pts.Num());
		}
	}
}

void FTransitionItemBase::ParseKeyFrameData(FByteBuffer* InBuffer, int32 curPos)
{

}

void FTransitionItemBase::Setup(FByteBuffer* InBuffer, int32 curPos)
{
	ParseBaseData(InBuffer, curPos);
	if (bHasTween)
	{
		ParseTweenData(InBuffer, curPos);
	}
	else
	{
		//InBuffer->Seek(curPos, 2);
		//DecodeValue(item, InBuffer, item->Data.IsSet() ? &item->Data.GetValue() : nullptr);
		ParseKeyFrameData(InBuffer, curPos);
	}
}

FTransitionItemBase* CreateTransitionItemWithType(const ETransitionActionType InActionType)
{
	FTransitionItemBase *item = nullptr;
	switch (InActionType)
	{
	case ETransitionActionType::XY:
		item = new FTransitionItemPos();
		break;
	case ETransitionActionType::Size:
		break;
	case ETransitionActionType::Scale:
		break;
	case ETransitionActionType::Pivot:
		break;
	case ETransitionActionType::Alpha:
		break;
	case ETransitionActionType::Rotation:
		break;
	case ETransitionActionType::Color:
		break;
	case ETransitionActionType::Animation:
		break;
	case ETransitionActionType::Visible:
		break;
	case ETransitionActionType::Sound:
		break;
	case ETransitionActionType::Transition:
		break;
	case ETransitionActionType::Shake:
		break;
	case ETransitionActionType::ColorFilter:
		break;
	case ETransitionActionType::Skew:
		break;
	case ETransitionActionType::Text:
		break;
	case ETransitionActionType::Icon:
		break;
	case ETransitionActionType::Unknown:
		break;
	default:
		break;
	}

	return item;
}

FTransitionItemPos::FTransitionItemPos()
	:FTransitionItemBase(ETransitionActionType::XY)
{
}

void FTransitionItemPos::ParseKeyFrameData(FByteBuffer* InBuffer, int32 curPos)
{

}

void FTransitionItemPos::RunItem()
{

}


