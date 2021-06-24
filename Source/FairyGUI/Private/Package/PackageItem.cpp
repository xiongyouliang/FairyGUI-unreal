#include "Package/PackageItem.h"
#include "Package/UIPackage.h"
#include "UI/FairyRoot.h"
#include "Utils/ByteBuffer.h"

FPackageItem::FPackageItem() :
    Owner(nullptr),
    Type(EPackageItemType::Unknown),
    ObjectType(EObjectType::Component),
    Size(0, 0),
    Texture(nullptr),
    bScaleByTile(false),
    TileGridIndice(0)
{
}

void FPackageItem::Load()
{
    Owner->GetItemAsset(AsShared());
}

TSharedPtr<FPackageItem> FPackageItem::GetBranch()
{
    if (Branches.IsSet() && Owner->BranchIndex != -1)
    {
        const FString& ItemID = Branches.GetValue()[Owner->BranchIndex];
        if (!ItemID.IsEmpty())
            return Owner->GetItem(ItemID);
    }

    return AsShared();
}

TSharedPtr<FPackageItem> FPackageItem::GetHighResolution()
{
    if (HighResolution.IsSet() && UFairyRoot::ContentScaleLevel > 0)
    {
        FString ItemID = HighResolution.GetValue()[UFairyRoot::ContentScaleLevel - 1];
        if (!ItemID.IsEmpty())
            return Owner->GetItem(ItemID);
    }

    return AsShared();
}

void FPackageItem::AddReferencedObjects(FReferenceCollector& Collector)
{
    if (Texture != nullptr)
        Collector.AddReferencedObject(Texture);
}