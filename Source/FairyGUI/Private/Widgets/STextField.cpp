#include "Widgets/STextField.h"
#include "Internationalization/BreakIterator.h"
#include "Utils/HTMLParser.h"
#include "Widgets/LoaderRun.h"
#include "Widgets/BitmapFontRun.h"
#include "Package/FairyPackageMgr.h"
#include "Package/FairyPackageItem.h"
#include "UI/FairyObject.h"

STextField::STextField() :
	bHTML(false),
	AutoSize(EAutoSizeType::None),
	bSingleLine(false),
	TextLayout(FSlateTextLayout::Create(this, FTextBlockStyle::GetDefault()))
{
	TextLayout->SetLineBreakIterator(FBreakIterator::CreateCharacterBoundaryIterator());
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
	FVector2D InViewSize = FVector2D::ZeroVector;
	if (GObject.IsValid())
	{
		InViewSize = GObject->GetSize();
	}

	if (AutoSize != InAutoSize)
	{
		AutoSize = InAutoSize;
		if (AutoSize == EAutoSizeType::Both)
		{
			TextLayout->SetWrappingWidth(0);
		}
		else
		{
			TextLayout->SetWrappingWidth(InViewSize.X);
		}
	}
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
	}
	TextLayout->DirtyLayout();
}

FVector2D STextField::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	if (TextLayout->IsLayoutDirty())
	{
		TextLayout->SetScale(LayoutScaleMultiplier);
		const_cast<STextField*>(this)->UpdateTextLayout();
	}
	FVector2D InViewSize = FVector2D::ZeroVector;
	if (GObject.IsValid())
	{
		InViewSize = GObject->GetSize();
	}
	return InViewSize;
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
	const FVector2D BlockSize = AllottedGeometry.GetLocalSize();
	const FVector2D ActualSize = TextLayout->GetSize();
	const FVector2D DrawSize = TextLayout->GetDrawSize();
	const FVector2D WrappedSize = TextLayout->GetWrappedSize();
	const FVector2D WrappedDrawSize = TextLayout->GetWrappedDrawSize();

	FVector2D AutoScrollOffset = FVector2D::ZeroVector; // default herizontal alignment is left and vertical alignment is top
	//if (BlockSize.X < ActualSize.X)
	//{
		if (TextFormat.HAlign == EHAlignType::Center)
		{
			AutoScrollOffset.X = (ActualSize.X - BlockSize.X) * 0.5f;
		}
		else if (TextFormat.HAlign == EHAlignType::Right)
		{
			AutoScrollOffset.X = ActualSize.X - BlockSize.X;
		}
	//}
	
	//if (BlockSize.Y < ActualSize.Y)
	//{
		if (TextFormat.VAlign == EVAlignType::Middle)
		{
			AutoScrollOffset.Y = ( ActualSize.Y - BlockSize.Y) * 0.5f;
		}
		else if (TextFormat.VAlign == EVAlignType::Bottom)
		{
			AutoScrollOffset.Y = ActualSize.Y - BlockSize.Y;
		}
	//}

	TextLayout->SetVisibleRegion(BlockSize, AutoScrollOffset);
	TextLayout->UpdateIfNeeded();

	LayerId = TextLayout->OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, ShouldBeEnabled(bParentEnabled));

	return LayerId;
}

void STextField::UpdateTextLayout()
{
	FVector2D InViewSize = FVector2D::ZeroVector;
	if (GObject.IsValid())
	{
		InViewSize = GObject->GetSize();
	}

	TextLayout->ClearLines();
	TextLayout->ClearLineHighlights();
	TextLayout->ClearRunRenderers();

	TextLayout->SetDefaultTextStyle(TextFormat.GetStyle());
	TextLayout->SetJustification((ETextJustify::Type)TextFormat.HAlign);
	TextLayout->SetWrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping);
	if (AutoSize == EAutoSizeType::Both)
	{
		TextLayout->SetWrappingWidth(0);
	}
	else
	{
		TextLayout->SetWrappingWidth(InViewSize.X);
	}
	TextLayout->SetMargin(FMargin(2, 2));
	TextLayout->SetLineHeightPercentage(1 + (TextFormat.LineSpacing - 3) / TextFormat.Size);

	HTMLElements.Reset();
	if (bHTML)
	{
		FHTMLParser::DefaultParser.Parse(Text, TextFormat, HTMLElements, FHTMLParser::DefaultParseOptions);
	}
	else
	{
		FHTMLElement TextElement;
		TextElement.Type = EHTMLElementType::Text;
		TextElement.Format = TextFormat;
		TextElement.Text = Text;
		HTMLElements.Add(MoveTemp(TextElement));
	}

	BuildLines();

	TextLayout->UpdateIfNeeded();

	//if (AutoSize == EAutoSizeType::Both)
	//{
	//	SetSize(TextLayout->GetSize());
	//}
	//else if (AutoSize == EAutoSizeType::Height)
	//{
	//	SetSize(FVector2D(InViewSize.X, TextLayout->GetSize().Y));
	//}
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

	TArray<FTextRange> LineRangesBuffer;
	for (int32 ElementIndex = 0; ElementIndex < HTMLElements.Num(); ++ElementIndex)
	{
		const FHTMLElement& Element = HTMLElements[ElementIndex];
		if (Element.Type == EHTMLElementType::Text)
		{
			LineRangesBuffer.Reset();

			FTextBlockStyle TextStyle = Element.Format.GetStyle();

			FTextRange::CalculateLineRangesFromString(Element.Text, LineRangesBuffer);

			for (int32 LineIndex = 0; LineIndex < LineRangesBuffer.Num(); ++LineIndex)
			{
				const FTextRange& LineRange = LineRangesBuffer[LineIndex];
				FString TextBlock = Element.Text.Mid(LineRange.BeginIndex, LineRange.Len());
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
					FTextRange ModelRange;
					ModelRange.BeginIndex = LineHelper.GetText().Len();
					LineHelper.GetText().Append(TextBlock);
					ModelRange.EndIndex = LineHelper.GetText().Len();

					LineHelper.GetRuns().Add(FSlateTextRun::Create(FRunInfo(), LineHelper.GetTextRef(), TextStyle, ModelRange));
				}

				if (LineIndex != LineRangesBuffer.Num() - 1)
				{
					LineHelper.bNewLine = true;
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