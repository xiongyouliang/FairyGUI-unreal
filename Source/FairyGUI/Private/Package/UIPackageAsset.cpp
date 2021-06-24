#include "Package/UIPackageAsset.h"
#include "EditorFramework/AssetImportData.h"

UUIPackageAsset::UUIPackageAsset()
{
    UE_LOG(LogTemp, Warning, TEXT("UUIPackageAsset construction"));
}

UUIPackageAsset::~UUIPackageAsset()
{
    UE_LOG(LogTemp, Warning, TEXT("UUIPackageAsset destruction"));
}

#if WITH_EDITORONLY_DATA
void UUIPackageAsset::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
    if (AssetImportData)
    {
        OutTags.Add(FAssetRegistryTag(SourceFileTagName(),
            AssetImportData->GetSourceData().ToJson(),
            FAssetRegistryTag::TT_Hidden));
#if WITH_EDITOR
        AssetImportData->AppendAssetRegistryTags(OutTags);
#endif
    }

    Super::GetAssetRegistryTags(OutTags);
}
#endif