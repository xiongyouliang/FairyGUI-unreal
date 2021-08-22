#include "UI/PopupMenu.h"
#include "UI/FairyRoot.h"
#include "Package/FairyPackage.h"
#include "UI/Controller/GController.h"
#include "FairyApplication.h"

const FName UPopupMenu::ClickMenu("ClickMenu");

UPopupMenu* UPopupMenu::CreatePopupMenu(const FString& ResourceURL)
{
    UPopupMenu* Instance = NewObject<UPopupMenu>();
    Instance->Create(ResourceURL);
    return Instance;
}

UPopupMenu::UPopupMenu()
{
}

UPopupMenu::~UPopupMenu()
{
}

void UPopupMenu::Create(const FString& ResourceURL)
{
    FString url = ResourceURL;
    if (url.IsEmpty())
    {
        url = UFairyConfig::Config->PopupMenu;
        if (url.IsEmpty())
        {
            UE_LOG(LogFairyGUI, Warning, TEXT("UIConfig.PopupMenu not defined"));
            return;
        }
    }

    ContentPane = UFairyPackageMgr::Get()->CreateObjectFromURL(GetOuter(), url)->As<UFairyComponent>();
    ContentPane->On(FFairyEventNames::AddedToStage).AddUObject(this, &UPopupMenu::OnAddedToStage);

    List = ContentPane->GetChild("list")->As<UGList>();
    List->RemoveChildrenToPool();

    List->AddRelation(ContentPane, ERelationType::Width);
    List->RemoveRelation(ContentPane, ERelationType::Height);
    ContentPane->AddRelation(List, ERelationType::Height);

    List->On(FFairyEventNames::ClickItem).AddUObject(this, &UPopupMenu::OnClickItem);
}

UGButton* UPopupMenu::AddItem(const FString& Caption, FGUIEventDelegate Callback)
{
    UGButton* item = List->AddItemFromPool()->As<UGButton>();
    item->SetTitle(Caption);
    item->SetGrayed(false);
    UGController* c = item->GetController("checked");
    if (c != nullptr)
        c->SetSelectedIndex(0);
    item->On(ClickMenu).Clear();
    if (Callback.IsBound())
        item->On(ClickMenu).Add(Callback);

    return item;
}

UGButton* UPopupMenu::AddItem(const FString& Caption, const FGUIEventDynDelegate& Callback)
{
    return AddItem(Caption, Callback.IsBound() ?
        FGUIEventDelegate::CreateUFunction(const_cast<UObject*>(Callback.GetUObject()), Callback.GetFunctionName())
        : FGUIEventDelegate());
}

UGButton* UPopupMenu::AddItemAt(const FString& Caption, int32 Index, FGUIEventDelegate Callback)
{
    UGButton* item = List->GetFromPool(List->DefaultItem)->As<UGButton>();
    List->AddChildAt(item, Index);

    item->SetTitle(Caption);
    item->SetGrayed(false);
    UGController* c = item->GetController("checked");
    if (c != nullptr)
        c->SetSelectedIndex(0);
    if (Callback.IsBound())
        item->On(ClickMenu).Add(Callback);

    return item;
}

UGButton* UPopupMenu::AddItemAt(const FString& Caption, int32 index, const FGUIEventDynDelegate& Callback)
{
    return AddItemAt(Caption, index, Callback.IsBound() ?
        FGUIEventDelegate::CreateUFunction(const_cast<UObject*>(Callback.GetUObject()), Callback.GetFunctionName())
        : FGUIEventDelegate());
}

void UPopupMenu::AddSeperator()
{
    if (UFairyConfig::Config->PopupMenuSeperator.IsEmpty())
    {
        UE_LOG(LogFairyGUI, Warning, TEXT("UIConfig.PopupMenuSeperator not defined"));
        return;
    }

    List->AddItemFromPool(UFairyConfig::Config->PopupMenuSeperator);
}

const FString& UPopupMenu::GetItemName(int32 Index) const
{
    UGButton* item = List->GetChildAt(Index)->As<UGButton>();
    return item->GetName();
}

void UPopupMenu::SetItemText(const FString& Name, const FString& Caption)
{
    UGButton* item = List->GetChild(Name)->As<UGButton>();
    item->SetTitle(Caption);
}

void UPopupMenu::SetItemVisible(const FString& Name, bool bVisible)
{
    UGButton* item = List->GetChild(Name)->As<UGButton>();
    if (item->IsVisible() != bVisible)
    {
        item->SetVisible(bVisible);
        List->SetBoundsChangedFlag();
    }
}

void UPopupMenu::SetItemGrayed(const FString& Name, bool bGrayed)
{
    UGButton* item = List->GetChild(Name)->As<UGButton>();
    item->SetGrayed(bGrayed);
}

void UPopupMenu::SetItemCheckable(const FString& Name, bool bCheckable)
{
    UGButton* item = List->GetChild(Name)->As<UGButton>();
    UGController* c = item->GetController("checked");
    if (c != nullptr)
    {
        if (bCheckable)
        {
            if (c->GetSelectedIndex() == 0)
                c->SetSelectedIndex(1);
        }
        else
            c->SetSelectedIndex(0);
    }
}

void UPopupMenu::SetItemChecked(const FString& Name, bool bCheck)
{
    UGButton* item = List->GetChild(Name)->As<UGButton>();
    UGController* c = item->GetController("checked");
    if (c != nullptr)
        c->SetSelectedIndex(bCheck ? 2 : 1);
}

bool UPopupMenu::IsItemChecked(const FString& Name) const
{
    UGButton* item = List->GetChild(Name)->As<UGButton>();
    UGController* c = item->GetController("checked");
    if (c != nullptr)
        return c->GetSelectedIndex() == 2;
    else
        return false;
}

bool UPopupMenu::RemoveItem(const FString& Name)
{
    UFairyObject* item = List->GetChild(Name);
    if (item != nullptr)
    {
        int32 index = List->GetChildIndex(item);
        List->RemoveChildToPoolAt(index);
        item->On(ClickMenu).Clear();

        return true;
    }
    else
        return false;
}

void UPopupMenu::ClearItems()
{
    int32 cnt = List->NumChildren();
    for (int32 i = 0; i < cnt; i++)
        List->GetChildAt(i)->On(ClickMenu).Clear();
    List->RemoveChildrenToPool();
}

int32 UPopupMenu::GetItemCount() const
{
    return List->NumChildren();
}

void UPopupMenu::Show(UFairyObject * AtObject, EPopupDirection Dir)
{
    UFairyApplication::Get()->GetUIRoot(this)->ShowPopup(ContentPane, AtObject, Dir);
}

void UPopupMenu::OnClickItem(UEventContext * Context)
{
    UGButton* item = Cast<UGButton>(Context->GetData().AsUObject());
    if (item == nullptr)
        return;

    if (item->IsGrayed())
    {
        List->SetSelectedIndex(-1);
        return;
    }

    UGController* c = item->GetController("checked");
    if (c != nullptr && c->GetSelectedIndex() != 0)
    {
        if (c->GetSelectedIndex() == 1)
            c->SetSelectedIndex(2);
        else
            c->SetSelectedIndex(1);
    }

    UFairyApplication::Get()->GetUIRoot(this)->HidePopup(ContentPane);

    item->DispatchEvent(ClickMenu, Context->GetData());
}

void UPopupMenu::OnAddedToStage(UEventContext * Context)
{
    List->SetSelectedIndex(-1);
    List->ResizeToFit(INT_MAX, 10);
}