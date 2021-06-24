#include "UI/GObjectPool.h"
#include "UI/FairyObject.h"
#include "Package/UIPackage.h"
#include "Package/UIPackageMgr.h"

UFairyObject* FGObjectPool::GetObject(const FString & URL)
{
    FString URL2 = UUIPackageMgr::Get()->NormalizeURL(URL);
    if (URL2.Len() == 0)
        return nullptr;

    UFairyObject* ret = nullptr;
    TArray<UFairyObject*>& arr = Pool.FindOrAdd(URL2);
    if (arr.Num() > 0)
        ret = arr.Pop();
    //else
        //ret = UUIPackageMgr::CreateObjectFromURL(GetOuter(), URL2);
    return ret;
}

void FGObjectPool::ReturnObject(UFairyObject* Obj)
{
    TArray<UFairyObject*>& arr = Pool.FindOrAdd(Obj->GetResourceURL());
    arr.Add(Obj);
}

void FGObjectPool::AddReferencedObjects(FReferenceCollector& Collector)
{
    for (auto& Elem : Pool)
    {
        Collector.AddReferencedObjects(Elem.Value);
    }
}