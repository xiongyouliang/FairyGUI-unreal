#include "Package/FairyPackageAsset.h"
#include "EditorFramework/AssetImportData.h"

UFairyPackageAsset::UFairyPackageAsset()
{
    UE_LOG(LogTemp, Warning, TEXT("UFairyPackageAsset construction"));
}

UFairyPackageAsset::~UFairyPackageAsset()
{
    UE_LOG(LogTemp, Warning, TEXT("UFairyPackageAsset destruction"));
}

#if WITH_EDITORONLY_DATA
void UFairyPackageAsset::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
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