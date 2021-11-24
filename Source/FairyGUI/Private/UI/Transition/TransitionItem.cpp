// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/TransitionItem.h"
#include "UI/FieldTypes.h"
#include "Utils/ByteBuffer.h"

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
	TweenConfigPtr->EaseType = (EFairyEaseType)InBuffer->ReadByte();
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
	InBuffer->Seek(curPos, 2);

	frameData.bChangeAxisX = InBuffer->ReadBool();
	frameData.bChangeAxisY = InBuffer->ReadBool();

	frameData.Pos.X = InBuffer->ReadFloat();
	frameData.Pos.Y = InBuffer->ReadFloat();

	if (InBuffer->Version >= 2)
	{
		frameData.bUsePercent = InBuffer->ReadBool();
	}
}

void FTransitionItemPos::RunItem()
{

}


