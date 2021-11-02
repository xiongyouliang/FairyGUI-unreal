#include "Package/FairyPackageMgr.h"

#include "FairyApplication.h"
#include "FairyCommons.h"
#include "Package/FairyPackageAsset.h"
#include "Package/FairyPackage.h"
#include "UI/UIObjectFactory.h"
#include "UI/GWindow.h"
#include "Utils/ByteBuffer.h"

UFairyPackageMgr* UFairyPackageMgr::Instance = nullptr;

UFairyPackageMgr::UFairyPackageMgr()
{
	UE_LOG(LogTemp, Warning, TEXT("UFairyPackageMgr::UFairyPackageMgr()"));
}

UFairyPackageMgr::~UFairyPackageMgr()
{
	UE_LOG(LogTemp, Warning, TEXT("UFairyPackageMgr::~UFairyPackageMgr()"));
}

UFairyPackageMgr* UFairyPackageMgr::Get()
{
	if (Instance == nullptr)
	{
		Instance = NewObject<UFairyPackageMgr>();
		Instance->AddToRoot();
	}
	return Instance;
}

void UFairyPackageMgr::SetBranch(const FString& InBranch)
{
	Branch = InBranch;
	bool empty = Branch.IsEmpty();
	for (auto& it : PackageInstByID)
	{
		UFairyPackage*& Pkg = it.Value;
		if (empty)
		{
			Pkg->BranchIndex = -1;
		}
		else if (Pkg->Branches.Num() > 0)
		{
			Pkg->BranchIndex = Pkg->Branches.IndexOfByKey(Branch);
		}
	}
}

FString UFairyPackageMgr::GetVar(const FString& VarKey)
{
	FString* Value = Vars.Find(VarKey);
	if (Value != nullptr)
	{
		return *Value;
	}
	else
	{
		return G_EMPTY_STRING;
	}
}

void UFairyPackageMgr::SetVar(const FString& VarKey, const FString& VarValue)
{
	Vars.Add(VarKey, VarValue);
}

UFairyPackage* UFairyPackageMgr::AddPackage(UFairyPackageAsset* InAsset)
{
	FString PathName = InAsset->GetPathName();
	UFairyPackage* Pkg = PackageInstByID.FindRef(PathName);
	if (Pkg != nullptr)
	{
		UE_LOG(LogFairyGUI, Warning, TEXT("Package already addedd"));
		return Pkg;
	}
	Pkg = NewObject<UFairyPackage>(this); // UFairyPackage outer is UFairyPackageMgr Singleton
	Pkg->Asset = InAsset;
	Pkg->AssetPath = PathName;
	FByteBuffer Buffer(InAsset->Data.GetData(), 0, InAsset->Data.Num(), false);
	Pkg->Load(&Buffer);

	//UFairyApplication::Get()->PackageList.Add(Pkg);
	PackageInstByID.Add(Pkg->ID, Pkg);
	PackageInstByID.Add(Pkg->AssetPath, Pkg);
	PackageInstByName.Add(Pkg->Name, Pkg);
	return Pkg;
}

UFairyPackage* UFairyPackageMgr::GetPackageByName(const FString& PackageName)
{
	auto it = PackageInstByName.Find(PackageName);
	if (it != nullptr)
	{
		return *it;
	}
	else 
	{
		return nullptr;
	}
}

UFairyPackage* UFairyPackageMgr::GetPackageByID(const FString& PackageID)
{
	auto it = PackageInstByID.Find(PackageID);
	if (it != nullptr)
	{
		return *it;
	}
	else
	{
		return nullptr;
	}
}

FString UFairyPackageMgr::ConvertToItemURL(const FString& PackageName, const FString& ResourceName)
{
	UFairyPackage* pkg = GetPackageByName(PackageName);
	if (pkg != nullptr)
	{
		TSharedPtr<FFairyPackageItem> pi = pkg->GetItemByName(ResourceName);
		if (pi.IsValid())
		{
			return "ui://" + pkg->GetID() + pi->ID;
		}
	}
	return "";
}

FString UFairyPackageMgr::NormalizeURL(const FString& URL)
{
	if (URL.IsEmpty()) 
	{
		return URL;
	}

	int32 pos1;
	if (!URL.FindChar('/', pos1)) 
	{
		return URL;
	}

	int32 pos2 = URL.Find("/", ESearchCase::CaseSensitive, ESearchDir::FromStart, pos1 + 2);
	if (pos2 == -1) 
	{
		return URL;
	}	
	else
	{
		FString pkgName = URL.Mid(pos1 + 2, pos2 - pos1 - 2);
		FString srcName = URL.Mid(pos2 + 1);
		return ConvertToItemURL(pkgName, srcName);
	}
}
TSharedPtr<FFairyPackageItem> UFairyPackageMgr::GetPackageItemByURL(const FString& URL)
{
	if (URL.IsEmpty()) 
	{
		return nullptr;
	}
		
	int32 pos1;
	if (!URL.FindChar('/', pos1)) 
	{
		return nullptr;
	}

	int32 pos2 = URL.Find("/", ESearchCase::CaseSensitive, ESearchDir::FromStart, pos1 + 2);
	if (pos2 == -1) 
	{
		if (URL.Len() > 13)
		{
			FString pkgId = URL.Mid(5, 8);
			UFairyPackage* pkg = GetPackageByID(pkgId);
			if (pkg != nullptr)
			{
				FString srcId = URL.Mid(13);
				return pkg->GetItem(srcId);
			}
		}
	}
	else
	{
		FString pkgName = URL.Mid(pos1 + 2, pos2 - pos1 - 2);
		UFairyPackage* pkg = GetPackageByName(pkgName);
		if (pkg != nullptr)
		{
			FString srcName = URL.Mid(pos2 + 1);
			return pkg->GetItemByName(srcName);
		}
	}

	return nullptr;
}

void UFairyPackageMgr::RemovePackage(const FString& IDOrName)
{
	UFairyPackage* pkg = GetPackageByName(IDOrName);
	if (pkg == nullptr)
	{
		pkg = GetPackageByID(IDOrName);
	}

	if (pkg != nullptr)
	{
		//TArray<UFairyPackage*>& PackageList = UFairyApplication::Get()->PackageList;
		//PackageList.Remove(pkg);

		PackageInstByID.Remove(pkg->ID);
		PackageInstByID.Remove(pkg->AssetPath);
		PackageInstByName.Remove(pkg->Name);
	}
	else 
	{
		UE_LOG(LogFairyGUI, Error, TEXT("invalid package name or id: %s"), *IDOrName);
	}
}

void UFairyPackageMgr::RemoveAllPackages()
{
	/*TArray<UFairyPackage*>& PackageList = UFairyApplication::Get()->PackageList;
	PackageList.Reset();*/

	PackageInstByID.Reset();
	PackageInstByName.Reset();
}

UFairyObject* UFairyPackageMgr::CreateObject(UObject* Outer, const FString& PackageName, const FString& ResourceName)
{
	UFairyPackage* pkg = GetPackageByName(PackageName);
	if (pkg) {
		return pkg->CreateObject(Outer, ResourceName);
	}
	else
	{
		return nullptr;
	}
}

UFairyObject* UFairyPackageMgr::CreateObjectFromURL(UObject* Outer, const FString& URL)
{
	TSharedPtr<FFairyPackageItem> PackageItem = GetPackageItemByURL(URL);
	if (PackageItem.IsValid()) 
	{
		return PackageItem->OwnerPackage->CreateObject(Outer, PackageItem);
	}
	else
	{
		return nullptr;
	}
}

UGWindow* UFairyPackageMgr::CreateWindow(UObject* Outer, const FString& PackageName, const FString& ResourceName)
{
	UFairyObject* ContentPane = CreateObject(Outer, PackageName, ResourceName);
	verifyf(ContentPane->IsA<UFairyComponent>(), TEXT("Window content should be a component"));
	UGWindow* Window = NewObject<UGWindow>(Outer);
	Window->SetContentPane(Cast<UFairyComponent>(ContentPane));

	return Window;
}