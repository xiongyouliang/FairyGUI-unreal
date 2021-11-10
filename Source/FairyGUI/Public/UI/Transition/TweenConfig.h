#pragma once

#include "CoreMinimal.h"
#include "Utils/ByteBuffer.h"
#include "UI/FieldTypes.h"
#include "Tween/GPath.h"
#include "Tween/EaseType.h"
#include "UI/Transition/TransitionItemData.h"

struct FTweenConfig {
	float Duration;
	EEaseType EaseType;
	int32 Repeat;
	bool bYoyo;

	FTransitionItemData StartData;
	FTransitionItemData EndData;
	TSharedPtr<FGPath> Path;

	FString EndLabel;
	FSimpleDelegate EndHook;

	FTweenConfig();
	virtual ~FTweenConfig() {};

	virtual void SetupStartData(FByteBuffer* InBuffer) {};
	virtual void SetupEndData(FByteBuffer* InBuffer) {};
};

FTweenConfig* CreateTwencConfigWithType(const ETransitionActionType InActionType);

struct FTweenConfigPos : public FTweenConfig
{
	struct Property 
	{
		bool bUsePercent;
		bool bChangeAxisX;
		bool bChangeAxisY;

		FVector2D Pos;
	};

	Property StartData;
	Property EndData;

	virtual void SetupStartData(FByteBuffer* InBuffer) override;
	virtual void SetupEndData(FByteBuffer* InBuffer) override;
};

struct FTweenConfigSize : public FTweenConfig
{
	bool bChangeWidth;
	bool bChangeHeight;

	FVector2D StartSize;
	FVector2D EndSize;

	virtual void SetupStartData(FByteBuffer* InBuffer) override;
	virtual void SetupEndData(FByteBuffer* InBuffer) override;
};