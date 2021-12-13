#include "UI/FairyObjectPool.h"
#include "UI/FairyObject.h"
#include "Package/FairyPackage.h"
#include "Package/FairyPackageMgr.h"

UFairyObject* FFairyObjectPool::GetOrCreateObject(UObject* Outer, const FString & URL)
{
    FString URL2 = UFairyPackageMgr::Get()->NormalizeURL(URL);
    if (URL2.Len() == 0)
    {
        return nullptr;
    }

    UFairyObject* ret = nullptr;
    TArray<UFairyObject*>& arr = Pool.FindOrAdd(URL2);
    if (arr.Num() > 0)
    {
        ret = arr.Pop();
    }
    else
    {
        ret = UFairyPackageMgr::Get()->CreateObjectFromURL(Outer, FName(URL2));
    }
    return ret;
}

void FFairyObjectPool::ReturnObject(UFairyObject* Obj)
{
    TArray<UFairyObject*>& arr = Pool.FindOrAdd(Obj->GetResourceURL());
    arr.Add(Obj);
}

void FFairyObjectPool::AddReferencedObjects(FReferenceCollector& Collector)
{
    for (auto& Elem : Pool)
    {
        Collector.AddReferencedObjects(Elem.Value);
    }
}