#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "UI/FieldTypes.h"
#include "FairyCommons.h"
#include "Utils/ByteBuffer.h"

struct FMovieClipData;
struct FBitmapFont;

class UFairyPackage;
class UNTexture;
class UFairyComponent;

class FAIRYGUI_API FFairyPackageItem : public TSharedFromThis<FFairyPackageItem>
{
private:
	friend class UFairyPackage;

	UFairyPackage* OwnerPackage;
	EPackageItemType PackageItemType;
	EFairyObjectType RawObjectType;

public:
	FFairyPackageItem();

	void Load();
	TSharedPtr<FFairyPackageItem> GetBranch();
	TSharedPtr<FFairyPackageItem> GetHighResolution();

	//virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

public:
	inline UFairyPackage* GetOwnerPackage() { return OwnerPackage; }
	inline EPackageItemType GetPackageItemType() { return PackageItemType; }
	inline EFairyObjectType GetRawFairyObjectType() { return RawObjectType; }
	inline EFairyObjectType GetExtendFairyObjectType();
public:
	FName ID;
	FName Name;
	FName File;
	FVector2D Size;

	TSharedPtr<FairyGUI::FByteBuffer> RawData;
	TOptional<TArray<FName>> Branches;
	TOptional<TArray<FName>> HighResolution;

	//atlas
	UNTexture* Texture;

	//image
	TOptional<FBox2D> Scale9Grid;
	bool bScaleByTile;
	int32 TileGridIndice;
	//PixelHitTestData* pixelHitTestData;

	//movie clip
	TSharedPtr<FMovieClipData> MovieClipData;

	//component
	FGComponentCreator ExtensionCreator;
	bool bTranslated;

	//font
	TSharedPtr<FBitmapFont> BitmapFont;

	//sound
	FSlateSound Sound;
};
