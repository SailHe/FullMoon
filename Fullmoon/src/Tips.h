/*
精灵类Sprite 动画精灵类AnimationSprite
按钮类Button 摄像机Camera
-模型GameObject 地形Land
资源管理类ResManager
精灵管理类SpriteManager
渲染管理类RenderManager
输入Input 声音Sound 纹理类Texture
游戏主类Game
*/

/*
x是列数left也是列数
图形编程时使用二维数组作为2D界面缓冲 左上角作为原点 很容易就会使人想到行 列 数组下标
但数学用的(x, y)表示法在将数组与像素点一一对应时却是有问题的
在[数组索引时](y, x) 而不用数学习惯的(x, y) 其余接口使用(x, y)(这样兼顾了两种习惯 懒得改了)
1.与生活习惯接轨:vs的debug是横着排的 而横着排 一般习惯就认为这是一行
2.与图形习惯接轨:图形中的h和w概念 也是竖着代表高 横着代表宽

上面全是屁话 平时x, y随便用没什么关系 可以将(x, y)视为(r, c)嘛
关键在于 它们之间相互转换时关系绝不能弄混: h y 对应 r,  w x 对应 c(而且最好不用hw作为迭代变量 而应将其作为上限判断用)
EG:当二维数组的编号与下标相转换时详见自制迭代函数
for(y < h){
for(x < w){}
}
当然一般先迭代y也不错
最后强调x是行方向   y才是行数(cols)
*/

/*
** lhs 左手边 rhs 右手边 (与左值 右值是有区别的)
** 左值（lvalue） 右值（rvalue） 左值引用（lvalue reference） 右值引用（rvalue reference）
** 右值引用(&&):对右值的引用(C++11) 实际上只是一种标识, 并不是说移动语义下的形参rhs就是右值了 它只是标识:我需要一个引用, 该引用的对象要是右值
** 有名字就能取地址 能取到地址就是左值 与类型无关 -> 有名字的都是左值 rhs也是
** 取地址运算后就算是函数返回值(右值)也会变为左值
** const int &	左值右值都可绑定
** const int &&	只能绑定常量右值(没什么应用场景)
** int &			只能绑定左值
** int &&		只能绑定右值
** 分别使用四者作为形参的函数会识别出与形参最接近的那个值
** 使用移动语义(move semantic)的情景:转移构造，转移赋值(move assignment/n. 分配；任务；作业；功课)
** 通用引用(universal reference)情境:必须满足T&&这种形式 类型T必须是通过推断得到的(函数模板参数 auto typedef decltype)->尽可能使用std::forward<T>()
** 右值拷贝构造(拷贝构造本身就必须使用引用:传入参数会调用拷贝构造 若不用引用形参 拷贝构造就是个死循环)
**注意:
*/

/*tips:
***段错误是访问了非法内存 系统给出的警告
***NULL在c中代表地址0, 而0在一般的操作系统中都是不可被访问的,此时会引发段错误 nullptr是有类型的
*** 用指针作形参 容易误会形参为0 (NULL被视为0 若存在形参为int构造函数 则会自动构造 结果引起错误:原本不同的两个类型就莫名其妙可以比较了

*** const相当于是道德约束而非法律约束,有些情况下是可改的
***为了方便查看: const不要写在最前面(若全是定义 一列下来不容易区分普通void返回值和const) 用同义的中间模式
***据此 引用和指针修饰最好靠近变量名而不是类型说明符(虽然整个连起来才是其类型 但这样const int* a将const移到具有同样意义的*之前时不能直接用鼠标拖拽... 而且这样比较好看)

***野指针解引用不会引发段错误 但是危害极大
*** 完全将内存交给类型去控制:若需要指针 在类型内部使用指针 外部接口统一用引用
***即:set传入引用 get返回常量引用(引用常量的引用) 而不返回常量指针(指向常量的指针)
***!!!
*** 若set支持传入指针形参 那么它必须能够自行管理传入的内存
***并且能够区分右值引用(实现移动 上面定义了外界接口统一用引用 这里只针对new操作符)
***!!!
*/


//类型的字段分类 上面是不需要自己管理的资源 下面是需要自己管理的 用置空方法null分割
//置空方法null的功能固定是置空自己管理内存的指针 和不需自己管理但可以置空的指针
//否则按(友元 常量 静态值 成员变量 指针)分类  (放于类首部)

//左边靠近什么就是什么const int*const是指针(顶层) 特别的, 最左边时是(底层) 若指定命名空间 也是底层
//有需要管理的指针域就必须定义 赋值 拷贝构造 以及析构方法 最好还能同时定义相应的右引用版本
//指针域用xx_表示(get的是 引用 不是 实体) 本来用_xx命名更利于代码提示 但由于这里引入了一个blin的库有一大堆名字前带_的所以...
//最优选择:优先定义赋值方法 再用其定义拷贝构造

/*
多态的关键之一是父类指针
运算符 -> 的重载比较特别,它只能是非静态的成员函数形式，而且没有参数。
如果返回值是一个原始指针,那么就将运算符的右操作数当作这个原始指针所指向类型的成员进行访问；
如果返回值是另一个类型的实例,那么就继续调用这个返回类型的 operator->(),
直到有一个调用返回一个原始指针为止，然后按第一种情况处理。

Pointer *operator->() const {
return pointer;
}
*/


/*
帧(Frame):影像动画中最小单位的单幅影像画面, 一帧就是一副静止的画面，连续的帧就形成动画;
基于帧的动画:通过显示一系列预先生成的静态帧图像模拟移动.
基于形状的动画(子画面动画):通过 独立于背景移动的图形对象(子画面)的位置变化实现移动.
*/
/*
tips: 精灵位置仅仅是个用于位置计算的概念 可以是区域内任何位置(头的位置, 重心的位置 都可以是其定义)
**精灵位置: 相对概念, 类型保证其可以用于即时位置计算, 内置了位置比较方法, 因此不提供外部接口
**绘图位置: 绝对概念, 区域的左角(left, top)
**精灵中心: 绝对概念, 区域的几何中心
place含义广泛，最普通用词，既可指很小的地点，又可指很大很远的地方或场所。
position多指物体[相对于]其他物体所处的位置或状态。
spot指相对较小的特定地点或事物所在地。
situation指物体在其周围环境中所处的位置或状态，侧重地点或场所的环境特征。
site指或大或小的地方，既可指供专门用途或特定活动的地点，又可指某一事件的地址。
location指某物[设置]的方向或地点。
locality指某物所处的客观位置和周围地区。
setting一般特指戏剧或小说所写的环境或场所。
scene常指真实事件或虚构故事发生的地点。
*/
/*
figure体形贴图
n. 数字；人物；图形；价格；（人的）体形；画像
vi. 计算；出现；扮演角色
vt. 计算；认为；描绘；象征
*/
/*
目的:修改父类尽可能少的修改(确切地说是 减少修改时对功能的理解时间)

继承原则:1.对子类开放的成员不应该是为了便于直接访问
(基本类型除外 若5个及以上的基本类型直接存在于类型中 应考虑分类)
2.只应该对子类开放由虚方法维护所有成员

继承的访问修饰符使用情形:
private: 子类不存在子类时
protected: 当不满足上一条时
public: 尽量不用(为了父类的本地化 父类不应该由外界维护)
应该返回常量的父类对象引用
然后为外界需要设置的参数编写set方法(最好换个本地化的名字 这时往往会发现没必要给外界的权限)

成员访问控制原则:坚决不要public, 其余同上
*/

/*
//将index后面的num个元素删除 并将后面的元素拼接到前面?
myVarrays &splice(int index, int num = 1) {
auto &arr = *this;
int len = arr.size();
for (int i = index + num; i < len; ++i) {
arr[i - num] = arr[i];
}
arr.resize(len - num);
return *this;
};*/
/*
拼接 https://blog.csdn.net/bichenggui/article/details/4674900
//void splice(iterator position , list& x)
将x的链表归并到当前list链表的position位置之前， list对象x将被清空

void splice(iterator position , list& , iterator i)
将一个list的迭代器i值所指的元素，归并到当前list链表中， 并将被归并元素从原链表中删除
//合并多个数组；合并多个字符串
myList &concat(list<T> const &addVec){
this->insert(this->end(), addVec.begin(), addVec.end());
return *this;
}
*/

//PS: 这其实就是个文本文件 只不过绿色的备注比较好看 而且写代码有格式控制