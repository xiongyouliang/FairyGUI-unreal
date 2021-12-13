#include "UI/UIObjectFactory.h"
#include "Package/FairyPackage.h"
#include "Package/FairyPackageItem.h"
#include "UI/FairyComponent.h"
#include "UI/GImage.h"
#include "UI/GMovieClip.h"
#include "UI/GTextField.h"
#include "UI/GRichTextField.h"
#include "UI/GTextInput.h"
#include "UI/GGraph.h"
#include "UI/GLoader.h"
#include "UI/GLoader3D.h"
#include "UI/GGroup.h"
#include "UI/GLabel.h"
#include "UI/FairyButton.h"
#include "UI/GComboBox.h"
#include "UI/GProgressBar.h"
#include "UI/GSlider.h"
#include "UI/GScrollBar.h"
#include "UI/FairyListView.h"
#include "UI/GTree.h"
#include "Package/FairyPackageMgr.h"

TMap<FString, FGComponentCreator> FUIObjectFactory::PackageItemExtensions;
FGLoaderCreator FUIObjectFactory::LoaderCreator;

void FUIObjectFactory::SetExtension(const FString& URL, FGComponentCreator Creator)
{
    if (URL.IsEmpty())
    {
        UE_LOG(LogFairyGUI, Warning, TEXT("Invaild url: %s"), *URL);
        return;
    }
    TSharedPtr<FFairyPackageItem> PackageItem = UFairyPackageMgr::Get()->GetPackageItemByURL(URL);
    if (PackageItem.IsValid())
    {
        PackageItem->ExtensionCreator = Creator;
    }

    PackageItemExtensions.Add(URL, Creator);
}

void FUIObjectFactory::SetExtension(const FString& URL, TSubclassOf<UFairyComponent> ClassType)
{
    SetExtension(URL, FGComponentCreator::CreateLambda([ClassType]() {
        return ::NewObject<UFairyComponent>(GetTransientPackage(), ClassType);
    }));
}

UFairyObject* FUIObjectFactory::NewObject(UObject* Outer, const TSharedPtr<FFairyPackageItem>& PackageItem)
{
    UFairyObject* FairyObject = nullptr;
    if (PackageItem->ExtensionCreator.IsBound())
    {
        FairyObject = PackageItem->ExtensionCreator.Execute();
    }
    else
    {
        FairyObject = FUIObjectFactory::NewObject(Outer, PackageItem->ObjectType);
    }

    if (FairyObject != nullptr)
    {
        FairyObject->SetPackageItem(PackageItem);
    }
    return FairyObject;
}

UFairyObject* FUIObjectFactory::NewObject(UObject* Outer, EObjectType Type)
{
    switch (Type)
    {
    case EObjectType::Image:
        return ::NewObject<UGImage>(Outer);
    case EObjectType::MovieClip:
        return ::NewObject<UGMovieClip>(Outer);
    case EObjectType::Component:
        return ::NewObject<UFairyComponent>(Outer);
    case EObjectType::Text:
        return ::NewObject<UGTextField>(Outer);
    case EObjectType::RichText:
        return ::NewObject<UGRichTextField>(Outer);
    case EObjectType::InputText:
        return ::NewObject<UGTextInput>(Outer);
    case EObjectType::Group:
        return ::NewObject<UGGroup>(Outer);
    case EObjectType::List:
        return ::NewObject<UFairyListView>(Outer);
    case EObjectType::Graph:
        return ::NewObject<UGGraph>(Outer);
    case EObjectType::Loader:
        if (LoaderCreator.IsBound())
        {
            return LoaderCreator.Execute();
        }
        else
        {
            return ::NewObject<UGLoader>(Outer);
        }
    case EObjectType::Button:
        return ::NewObject<UFairyButton>(Outer);
    case EObjectType::Label:
        return ::NewObject<UGLabel>(Outer);
    case EObjectType::ProgressBar:
        return ::NewObject<UGProgressBar>(Outer);
    case EObjectType::Slider:
        return ::NewObject<UGSlider>(Outer);
    case EObjectType::ScrollBar:
        return ::NewObject<UGScrollBar>(Outer);
    case EObjectType::ComboBox:
        return ::NewObject<UGComboBox>(Outer);
    case EObjectType::Tree:
        return ::NewObject<UGTree>(Outer);
    case EObjectType::Loader3D:
        return ::NewObject<UGLoader3D>(Outer);
    default:
        return nullptr;
    }
}

void FUIObjectFactory::ResolvePackageItemExtension(const TSharedPtr<FFairyPackageItem>& PackageItem)
{
    auto it = PackageItemExtensions.Find("ui://" + PackageItem->OwnerPackage->GetID() + PackageItem->ID);
    if (it != nullptr)
    {
        PackageItem->ExtensionCreator = *it;
        return;
    }
    it = PackageItemExtensions.Find("ui://" + PackageItem->OwnerPackage->GetName() + "/" + PackageItem->Name);
    if (it != nullptr)
    {
        PackageItem->ExtensionCreator = *it;
        return;
    }
    PackageItem->ExtensionCreator = nullptr;
}
