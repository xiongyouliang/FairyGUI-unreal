#pragma once

#include "CoreMinimal.h"
#include "Utils/ByteBuffer.h"
#include "UI/FieldTypes.h"
#include "Tween/GPath.h"
#include "Tween/FairyEaseType.h"
#include "UI/Transition/TransitionItemData.h"

struct FTweenConfig {
	float Duration;
	EFairyEaseType EaseType;
	int32 Repeat;
	bool bYoyo;

	TSharedPtr<FGPath> Path;

	FString EndLabel;
	FSimpleDelegate EndHook;

	FTweenConfig();
	virtual ~FTweenConfig() {};

	virtual void SetupStartData(FByteBuffer* InBuffer) {};
	virtual void SetupEndData(FByteBuffer* InBuffer) {};
};

FTweenConfig* CreateTwencConfigWithType(const ETransitionActionType InActionType);

struct KeyFrameDataPos
{
	bool bUsePercent = false;
	bool bChangeAxisX = true;
	bool bChangeAxisY = true;
	FVector2D Pos = FVector2D::ZeroVector;
};

struct FTweenConfigPos : public FTweenConfig
{
	KeyFrameDataPos StartData;
	KeyFrameDataPos EndData;

	virtual void SetupStartData(FByteBuffer* InBuffer) override;
	virtual void SetupEndData(FByteBuffer* InBuffer) override;
};

struct KeyFrameDataSize 
{
	bool bChangeWidth = true;
	bool bChangeHeight = true;
	FVector2D Size = FVector2D::ZeroVector;
};

struct FTweenConfigSize : public FTweenConfig
{
	KeyFrameDataSize StartData;
	KeyFrameDataSize EndData;

	virtual void SetupStartData(FByteBuffer* InBuffer) override;
	virtual void SetupEndData(FByteBuffer* InBuffer) override;
};