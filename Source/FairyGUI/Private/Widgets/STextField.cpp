#include "Widgets/STextField.h"
#include "Internationalization/BreakIterator.h"
#include "Utils/HTMLParser.h"
#include "Widgets/LoaderRun.h"
#include "Widgets/BitmapFontRun.h"
#include "Package/FairyPackageMgr.h"
#include "Package/FairyPackageItem.h"
#include "UI/FairyObject.h"
#include "Framework/Text/PlainTextLayoutMarshaller.h"

STextField::STextField() :
	bHTML(false),
	AutoSize(EAutoSizeType::None),
	bSingleLine(false),
	TextLayout(FSlateTextLayout::Create(this, FTextBlockStyle::GetDefault()))
{
	TextLayout->SetLineBreakIterator(FBreakIterator::CreateCharacterBoundaryIterator());
	TextLayout->SetTextFlowDirection(ETextFlowDirection::LeftToRight);

	Marshaller = FPlainTextLayoutMarshaller::Create();
}

void STextField::Construct(const FArguments& InArgs)
{
	SDisplayObject::Construct(SDisplayObject::FArguments().GObject(InArgs._GObject));
}

void STextField::SetText(const FString& InText, bool bInHTML)
{
	const int32 OldLength = Text.Len();

	// Only compare reasonably sized strings, it's not worth checking this
	// for large blocks of text.
	if (bHTML == bInHTML && OldLength <= 20)
	{
		if (InText.Compare(Text, ESearchCase::CaseSensitive) == 0)
		{
			return;
		}
	}

	Text = InText;
	bHTML = bInHTML;
	TextLayout->DirtyLayout();
	Invalidate(EInvalidateWidget::LayoutAndVolatility);
}

void STextField::SetAutoSize(EAutoSizeType InAutoSize)
{
	//FVector2D InViewSize = FVector2D::ZeroVector;
	//if (GObject.IsValid())
	//{
	//	InViewSize = GObject->GetSize();
	//}

	//if (AutoSize != InAutoSize)
	//{
	//	AutoSize = InAutoSize;
	//	if (AutoSize == EAutoSizeType::Both)
	//	{
	//		TextLayout->SetWrappingWidth(0);
	//	}
	//	else
	//	{
	//		TextLayout->SetWrappingWidth(InViewSize.X);
	//	}
	//}
}

void STextField::SetSingleLine(bool bInSingleLine)
{
	if (bSingleLine != bInSingleLine)
	{
		bSingleLine = bInSingleLine;
		TextLayout->DirtyLayout();
	}
}

void STextField::SetTextFormat(const FNTextFormat& InFormat)
{
	if (&InFormat != &TextFormat)
	{
		TextFormat = InFormat;
		TextLayout->DirtyLayout();
	}
}

FVector2D STextField::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	FVector2D InViewSize = FVector2D::ZeroVector;
	TWeakObjectPtr<UFairyObject> ObjectPtr = GetFairyObject();
	if (ObjectPtr.IsValid())
	{
		InViewSize = ObjectPtr->GetRelationSize();
	}

	if (TextFormat.bSingleLine)
	{
		TextLayout->SetWrappingWidth(0.0f);
	}
	else
	{
		TextLayout->SetWrappingWidth(InViewSize.X);
	}
	TextLayout->SetWrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping);
	TextLayout->SetScale(LayoutScaleMultiplier);
	TextLayout->SetDefaultTextStyle(TextFormat.GetStyle());
	TextLayout->SetJustification((ETextJustify::Type)TextFormat.HAlign); // horizontal only

	float NewPercentage = FMath::Max(1.0f, 1.0f + (TextFormat.LineSpacing - 3) / TextFormat.Size);
	TextLayout->SetLineHeightPercentage(NewPercentage); // as line spacing

	if (TextLayout->IsLayoutDirty())
	{
		const_cast<STextField*>(this)->UpdateTextLayout();
	}
	TextLayout->UpdateIfNeeded();
	return TextLayout->GetSize();
}

void STextField::EnsureSizeCorrect()
{
	if (TextLayout->IsLayoutDirty() && (AutoSize == EAutoSizeType::Both || AutoSize == EAutoSizeType::Height))
	{
		UpdateTextLayout();
	}
}

FChildren* STextField::GetChildren()
{
	return TextLayout->GetChildren();
}

void STextField::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	TextLayout->ArrangeChildren(AllottedGeometry, ArrangedChildren);
}

int32 STextField::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const FVector2D WidgetSize = AllottedGeometry.GetLocalSize();
	const FVector2D TextLayoutSize = TextLayout->GetSize();

	FVector2D AutoScrollOffset = FVector2D::ZeroVector; // default herizontal alignment is left and vertical alignment is top
	if (WidgetSize.X < TextLayoutSize.X)
	{
		if (TextFormat.HAlign == EHAlignType::Center)
		{
			AutoScrollOffset.X = (TextLayoutSize.X - WidgetSize.X) * 0.5f;
		}
		else if (TextFormat.HAlign == EHAlignType::Right)
		{
			AutoScrollOffset.X = TextLayoutSize.X - WidgetSize.X;
		}
	}
	
	// TextLayout has no vertical align attribute, use AutoScrollOffset instead
	if (TextFormat.VAlign == EVAlignType::Middle)
	{
		AutoScrollOffset.Y = (TextLayoutSize.Y - WidgetSize.Y) * 0.5f;
	}
	else if (TextFormat.VAlign == EVAlignType::Bottom)
	{
		AutoScrollOffset.Y = TextLayoutSize.Y - WidgetSize.Y;
	}

	TextLayout->SetVisibleRegion(WidgetSize, AutoScrollOffset*TextLayout->GetScale());
	TextLayout->UpdateIfNeeded();

	return TextLayout->OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, ShouldBeEnabled(bParentEnabled));
}

void STextField::UpdateTextLayout()
{
	Marshaller->ClearDirty();
	TextLayout->ClearLines();
	TextLayout->ClearLineHighlights();
	TextLayout->ClearRunRenderers();

	Marshaller->SetText(Text, *TextLayout);
}

void STextField::BuildLines()
{
	class FLineHelper
	{
	public:
		TArray<FTextLayout::FNewLineData> Lines;
		FTextLayout::FNewLineData* LastLineData;
		bool bNewLine;

		FLineHelper()
		{
			NewLine();
		}

		TSharedRef<FString>& GetTextRef()
		{
			if (bNewLine)
			{
				NewLine();
			}
			return LastLineData->Text;
		}

		FString& GetText()
		{
			if (bNewLine)
			{
				NewLine();
			}
			return LastLineData->Text.Get();
		}

		TArray<TSharedRef<IRun>>& GetRuns()
		{
			if (bNewLine)
			{
				NewLine();
			}
			return LastLineData->Runs;
		}

	private:
		void NewLine()
		{
			Lines.Emplace(MakeShareable(new FString()), TArray<TSharedRef<IRun>>());
			LastLineData = &Lines.Last();
			bNewLine = false;
		}
	} LineHelper;

	TSharedPtr<FBitmapFont> BitmapFont;
	if (TextFormat.Face.StartsWith("ui://"))
	{
		TSharedPtr<FFairyPackageItem> FontItem = UFairyPackageMgr::Get()->GetPackageItemByURL(TextFormat.Face);
		if (FontItem.IsValid())
		{
			FontItem->Load();
			BitmapFont = FontItem->BitmapFont;
		}
	}

	TArray<FTextRange> LineRanges;
	for (int32 ElementIndex = 0; ElementIndex < HTMLElements.Num(); ++ElementIndex)
	{
		const FHTMLElement& Element = HTMLElements[ElementIndex];
		if (Element.Type == EHTMLElementType::Text)
		{
			LineRanges.Reset();

			FTextBlockStyle TextStyle = Element.Format.GetStyle();

			FTextRange::CalculateLineRangesFromString(Element.Text, LineRanges);

			for (int32 LineIndex = 0; LineIndex < LineRanges.Num(); ++LineIndex)
			{
				const FTextRange& LineRange = LineRanges[LineIndex];
				FString TextBlock = Element.Text.Mid(LineRange.BeginIndex, LineRange.Len());
				
				LineHelper.bNewLine = true;
				if (BitmapFont.IsValid())
				{
					int32 len = TextBlock.Len();
					for (int32 CharIndex = 0; CharIndex < len; CharIndex++)
					{
						FTextRange ModelRange;
						ModelRange.BeginIndex = LineHelper.GetText().Len();
						LineHelper.GetText().AppendChar(TextBlock[CharIndex]);
						ModelRange.EndIndex = LineHelper.GetText().Len();
						LineHelper.GetRuns().Add(FBitmapFontRun::Create(LineHelper.GetTextRef(), BitmapFont.ToSharedRef(), 0, ModelRange));
					}
				}
				else
				{
					TSharedRef<FString> LineText = MakeShareable(new FString(TextBlock));
					LineHelper.GetRuns().Add(FSlateTextRun::Create(FRunInfo(), LineText, TextStyle));
				}
			}
		}
		else if (Element.Type == EHTMLElementType::Image)
		{
			FTextRange ModelRange;
			ModelRange.BeginIndex = LineHelper.GetText().Len();
			LineHelper.GetText().Append(TEXT("\x200B")); // Zero-Width Breaking Space
			ModelRange.EndIndex = LineHelper.GetText().Len();
			LineHelper.GetRuns().Add(FLoaderRun::Create(Element, LineHelper.GetTextRef(), 0, ModelRange));
		}
	}

	TextLayout->AddLines(LineHelper.Lines);
}