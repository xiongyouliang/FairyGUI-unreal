#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FairyPackageAsset.generated.h"

UCLASS()
class FAIRYGUI_API UFairyPackageAsset : public UObject
{
    GENERATED_BODY()

public:

    UFairyPackageAsset();
    ~UFairyPackageAsset();

    UPROPERTY(EditAnywhere)
    TArray<uint8> Data;

#if WITH_EDITORONLY_DATA
    UPROPERTY(Instanced)
    class UAssetImportData* AssetImportData;

    virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
#endif
};