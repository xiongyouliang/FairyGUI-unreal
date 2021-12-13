#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FairyPackage.generated.h"

class FFairyPackageItem;
class UFairyObject;
class FByteBuffer;
class UFairyPackageAsset;

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyPackage : public UObject
{
    GENERATED_BODY()

public:
    static int32 Constructing;

public:
    UFairyPackage();
    virtual  ~UFairyPackage();

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    const FName GetID() const { return ID; }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    const FName GetName() const { return Name; }

    TSharedPtr<FFairyPackageItem> GetItem(const FName& ResourceID) const;
    TSharedPtr<FFairyPackageItem> GetItemByName(const FName& ResourceName);
    void* GetItemAsset(const TSharedPtr<FFairyPackageItem>& Item);

    UFairyObject* CreateObject(UObject* Owner, const FName& ResourceName);
    UFairyObject* CreateObject(UObject* Owner, const TSharedPtr<FFairyPackageItem>& Item);

private:
    void Load(FByteBuffer* Buffer);
    void LoadAtlas(const TSharedPtr<FFairyPackageItem>& Item);
    void LoadImage(const TSharedPtr<FFairyPackageItem>& Item);
    void LoadMovieClip(const TSharedPtr<FFairyPackageItem>& Item);
    void LoadFont(const TSharedPtr<FFairyPackageItem>& Item);
    void LoadSound(const TSharedPtr<FFairyPackageItem>& Item);

    UObject* LoadSoundObject(FName GameAssetFilePath);

private:
    FName ID;
    FName Name;
    FName AssetPath;

    UPROPERTY(Transient)
    UFairyPackageAsset* Asset;

    TArray<TSharedPtr<FFairyPackageItem>> Items;
    TMap<FName, TSharedPtr<FFairyPackageItem>> ItemsByID;
    TMap<FName, TSharedPtr<FFairyPackageItem>> ItemsByName;
    TMap<FName, struct FAtlasSprite*> Sprites;
    
    UPROPERTY()
    TMap<FName, UObject*> SoundsByName;

    FName CustomID;
    TArray<TMap<FName, FName>> Dependencies;
    TArray<FName> Branches;
    int32 BranchIndex;

    friend class UFairyPackageMgr;
    friend class FFairyPackageItem;
    friend class UFairyApplication;
};
