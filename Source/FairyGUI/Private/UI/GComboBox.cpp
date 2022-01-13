#include "UI/GComboBox.h"
#include "Package/FairyPackage.h"
#include "Package/FairyPackageMgr.h"
#include "UI/GTextField.h"
#include "UI/GTextInput.h"
#include "UI/GLabel.h"
#include "UI/FairyButton.h"
#include "UI/Controller/FairyController.h"
#include "UI/Controller/Gears/FairyGearType.h"
#include "UI/FairyListView.h"
#include "UI/FairyRoot.h"
#include "Utils/ByteBuffer.h"

UGComboBox::UGComboBox() :
    bItemsUpdated(true),
    SelectedIndex(-1)
{
    
}

UGComboBox::~UGComboBox()
{
}

const FString& UGComboBox::GetText() const
{
    if (TitleObject != nullptr)
        return TitleObject->GetText();
    else
        return G_EMPTY_STRING;
}

void UGComboBox::SetText(const FString& InText)
{
    if (TitleObject != nullptr)
        TitleObject->SetText(InText);
    UpdateGear(EFairyGearType::Text);
}

const FString& UGComboBox::GetIcon() const
{
    if (IconObject != nullptr)
        return IconObject->GetIcon();
    else
        return G_EMPTY_STRING;
}

void UGComboBox::SetIcon(const FString & InIcon)
{
    if (IconObject != nullptr)
        IconObject->SetIcon(InIcon);
    UpdateGear(EFairyGearType::Icon);
}

FColor UGComboBox::GetTitleColor() const
{
    UGTextField* TextField = GetTextField();
    if (TextField)
        return TextField->GetTextFormat().Color;
    else
        return FColor::Black;
}

void UGComboBox::SetTitleColor(const FColor & InColor)
{
    UGTextField* TextField = GetTextField();
    if (TextField)
    {
        TextField->GetTextFormat().Color = InColor;
        TextField->ApplyFormat();
    }
}

int32 UGComboBox::GetTitleFontSize() const
{
    UGTextField* TextField = GetTextField();
    if (TextField)
        return TextField->GetTextFormat().Size;
    else
        return 0;
}

void UGComboBox::SetTitleFontSize(int32 InFontSize)
{
    UGTextField* TextField = GetTextField();
    if (TextField)
    {
        TextField->GetTextFormat().Size = InFontSize;
        TextField->ApplyFormat();
    }
}

const FString& UGComboBox::GetValue() const
{
    if (SelectedIndex >= 0 && SelectedIndex < Values.Num())
        return Values[SelectedIndex];
    else
        return G_EMPTY_STRING;
}

void UGComboBox::SetValue(const FString& InValue)
{
    SetSelectedIndex(Values.Find(InValue));
}

void UGComboBox::SetSelectedIndex(int32 InIndex)
{
    if (SelectedIndex == InIndex)
        return;

    SelectedIndex = InIndex;
    if (SelectedIndex >= 0 && SelectedIndex < Items.Num())
    {
        SetText(Items[SelectedIndex]);
        if (Icons.Num() > 0 && SelectedIndex != -1 && SelectedIndex < Icons.Num())
            SetIcon(Icons[SelectedIndex]);
    }
    else
    {
        SetTitle(G_EMPTY_STRING);
        if (Icons.Num() > 0)
            SetIcon(G_EMPTY_STRING);
    }

    UpdateSelectionController();
}

void UGComboBox::Refresh()
{
    if (Items.Num() > 0)
    {
        if (SelectedIndex >= Items.Num())
            SelectedIndex = Items.Num() - 1;
        else if (SelectedIndex == -1)
            SelectedIndex = 0;
        SetTitle(Items[SelectedIndex]);
    }
    else
    {
        SetTitle(G_EMPTY_STRING);
        SelectedIndex = -1;
    }

    if (Icons.Num() > 0)
    {
        if (SelectedIndex != -1 && SelectedIndex < Icons.Num())
            SetIcon(Icons[SelectedIndex]);
        else
            SetIcon(G_EMPTY_STRING);
    }

    bItemsUpdated = true;
}

void UGComboBox::SetState(const FName& InState)
{
    if (ButtonController != nullptr)
    {
        ButtonController->SetSelectedPage(InState);
    }
}

void UGComboBox::SetCurrentState()
{
    if (IsGrayed() && ButtonController != nullptr && ButtonController->HasPage(UFairyButton::DISABLED))
    {
        SetState(UFairyButton::DISABLED);
    }
    else if (DropdownObject != nullptr && DropdownObject->GetParent() != nullptr)
    {
        SetState(UFairyButton::DOWN);
    }
    else
    {
        SetState(bOver ? UFairyButton::OVER : UFairyButton::UP);
    }
}

void UGComboBox::UpdateSelectionController()
{
    if (SelectionController != nullptr && !SelectionController->bChanging && SelectedIndex < SelectionController->GetPageCount())
    {
        UFairyController* c = SelectionController;
        SelectionController = nullptr;
        c->SetSelectedIndex(SelectedIndex);
        SelectionController = c;
    }
}

void UGComboBox::UpdateDropdownList()
{
    if (bItemsUpdated)
    {
        bItemsUpdated = false;
        RenderDropdownList();
        ListObject->ResizeToFit(VisibleItemCount);
    }
}

void UGComboBox::ShowDropdown()
{
    UpdateDropdownList();
    if (ListObject->GetSelectionMode() == EListSelectionMode::Single)
        ListObject->SetSelectedIndex(-1);
    DropdownObject->SetWidth(Size.X);
    ListObject->EnsureBoundsCorrect();

    UFairyApplication::Get()->GetUIRoot(this)->TogglePopup(DropdownObject, this, PopupDirection);
    if (DropdownObject->GetParent() != nullptr)
        SetState(UFairyButton::DOWN);
}

void UGComboBox::RenderDropdownList()
{
    ListObject->RemoveChildrenToPool();
    int32 cnt = Items.Num();
    for (int32 i = 0; i < cnt; i++)
    {
        UFairyObject* Obj = ListObject->AddItemFromPool();
        Obj->SetText(Items[i]);
        Obj->SetIcon((Icons.Num() > 0 && i < Icons.Num()) ? Icons[i] : G_EMPTY_STRING);
        Obj->SetName(i < Values.Num() ? Values[i] : G_EMPTY_STRING);
    }
}

void UGComboBox::ApplyController(UFairyController* Controller)
{
    UFairyComponent::ApplyController(Controller);

    if (SelectionController == Controller)
        SetSelectedIndex(Controller->GetSelectedIndex());
}

void UGComboBox::HandleGrayedChanged()
{
    if (ButtonController != nullptr && ButtonController->HasPage(UFairyButton::DISABLED))
    {
        if (IsGrayed())
            SetState(UFairyButton::DISABLED);
        else
            SetState(UFairyButton::UP);
    }
    else
        UFairyComponent::HandleGrayedChanged();
}

UGTextField* UGComboBox::GetTextField() const
{
    if (TitleObject->IsA<UGTextField>())
        return Cast<UGTextField>(TitleObject);
    else if (TitleObject->IsA<UGLabel>())
        return Cast<UGLabel>(TitleObject)->GetTextField();
    else if (TitleObject->IsA<UFairyButton>())
        return Cast<UFairyButton>(TitleObject)->GetTextField();
    else
        return nullptr;
}

FNVariant UGComboBox::GetProp(EObjectPropID PropID) const
{
    switch (PropID)
    {
    case EObjectPropID::Color:
        return FNVariant(GetTitleColor());
    case EObjectPropID::OutlineColor:
    {
        UGTextField* TextField = GetTextField();
        if (TextField != nullptr)
            return FNVariant(TextField->GetTextFormat().OutlineColor);
        else
            return FNVariant(FColor::Black);
    }
    case EObjectPropID::FontSize:
        return FNVariant(GetTitleFontSize());
    default:
        return UFairyComponent::GetProp(PropID);
    }
}

void UGComboBox::SetProp(EObjectPropID PropID, const FNVariant& InValue)
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
    default:
        UFairyComponent::SetProp(PropID, InValue);
        break;
    }
}

void UGComboBox::ConstructExtension(FairyGUI::FByteBuffer* Buffer)
{
    VisibleItemCount = UFairyConfig::Config->DefaultComboBoxVisibleItemCount;

    Buffer->Seek(0, 6);

    ButtonController = GetController("button");
    TitleObject = GetChild("title");
    IconObject = GetChild("icon");

    const FString& dropdownResource = Buffer->ReadStringFromCache();
    if (!dropdownResource.IsEmpty())
    {
        DropdownObject = Cast<UFairyComponent>(UFairyPackageMgr::Get()->CreateObjectFromURL(this, FName(dropdownResource)));
        verifyf(DropdownObject != nullptr, TEXT("should be a component."));

        ListObject = Cast<UFairyListView>(DropdownObject->GetChild("list"));
        verifyf(ListObject != nullptr, TEXT("should container a list component named list."));

        ListObject->On(FFairyEventNames::ClickItem).AddUObject(this, &UGComboBox::OnClickItem);

        ListObject->AddRelation(DropdownObject, ERelationType::Width);
        ListObject->RemoveRelation(DropdownObject, ERelationType::Height);

        DropdownObject->AddRelation(ListObject, ERelationType::Height);
        DropdownObject->RemoveRelation(ListObject, ERelationType::Width);

        DropdownObject->On(FFairyEventNames::RemovedFromStage).AddUObject(this, &UGComboBox::OnPopupWinClosed);
    }

    On(FFairyEventNames::RollOver).AddUObject(this, &UGComboBox::OnRollOverHandler);
    On(FFairyEventNames::RollOut).AddUObject(this, &UGComboBox::OnRollOutHandler);
    On(FFairyEventNames::TouchBegin).AddUObject(this, &UGComboBox::OnTouchBeginHandler);
    On(FFairyEventNames::TouchEnd).AddUObject(this, &UGComboBox::OnTouchEndHandler);
}

void UGComboBox::SetupAfterAdd(FairyGUI::FByteBuffer* Buffer, int32 BeginPos)
{
    UFairyComponent::SetupAfterAdd(Buffer, BeginPos);

    if (!Buffer->Seek(BeginPos, 6))
    {
        return;
    }

    if ((EFairyObjectType)Buffer->ReadByte() != PackageItem->GetRawFairyObjectType())
    {
        return;
    }

    const FString* str;
    bool hasIcon = false;
    int32 itemCount = Buffer->ReadShort();
    for (int32 i = 0; i < itemCount; i++)
    {
        int32 nextPos = Buffer->ReadShort();
        nextPos += Buffer->GetPos();

        Items.Add(Buffer->ReadStringFromCache());
        Values.Add(Buffer->ReadStringFromCache());
        if ((str = Buffer->ReadSP()) != nullptr)
        {
            if (!hasIcon)
            {
                for (int32 j = 0; j < Items.Num() - 1; j++)
                {
                    Icons.Add(G_EMPTY_STRING);
                }
            }
            Icons.Add(*str);
        }

        Buffer->SetPos(nextPos);
    }

    if ((str = Buffer->ReadSP()) != nullptr)
    {
        SetTitle(*str);
        SelectedIndex = Items.Find(*str);
    }
    else if (Items.Num() > 0)
    {
        SelectedIndex = 0;
        SetTitle(Items[0]);
    }
    else
    {
        SelectedIndex = -1;
    }

    if ((str = Buffer->ReadSP()) != nullptr)
    {
        SetIcon(*str);
    }

    if (Buffer->ReadBool())
    {
        SetTitleColor(Buffer->ReadColor());
    }
    int32 iv = Buffer->ReadInt();
    if (iv > 0)
    {
        VisibleItemCount = iv;
    }
    PopupDirection = (EPopupDirection)Buffer->ReadByte();

    iv = Buffer->ReadShort();
    if (iv >= 0)
    {
        SelectionController = GetParent()->GetControllerAt(iv);
    }
}

void UGComboBox::OnClickItem(UEventContext* Context)
{
    if (DropdownObject->GetParent()->IsA<UFairyRoot>())
        ((UFairyRoot*)DropdownObject->GetParent())->HidePopup(DropdownObject);
    SelectedIndex = INT_MIN;
    SetSelectedIndex(ListObject->GetChildIndex(Cast<UFairyObject>(Context->GetData().AsUObject())));

    DispatchEvent(FFairyEventNames::Changed);
}

void UGComboBox::OnRollOverHandler(UEventContext* Context)
{
    bOver = true;
    if (bDown || (DropdownObject != nullptr && DropdownObject->GetParent() != nullptr))
        return;

    SetCurrentState();
}

void UGComboBox::OnRollOutHandler(UEventContext* Context)
{
    bOver = false;
    if (bDown || (DropdownObject != nullptr && DropdownObject->GetParent() != nullptr))
        return;

    SetCurrentState();
}

void UGComboBox::OnTouchBeginHandler(UEventContext* Context)
{
    if (Context->GetMouseButton() != EKeys::LeftMouseButton)
        return;

    if (Context->GetInitiator()->IsA<UGTextInput>())
        return;

    bDown = true;

    if (DropdownObject != nullptr)
        ShowDropdown();

    Context->CaptureTouch();
}

void UGComboBox::OnTouchEndHandler(UEventContext* Context)
{
    if (Context->GetMouseButton() != EKeys::LeftMouseButton)
        return;

    if (bDown)
    {
        bDown = false;
        if (DropdownObject != nullptr && DropdownObject->GetParent() != nullptr)
            SetCurrentState();
    }
}

void UGComboBox::OnPopupWinClosed(UEventContext* Context)
{
    SetCurrentState();
}
