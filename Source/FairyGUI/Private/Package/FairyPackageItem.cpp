#include "Package/FairyPackageItem.h"
#include "Package/FairyPackage.h"
#include "UI/FairyRoot.h"
#include "Utils/ByteBuffer.h"

FFairyPackageItem::FFairyPackageItem() :
    Owner(nullptr),
    Type(EPackageItemType::Unknown),
    ObjectType(EObjectType::Component),
    Size(0, 0),
    Texture(nullptr),
    bScaleByTile(false),
    TileGridIndice(0)
{
}

void FFairyPackageItem::Load()
{
    Owner->GetItemAsset(AsShared());
}

TSharedPtr<FFairyPackageItem> FFairyPackageItem::GetBranch()
{
    if (Branches.IsSet() && Owner->BranchIndex != -1)
    {
        const FString& ItemID = Branches.GetValue()[Owner->BranchIndex];
        if (!ItemID.IsEmpty())
            return Owner->GetItem(ItemID);
    }

    return AsShared();
}

TSharedPtr<FFairyPackageItem> FFairyPackageItem::GetHighResolution()
{
    if (HighResolution.IsSet() && UFairyRoot::ContentScaleLevel > 0)
    {
        FString ItemID = HighResolution.GetValue()[UFairyRoot::ContentScaleLevel - 1];
        if (!ItemID.IsEmpty())
            return Owner->GetItem(ItemID);
    }

    return AsShared();
}

void FFairyPackageItem::AddReferencedObjects(FReferenceCollector& Collector)
{
    if (Texture != nullptr)
        Collector.AddReferencedObject(Texture);
}