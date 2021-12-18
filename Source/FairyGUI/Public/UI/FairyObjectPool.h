#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

class UFairyObject;

class FFairyObjectPool : public FGCObject
{
public:
    UFairyObject* GetOrCreateObject(UObject* Outer, const FName& URL);
    void ReturnObject(UFairyObject* Obj);

    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

private:
    TMap<FName, TArray<UFairyObject*>> Pool;
};
