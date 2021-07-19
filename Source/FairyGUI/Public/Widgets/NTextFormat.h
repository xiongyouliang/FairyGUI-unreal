#pragma once

#include "Slate.h"
#include "UI/FieldTypes.h"
#include "NTextFormat.generated.h"

USTRUCT(BlueprintType)
struct FAIRYGUI_API FNTextFormat
{
	GENERATED_USTRUCT_BODY()
public:
	FNTextFormat();
	bool EqualStyle(const FNTextFormat& AnotherFormat) const;
	FTextBlockStyle GetStyle() const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
		bool bBold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
		bool bItalic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
		bool bUnderline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
		bool bSingleLine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
		int32 Size;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
		int32 LineSpacing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
		int32 LetterSpacing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
		int32 OutlineSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
		EHAlignType HAlign;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
		EVAlignType VAlign;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
		EAutoSizeType AutoSizeType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
		FVector2D ShadowOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
		FColor Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
		FColor OutlineColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
		FColor ShadowColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
		FString Face;
};