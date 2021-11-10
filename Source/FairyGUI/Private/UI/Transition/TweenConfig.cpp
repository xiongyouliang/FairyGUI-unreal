#include "UI/Transition/TweenConfig.h"

FTweenConfig* CreateTwencConfigWithType(ETransitionActionType InActionType)
{
	FTweenConfig* tweenConfig = nullptr;
	switch (InActionType)
	{
	case ETransitionActionType::XY:
		tweenConfig = new FTweenConfigPos();
		break;
	case ETransitionActionType::Size:
		tweenConfig = new FTweenConfigSize();
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
	return tweenConfig;
}

FTweenConfig::FTweenConfig() : EaseType(EEaseType::QuadOut) {}

void FTweenConfigPos::SetupStartData(FByteBuffer* InBuffer)
{
	StartData.bChangeAxisX = InBuffer->ReadBool();
	StartData.bChangeAxisY = InBuffer->ReadBool();
	StartData.Pos.X = InBuffer->ReadFloat();
	StartData.Pos.Y = InBuffer->ReadFloat();

	if (InBuffer->Version >= 2)
	{
		StartData.bUsePercent = InBuffer->ReadBool(); //percent
	}
}

void FTweenConfigPos::SetupEndData(FByteBuffer* InBuffer)
{
	EndData.bChangeAxisX = InBuffer->ReadBool();
	EndData.bChangeAxisY = InBuffer->ReadBool();
	EndData.Pos.X = InBuffer->ReadFloat();
	EndData.Pos.Y = InBuffer->ReadFloat();

	if (InBuffer->Version >= 2)
	{
		EndData.bUsePercent = InBuffer->ReadBool(); //percent
	}
}

void FTweenConfigSize::SetupStartData(FByteBuffer* InBuffer)
{
	bChangeWidth = InBuffer->ReadBool();
	bChangeHeight = InBuffer->ReadBool();

	StartSize.X = InBuffer->ReadFloat();
	StartSize.Y = InBuffer->ReadFloat();
}

void FTweenConfigSize::SetupEndData(FByteBuffer* InBuffer)
{
	bChangeWidth = InBuffer->ReadBool();
	bChangeHeight = InBuffer->ReadBool();

	EndSize.X = InBuffer->ReadFloat();
	EndSize.Y = InBuffer->ReadFloat();
}
