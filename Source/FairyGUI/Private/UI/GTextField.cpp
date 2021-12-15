#include "UI/GTextField.h"
#include "UI/GRichTextField.h"
#include "Utils/ByteBuffer.h"
#include "Utils/UBBParser.h"
#include "Widgets/STextField.h"

UGTextField::UGTextField()
	:bUBBEnabled(false)
	, bFormatApplied(false)
	, bSupportHTML(false)
{
	if (!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
	{
		DisplayObject = Content = SNew(STextField).GObject(this);
		bSupportHTML = IsA<UGRichTextField>();
		DisplayObject->SetInteractable(bSupportHTML);
	}
}

UGTextField::~UGTextField()
{
}

void UGTextField::SetText(const FString& InText)
{
	if (!bFormatApplied)
	{
		ApplyFormat();
	}

	Text = InText;

	if (bUBBEnabled)
	{
		FString parsedText = FUBBParser::DefaultParser.Parse(Text);
		if (TemplateVars.IsSet())
		{
			parsedText = ParseTemplate(parsedText);
		}
		Content->SetText(parsedText, true);
	}
	else
	{
		if (TemplateVars.IsSet())
		{
			Content->SetText(ParseTemplate(Text), bSupportHTML);
		}
		else
		{
			Content->SetText(Text, bSupportHTML);
		}
	}

	UpdateGear(6);
	UpdateSize();
}

void UGTextField::SetUBBEnabled(bool bFlag)
{
	if (bUBBEnabled != bFlag)
	{
		bUBBEnabled = bFlag;
		SetText(Text);
	}
}

EAutoSizeType UGTextField::GetAutoSize() const
{
	return Content->GetAutoSize();
}

void UGTextField::SetAutoSize(EAutoSizeType InAutoSize)
{
	Content->SetAutoSize(InAutoSize);

	if (!bUnderConstruct)
	{
		UpdateSize();
	}
}

bool UGTextField::IsSingleLine() const
{
	return Content->IsSingleLine();
}

void UGTextField::SetSingleLine(bool bFlag)
{
	Content->SetSingleLine(bFlag);
}

FNTextFormat& UGTextField::GetTextFormat()
{
	return Content->GetTextFormat();
}

void UGTextField::SetTextFormat(const FNTextFormat& InTextFormat)
{
	Content->SetTextFormat(InTextFormat);
	bFormatApplied = true;

	UpdateGear(4);
	if (!bUnderConstruct)
	{
		UpdateSize();
	}
}

void UGTextField::ApplyFormat()
{
	SetTextFormat(Content->GetTextFormat());
}

FVector2D UGTextField::GetTextSize()
{
	return FVector2D::ZeroVector;
}

UGTextField* UGTextField::SetVar(const FString& VarKey, const FString& VarValue)
{
	if (!TemplateVars.IsSet())
	{
		TemplateVars.Emplace();
	}
	TemplateVars.GetValue().Add(VarKey, VarValue);

	return this;
}

void UGTextField::FlushVars()
{
	SetText(Text);
}

FString UGTextField::ParseTemplate(const FString& Template)
{
	int32 pos1 = 0, pos2 = 0;
	int32 pos3;
	FString tag;
	FString value;
	FString buffer;
	TMap<FString, FString>& Vars = TemplateVars.GetValue();

	while ((pos2 = Template.Find("{", ESearchCase::CaseSensitive, ESearchDir::FromStart, pos1)) != -1)
	{
		if (pos2 > 0 && Template[pos2 - 1] == '\\')
		{
			buffer.Append(*Template + pos1, pos2 - pos1 - 1);
			buffer.AppendChar('{');
			pos1 = pos2 + 1;
			continue;
		}

		buffer.Append(*Template + pos1, pos2 - pos1);
		pos1 = pos2;
		pos2 = Template.Find("}", ESearchCase::CaseSensitive, ESearchDir::FromStart, pos1);
		if (pos2 == -1)
		{
			break;
		}

		if (pos2 == pos1 + 1)
		{
			buffer.Append(*Template + pos1, 2);
			pos1 = pos2 + 1;
			continue;
		}

		tag = Template.Mid(pos1 + 1, pos2 - pos1 - 1);
		if (tag.FindChar('=', pos3))
		{
			FString* ptr = Vars.Find(tag.Mid(0, pos3));
			if (ptr != nullptr)
			{
				buffer.Append(*ptr);
			}
			else
			{
				buffer.Append(tag.Mid(pos3 + 1));
			}
		}
		else
		{
			FString* ptr = Vars.Find(tag);
			if (ptr != nullptr)
			{
				buffer.Append(*ptr);
			}
		}
		pos1 = pos2 + 1;
	}

	if (pos1 < Template.Len())
	{
		buffer.Append(Template.Mid(pos1, Template.Len() - pos1));
	}

	return buffer;
}

void UGTextField::UpdateSize()
{

}

FNVariant UGTextField::GetProp(EObjectPropID PropID) const
{
	switch (PropID)
	{
	case EObjectPropID::Color:
		return FNVariant(Content->GetTextFormat().Color);
	case EObjectPropID::OutlineColor:
		return FNVariant(Content->GetTextFormat().OutlineColor);
	case EObjectPropID::FontSize:
		return FNVariant(Content->GetTextFormat().Size);
	default:
		return UFairyObject::GetProp(PropID);
	}
}

void UGTextField::SetProp(EObjectPropID PropID, const FNVariant& InValue)
{
	switch (PropID)
	{
	case EObjectPropID::Color:
		Content->GetTextFormat().Color = InValue.AsColor();
		ApplyFormat();
		break;
	case EObjectPropID::OutlineColor:
		Content->GetTextFormat().OutlineColor = InValue.AsColor();
		ApplyFormat();
		break;
	case EObjectPropID::FontSize:
		Content->GetTextFormat().Size = InValue.AsInt();
		ApplyFormat();
		break;
	default:
		UFairyObject::SetProp(PropID, InValue);
		break;
	}
}

void UGTextField::SetupBeforeAdd(FairyGUI::FByteBuffer* Buffer, int32 BeginPos)
{
	UFairyObject::SetupBeforeAdd(Buffer, BeginPos);

	Buffer->Seek(BeginPos, 5);

	FNTextFormat& TextFormat = Content->GetTextFormat();
	TextFormat.Face				= Buffer->ReadStringFromCache();
	TextFormat.Size				= Buffer->ReadShort();
	TextFormat.Color			= Buffer->ReadColor();
	TextFormat.HAlign			= (EHAlignType)Buffer->ReadByte();
	TextFormat.VAlign			= (EVAlignType)Buffer->ReadByte();
	TextFormat.LineSpacing		= Buffer->ReadShort();
	TextFormat.LetterSpacing	= Buffer->ReadShort();
	bUBBEnabled					= Buffer->ReadBool();
	TextFormat.AutoSizeType		= (EAutoSizeType)Buffer->ReadByte();
	TextFormat.bUnderline		= Buffer->ReadBool();
	TextFormat.bItalic			= Buffer->ReadBool();
	TextFormat.bBold			= Buffer->ReadBool();
	TextFormat.bSingleLine		= Buffer->ReadBool();

	if (Buffer->ReadBool())
	{
		TextFormat.OutlineColor = Buffer->ReadColor();
		TextFormat.OutlineSize	= Buffer->ReadFloat();
	}

	if (Buffer->ReadBool())
	{
		TextFormat.ShadowColor	= Buffer->ReadColor();
		float f1 = Buffer->ReadFloat();
		float f2 = Buffer->ReadFloat();
		TextFormat.ShadowOffset	= FVector2D(f1, f2);
	}

	if (Buffer->ReadBool())
	{
		TemplateVars.Emplace();
	}
}

void UGTextField::SetupAfterAdd(FairyGUI::FByteBuffer* Buffer, int32 BeginPos)
{
	UFairyObject::SetupAfterAdd(Buffer, BeginPos);
	Buffer->Seek(BeginPos, 6);

	const FString& str = Buffer->ReadStringFromCache();
	if (!str.IsEmpty())
	{
		SetText(str);
	}
	ApplyFormat();
}
