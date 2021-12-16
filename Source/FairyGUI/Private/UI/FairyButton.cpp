#include "UI/FairyButton.h"
#include "UI/GTextField.h"
#include "UI/GLabel.h"
#include "UI/Controller/FairyController.h"
#include "Utils/ByteBuffer.h"

#include "FairyApplication.h"

const FString UFairyButton::UP = TEXT("up");
const FString UFairyButton::DOWN = TEXT("down");
const FString UFairyButton::OVER = TEXT("over");
const FString UFairyButton::SELECTED_OVER = TEXT("selectedOver");
const FString UFairyButton::DISABLED = TEXT("disabled");
const FString UFairyButton::SELECTED_DISABLED = TEXT("selectedDisabled");

UFairyButton::UFairyButton() :
	bChangeStateOnClick(true),
	DownEffectValue(0.8f)
{
	
}

UFairyButton::~UFairyButton()
{
}

void UFairyButton::SetText(const FString& InText)
{
	Title = InText;
	if (TitleObject != nullptr)
	{
		TitleObject->SetText(InText);
	}
	UpdateGear(6);
}

const FString& UFairyButton::GetIcon() const
{
	if (IconObject != nullptr)
	{
		return IconObject->GetIcon();
	}
	else
	{
		return G_EMPTY_STRING;
	}
}

void UFairyButton::SetIcon(const FString & InIcon)
{
	if (IconObject != nullptr)
	{
		IconObject->SetIcon(InIcon);
	}
	UpdateGear(7);
}

void UFairyButton::SetSelectedTitle(const FString& InTitle)
{
	SelectedTitle = InTitle;
	if (TitleObject != nullptr)
	{
		TitleObject->SetText((bSelected && SelectedTitle.Len() > 0) ? SelectedTitle : Title);
	}
}

void UFairyButton::SetSelectedIcon(const FString& InIcon)
{
	SelectedIcon = InIcon;
	if (IconObject != nullptr)
	{
		IconObject->SetIcon((bSelected && SelectedIcon.Len() > 0) ? SelectedIcon : Icon);
	}
}

FColor UFairyButton::GetTitleColor() const
{
	UGTextField* TextField = GetTextField();
	if (TextField)
	{
		return TextField->GetTextFormat().Color;

	}
	else
	{
		return FColor::Black;
	}
}

void UFairyButton::SetTitleColor(const FColor & InColor)
{
	UGTextField* TextField = GetTextField();
	if (TextField)
	{
		TextField->GetTextFormat().Color = InColor;
		TextField->ApplyFormat();
	}
}

int32 UFairyButton::GetTitleFontSize() const
{
	UGTextField* TextField = GetTextField();
	if (TextField)
	{
		return TextField->GetTextFormat().Size;
	}
	else
	{
		return 0;
	}
}

void UFairyButton::SetTitleFontSize(int32 InFontSize)
{
	UGTextField* TextField = GetTextField();
	if (TextField)
	{
		TextField->GetTextFormat().Size = InFontSize;
		TextField->ApplyFormat();
	}
}

void UFairyButton::SetSelected(bool bInSelected)
{
	if (Mode == EButtonMode::Common)
	{
		return;
	}

	if (bSelected != bInSelected)
	{
		bSelected = bInSelected;
		SetCurrentState();
		if (!SelectedTitle.IsEmpty() && TitleObject != nullptr)
		{
			TitleObject->SetText(bSelected ? SelectedTitle : Title);
		}

		if (!SelectedIcon.IsEmpty())
		{
			const FString& str = bSelected ? SelectedIcon : Icon;
			if (IconObject != nullptr)
			{
				IconObject->SetIcon(str);
			}
		}

		if (RelatedController != nullptr && GetParent() != nullptr && !GetParent()->bBuildingDisplayList)
		{
			if (bSelected)
			{
				RelatedController->SetSelectedPageID(RelatedPageID);
				if (RelatedController->bAutoRadioGroupDepth)
				{
					GetParent()->AdjustRadioGroupDepth(this, RelatedController);
				}
			}
			else if (Mode == EButtonMode::Check && RelatedController->GetSelectedPageID() == RelatedPageID)
			{
				RelatedController->SetOppositePageID(RelatedPageID);
			}
		}
	}
}

void UFairyButton::SetRelatedController(UFairyController* InController)
{
	RelatedController = InController;
}

void UFairyButton::SetState(const FString& InState)
{
	if (ButtonController != nullptr)
	{
		ButtonController->SetSelectedPage(InState);
	}

	if (DownEffect == EButtonPressedEffect::Dark)
	{
		int32 cnt = this->NumChildren();
		if (InState == UFairyButton::DOWN || InState == UFairyButton::SELECTED_OVER || InState == UFairyButton::SELECTED_DISABLED)
		{
			int32 c = DownEffectValue * 255;
			FNVariant Color(FColor(c, c, c, 255));
			for (int32 i = 0; i < cnt; i++)
			{
				UFairyObject* Obj = this->GetChildAt(i);
				if (!Obj->IsA<UGTextField>())
				{
					Obj->SetProp(EObjectPropID::Color, Color);
				}
			}
		}
		else
		{
			FNVariant Color(FColor::White);
			for (int32 i = 0; i < cnt; i++)
			{
				UFairyObject* Obj = this->GetChildAt(i);
				if (!Obj->IsA<UGTextField>())
				{
					Obj->SetProp(EObjectPropID::Color, Color);
				}
			}
		}
	}
	else if (DownEffect == EButtonPressedEffect::Scale)
	{
		if (InState == UFairyButton::DOWN || InState == UFairyButton::SELECTED_OVER || InState == UFairyButton::SELECTED_DISABLED)
		{
			if (!bDownScaled)
			{
				bDownScaled = true;
				SetScale(GetScale() * DownEffectValue);
			}
		}
		else
		{
			if (bDownScaled)
			{
				bDownScaled = false;
				SetScale(GetScale() / DownEffectValue);
			}
		}
	}
}

void UFairyButton::SetCurrentState()
{
	if (IsGrayed() && ButtonController != nullptr && ButtonController->HasPage(DISABLED))
	{
		if (bSelected)
		{
			SetState(UFairyButton::SELECTED_DISABLED);
		}
		else
		{
			SetState(UFairyButton::DISABLED);
		}
	}
	else
	{
		if (bSelected)
		{
			SetState(bOver ? UFairyButton::SELECTED_OVER : UFairyButton::DOWN);
		}
		else
		{
			SetState(bOver ? UFairyButton::OVER : UFairyButton::UP);
		}
	}
}

UGTextField * UFairyButton::GetTextField() const
{
	if (TitleObject->IsA<UGTextField>())
	{
		return Cast<UGTextField>(TitleObject);
	}
	else if (TitleObject->IsA<UGLabel>())
	{
		return Cast<UGLabel>(TitleObject)->GetTextField();
	}
	else if (TitleObject->IsA<UFairyButton>())
	{
		return Cast<UFairyButton>(TitleObject)->GetTextField();
	}
	else
	{
		return nullptr;
	}
}

FNVariant UFairyButton::GetProp(EObjectPropID PropID) const
{
	switch (PropID)
	{
	case EObjectPropID::Color:
		return FNVariant(GetTitleColor());
	case EObjectPropID::OutlineColor:
	{
		UGTextField* TextField = GetTextField();
		if (TextField != nullptr)
		{
			return FNVariant(TextField->GetTextFormat().OutlineColor);
		}
		else
		{
			return FNVariant(FColor::Black);
		}
	}
	case EObjectPropID::FontSize:
		return FNVariant(GetTitleFontSize());
	case EObjectPropID::Selected:
		return FNVariant(IsSelected());
	default:
		return UFairyComponent::GetProp(PropID);
	}
}

void UFairyButton::SetProp(EObjectPropID PropID, const FNVariant& InValue)
{
	switch (PropID)
	{
	case EObjectPropID::Color:
		SetTitleColor(InValue.AsColor());
		break;
	case EObjectPropID::OutlineColor:
	{
		UGTextField* TextField = GetTextField();
		if (TextField != nullptr)
		{
			TextField->GetTextFormat().OutlineColor = InValue.AsColor();
			TextField->ApplyFormat();
		}
		break;
	}
	case EObjectPropID::FontSize:
		SetTitleFontSize(InValue.AsInt());
		break;
	case EObjectPropID::Selected:
		SetSelected(InValue.AsBool());
		break;
	default:
		UFairyComponent::SetProp(PropID, InValue);
		break;
	}
}

void UFairyButton::ConstructExtension(FairyGUI::FByteBuffer* Buffer)
{
	Sound = UFairyConfig::Config->ButtonSound;
	SoundVolumeScale = UFairyConfig::Config->ButtonSoundVolumeScale;

	Buffer->Seek(0, 6);

	Mode = (EButtonMode)Buffer->ReadByte();
	Buffer->ReadS(Sound);
	SoundVolumeScale = Buffer->ReadFloat();
	DownEffect = (EButtonPressedEffect)Buffer->ReadByte();
	DownEffectValue = Buffer->ReadFloat();
	if (DownEffect == EButtonPressedEffect::Scale)
	{
		SetPivot(FVector2D(0.5f, 0.5f), IsPivotAsAnchor());
	}

	ButtonController = GetController(TEXT("button"));
	TitleObject = GetChild(TEXT("title"));
	IconObject = GetChild(TEXT("icon"));
	if (TitleObject != nullptr)
	{
		Title = TitleObject->GetText();
	}

	if (IconObject != nullptr)
	{
		Icon = IconObject->GetIcon();
	}

	if (Mode == EButtonMode::Common)
	{
		SetState(UFairyButton::UP);
	}

	On(FFairyEventNames::RollOver).AddUObject(this, &UFairyButton::OnRollOverHandler);
	On(FFairyEventNames::RollOut).AddUObject(this, &UFairyButton::OnRollOutHandler);
	On(FFairyEventNames::TouchBegin).AddUObject(this, &UFairyButton::OnTouchBeginHandler);
	On(FFairyEventNames::TouchEnd).AddUObject(this, &UFairyButton::OnTouchEndHandler);
	On(FFairyEventNames::Click).AddUObject(this, &UFairyButton::OnClickHandler);
	On(FFairyEventNames::RemovedFromStage).AddUObject(this, &UFairyButton::OnRemovedFromStageHandler);
}

void UFairyButton::SetupAfterAdd(FairyGUI::FByteBuffer* Buffer, int32 BeginPos)
{
	Super::SetupAfterAdd(Buffer, BeginPos);

	if (!Buffer->Seek(BeginPos, 6))
	{
		return;
	}

	if ((EObjectType)Buffer->ReadByte() != PackageItem->ObjectType)
	{
		return;
	}

	const FString* str;

	if ((str = Buffer->ReadSP()) != nullptr)
	{
		SetTitle(*str);
	}

	if ((str = Buffer->ReadSP()) != nullptr)
	{
		SetSelectedTitle(*str);
	}

	if ((str = Buffer->ReadSP()) != nullptr)
	{
		SetIcon(*str);
	}

	if ((str = Buffer->ReadSP()) != nullptr)
	{
		SetSelectedIcon(*str);
	}

	if (Buffer->ReadBool())
	{
		SetTitleColor(Buffer->ReadColor());
	}

	int32 iv = Buffer->ReadInt();
	if (iv != 0)
	{
		SetTitleFontSize(iv);
	}

	iv = Buffer->ReadShort();
	if (iv >= 0)
	{
		RelatedController = GetParent()->GetControllerAt(iv);
	}
	RelatedPageID = Buffer->ReadStringFromCache();

	Sound = Buffer->ReadStringFromCache();
	if (Buffer->ReadBool())
	{
		SoundVolumeScale = Buffer->ReadFloat();
	}

	SetSelected(Buffer->ReadBool());
}

void UFairyButton::ApplyController(UFairyController* Controller)
{
	UFairyComponent::ApplyController(Controller);

	if (RelatedController == Controller)
	{
		SetSelected(RelatedPageID == Controller->GetSelectedPageID());
	}
}

void UFairyButton::OnRollOverHandler(UEventContext* Context)
{
	if (ButtonController == nullptr || !ButtonController->HasPage(OVER))
	{
		return;
	}

	bOver = true;
	if (bDown)
	{
		return;
	}

	if (IsGrayed() && ButtonController->HasPage(DISABLED))
	{
		return;
	}

	SetState(bSelected ? SELECTED_OVER : OVER);
}

void UFairyButton::OnRollOutHandler(UEventContext* Context)
{
	if (ButtonController == nullptr || !ButtonController->HasPage(OVER))
	{
		return;
	}

	bOver = false;
	if (bDown)
	{
		return;
	}

	if (IsGrayed() && ButtonController->HasPage(DISABLED))
	{
		return;
	}

	SetState(bSelected ? DOWN : UP);
}

void UFairyButton::OnTouchBeginHandler(UEventContext* Context)
{
	if (Context->GetMouseButton() != EKeys::LeftMouseButton)
	{
		return;
	}

	bDown = true;
	Context->CaptureTouch();

	if (Mode == EButtonMode::Common)
	{
		if (IsGrayed() && ButtonController != nullptr && ButtonController->HasPage(DISABLED))
		{
			SetState(UFairyButton::SELECTED_DISABLED);
		}
		else
		{
			SetState(UFairyButton::DOWN);
		}
	}
}

void UFairyButton::OnTouchEndHandler(UEventContext* Context)
{
	if (Context->GetMouseButton() != EKeys::LeftMouseButton)
	{
		return;
	}

	if (bDown)
	{
		bDown = false;
		if (Mode == EButtonMode::Common)
		{
			if (IsGrayed() && ButtonController != nullptr && ButtonController->HasPage(UFairyButton::DISABLED))
			{
				SetState(UFairyButton::DISABLED);
			}
			else if (bOver)
			{
				SetState(UFairyButton::OVER);
			}
			else
			{
				SetState(UFairyButton::UP);
			}
		}
		else
		{
			if (!bOver && ButtonController != nullptr
				&& (ButtonController->GetSelectedPage() == UFairyButton::OVER || ButtonController->GetSelectedPage() == UFairyButton::SELECTED_OVER))
			{
				SetCurrentState();
			}
		}
	}
}

void UFairyButton::OnClickHandler(UEventContext* Context)
{
	if (!Sound.IsEmpty())
	{
		UFairyApplication::Get()->PlaySound(Sound, SoundVolumeScale);
	}

	if (Mode == EButtonMode::Check)
	{
		if (bChangeStateOnClick)
		{
			SetSelected(!bSelected);
			DispatchEvent(FFairyEventNames::Changed);
		}
	}
	else if (Mode == EButtonMode::Radio)
	{
		if (bChangeStateOnClick && !bSelected)
		{
			SetSelected(true);
			DispatchEvent(FFairyEventNames::Changed);
		}
	}
	else
	{
		if (RelatedController != nullptr)
		{
			RelatedController->SetSelectedPageID(RelatedPageID);
		}
	}
}

void UFairyButton::OnRemovedFromStageHandler(UEventContext* Context)
{
	if (bOver)
	{
		OnRollOutHandler(Context);
	}
}
