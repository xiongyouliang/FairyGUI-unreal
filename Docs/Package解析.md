# FairyGUI包管理

## FairyGUI包机制：
FairyGUI编辑器中，资源和组件以包的形式来管理；参看官方的文档：[包](https://fairygui.com/docs/editor/package)

### 从导出的二进制和解析库来看：
- FairyGUI编辑器中的每个包，导出到xxx.fui文件；纹理文件部分，会合并成单一的纹理集文件xxx_atlas0.png；
- 如果纹理数量多，超过单一的纹理集尺寸，会自动分成xxx_atlas0.png, xxx_atlas1.png等；
- 以上xxx为包名；

- UE4编辑器中导入: xxx.fui文件和xxx_atlas0.png文件，导入后对应文件为xxx.uasset, xxx_atlas0.uasset;
- UE4解析库中存在```class UFairyPackageAsset```类，该类只是简单的持有包的二进制数据；
- 编辑器中的每个包，对应解析库中```class UFairyPackage```的一个实例;
- 重构后，添加了```class UFairyPackageMgr```类，用来管理所有的```class UFairyPackage```实例，以及提供一些包关联的接口；
- 每个包中可以包含多个组件，每个组件对应解析库中```class UFairyPackageItem```的一个实例；即有```class UFairyPackage```类负责解析、构建、管理```class UFairyPackageItem```实例；
- 依据```class UFairyPackageItem```实例的类型、二进制的数据成员RawData来创建对应的组件实例；
- 一个组件中的元件可以是另外一个组件，反应在类对象模型中，一个```class UFairyPackageItem```实例，关联另外```class UFairyPackageItem```实例，该关系通过```class UFairyPackageItem```实例的ID属性来关联；


## 包管理重构后的类抽象层次：
- UFairyPackageMgr // 单例，可通过UE4的反射机制传递到Lua环境中来管理
    - UFairyPackage
        - UFairyPackageItem

## FByteBuffer类
包加载后，整个包的二进制数据由UFairyPackage对象持有；

class FByteBuffer是一个Helper类，用来提供方便访问二进制数据的api，并记录游标位置；

一个FByteBuffer对象可以是另一个FByteBuffer对象的Sub Buffer;

class FFairyPackageItem持有的二进制数据，其实是class UFairyPackage的Sub Buffer;

