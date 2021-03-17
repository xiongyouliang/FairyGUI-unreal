#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UIPackageMgr.generated.h"

class FPackageItem;
class UGObject;
class FByteBuffer;
class UUIPackageAsset;


UCLASS(BlueprintType)
class FAIRYGUI_API UUIPackageMgr : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	static const FString& GetBranch() { return Branch; }

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	static void SetBranch(const FString& InBranch);


	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	static FString GetVar(const FString& VarKey);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	static void SetVar(const FString& VarKey, const FString& VarValue);

	// Package Manager Interface start
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	static UUIPackage* AddPackage(UObject* WorldContextObject, UUIPackageAsset* InAsset);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	static UUIPackage* GetPackageByName(const FString& PackageName);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	static UUIPackage* GetPackageByID(const FString& PackageID);

	static FString ConvertToItemURL(const FString& PackageName, const FString& ResourceName);
	static FString NormalizeURL(const FString& URL);
	static TSharedPtr<FPackageItem> GetPackageItemByURL(const FString& URL);
	

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	static void RemovePackage(const FString& IDOrName);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	static void RemoveAllPackages();
	// Package Manager Interface end

	UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (DisplayName = "Create UI", DeterminesOutputType = "ClassType"))
	static UGObject* CreateObject(UObject* Outer, const FString& PackageName, const FString& ResourceName);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (DisplayName = "Create UI From URL", DeterminesOutputType = "ClassType"))
	static UGObject* CreateObjectFromURL(UObject* Outer, const FString& URL);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	static UGWindow* CreateWindow(UObject* Outer, const FString& PackageName, const FString& ResourceName);

	

private:
	//UUIPackageMgr() {};

	static TMap<FString, UUIPackage*> PackageInstByID;
	static TMap<FString, UUIPackage*> PackageInstByName;
	static TMap<FString, FString> Vars;
	static FString Branch;
};

