#include "Widgets/NTextFormat.h"
#include "UI/FairyConfig.h"
#include "Engine/Font.h"

FNTextFormat::FNTextFormat() :
	bBold(false),
	bItalic(false),
	bUnderline(false),
	bSingleLine(false),
	Size(12),
	LineSpacing(3),
	LetterSpacing(0),
	OutlineSize(0),
	HAlign(EHAlignType::Left),
	VAlign(EVAlignType::Top),
	AutoSizeType(EAutoSizeType::None),
	ShadowOffset(ForceInit),
	Color(FColor::White),
	OutlineColor(FColor::Black),
	ShadowColor(FColor::Black)
{

}

bool FNTextFormat::EqualStyle(const FNTextFormat& AnotherFormat) const
{
	return Size == AnotherFormat.Size
		&& Color == AnotherFormat.Color
		&& bBold == AnotherFormat.bBold
		&& bUnderline == AnotherFormat.bUnderline
		&& bItalic == AnotherFormat.bItalic
		&& HAlign == AnotherFormat.HAlign
		&& VAlign == AnotherFormat.VAlign;
}

FTextBlockStyle FNTextFormat::GetStyle() const
{
	FTextBlockStyle Style;
	
	FString TempStr;
	if (!Face.IsEmpty())
	{
		TempStr = Face.TrimStart();
	}
	else if (!UFairyConfig::Config->DefaultFont.IsEmpty())
	{
		TempStr = UFairyConfig::Config->DefaultFont.TrimStart();
	}
	FString FontName = TempStr.Replace(TEXT(" "), TEXT(""));
	if (!FontName.StartsWith("ui://"))
	{
		int32 TargetSize = FMath::CeilToInt(Size * 0.75);

		// make sure use right UFont* object, FCoreStyle::GetDefaultFontStyle() return a FSlateFontInfo object without UFont* object
		FSlateFontInfo TargetFontInfo = FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), TargetSize);
		
		// todo start: This is a test and verify code, Fonts objects need load and manager with some other Manager(or Object)
		UFont* TargetFontPtr = nullptr;
		if (!FontName.IsEmpty())
		{
			FString FullFontAssetPath = TEXT("/Game/Fonts/") + FontName;
			if (FPaths::FileExists(FullFontAssetPath))
			{
				TargetFontPtr = LoadObject<UFont>(nullptr, *FullFontAssetPath);
			}
		}
		// todo end
		if (TargetFontPtr)
		{
			TargetFontInfo = FSlateFontInfo(TargetFontPtr, TargetSize);
		}

		// set FSlateFontInfo's bold and italic attribute
		if (bBold && bItalic)
		{
			TargetFontInfo.TypefaceFontName = TEXT("BoldItalic");
		}
		else
		{
			if (bBold)
			{
				TargetFontInfo.TypefaceFontName = TEXT("Bold");
			}
			else if (bItalic)
			{
				TargetFontInfo.TypefaceFontName = TEXT("Italic");
			}
			else
			{
				TargetFontInfo.TypefaceFontName = TEXT("Regular");
			}
		}
		TargetFontInfo.OutlineSettings.OutlineSize = OutlineSize;
		TargetFontInfo.OutlineSettings.OutlineColor = OutlineColor;
		Style.SetFont(TargetFontInfo);
	}

	Style.SetColorAndOpacity(FSlateColor(FLinearColor(Color)));
	Style.SetShadowOffset(ShadowOffset);
	Style.SetShadowColorAndOpacity(ShadowColor);
	// todo: strike and underline not implement

	return MoveTemp(Style);
}