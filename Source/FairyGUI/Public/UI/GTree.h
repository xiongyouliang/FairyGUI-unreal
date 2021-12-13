#pragma once

#include "UI/FairyListView.h"
#include "GTree.generated.h"

UCLASS(BlueprintType)
class FAIRYGUI_API UGTree : public UFairyListView
{
    GENERATED_BODY()

public:
    UGTree();
    virtual ~UGTree();
};