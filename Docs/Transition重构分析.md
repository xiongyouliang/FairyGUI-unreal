# Transition系统重构分析

FairyGUI编辑器中称为[【动效】](https://fairygui.com/docs/editor/transition)

可以理解为基于时间轴的事件系统

在时间轴的时间线推进过程中，通过关键帧设定元件的属性；两个关键帧之间可选择建立补间动画；

从编辑器操作和UE4运行库代码的分析可以看到：
1. Transition对象绑定组件并存储在class UFairyComponent::Transitions数组中, 即一个组件可以建立多个Transition;
2. 一个Transition对象由若干TransitionItem组成；
3. 一个TransitionItem代表元件的某一属性的时间轴；
4. Editor中，处于组件中的元件，可以添加多条不同属性的时间轴
5. 在时间轴上建立关键帧，关键帧选中时可以在【检查器】中设定相应属性值；
6. 两个关键帧之间可以选择建立补帧动画；

# 问题
编辑器非开源，这个没有办法重构；

下面罗列一下官方UE4运行库的问题
1. Transition对象存储在组件类中，而组件是所有高阶复合组件的基类，而并不是所有的组件都会添加【动效】功能，造成额外的内存负担；
2. class UTransition和class FTransitionItem两个类承载了元件所有属性时间轴和关键帧数据的抽象；造成代码逻辑混乱和高度的耦合，逻辑结构不清晰和数据对象内存非必要的开销；

# 重构思路
1. 新增一个TransitionMgr类，用来管理Transition对象和Component对象之间的关联关系；
2. 重构class UTransition，除了Transition本身必要的数据，class UTransition负责class FTransitionItem对象的管理，但不负责具体的时间轴和补间动画逻辑；
3. 重构class FTransitionItem，使用类继承关系，建立基类class FTransitionItemBase并设定好虚接口；
4. 不同属性的TransitionItem从class FTransitinItemBase派生而来，并管理自身关键帧数据和补间动画；
5. 关于命名：统一修改成UFairyXXX或FFairyXXX;

