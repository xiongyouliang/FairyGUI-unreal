#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UIPackageMgr.generated.h"

class FPackageItem;
class UFairyObject;
class FByteBuffer;
class UUIPackageAsset;


UCLASS(BlueprintType)
class FAIRYGUI_API UUIPackageMgr : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	static UUIPackageMgr* Get();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	const FString& GetBranch() { return Branch; }

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetBranch(const FString& InBranch);


	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FString GetVar(const FString& VarKey);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetVar(const FString& VarKey, const FString& VarValue);

	// Package Manager Interface start
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UUIPackage* AddPackage(UUIPackageAsset* InAsset);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UUIPackage* GetPackageByName(const FString& PackageName);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UUIPackage* GetPackageByID(const FString& PackageID);

	FString ConvertToItemURL(const FString& PackageName, const FString& ResourceName);
	FString NormalizeURL(const FString& URL);
	TSharedPtr<FPackageItem> GetPackageItemByURL(const FString& URL);
	

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void RemovePackage(const FString& IDOrName);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void RemoveAllPackages();
	// Package Manager Interface end

	UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (DisplayName = "Create UI", DeterminesOutputType = "ClassType"))
	UFairyObject* CreateObject(UObject* Outer, const FString& PackageName, const FString& ResourceName);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (DisplayName = "Create UI From URL", DeterminesOutputType = "ClassType"))
	UFairyObject* CreateObjectFromURL(UObject* Outer, const FString& URL);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UGWindow* CreateWindow(UObject* Outer, const FString& PackageName, const FString& ResourceName);

private:
	UUIPackageMgr();
	~UUIPackageMgr();

	UPROPERTY()
	TMap<FString, UUIPackage*> PackageInstByID;
	UPROPERTY()
	TMap<FString, UUIPackage*> PackageInstByName;
	TMap<FString, FString> Vars;
	FString Branch;

	static UUIPackageMgr* Instance;
};

