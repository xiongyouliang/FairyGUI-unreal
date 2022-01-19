# Controller功能重构分析：
## 官方文档概述：
* 分页：一个组件可以由多个页面组成；可以通过Controller来控制页面的切换
* 按钮状态： 按钮通常由按下、鼠标悬浮等多个状态，可以利用控制器为每个状态安排不同的显示内容；
* 属性变化：利用控制器，可以使元件具有多个不同的形态，可以在不同形态之间切换
从文档描述来看，其实就是一个状态机；


## 功能分析
结合文档描述和FairyGUI实操来看，可以罗列下列功能点：
* 一个组件可以添加多个控制器；
* 每个控制器可以添加多个页(状态)，每个页由ID、Name、RemarkText三个属性组成；runtime中可以切换到控制器的不同页；
* 可以设定一个控制器的默认页(其实就是默认状态);
* 可以为一个控制器添加多个动作，动作分两种：播放动效、改变其他控制器的页面; 动作描述当前控制器从一个页切换到另一个页时需要执行的逻辑；
    * 播放动效好理解：指定一个当前组件已配置的动效来播放；
    * 改变其他控制器的页面：即改变其他控制器的状态；这里的其他控制器可以是当前组件的其他控制器，也可以时当前组件的自组件中的控制器；该功能看似强大，其实非常粗暴，应该要避免使用，关联链一长，自己都不知道绕哪里去了~

从功能点罗列来看，这是一个两级的状态，但是控制器是同时起作用的; 

## 控制器的使用
以上只是罗列了给组件添加控制器，下面罗列下控制器在编辑器中的使用：
* 当前组件中的任何一个child元件有一个【属性控制】设定；
* 可以为元件指定多项属性控制；
* 每个属性控制中可以指定一个控制器+多个该控制器的页；

即：当组件的控制器处于的某个页时，如果该元件的属性控制设定匹配，触发该元件的属性变化；

## 代码抽象层分析
下面罗列一下官方代码的抽象：
* class UFairyComponent是组件：
    * ```TArray<UFairyController*> Controllers;```成员存储所有的控制器；
    * ```UFairyController* ApplyingController;```成员标识当前正在应用的控制器（如果有的话）；
* class UFairyController是控制器类：
    * ```TArray<FString> PageIDs;```成员用来存储页ID信息；
    * ```TArray<FString> PageNames;```成员存储页Name信息；
    * ```TIndirectArray<FControllerAction> Actions;```成员存储动作信息
    * 其他一些控制器信息

* class UFairyObject是所有元件（包括组件）的基类，元件的属性控制信息放在该类中：
    * ```bool bHandlingController = false;```成员标识当前是否受控制器控制
	* ```FGearBase* Gears[10];```成员存储设定的【属性控制】条目信息
    * 一个class FGearBase对象会负责关联元件对象和控制器对象，并保存设定信息；

## 运行逻辑
* 当前组件对象通过二进制数据构建后，会触发一次所有控制器的apply操作；
* apply逻辑中，通过遍历该组件的所有Child, 如果该Child的Gears中的参数匹配到了该控制器的当前页，就会执行对应的属性设定参数；
* 在runtime中，可以通过修改该某个控制器的当前页，达到间接控制Child的属性目的；

## 需要重构的问题
1. class UFairyObject是所有元件的基类，相关的data member给不需要添加控制器功能的对象带来额外的负担，这种负担贯穿于整个FairGUI对象中；class UFairyComponent有同样的问题；
2. 当控制器被apply时，遍历组件的所有childred带来额外的负担，因为一个组件中并不会所有的child元件都会添加属性控制；也就是说当前apply动作的效率可能是极低的；
3. 通过前面的分析：一个状态抽象，一个是apply动作，使用State和Observer模式，外加一个第三方的管理器对象可以很好的解决上面存在的问题；

设计一个class UFairyControllerMgr（当然，也可以在现有的Mgr类上进行扩展，比如UFairyApplication类等），将组件的Controller、带有属性控制的child以合理的数据结构收集起来；并与当前组件做好关联；当组件需要apply所有或某个控制器时，转交给这个Mgr对象来处理；

## 新的对象模型
* UFairyControllerMgr对象，负责管理所有组件的所有Controller对象；
    * UFairyController对象从class UFairyComponent移动到UFairyControllerMgr，并与目标UFairyComponent对象关联；保留class UFairyComponent关于Controller相关的API；
* 构建观察者模式：Controller充当Subject， Gear充当Observer；Gear包含设定的属性信息并关联元件；
* Gears对象从class UFairyObject移动到UFairyController; 当该UFairyController对象的状态变化时，符合条件Gears执行各自的动作；





