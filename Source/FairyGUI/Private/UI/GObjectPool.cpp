#include "UI/GObjectPool.h"
#include "UI/GObject.h"
#include "UI/UIPackage.h"
#include "UI/UIPackageMgr.h"

UGObject* FGObjectPool::GetObject(const FString & URL)
{
    FString URL2 = UUIPackageMgr::Get()->NormalizeURL(URL);
    if (URL2.Len() == 0)
        return nullptr;

    UGObject* ret = nullptr;
    TArray<UGObject*>& arr = Pool.FindOrAdd(URL2);
    if (arr.Num() > 0)
        ret = arr.Pop();
    //else
        //ret = UUIPackageMgr::CreateObjectFromURL(GetOuter(), URL2);
    return ret;
}

void FGObjectPool::ReturnObject(UGObject* Obj)
{
    TArray<UGObject*>& arr = Pool.FindOrAdd(Obj->GetResourceURL());
    arr.Add(Obj);
}

void FGObjectPool::AddReferencedObjects(FReferenceCollector& Collector)
{
    for (auto& Elem : Pool)
    {
        Collector.AddReferencedObjects(Elem.Value);
    }
}