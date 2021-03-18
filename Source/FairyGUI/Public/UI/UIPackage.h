#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UIPackage.generated.h"

class FPackageItem;
class UGObject;
class FByteBuffer;
class UUIPackageAsset;

UCLASS(BlueprintType)
class FAIRYGUI_API UUIPackage : public UObject
{
    GENERATED_BODY()

public:
    static int32 Constructing;

public:
    UUIPackage();
    virtual  ~UUIPackage();

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    const FString& GetID() const { return ID; }
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    const FString& GetName() const { return Name; }

    TSharedPtr<FPackageItem> GetItem(const FString& ResourceID) const;
    TSharedPtr<FPackageItem> GetItemByName(const FString& ResourceName);
    void* GetItemAsset(const TSharedPtr<FPackageItem>& Item);

    UGObject* CreateObject(UObject* Owner, const FString& ResourceName);
    UGObject* CreateObject(UObject* Owner, const TSharedPtr<FPackageItem>& Item);

private:
    void Load(FByteBuffer* Buffer);
    void LoadAtlas(const TSharedPtr<FPackageItem>& Item);
    void LoadImage(const TSharedPtr<FPackageItem>& Item);
    void LoadMovieClip(const TSharedPtr<FPackageItem>& Item);
    void LoadFont(const TSharedPtr<FPackageItem>& Item);
    void LoadSound(const TSharedPtr<FPackageItem>& Item);

private:
    FString ID;
    FString Name;
    FString AssetPath;
    UPROPERTY(Transient)
    UUIPackageAsset* Asset;

    TArray<TSharedPtr<FPackageItem>> Items;
    TMap<FString, TSharedPtr<FPackageItem>> ItemsByID;
    TMap<FString, TSharedPtr<FPackageItem>> ItemsByName;
    TMap<FString, struct FAtlasSprite*> Sprites;
    FString CustomID;
    TArray<TMap<FString, FString>> Dependencies;
    TArray<FString> Branches;
    int32 BranchIndex;

    static TMap<FString, UUIPackage*> PackageInstByID;
    static TMap<FString, UUIPackage*> PackageInstByName;
    static TMap<FString, FString> Vars;
    static FString Branch;

    friend class UUIPackageMgr;
    friend class FPackageItem;
    friend class UFairyApplication;
};
