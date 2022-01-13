#include "Package/FairyPackageItem.h"
#include "Package/FairyPackage.h"
#include "UI/FairyRoot.h"
#include "Utils/ByteBuffer.h"

FFairyPackageItem::FFairyPackageItem() :
	OwnerPackage(nullptr),
	PackageItemType(EPackageItemType::Unknown),
	RawObjectType(EFairyObjectType::Component),
	Size(0, 0),
	Texture(nullptr),
	bScaleByTile(false),
	TileGridIndice(0)
{
}

void FFairyPackageItem::Load()
{
	if (OwnerPackage)
	{
		OwnerPackage->GetItemAsset(AsShared());
	}
}

TSharedPtr<FFairyPackageItem> FFairyPackageItem::GetBranch()
{
	if (Branches.IsSet() && OwnerPackage->BranchIndex != -1)
	{
		const FName& ItemID = Branches.GetValue()[OwnerPackage->BranchIndex];
		if (!ItemID.IsNone())
		{
			return OwnerPackage->GetItem(ItemID);
		}
	}

	return AsShared();
}

TSharedPtr<FFairyPackageItem> FFairyPackageItem::GetHighResolution()
{
	if (HighResolution.IsSet() && UFairyRoot::ContentScaleLevel > 0)
	{
		FName ItemID = HighResolution.GetValue()[UFairyRoot::ContentScaleLevel - 1];
		if (!ItemID.IsNone())
		{
			return OwnerPackage->GetItem(ItemID);
		}
	}

	return AsShared();
}

EFairyObjectType FFairyPackageItem::GetExtendFairyObjectType()
{
	EFairyObjectType TargetObjectType = RawObjectType;
	if (TargetObjectType == EFairyObjectType::Component && RawData.IsValid())
	{
		/**
		* Simulate component construct code to get overflow value to identify the component really type;
		*/
		FairyGUI::FByteBuffer* Buffer = RawData.Get();
		int32 oldPos = Buffer->GetPos();

		Buffer->Seek(0, 0);

		// Object Size attribute
		Buffer->ReadInt();
		Buffer->ReadInt();

		if (Buffer->ReadBool())
		{
			Buffer->ReadInt();
			Buffer->ReadInt();
			Buffer->ReadInt();
			Buffer->ReadInt();
		}

		// Object Pivot attribute
		if (Buffer->ReadBool())
		{
			Buffer->ReadFloat();
			Buffer->ReadFloat();
			Buffer->ReadBool();
		}

		// Component Margin attribute
		if (Buffer->ReadBool())
		{
			Buffer->ReadInt();
			Buffer->ReadInt();
			Buffer->ReadInt();
			Buffer->ReadInt();
		}
		EOverflowType overflow = (EOverflowType)Buffer->ReadByte();
		if (overflow == EOverflowType::Visible)
		{
			TargetObjectType = EFairyObjectType::Component;
		}
		else if (overflow == EOverflowType::Hidden)
		{
			TargetObjectType = EFairyObjectType::ClippingComponent;
		}
		else if (overflow == EOverflowType::Scroll)
		{
			TargetObjectType = EFairyObjectType::ScrollComponent;
		}

		Buffer->SetPos(oldPos);
	}
	return TargetObjectType;
}

//void FFairyPackageItem::AddReferencedObjects(FReferenceCollector& Collector)
//{
//	if (Texture != nullptr)
//	{
//		Collector.AddReferencedObject(Texture);
//	}
//}