#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

class UFairyObject;

class FGObjectPool : public FGCObject
{
public:
    UFairyObject* GetObject(const FString& URL);
    void ReturnObject(UFairyObject* Obj);

    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

private:
    TMap<FString, TArray<UFairyObject*>> Pool;
};