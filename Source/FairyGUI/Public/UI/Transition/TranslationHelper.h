#pragma once

#include "CoreMinimal.h"

class FFairyPackageItem;

class FTranslationHelper
{
public:
    static TMap<FString, TMap<FString, FString>> Strings;

    static void LoadFromXML(const FString XmlString);
    static void TranslateComponent(const TSharedPtr<FFairyPackageItem>& Item);
};