#ifndef _TREEOBJECT_H
#define _TREEOBJECT_H

#include"../stdafx.h"
#include"../ExtendSpace.h"
/*
*结语:
*树毕竟只是容器 适合于增删查的动态操作 不适合用来作遍历
*这只能算是PTA和数据结构课程或者说幼稚与偏执想法的遗留产物 以后估计就算有这样的想法也鲜有这样的机会与环境了
*纠正了许多常识性但根深蒂固的认识错误
*对[树结构]的认识与理解异于常人
*对[C++的标准库]使用 以及[模板编程]{类模板 函数模板} 有了一些心得(C++高级编程)
*对[面向对象编程]多了几分理解和经验
*对[算法]优劣的衡量有一定理解(上限O 线性Θ 下限Ω)
*这一完全的树对象继承关系的优势在于相互间的转换很方便不费时(但本身效率比不上单独实现的)
*/
/*树 抽象类(接口类)*/
class Tree{
public:
	//遍历的顺序类型枚举
	enum TraversalOrderEnum{
		ORDER_PREFIX_ROOT, //先根序
		ORDER_INFIX_ROOT, //中根序
		ORDER_POST_ROOT, //后根序
		ORDER_LEVEL, //层序
		ORDER, //顺序(就是先根序 但可以中止遍历)
		RE_ORDER, //逆序
		NONE_ORDER	//无: 未指定
	};
	int Min(int a, int b){
		return a < b ? a : b;
	}
	int Max(int a, int b){
		return a > b ? a : b;
	}
protected:
	//数组表
	template<typename T> using ArrayList = std::vector<T>;
	template<typename T> using stack = std::stack<T>;
	template<typename T> using queue = std::queue<T>;
	template<typename T> using priority_queue = std::priority_queue<T>;
	template<typename T> using greater = std::greater<T>;
	using string = std::string;
	//template<typename T> using vector = StandardExtend::ArrayList<T>;
};

/*二叉(度)树*/
template<class T>
class BinTree :public Tree{
public:
	typedef T Element;
	/*二叉树结点*/
	typedef class BTNode{
	public:
		BTNode(){}
		//获取子类height字段
		virtual int getHeight(){
			return depthOf(this);
		}
		//设置子类height字段
		virtual void setHeight(int){}
		//获取子类的添加字段:weight
		virtual int getValue(){
			return 0;
		}
		//设置子类添加字段
		virtual void setValue(int){}
		//这样无需默认构造函数
		BTNode(Element const&data_) : Data(data_){}
		/*BTNode(Element const&data_){
			Data = data_;
		}*/
		//拷贝构造 左右孩子结点置NULL
		BTNode(BTNode &rhs){
			(*this) = rhs;
			Left = Right = NULL;
		}
		virtual ~BTNode(){
			DE_PRINTF("BTNode析构");
		}
		//赋值 默认的全域赋值
		BTNode& operator=(const BTNode&) = default;
		int height(){
			return depthOf(this);
		}
		//叶子结点判断
		bool isLeave(){
			return Left == NULL && Right == NULL;
		}
		/*
		bool operator=(HuTrNode const &rhs){
		return Weight == rhs.Weight;
		}
		bool operator=(int const &rhs){
		return Weight == rhs;
		}
		bool operator<(HuTrNode const &rhs){
		return Weight < rhs.Weight;
		}
		bool operator>(HuTrNode const &rhs){
		return Weight > rhs.Weight;
		}
		bool operator<=(HuTrNode const &rhs){
		return Weight <= rhs.Weight;
		}
		bool operator>=(HuTrNode const &rhs){
		return Weight >= rhs.Weight;
		}
		bool operator==(HuTrNode const &rhs){
		return Weight == rhs.Weight;
		}
		bool operator!=(HuTrNode const &rhs){
		return Weight != rhs.Weight;
		}
		*/

		BTNode* Left = NULL;		/* 指向左子树 */
		BTNode* Right = NULL;		/* 指向右子树 */
		bool v = false;				/* 表示是否visit过此结点 */
		Element Data;				/* 结点数据:若为结构体,结构体提供比较方法key(增删查都会使用的关键字)在结构体内 否则key就是Data weight-height不能作为key(不唯一)*/

	} *BT, *Position;
	BinTree(){}
	//拷贝构造 参数只能是引用(参数的传递本身就是拷贝构造)
	BinTree(const BinTree &rhs){
		DE_PRINTF("BT拷贝构造");
		destroy(root);
		assignment(root, rhs.root);
	}
	/*序列构造方法*/
	//缺省的遍历序列放置元素个数
	BinTree(Element const *preOrder, Element const *inOrder, int n){
		prefInBuild(preOrder, inOrder, root, n);
	}
	BinTree(int n, Element const *inOrder, Element const *postOder){
		postInBuild(root, inOrder, postOder, n);
	}
	/*6
	Push 1
	Push 2
	Push 3
	Pop
	Pop
	Push 4
	Pop
	Pop
	Push 5
	Push 6
	Pop
	Pop
	output:3 4 2 6 5 1
	BinTree<T> t = BinTree<T>(
	[](char *ORDER){ return (bool)~scanf("%s", ORDER); },
	[](T* num){scanf("%d", num); });*/
	//堆栈操作式构造 (堆栈操作获取方法, 结点数据获取方法)
	BinTree(bool(*getOrder)(char *), void(*getData)(T*)){
		ArrayList<T> preOrder, inOrder;
		stack<T> s;
		char str[5];
		while (getOrder(str)) {
			int len = strlen(str);
			if (len == 4) {//Push
				int num;
				getData(&num);
				preOrder.push_back(num);//第一次遇到时(先序)
				s.push(num);
			}
			else if (len == 3){//Pop
				inOrder.push_back(s.top());//第二次遇到时(中序)
				s.pop();
			}//else 错误
		}
		//if(preOrder.size() != inOrder.size());//错误
		prefInBuild(preOrder, 0, inOrder, 0, root, preOrder.size());
	}
	//destructor
	virtual ~BinTree(){
		destroy(root);
		DE_PRINTF("BT析构");
	}

	virtual void clear(){
		destroy(root);
		lastInsertPosition = nullptr;
		size_ = 0;
	}

	Element getRootData(){
		return root->Data;
	}
	const Position getRoot(){
		return root;
	}
	//判断二叉树是否为空
	bool empty(BT bT){
		return bT == NULL;
	}
	//返回树高
	int height(){
		return depthOf(root);
	}

	template<class Fun>
	//Θ(2*N)     Tree::ORDER, bool visit(BinTree<T>::BT node) 只有顺序和逆序的访问带有返回值(允许中止)
	void traversal(TraversalOrderEnum type, Fun visit){
		if (type == ORDER_PREFIX_ROOT)
			preTraversal(root, visit);
		else if (type == ORDER_INFIX_ROOT)
			infTraversal(root, visit);
		else if (type == ORDER_POST_ROOT)
			postTraversal(root, visit);
		else if (type == ORDER_LEVEL)
			levelTraversal(root, visit);
		else if (type == ORDER)
			orderTraversal(root, visit);
		else if (type == RE_ORDER)
			reOrderTraversal(root, visit);
		else;//NONE_ORDER 不作traversal_
	}
	/*
	先序[1] 2  3  4  5  6
	中序 3  2  4 [1] 6  5
	后序 3  4  2  6  5 [1]
	1
	2     5
	3 4   * 6
	*/
	void orderTranslation(Element *buf, Element *preOrder, Element *inOrder, Element *postOder, int n){
		if (inOrder == nullptr)
			puts("must know inorder");
		else if (preOrder == nullptr && postOder != nullptr)
			calcPrefOrder(buf, inOrder, postOder, n);
		else if (postOder == nullptr)
			calcPostOrder(preOrder, inOrder, buf, n);
		else
			printf("preOrder == null %d\nin == null %d\npost == null %d\n"
			, preOrder == nullptr, inOrder == nullptr, postOder == nullptr);
	}
	//镜像树: mirroring reversal镜像反转 转换后不能使用原来的任何基于比较的方法(若是搜索树:左小右大->左大右小)
	void mirReversal(){
		queue<BT> q;
		q.push(root);
		while (!q.empty()){
			BT t = q.front(); q.pop();
			//将所有非空子树送入转换队列
			if (t->Left) q.push(t->Left);
			if (t->Right) q.push(t->Right);
			//将树根的左右子树转换
			swap(t->Left, t->Right);
		}
	}
	//同构判断
	bool omorphism(BinTree const&T2){
		return isomorphic(this->root, T2.root);
	}
	//将数组内的数据填入现有的树结构中
	//tips:1.若为有序数组则填充完毕后是一课二叉搜索树 2.可以利用静态二叉树储存输入结构 或是原结构
	//完全二叉树可以根据数组直接构造(但只有完全二叉搜索树这样的功能有实际作用)
	//普通二叉树只能在已知结构的前提下使用类似的将数组填入树结构的功能
	bool fillData(ArrayList<T> &dataA){
		return fillData(dataA, 0, dataA.size(), root);
	}
	int size(){
		return size_;
	}
protected:
	const int LEN = sizeof(class BTNode);
	BT root = NULL;
	Position lastInsertPosition = NULL;//(结点生成器最后生成的结点)无法用这个判断插入成功与否
	bool isInsert = false;//是否执行了插入操作(判断插入是否成功)
	int size_ = 0;//有效的元素个数
	queue<Element*> freeMem;//空闲内存
	Element* memoryBlock = NULL;//内存块可以将二叉树的局部储存在这里 超出部分使用外部分配的内存
	//避免二叉树赋值 还有三个字段没有复制
	BinTree& operator= (const BinTree& rhs){
		DE_PRINTF("BT赋值");
		//先要析构自己的root
		destroy(root);
		assignment(root, rhs.root);
		return *this;
	}
	//返回子二叉树的规模
	int scaleOf(BT t){
		return empty(t) ? 0 : scaleOf(t->Left) + scaleOf(t->Right) + 1;
	}
	//返回结点深度：(根据公式 Height = max(Hl, Hr) + 1 由后序遍历改编实现)
	static int depthOf(Position bT){
		int HL, HR;
		if (bT){
			HL = depthOf(bT->Left);
			HR = depthOf(bT->Right);
			return (HL > HR ? HL : HR) + 1;
		}
		else
			return 0;//空树深度为零
	}
	//(数组, 根结点数据下标, 当前根下的数据个数, 结构来源树)
	bool fillData(ArrayList<T> &dataA, int dataRootSub, int n, BT t){
		if (empty(t))
			return true;
		int nl = scaleOf(t->Left), nr = scaleOf(t->Right);//左右子树规模
		//整棵树的规模比较
		if (nl + nr + 1 != n)
			return false;
		//数据填充
		t->Data = dataA[dataRootSub + nl];
		//左子树递归							//1是1个根结点
		fillData(dataA, dataRootSub, n - nr - 1, t->Left);
		//右子树递归      //根1和左子树nl 都"小于"右子树
		fillData(dataA, dataRootSub + nl + 1, n - nl - 1, t->Right);
		return true;
	}
	//结点指针销毁方法(相当于结点的析构)
	virtual void destroy(BT &r){
		if (r){
			destroy(r->Left);
			destroy(r->Right);
			delete r;
			r = NULL;
		}
	}

	//层序遍历---从上到下->从左到右(队列实现)
	template<class Fun>
	void levelTraversal(BT bT, Fun visit){
		if (!bT)return;
		queue<BT> q;
		q.push(bT);
		while (!q.empty()){
			bT = q.front(); q.pop();
			visit(bT);
			if (bT->Left)
				q.push(bT->Left);
			if (bT->Right)
				q.push(bT->Right);
		}
	}
	//先序---子树根->左子树->右子树;(递归实现)
	template<class Fun>
	void preTraversal(BT const bT, Fun visit){
		if (bT){
			visit(bT);
			preTraversal(bT->Left, visit);
			preTraversal(bT->Right, visit);
		}
	}
	//中序---左子树->子树根->右子树;
	template<class Fun>
	void infTraversal(BT const bT, Fun visit){
		if (bT){
			infTraversal(bT->Left, visit);
			visit(bT);
			infTraversal(bT->Right, visit);
		}
	}
	template<class Fun>//顺序遍历(可中止的中序遍历)
	bool orderTraversal(BT const bT, Fun visit){
		if (bT){
			if (orderTraversal(bT->Left, visit))
				return true;
			if (visit(bT))//返回值表示是否中止
				return true;
			return orderTraversal(bT->Right, visit);
		}
		return false;
	}
	template<class Fun>//逆序遍历(tips:含有重复的元素时用顺序计算逆序排名是有问题的)
	bool reOrderTraversal(BT const bT, Fun visit){
		if (bT){
			if (reOrderTraversal(bT->Right, visit))
				return true;
			if (visit(bT))//返回值表示是否中止
				return true;
			return reOrderTraversal(bT->Left, visit);
		}
		return false;
	}
	//后序---左子树->右子树->子树根;
	template<class Fun>
	void postTraversal(BT const bT, Fun visit){
		if (bT){
			postTraversal(bT->Left, visit);
			postTraversal(bT->Right, visit);
			visit(bT);
		}
	}

	//先中构建
	void prefInBuild(Element const *preOrder, Element const *inOrder, BT &t, int n){
		int Ln;/*左子子树长度*/
		if (n == 0)return;
		if (t == NULL)
			t = nodeCreater(*preOrder);
		else/*转化关系:先序遍历数组的首元素就是子树根*/
			t->Data = *preOrder;
		for (Ln = 0; Ln < n && inOrder[Ln] != *preOrder; Ln++);
		prefInBuild(preOrder + 1, inOrder, t->Left, Ln);/*先序遍历数组向左子树遍历一个元素 其余数组保持不变*/
		prefInBuild(preOrder + Ln + 1, inOrder + Ln + 1, t->Right, n - Ln - 1);/*向右子树遍历一个元素*/
	}
	//先中构建ArrayList
	void prefInBuild(ArrayList<Element> &preOrder, int preRoot, ArrayList<Element> &inOrder, int inRoot, BT &t, int n){
		int Ln;/*左子子树长度*/
		if (n == 0)return;
		if (t == NULL)
			t = nodeCreater(preOrder[preRoot]);
		else/*转化关系:先序遍历数组的首元素就是子树根*/
			t->Data = preOrder[preRoot];
		for (Ln = 0; Ln < n && inOrder[inRoot + Ln] != preOrder[preRoot]; Ln++);
		prefInBuild(preOrder, preRoot + 1, inOrder, inRoot, t->Left, Ln);/*先序遍历数组向左子树遍历一个元素 其余数组保持不变*/
		prefInBuild(preOrder, preRoot + Ln + 1, inOrder, inRoot + Ln + 1, t->Right, n - Ln - 1);/*向右子树遍历一个元素*/
	}
	//后中构建
	void postInBuild(BT &t, Element const *inOrder, Element const *postOder, int n){
		int Ln;/*左子树长度*/
		if (n == 0)return;
		if (t == NULL)
			t = nodeCreater(postOder[n - 1]);
		else/*转化关系:后序遍历数组的尾元素就是子树根*/
			t->Data = postOder[n - 1];
		for (Ln = 0; Ln < n && inOrder[Ln] != postOder[n - 1]; Ln++);//获取左子树长度
		postInBuild(t->Left, inOrder, postOder, Ln);/*先序遍历数组进入左子树*/
		postInBuild(t->Right, inOrder + Ln + 1, postOder + Ln, n - Ln - 1);/*进入右子树*/
	}
	//后中构建 优化版
	BT postInBuild(int *inOrder, int *postOder, int n){
		int Ln;/*左子树长度*/
		if (n == 0)	return NULL;

		Position t = (Position)malloc(sizeof(struct TreeNode));
		memset(t, 0, sizeof(struct TreeNode));
		t->Element = postOder[n - 1];/*转化关系:后序遍历数组的尾元素就是子树根*/

		for (Ln = 0; Ln < n && inOrder[Ln] != postOder[n - 1]; Ln++);//获取左子树长度
		t->Left = postInBuild(inOrder, postOder, Ln);/*先序遍历数组进入左子树*/
		t->Right = postInBuild(inOrder + Ln + 1, postOder + Ln, n - Ln - 1);/*进入右子树*/
		return t;
	}

	void calcPostOrder(string const &sPre, string const &sMed, string &sPostBuffer){
		//中序中的根所在位置
		string::size_type medRootSub = sMed.find(sPre[0]);
		//左子树的长度
		string::size_type leftSubLen = medRootSub;
		/*
		A B CD	前序(Root Left Right)
		B A DC	中序(Left Root Right)

		BDC A	后序(Left Right Root)
		*/

		//若存在左子树则向左子树递归
		if (leftSubLen > 0){
			//先序第一个值(root)右边取左子树的长度即是左子树
			string sPreLeftSub = sPre.substr(1, leftSubLen);
			//root的左边
			string sMedLeftSub = sMed.substr(0, leftSubLen);
			calcPostOrder(sPreLeftSub, sMedLeftSub, sPostBuffer);
		}
		else{
			//do nothing
		}

		//若存在右子树则向右子树递归
		if (medRootSub + 1 < sMed.size()){
			//先序第一个值(root)的右边跳过左子树长度取所有即是右子树
			string sPreRightSub = sPre.substr(1 + leftSubLen);
			//root的右边
			string sMedRightSub = sMed.substr(medRootSub + 1);
			calcPostOrder(sPreRightSub, sMedRightSub, sPostBuffer);
		}
		else{
			//do nothing
		}

		if (medRootSub != string::npos){
			//加上root
			sPostBuffer += sMed.at(medRootSub);
		}
		else{
			//do nothing
		}
	}
	/*根据先序和中序遍历输出后序遍历序列 (先 中 后 子树元素个数)*/
	template<class Iterator>
	//支持加减运算的迭代器(裸指针也行 postOder对应的容器必须具有至少n个元素)
	static void calcPostOrder(Iterator preOrder, Iterator inOrder, Iterator postOder, int n){
		int Ln;/*左子子树长度*/
		if (n == 0)return;
		postOder[n - 1] = *preOrder;/*转化关系:先序遍历数组的首元素就是子树根*/
		for (Ln = 0; Ln < n && inOrder[Ln] != *preOrder; ++Ln);
		calcPostOrder(preOrder + 1, inOrder, postOder, Ln);/*先序遍历数组向左子树遍历一个元素 其余数组保持不变*/
		calcPostOrder(preOrder + Ln + 1, inOrder + Ln + 1, postOder + Ln, n - Ln - 1);/*向右子树遍历一个元素*/
	}
	/*根据先序和中序遍历输出后序遍历序列 (先 中 后 子树元素个数)*/
	void calcPostOrder(Element *preOrder, Element *inOrder, Element *postOder, int n){
		int Ln;/*左子子树长度*/
		if (n == 0)return;
		postOder[n - 1] = *preOrder;/*转化关系:先序遍历数组的首元素就是子树根*/
		for (Ln = 0; Ln < n && inOrder[Ln] != *preOrder; Ln++);
		calcPostOrder(preOrder + 1, inOrder, postOder, Ln);/*先序遍历数组向左子树遍历一个元素 其余数组保持不变*/
		calcPostOrder(preOrder + Ln + 1, inOrder + Ln + 1, postOder + Ln, n - Ln - 1);/*向右子树遍历一个元素*/
	}
	/*根据后序和中序遍历输出先序遍历序列 只修改中间两句话即可*/
	template<class Iterator>
	static void calcPrefOrder(Iterator preOrder, Iterator inOrder, Iterator postOder, int n){
		int Ln;/*左子树长度*/
		if (n == 0)return;
		*preOrder = postOder[n - 1];/*转化关系:后序遍历数组的尾元素就是子树根*/
		for (Ln = 0; Ln < n && inOrder[Ln] != postOder[n - 1]; ++Ln);//获取左子树长度
		calcPrefOrder(preOrder + 1, inOrder, postOder, Ln);/*先序遍历数组进入左子树*/
		calcPrefOrder(preOrder + Ln + 1, inOrder + Ln + 1, postOder + Ln, n - Ln - 1);/*进入右子树*/
	}
	/*根据后序和中序遍历输出先序遍历序列 只修改中间两句话即可*/
	void calcPrefOrder(Element *preOrder, Element *inOrder, Element *postOder, int n){
		int Ln;/*左子树长度*/
		if (n == 0)return;
		*preOrder = postOder[n - 1];/*转化关系:后序遍历数组的尾元素就是子树根*/
		for (Ln = 0; Ln < n && inOrder[Ln] != postOder[n - 1]; Ln++);//获取左子树长度
		calcPrefOrder(preOrder + 1, inOrder, postOder, Ln);/*先序遍历数组进入左子树*/
		calcPrefOrder(preOrder + Ln + 1, inOrder + Ln + 1, postOder + Ln, n - Ln - 1);/*进入右子树*/
	}
	/*根据后序和中序遍历输出层序遍历序列 后中->先->先序建树->层序遍历*/
	void calcLeveOrder(Element *preOrder, Element *inOrder, Element *postOder, int n){
		puts("has not imp");
		exit(-1);
	}

	//返回是否T1与T2是否同构 isomorphic(adj. [物] 同构的；同形的)  isomorphism_(n. 类质同像，[物化] 类质同晶；同形)
	bool isomorphic(BT T1, BT T2){
		if ((T1 == NULL && T2 != NULL) || (T1 != NULL && T2 == NULL))//深度不同
			return false;
		else if (T1 == NULL && T2 == NULL)
			return true;
		else/*两颗树都不为空*/
		{
			if (T1->Data != T2->Data)//数据不同
				return false;
			else/*当前数据相等*/
			{	/*按当前顺序尝试匹配左右子树*/
				if (isomorphic(T1->Left, T2->Left))
					return isomorphic(T1->Right, T2->Right);
				else/*否则转换左右子树匹配*/
				{
					if (isomorphic(T1->Left, T2->Right))
						return isomorphic(T1->Right, T2->Left);
					else
						return false;
				}
			}
		}
	}

	//递归拷贝赋值
	void assignment(BT &lhs, const BT rhs){
		if (!empty(rhs)){
			lhs = new BTNode(*rhs);
			assignment(lhs->Left, rhs->Left);
			assignment(lhs->Right, rhs->Right);
		}
	}
	//镜像赋值(负号)
	void reAssignment(){}
	//重复插入处理
	void repetitionInsert(Position t){
		//x==t->Data 无需插入 手动更新lastInsertPosition
		lastInsertPosition = t;
		isInsert = false;
	}
public:
	//结点生成器 返回一个未使用的结点 若不存在未使用结点 返回NULL 只能插入使用
	virtual BT nodeCreater(Element const &data_){
		lastInsertPosition = new BTNode(data_);
		isInsert = lastInsertPosition == NULL ? false : true;
		++size_;
		return lastInsertPosition;
		/*
		bST = (BST)malloc(sizeof(struct TNode));
		memset(bST, 0, sizeof(struct TNode));
		bST->Element = x;
		*/
	}
	//结点擦除器 将结点置为未使用状态
	virtual void nodeEraser(Position &del){
		free(del);
		del = NULL;
		--size_;
	}
};

/*二叉搜索（查找 排序 判定）树*/
/*折半查找的判定树:按照比较的次数生成判定树，比较1次的是根结点，比较2次的在第二层，比较3次的在第三层...
其实就是递归的将mid作为父结点
不过注意(左小右大的二叉搜索树):1 2 (mid=1)   4 5 (mid=4)即小的结点作为父结点的可能性更大
更有可能呈现135度的倾斜; 右子树结点更可能多一些*/
/*
性质：
①非空 左子树的所有键值 小于其根节点的键值
②非空 右子树的所有键值 大于其根结点的键值
③左右子树都是二叉搜索树
④没有键值相等的节点
最大元素：最右分支（一直bST = bST->Right）即可
最小元素：最左分支

n个结点的判定树depthOf = [log2(n)]+1
判定树上每个结点的查找次数==所在层数
查找成功次数<=depthOf
4层满二叉树的平均查找次数ASL = (4*8+3*4+2*2+1*1)/15
*/
template<class T>
class BinSearchTree : public virtual BinTree<T> {
public:
	typedef typename BinTree<T>::BT BST;
	//using BinTree<T>::Element;错误用法
	//using Position = BinTree<T>::Position;错误用法
	typedef typename BinTree<T>::Position Position;
	typedef typename BinTree<T>::Element Element;
	using BinTree<T>::root;
	using BinTree<T>::lastInsertPosition;
	using BinTree<T>::traversal;
	using BinTree<T>::destroy;
	using BinTree<T>::nodeCreater;
	using BinTree<T>::nodeEraser;
	//constructor
	BinSearchTree(){}
	//拷贝构造 直接调的父类方法
	BinSearchTree(const BinSearchTree &rhs) :BinTree<T>(rhs)/*这样并未发生强制转换*/{
		DE_PRINTF("BST拷贝构造");
	}
	/*只要不改变左右子树各自的先序插入序列其插入结果与先构建左子树或先构建右子树无关
	先序与普通插入构建序列的区别:
	[先序序列]:[82] 76 23 80 | 90 95
	[插入序列] 82 90 76 23 95 80 构建的树与其是同一棵树*/
	//先序构造 (先序遍历序列左端,序列右端+元素个数)
	BinSearchTree(Element *preLeft, Element *preRight, int eqaul = 0){
		while (preLeft < preRight){
			if (!insert(*preLeft)){
				puts("构造失败!");
				system("pause");
			}
			++preLeft;
		}
	}
	//先序构造 ArrayList
	BinSearchTree(JCE::ArrayList<Element> &preOrder, int eqaul = 0){
		/*若preOrder参数不是引用
		**会发生:
		**ArrayList拷贝构造
		**ArrayList析构
		**BT拷贝构造
		**BST拷贝构造
		**BST析构
		**BT析构
		**ArrayList析构(不知道究竟是构造两次析构两次 还是构造一次析构一次)
		(不理解为什么在构造函数里 用传值拷贝参数会调用这一系列 我觉得完全没必要啊
		尤其是一开始没有写相应函数出错了...)
		注意!!!
		即便是BinSearchTree t = BinSearchTree(ArrayList<>);
		这样的语句也不会调用赋值函数 而是调用的拷贝构造函数
		preOrder是引用的话上面所有的屁事都没了
		*/
		for (int i = 0; i < preOrder.size(); ++i) {
			if (!insert(preOrder[i])){
				puts("构造失败!");
				system("pause");
			}
		}
	}
	//析构deleter
	virtual ~BinSearchTree() override{
		DE_PRINTF("BST析构");
	}
	//赋值 很费时间 避免无意间的二叉树赋值
	BinSearchTree& operator= (const BinSearchTree& rhs){
		DE_PRINTF("BST赋值");
		//<==>两次父类拷贝构造 一次父类赋值 两次父类析构
		//(BinTree)(*this) = (BinTree)rhs;
		// ==>强制转换其实是调用的拷贝构造方法(这样效率不高) 所以应当为子类编写自己的赋值函数(即使没有新增加的域)
		BinTree<T>::destroy(root);//先要销毁自己的root
		BinTree<T>::assignment(root, rhs.root);
		return *this;
	}

	//返回倒数第k大的元素位置 不存在返回NULL;(利用BST中序遍历是顺序的特点:反过来是逆序)
	BST findRKth(BST t, int *k){
		if (t){
			BST r = findRKth(t->Right, k);//1.<阶段一>一直到最右边
			if (r) return r;//中转返回: 若不是2返回而来(3或4)则返回

			if (--(*k) == 0)//3.<阶段二> 按顺序由最后开始退k个结点
				return t;//终点返回: 已是倒数第k个结点

			return findRKth(t->Left, k);//4.<阶段三>前面都没有返回 说明满足要求的结点在左子树(中序):相当于一个新的调用
			//调用返回: 返回给1; 返回给4; 返回给初始调用0; 可能由1 2(NULL 其实也是4 NULL的情况只可能是最左边到头仍没有得到3的返回) 3返回而来
		}
		return NULL;//2.终点返回: 到头 返回NULL
	}
	//返回正数第k个元素(序号为k的元素 从1开始)
	BST findKth(int k){
		int id = 1;
		Position t;
		traversal(Tree::ORDER, [&](Position bt){
			t = bt;
			return id++ == k;//找到就中止
		});
		return t;
	}
	//返回特定元素的等级(比参数大的元素的个数) O(2*N)
	int getRankOf(T &x){
		int rank = 1;
		traversal(Tree::RE_ORDER, [&](Position bt){
			//15 30 40 45 50 50   rank(45) = 3 不是序号3 更不是2
			if (bt->Data > x)//去掉这句就是求序号
				++rank;
			return bt->Data == x;//找到就中止
		});
		return rank;
	}
	//计算所有元素的等级 (计算完毕后将total sort一下再用类似的方法遍历一下统计出总排名即可) Θ(2*N)范程
	//虽然N个insert(O(logN))与N个读取后sort(O(N*logN))的效率差不多 但论遍历效率tree Θ(2*N), ArrayList Θ(N)
	void calcRank(JCE::ArrayList<T> &total, int cnt = -1){
		cnt = cnt < 0 ? BinTree<T>::size() : cnt;//默认计算所有人的排名
		int pastId = 0;//序号
		int reCnt = 0;//当前元素现在的重复个数
		Position pastP = NULL;
		traversal(Tree::RE_ORDER, [&](Position bt){
			if (pastP != NULL && pastP->Data.score == bt->Data.score)
				++reCnt;
			else
				reCnt = 0;
			bt->Data.rank = ++pastId - reCnt;//等级 = 序号 - 重复个数
			total.push_back(bt->Data);//统计总排名
			pastP = bt;
			return cnt-- == 0;//计算前cnt名 若cnt为零则中止
		});
	}
	/*返回二叉搜索树bST中最小元结点的指针；*/
	Position findMin(){
		BST bST = root;
		while (bST && bST->Left)
			bST = bST->Left;
		return bST;
		/*
		//1.
		if (!bST)//基本定义：树为空返回NULL
		return NULL;
		else if (!bST->Left)
		return bST;//基本定义：左子叶结点是最小元素
		else
		return FindMin(bST->Left);//递归定义：不是左子叶时进入左子树继续查找
		//2.更简易版: 还有左子树吗 有就递归 没有就返回
		return t->Left ? findMin(t->Left) : t->Data;
		*/
	}
	/*返回二叉搜索树bST中最大元结点的指针。*/
	Position findMax(){
		BST bST = root;
		while (bST && bST->Right)
			bST = bST->Right;
		return bST;
	}

	//返回是否是二叉搜索树
	bool IsBST(BST t){
		if (t){
			if (!IsBST(t->Left))
				return false;
			if (!IsBST(t->Right))
				return false;
			if (t->Left){
				bool is = t->Left->Data < t->Data;
				if (t->Right){
					is = is && t->Data < t->Right->Data;
					if (!is) return false;
					is = is && findMin(t->Right) > t->Data;//右子树最小元素大于根结点
					if (!is) return false;
				}
				is = is && findMax(t->Left) < t->Data;//左子树最大元素小于根结点
				return is;
			}
		}
		return true;
	}
	//返回两个结点的公共祖先(先序) 优化遍历方式: 算法效率O(n)->O(h)不再死板地按照先序先左后右 LCA
	Position ancestor(Position t, int u, int v){
		if (t){
			/*都小向左 都大向右*/
			if (u < t->Data && v < t->Data)
				return ancestor(t->Left, u, v);
			if (u > t->Data && v > t->Data)
				return ancestor(t->Right, u, v);
			/*一大一小 返回 优化存在判断*/
			return ((find(t, u) && find(t, v)) ? t : NULL);
		}
		return NULL;
	}
	void clearVisit(Tree::TraversalOrderEnum type_){
		BinTree<T>::traversal(type_, [&](Position const t){
			if (t->v){
				t->v = false;
				return true;
			}
			else//当且仅当 遍历方式与visit清理方式相同时 当前结点没有遍历过子结点必定没有遍历过
				return false;//中止
		});
	}


	/*返回x在二叉搜索树bST中的位置; 若找不到则返回NULL*/
	Position find(T const&x){
		return findOf(root, x);
	}
	//自定义查找(自定义小于符号)
	template<typename LessCmp>
	Position find(T const&x, LessCmp less){
		return findOf(root, x, less);
	}
	//插入 O(logN)
	JCE::pair<Position, bool> insert(Element const&x){
		root = Insert(root, x);
		//与map的insert返回值类似，重复insert 返回<重复Position, false>，这个技巧在面试如何找出2个数组相同的数字的时候有奇效
		return{ BinTree<T>::lastInsertPosition, BinTree<T>::isInsert };
	}
	//序列插入 局部有序插入O(1) 无序O(logN) 但如果Avl使用此插入会使Avl退化为普通BST(相当于优化的链表: 插入效率变高 查找效率相对Avl退化)
	//(例如654789)->4 5 [6] Orderly被置false 7 [手动]置true 8 9 可实现局部有序插入
	JCE::pair<Position, bool> insertOrderly(Element const&x, bool &Orderly){
		if (!Orderly){//若已经不是有序的插入 则退化为普通插入
			root = Insert(root, x);
		}
		else{
			if (!BinTree<T>::empty(root)){
				if (x < root->Data){//若即将插入的数据在左子树
					if (x < lastInsertPosition->Data){//x已位于左子树 若x还比lastInsertPosition小后者必定也在左子树
						Insert(lastInsertPosition, x);//于是直接往lastInsertPosition的左边插即可
						Orderly = true;
					}
					else{//否则应该插入的位置位于左子树:root和lastInsertPosition中间
						Orderly = false;
						root = Insert(root, x);
					}
				}
				else{//相等时直接传入使Insert判断
					if (lastInsertPosition->Data < x){///@x > lastInsertPosition->Data
						Insert(lastInsertPosition, x);
						Orderly = true;
					}
					else{
						Orderly = false;
						root = Insert(root, x);
					}
				}
			}
			else{
				Orderly = true;
				root = Insert(root, x);
			}
		}
		return{ lastInsertPosition, BinTree<T>::isInsert };
	}
	//删除 删除成功返回true
	bool erase(Element const&x){
		bool succeed = true;
		root = Delete(root, x, succeed);
		return succeed;
	}
protected:
	Position findOf(Position bST, T const &x){
		while (bST){
			//@调试结果 增删改查 的这个顺序应该保持一致 或者只用一种比较符号
			if (x < bST->Data)
				bST = bST->Left;
			else if (bST->Data < x)//@x > t->Data
				bST = bST->Right;/*向右子树移动继续查找*/
			else
				return  bST;
 		}
		return NULL;
	}
	template<typename LessCmp>
	Position findOf(Position bST, T const&x, LessCmp less){
		while (bST){
			if (less(bST->Data, x))
				bST = bST->Right;/*向右子树移动继续查找*/
			else if (less(x, bST->Data))
				bST = bST->Left;
			else
				return  bST;
		}
		return NULL;
	}
	/*函数Insert将x插入二叉搜索树t并返回结果树的根结点指针*/
	virtual BST Insert(BST t, Element const&x){
		if (!t)//树空 返回一个未使用结点
			t = nodeCreater(x);
		else if (x < t->Data)
			t->Left = Insert(t->Left, x);
		else if (t->Data < x)//@x > t->Data
			t->Right = Insert(t->Right, x);
		else {
			BinTree<T>::repetitionInsert(t);
		}
		return t;
	}
	/*从二叉搜索树bST中删除x,返回新根; 若x不在树中tag->!tag 返回原树根*/
	virtual BST Delete(BST bST, Element const&x, bool &tag){
		Position del = bST;
		/*阶段一:查找对应元素*/
		if (!bST)//未找到匹配元素 删除失败
			tag = false;//@调试结果 原来是 tag=false; tag = !tag;
		else if (x < bST->Data)
			bST->Left = Delete(bST->Left, x, tag);//子树根节点可能会发生变化
		else if (bST->Data < x)//@x > t->Data
			bST->Right = Delete(bST->Right, x, tag);
		/*阶段二:删除匹配元素*/
		else if (bST->Left && bST->Right){
			del = findMinOf(bST->Right);//用右子树的最小(或左子树最大)元素填充需删除的结点
			bST->Data = del->Data;
			/*阶段三:维持结构*/
			bST->Right = Delete(bST->Right, del->Data, tag);//删除右子树的最小元素
		}
		else {//只有一个结点或无结点(返回Left的NULL)
			if (!bST->Left)
				bST = bST->Right;
			else
				bST = bST->Left;
			nodeEraser(del);
		}
		return bST;
	}
	//Delete时要用
	Position findMinOf(BST bST){
		while (bST && bST->Left)
			bST = bST->Left;
		return bST;
	}
	Position findMaxOf(BST bST){
		while (bST && bST->Right)
			bST = bST->Right;
		return bST;
	}
};

/*
红黑树不是严格平衡树，不需要在每次插入删除的时候调整，avl是严格平衡树，每次插入删除的时候都要旋转到平衡状态。
从统计角度来说红黑树的性能要高一点，但查找操作并不能严格保证O(logN)，所以如果一棵树很少修改的话用AVL或许能快一点，
但话说回来，如果几乎不修改的话用有序数组+二分查找性能比各种树都要好。
*/
/*AVL平衡树 -------------------不能瞎用二叉树的操作(比如删除)*/
template<class T>//多重继承
class AvlTree :public BinSearchTree<T> {
	typedef typename BinSearchTree<T>::BST AVLT;
	typedef typename BinTree<T>::BT BT;
	using Element = T;
	using Tree::Max;
	using BinSearchTree<T>::root;
	typedef class AvlNode : public BinTree<T>::BTNode{
	public:
		int Height = 0;//树高  Data意义同BinTree
		int getValue()override{
			return Height;
		}
		void setValue(int h)override{
			Height = h;
		}
		int getHeight()override{
			return Height;
		}
		void setHeight(int h)override{
			Height = h;
		}
		AvlNode(int data_, int height_)
			:BinTree<T>::BTNode(data_){
				Height = height_;
			}
		virtual ~AvlNode()override{
			DE_PRINTF("AvlNode析构");
		}
	}*AvlTr;

	//计算avl树t的高度(深)
	int heightOf(AVLT t){//其实与BinTree的depth一样 只不过为了提高效率avl增加了一个height域并且对算法略作优化
		if (!t)
			return 0;
		else if (t->Left && t->Right)
			return Max(t->Left->getHeight(), t->Right->getHeight()) + 1;
		else if (t->Left)
			return t->Left->getHeight() + 1;
		else if (t->Right)
			return t->Right->getHeight() + 1;
		else
			return 1;
		/*展开下一层后效率提高特别大 优于下面的简易写法*/
		//return t ? Max(heightOf(t->Left), heightOf(t->Right)) + 1 : 0;
	}
	/*两种概念:Rotation(旋转) 和 Elevate提升
	(本实现与书上经典实现相同 但并没有旋转概念)
	有两种理解方式或设计原则: 见名知意于方法效果; 见名知意于方法调用
	四种调整策略中应用不同的概念解释和记忆或多或少自相矛盾
	斟酌了很久最后用提升概念(旋转概念太相对了 而且老让我觉得他所谓的左旋其实是右旋 而且就算是旋转也解释不通)
	给了原有算法一个新的解释 达到了无歧义的理解效果*/

	/*左侧提升(LL) :将的左子节点B做提升至A处,A变为B提升方向上的子结点 更新树高 返回新根B*/
	AVLT leftElevate(AVLT A){
		AVLT B = A->Left;
		A->Left = B->Right;
		B->Right = A;
		A->setHeight(Max(heightOf(A->Left), heightOf(A->Right)) + 1);
		B->setHeight(Max(heightOf(B->Left), A->getHeight()) + 1);//a的height上面已经计算 直接获取即可
		return B;
	}
	/*右侧提升(RR) :将的左子节点B做提升至A处,A变为B提升方向上的子结点 更新树高 返回新根B*/
	AVLT rightElevate(AVLT A){
		AVLT B = A->Right;
		A->Right = B->Left;
		B->Left = A;
		A->setHeight(Max(heightOf(A->Left), heightOf(A->Right)) + 1);
		B->setHeight(Max(heightOf(B->Right), A->getHeight()) + 1);
		return B;
	}
	/*问题出现在左子树的右侧:先对A的左子树做右侧提升 再对A做左侧提升*/
	AVLT LR(AVLT A){
		A->Left = rightElevate(A->Left);
		return leftElevate(A);
	}
	/*问题出现在右子树的左侧:先对A的右子树做左侧提升 再对A做右侧提升*/
	AVLT RL(AVLT A){
		A->Right = leftElevate(A->Right);/*更新A的右子树*/
		return rightElevate(A);
	}
	/*将x插入平衡树 并返回调整后的平衡树*/
	BT Insert(BT t, Element const&x)override{
		if (!t){/*若插入空树 则新建一个包含结点的树*/
			t = BinTree<T>::nodeCreater(x);
		}
		else if (x < t->Data){/*往左子树插入*/
			t->Left = Insert(t->Left, x);/*t的平衡因子BF*/
			if (heightOf(t->Left) - heightOf(t->Right) == 2){/*若插入后其高度height不平衡*/
				if (x < t->Left->Data)/*LL型 直接做左侧提升即可*/
					t = leftElevate(t);
				else/*LR型调用LR方法*/
					t = LR(t);
			}
		}
		else if (x > t->Data){
			t->Right = Insert(t->Right, x);
			if (heightOf(t->Left) - heightOf(t->Right) == -2){
				if (x > t->Right->Data)
					t = rightElevate(t);
				else
					t = RL(t);
			}
		}
		else{
			BinTree<T>::repetitionInsert(t);
		}
		t->setHeight(Max(heightOf(t->Left), heightOf(t->Right)) + 1);/*更新树高*/
		return t;
	}
public:
	AvlTree(){}
	~AvlTree(){
		DE_PRINTF("AVL析构");
	}
};

/*
静态储存:将数组视为内存块, 在此内存块内"申请"结点实现二叉树的话 这与普通的链接实现无异 而且更为费事 不予考虑
顺序储存:将数组按顺序编号,1号作为根结点,之后 编号为2n是左子结点 2n+1是右子结点 是理想的完全二叉树存储方式
.首先,所有的树都可以用链式储存,其节约内存,但缺乏随机访问性
.以下几种情况可以考虑非链式储存的树结构
已知树(最好是完全二叉树)的结构(用序号代表子结点)	:静态储存(唯一用武之地)
已知是完全二叉树								:顺序储存
.已知数据查询(PS:高度Height = max(Hl, Hr) + 1)
普通数组储存 读入数据Θ(N)
不使用数据结构:			排序O(N*logN)				binary_search O(logN))			增删 O(N) = 顺序比对并移动
顺序储存的完全二叉搜索树	排序				构造Θ(N)		find		  O(logN)			增删 O(H) = 同BST但 H(CBT) = log(N)+1 即增删O(logN)
AVL平衡树	插入构造O(N*logN) 其余与CBST性质类似 但维护结构需要额外时间
BST			插入构造O(N*logN) 增删O(H) log(N-1) <= H(BST) <= N, 平均 H = log(N)+1
前者构造方便,查找效率与后者相当, 但后者只是根到叶局部有序, 维护成本远小于前者
.实现堆(CBT 最大小树)
普通数组:难以维护, 不考虑
链式BST实现的Heap:构造 Ω(N),		push O(H)	pop O(H)
顺序CBT实现的Heap:构造 O(N),		push O(logN)		pop O(logN)
使用顺序储存实现堆是最佳选择
*/
/*静态二叉树Static Binary Tree(用数组储存的二叉树不一定是完全二叉树)*/
template<class T>
class StaBinTree :public virtual BinTree<T>{
protected:
	typedef typename BinTree<T>::BT BT;//levl
	typedef typename BinTree<T>::BT structArray;
	typedef typename BinTree<T>::Position Position;
	using BinTree<T>::root;
	using BinTree<T>::LEN;
	typedef T ElementSBT;
	structArray struA = NULL;//Left和Right储存左右孩子位于数组内的地址
	int capacity = 0;//数组长度, 所有元素个数(size+1) 至少为1
	int size = 0;//已储存,有效 的元素个数 (cap-1) struA尾元素下标

	//通过输入构建树结构 返回根结点 tips: getSub必须将参数置为数组内孩子结点的下标(int)
	template<class Fun1, class Fun2>
	Position buildStructure(Fun1 getData, Fun2 getSub, int nullChar){
		int n = capacity - 1;
		int i, sum = (n - 1)*n / 2;
		int leftSub, rightSub;
		Position rt = struA + 1;

		while ((i = getchar()) == '\n' || i == ' ');
		ungetc(i, stdin);//清空输入流 bug:工程使用时应去掉

		for (i = 0; i < n; i++){
			Position leftChild = NULL, rightChild = NULL;
			getData(&rt[i].Data);
			getSub(&leftSub, &rightSub);
			if (leftSub != nullChar){
				leftChild = rt + leftSub;
				sum -= leftSub;
			}

			if (rightSub != nullChar){
				rightChild = rt + rightSub;
				sum -= rightSub;
			}
			rt[i].Left = leftChild, rt[i].Right = rightChild;
		}
		return n == 0 ? NULL :
			0 < sum&&sum < size ? rt + sum : NULL;
	}
	bool full(){
		return size == capacity - 1;
	}

	bool empty(){
		return size == 0;
	}
	//构建结构体数组 会自动多申请一个用于存储0号哨兵
	void reCapacity(int capacity_){
		struA = (structArray)realloc(struA, LEN*++capacity_);
		if (capacity == 0)//首次申请->全域初始化(排除已使用的size 一般是0)
			memset(struA + size, 0, LEN*(capacity_ - size));
		else//再次申请->root域[struA+cap, struA+cap](排除原有的cap)初始化, struA+cap已经是数组末尾的后一个
			memset(struA + capacity, 0, LEN*(capacity_ - capacity));
		capacity = capacity_;
	}
public:
	using BinTree<T>::height;
	//初始化root 
	void iniARoot(){//root不包括哨兵(0号)
		root = struA + 1;
	}
	//构造静态完全二叉树 root和size需要子类初始化 向上转型时会用root (可存储的元素个数)
	StaBinTree(int n){
		reCapacity(n);
	}
	/*
	8
	1 -
	- -
	0 -
	2 7
	- -
	- -
	5 -
	4 6
	*/
	/*StaBinTree<T> t = StaBinTree<T>(n,
	[](T*data_){static int i = 0; *data_ = i++; },
	[](int* lSub, int* rSub){char l, r; scanf("%c %c", &l, &r); *lSub = l-'0', *rSub = r-'0'; },
	'-'-'0');
	t.traversal(t.ORDER_POST_ROOT);
	t.traversalDisplay();*/
	/*
	9
	1 6
	2 3
	-1 -1
	-1 4
	5 -1
	-1 -1
	7 -1
	-1 8
	-1 -1
	73 45 11 58 82 25 67 38 42
	*/
	/*StaBinTree<T> t = StaBinTree<T>(n,
	[](T*data_){static int i = 0; *data_ = i++; },
	[](int* lSub, int* rSub){scanf("%d %d", lSub, rSub); },
	-1);*/
	//结构已知构造(数据数目, 数据获取方法, 下标获取方法, 空数据值)
	//混淆概念的 这构造出来连顺序储存的二叉树都不是 完完全全只是把这段数组当内存使用然后构建链接二叉树而已
	/*lambda是通过创建一个重载了操作符()的小类来实现的，一个lambda函数是该类的一个实例
	**捕获lambda函数外的具有自动存储时期的变量。函数体与这些变量的集合合起来叫闭包。
	**一个没有指定任何捕获的lambda函数,可以显式转换成一个具有相同声明形式函数指针
	**但当捕获列表出现时，其入口包含了实例化时对捕获对象的封装，而这个捕获对象的实体是以类成员的身份存在的，因此无法被转化成函数指针。*/
	//此版本可以使用lambda捕获列表 若结构并不总是完整的 但root是特定的 可以指定root的下标
	template<class Fun1, class Fun2>
	StaBinTree(int n, Fun1 getData, Fun2 getSub, int nullChar, int rootSub = -1){
		reCapacity(n);
		root = buildStructure(getData, getSub, nullChar);//用StaBinTree实现便于方法内的子树下标的计算
		if (rootSub > 0)
			root = struA + rootSub;
		size = capacity - 1;
	}
	virtual ~StaBinTree() override{
		free(struA);//malloc 不能delete?
		root = NULL;//保证基类的正常析构
		DE_PRINTF("SCT析构");
	}
	//返回数组内的结点编号 1号为root 0号为哨兵
	int index(Position t){
		/*第一个元素插入时root=NULL 所以链接左右孩子链接方法不能用root 更何况root=1时的规律是针对数组成立的*/
		return t - struA;
	}
	//返回数组内的结点位置
	Position position(int i){
		return struA + i;
	}
	//返回第lv层最多的结点个数
	//返回第i层的叶结点个数 lv越界返回-1
	int leavesCount(int lv){
		if (lv > height())
			return -1;
		int count = 0;
		JCE::queue<JCE::pair<BT, int>> q;//t, lv
		q.push(std::make_pair(root, 0));
		while (!q.empty()){
			auto p = q.front();
			q.pop();
			if (!empty(p.first->Left))
				q.push({ p.first->Left, p.second + 1 });
			if (!empty(p.first->Right))
				q.push({ p.first->Right, p.second + 1 });
			if (p.first->isLeave() && p.second == lv)
				++count;
			else if (p.second > lv)
				break;
			//else continue;
		}
		return count;
	}
};

//说白了就是被限制了内存空间的链接二叉树 但由于普通二叉树不常用(没有有存在意义的insert方法)
//出现这种冲突的根本原因在于 我将链接结点实现的 二叉树当成了一个既成标准(接口) 而初衷仅仅是为了可以便于向上转型 所以出现了一些矛盾
//(既想有链接实现的动态性和内存节约 又想有静态实现的随机访问效率
/*
可以传入一个重载了[]的容器给树 当作"结构内存"
如果该容器是顺序容器的话 在树的内部呈现动态性 在外部呈现静态性
这样一来静态实现与链接实现合二为一了 可是劣势也是明显的可以说又回到了当初创造链接结点的初衷:节约内存 以及在未知数据面前顺序实现的无力性
上一问题可以解决:树数据结构内部存在两种内存申请方法 一种是在传入的容器内存里申请 一种是在外部申请(有一个注册表用于注销这一部分内存 可以通过对注册表struA(指针数组)的访问达到即时随机访问)
以上全是意淫,始终有一个问题没有解决:顺序表删除某条内容后那条记录作废了 但你需要新内存时如何知道哪里有空内存 于是又需要查找
好吧可以将删除的内存指针与末尾内存指针交换(二叉树用的是指向数组的二级指针 相当于用到了三级指针(呸)) 但,这与普通的链接式二叉树又完全没区别了 这是理解了系统内存的运作方式?
*/
/*静态二叉搜索树Static Binary Search Tree*/
/*SequentialBinTree顺序储存的二叉树*/
template<class T>//虚继承(virtual必须写在中间) 解决多继承中菱形继承的情况下的基类成员的冲突
class StaBinSearchTree :public virtual StaBinTree<T>, public virtual BinSearchTree<T>{
public:
	StaBinSearchTree(int n) :StaBinTree<T>(n){
		size = n;
	}
	~StaBinSearchTree() override{
		DE_PRINTF("SCBT析构");
	}
protected:
	typedef typename BinTree<T>::Position Position;
	typedef T Element;
	typedef Position BT;
	typedef BT structArray;
	using BinTree<T>::root;
	using StaBinTree<T>::struA;
	using StaBinTree<T>::size;
	using StaBinTree<T>::capacity;
	using StaBinTree<T>::full;
	void nodeEraser(Position &del)override{
		del->Data = 0;//0代表初始状态 只为了标识用 并无特殊用处
		del->Left = del->Right = NULL;//静态数组的删除并非实际删除
		del = NULL;
		--size;///
	}
	BT nodeCreater(Element data_)override{
		if (full())
			return NULL;
		struA[size + 1].Data = data_;//0号元素用于哨兵
		++size;///
		/*
		isInsert = lastInsertPosition == NULL ? false : true;  bug
		BinTree<T>::lastInsertPosition = struA + size + 1;
		*/
		return struA + size + 1;
	}
};

/*完全二叉搜索树 使用数组构造 实现对其的静态二分查找 若需要动态增删功能 需要向上转型为二叉搜索树*/
/*与二分查找是有区别的 二分查找的序列是有序的 建立CBST需要有序序列 但建立完后其本身的内置序列并非有序的(其首元素相当于序列中值)*/
/*这其实该叫顺序储存的二叉树而不是完全二叉树 只是说完全二叉树用顺序存储很完美*/
template<class T>
class CompleteBinSearchTree :public StaBinTree<T>{
public:
	using Position = typename StaBinTree<T>::Position;
	using StaBinTree<T>::full;
	using StaBinTree<T>::index;
	using StaBinTree<T>::position;
protected:
	typedef typename BinTree<T>::BT BT;
	typedef Position structArray;
	typedef T Element;
	using StaBinTree<T>::root;
	using StaBinTree<T>::struA;
	using StaBinTree<T>::size;
	using StaBinTree<T>::capacity;
	using Tree::Min;

	/*返回StaBinSearchTree左子树的规模 (总结点数)*/
	int getLeftScaleL(double n){
		/*2^h - 1 + x = n*/
		int h = (int)log2(n + 1);/*向下取整*/
		int x = (int)(n + 1 - pow(2.0, h));/*最下层单出的结点数*/
		return (int)pow(2.0, h - 1) - 1 + Min(x, (int)pow(2.0, h - 1));
	}
	Position getLeftChild(Position t){
		int i = index(t);
		return 2 * i <= size ? struA + 2 * i : nullptr;
	}
	Position getRightChild(Position t){
		int i = index(t);
		return 2 * i + 1 <= size ? struA + 2 * i + 1 : nullptr;
	}
	Position getParent(Position t){
		int i = index(t) / 2;
		return i > 0 ? struA + (i) : nullptr;//1号是根结点没有父结点(1/2==0)
	}
	Position getSibling(Position t){
		int i = index(t);
		//1号是根结点没有兄弟
		return i > 1 ?
			i % 2 == 0 ? struA + i + 1 : struA + i - 1
			: nullptr;
	}

	//[完全二叉树]的序列构建  数据源数组(fitst, last) 目标数组struA		func(A, A+n, 0);
	Position buildComplete(T *first, T *last, int TRoot = 1){
		int n = last - first;
		if (n == 0)
			return NULL;
		int nl = getLeftScaleL(n);
		struA[TRoot].Data = first[nl];/*first由0开始数nl个即是子树根*//*左子树从1开始编号下标是2i*/
		struA[TRoot].Left = buildComplete(first, first + nl, 2 * TRoot);
		struA[TRoot].Right = buildComplete(first + nl + 1, last, 2 * TRoot + 1);
		return struA + TRoot;
		/* 0 1 2 [[3 4 5 [6] 7 8]] 9 */
	}
	/*完全二叉树层序遍历(顺序遍历有序数组)*/
	template<class Fun>
	void levelTraversal(BT bT, Fun visit){//无法override 但名字应该覆盖了
		if (bT == NULL)return;
		FOR(i, 0, size){//bT必须时root不能是struA(root = struA+1)
			if (!visit(bT + i))
				return;//中止
		}
	}
public:
	/*sort(iniA, iniA + n, less<T>());
	**翻转构造:sort(iniA, iniA + n, greater<T>()); 完全二叉树的左大右小树不支持左小右大树的镜像翻转 普通BinTree支持*/
	//使用数组构造唯一的完全二叉树 (由于完全二叉树不能增删 没有size和capacity之分) tips::若数组有序 则为搜索树
	CompleteBinSearchTree(int n, T *iniA = NULL)
		:StaBinTree<T>(n){
			if (iniA != NULL && n > 0){
				root = buildComplete(iniA, iniA + n);
				size = n;
			}
		}
	//直接把数组copy进静态数组里
	CompleteBinSearchTree(T *iniA, int n)
		:StaBinTree<T>(n){
			if (iniA != NULL && n > 0){
				root = struA + 1;
				for (int i = 0; i < n; ++i)
					root[i].Data = iniA[i];
				size = n;
			}
		}
	~CompleteBinSearchTree() override{
		DE_PRINTF("CBT析构");
	}

	Position find(Element x){
		return NULL;//not impl
	}
	//返回两个结点的公共祖先 越界返回哨兵(0号结点)
	Position ancestor(int i1, int i2){
		if (i1 > size || i2 > size || i1 < 0 || i2 < 0)
			return position(0);
		if (i1 == i2)
			return position(i1);
		else if (i1 > i2)
			ancestor(index(getParent(position(i1))), i2);//return 加不加只要最终有返回都会返回 效果一样
		else
			ancestor(i1, index(getParent(position(i2))));//return
	}
};

/*	make_heap:		根据指定的迭代器区间以及一个可选的比较函数，来创建一个heap. O(N)
push_heap:		把指定区间的最后一个元素插入到heap中. O(logN)
pop_heap:		弹出heap顶元素, 将其放置于区间末尾. O(logN)
sort_heap：		堆排序算法，通常通过反复调用pop_heap来实现. N*O(logN)
C++11加入了两个新成员：
is_heap:		判断给定区间是否是一个heap. O(N)
is_heap_until:	找出区间中第一个不满足heap条件的位置. O(N)*/
/*C++中heap以算法的形式提供，要使用这几个api需要#include <algorithm>*/
/*本算法以面向对象式实现
若存放非基本类型时 需重载赋值号
若存放指针时 需要自行销毁哨兵以及自己申请的内存 建议使用插入构造*/
/*堆 是最大(小)树, 完全二叉树*/
//如果仅仅是出堆而没有入堆的话 排个序O(N*logN) 此时入堆O(N) 出堆O(1)
template<class T>
class Heap : public CompleteBinSearchTree<T>{
	typedef T Element;
	typedef typename BinSearchTree<T>::Position Position;
	using BinTree<T>::root;
	using StaBinTree<T>::struA;
	using StaBinTree<T>::size;
	using StaBinTree<T>::capacity;
	using StaBinTree<T>::full;
	using StaBinTree<T>::empty;
	using CompleteBinSearchTree<T>::getLeftChild;
	using CompleteBinSearchTree<T>::getRightChild;
	using CompleteBinSearchTree<T>::getParent;
	using CompleteBinSearchTree<T>::getSibling;

	//使用less<T> greater<T> 或 比较符号重载 都有同样地问题: 两者没有同一个表示方法(带有重载()的父类)不便于大堆小堆的即时重构
	int(*cmper)(const T &min, const T &max) = NULL;
public:
	/*(堆的大小 小于比较方法 任意初始化序列序列 该序列的元素个数)  注意: 用数组构造的堆与插入初始化的堆 的结构不一样*/
	Heap(int sizeHeap, T *iniA = NULL, int nIniA = 0)
		: CompleteBinSearchTree<T>(sizeHeap){
			root = struA + 1;//没有有效元素也要对root赋值
			size = nIniA;
			for (Position t = struA + 1; t < root + nIniA; ++t) {/*为每个元素赋初始权值*/
				t->Data = iniA[t - root];//若T类型不是基本类型 需要重载赋值号
				linkToChildren(t);//只要没越界就链接->遍历时以size为结束遍历的标志 而不是子结点是否为空?
			}
		}
	~Heap()override{
		DE_PRINTF("Heap析构");
	}

	Element pop(){
		if (empty()){
			puts("堆已空 无法删除");
			return NULL;
		}
		if (notBuild()){
			puts("未构建");
			return false;
		}
		Element Item = struA[1].Data;/*取出即将返回的值*/
		/*矛盾在于:Size--删除的是尾元素 但是根据堆的定义我们应该删除1号元素
		将尾元素替换下滤时的p元素下滤 这样便于操作 且实际上删除的是1号元素*/
		PercoDown(1, "删除");//注意::方法内size--了
		unlinkToParent(struA + size);
		return Item;
	}
	bool push(Element Item){
		int i;
		if (full()){
			puts("堆已满 无法插入");
			return false;
		}
		if (notBuild()){
			puts("未构建");
			return false;
		}
		i = ++size;/*i指向[插入后]堆中最后一个元素的位置*/
		for (; cmper(struA[i / 2].Data, Item) < 0; i /= 2)
			struA[i].Data = struA[i / 2].Data;//从最后一个有孩子的结点开始 向上过滤结点
		struA[i].Data = Item;/*插入*/
		linkToChildren(struA + size);
		return true;
	}
	/*对于一个已读入的数据但需要调整的最大或最小或最小堆(自下而上调整)
	从[最后一个子节点的父节点 即 倒数第一个有儿子的结点]
	每次从左右结点中挑一个"大"的结点做下滤操作开始调整到根节点1*/
	/*线性堆重构 复杂度O(N)*/
	//最小堆 参数: maxCmp(大于)哨兵(最小值)	注意: 负数太小减法会变正:传入MAX_INT32/2即可
	//最大堆 参数: mixCmp(小于)哨兵(最大值)
	void build(T sentry, int(*cmper_)(const T &min, const T &max)){
		cmper = cmper_;
		struA[0].Data = sentry;
		for (int i = size / 2; i > 0; i--){
			PercoDown(i, "下滤");
		}
	}
protected:
	//上滤 insert调整
	void percolateUp(int i, Element *struA, int size){
		Element Item = struA[i];//i指向堆中需上滤元素的位置
		for (; struA[i / 2] - Item > 0; i /= 2)
			struA[i] = struA[i / 2];//从上滤结点的父结点开始 向上过滤结点 若父结点大于子结点则继续
		struA[i] = Item;
	}
	//下滤 :build调整(从末结点开始提升 等价于从最后一个父结点开始下滤) delete调整
	void percolateDown(int Parent, Element *struA, int size){
		int Child;
		Element x = struA[Parent];//取出需要下滤的值
		for (; 2 * Parent <= size; Parent = Child){
			Child = 2 * Parent;//若左儿子==size; 则右儿子不存在
			if (Child != size && struA[Child] - struA[Child + 1] > 0)
				++Child;//选取左右儿子中小的一个
			if (x - struA[Child] < 0)//插入元素小于目标时终止 大于时继续
				break;
			else//将孩子上移<==>将temp下移
				struA[Parent] = struA[Child];
		}
		struA[Parent] = x;
	}

	/*下滤函数 将H中以H->Data[p]为根的子堆调整为最大或最小或最小堆*/
	void PercoDown(int Start, char const *Order){
		int Parent, Child;
		Element x;
		/*下滤：取出自己          从自己开始找到一个合适的位置*/
		/*删除：取出尾元素 Size-- 从堆根开始*/
		int p = strcmp(Order, "删除") == 0 ? size-- : Start;
		x = struA[p].Data;/*取出需要下滤的值*/
		for (Parent = Start; 2 * Parent <= size; Parent = Child){
			Child = 2 * Parent;/*若左儿子==size; 则右儿子不存在*/
			if (Child != size && cmper(struA[Child].Data, struA[Child + 1].Data) < 0)
				Child++;/*选取左右儿子中大或小的一个*/
			if (cmper(x, struA[Child].Data) >= 0)
				break;
			else/*将孩子上移<==>将temp下移*/
				struA[Parent].Data = struA[Child].Data;
		}
		struA[Parent].Data = x;
	}
	//返回是否已经构建
	bool notBuild(){
		return struA[0].Data == NULL;
	}
	//子结点链接:链接当前结点的孩子结点
	void linkToChildren(Position parent){
		parent->Left = getLeftChild(parent);
		parent->Right = getRightChild(parent);
	}
	//子结点分离:解除父亲结点对孩子结点的链接
	void unlinkToParent(Position delChild){
		Position parent = getParent(delChild);
		//if (!empty(parent))
		if (parent != nullptr)
			linkToChildren(parent);//若size已经--那么越界无效valid的子结点会被置NULL
	}
};

/*
没有度为1的结点
n个叶子结点的huffman树的节点数=2n-1
*/
/*带权路径最小的二叉树称为最优二叉树或huffman树(结构并不唯一)
huffman树的判断标准:最优前缀编码树 <==>最优编码(WPL或textLen最小) && 前缀码(短码不能是长码的前缀)*/
template<class T>
class HuffmanTree : public BinTree<T>{
protected:
	using BinTree<T>::root;
	//子类需要递归子结点的地方必定用到Left-Right只能用BinTree的Position-论getValue()存在的必要性
	using Position = typename BinTree<T>::Position;
	using Element = typename BinTree<T>::Element;
	typedef class HuTrNode : public BinTree<T>::BTNode{
	public:
		int Weight = 0;//权重  Data作为 编码字符
		int getValue()override{
			return Weight;
		}
		void setValue(int w)override{
			Weight = w;
		}
		HuTrNode(int data_, int weight_)
			:BinTree<T>::BTNode(data_){
				Weight = weight_;
			}
		~HuTrNode()override{
			DE_PRINTF("HuTrNode析构");
		}
	}*HuTr;//哈夫曼树
	HuTr huffRoot = NULL;
public:
	/*哈夫曼树的构建 最小堆实现 (字符序列, 权重序列(一般是频值freq), 序列公共大小, huf叶子结点数)*/
	HuffmanTree(T* character, int *freq, int nHuf) {
		Heap<HuTr> h = Heap<HuTr>(nHuf);
		HuTr sentry = new HuTrNode(0, -MAX_INT32 / 2);//没有赋值方法 只能new?
		//lambda表达式, 匿名函数 p345
		h.build(sentry, [/*捕获列表*/](const HuTr &min, const HuTr &max){
			return max->Weight - min->Weight;
		});
		for (int i = 0; i < nHuf; ++i)	h.push(new HuTrNode(character[i], freq[i]));
		/*做size-1次合并 每次将权值最小的两颗树合并 但是运算途中size会变化 所以用n*/
		while (nHuf-- > 1){
			HuTr t = new HuTrNode(0, 0);//当泛型为char时 无效结点为'\0'不会输出(自动省略无效的非叶结点) 否则需要重写遍历方法
			t->Left = h.pop();
			t->Right = h.pop();
			t->setValue(t->Left->getValue() + t->Right->getValue());/*计算新权值*/
			h.push(t);
		}
		huffRoot = h.pop();
		root = huffRoot;
		delete sentry;
	}/*频值向量freq[0]代表权值个数 0<=freq[i])是权值 i[1,*freq O(N log(N))*/
	//对任意句子构造huffman树

	~HuffmanTree()override{
		DE_PRINTF("HuffmanTree析构");
	}
	//清空输入流直到遇见end前所有字符
	static void flushInput(char end){
		char c;
		while ((c = getchar()) != end);
		ungetc(c, stdin);
	}
	//清空输入流的换行与空格并返回非换行空格的那个字符 不可见的(Invisible)
	static void flushBlanckLF(){
		char c;
		while ((c = getchar()) == '\n' || c == ' ');
		ungetc(c, stdin);
	}
	//返回长度为maxLen的char型字符串 若get = true自动调用gets(str);
	static char* newSentence(int maxLen, bool get = false, char end = '\n'){
		//str.resize(maxLen);//str的length() size都是maxLen 于是弃用 但是若换成new char 又回到了内存管理的问题
		//cin.getline(&str[0], maxLen, end);
		char* str = (char*)malloc(sizeof(char)* maxLen);
		memset(str, 0, sizeof(char)* maxLen);
		if (get) gets(str);
		return str;
	}
	//返回编码长度(编码字串)
	static int codeLen(char *code){
		return strlen(code);
	}
	static int codeLen(JCE::String const&code){
		return code.length();
	}
	/*tips:
	depth = codeLen+1 <-> depth(root)=1; (正规的, 书上的depth定义)
	depth = codeLen  <--> depth(root)=0
	(所以: 文本长度=权重*编码长度, WPL=权重*(编码长度+1))
	*/
	//返回文本的WPL
	static int wpl(JCE::ArrayList<char*> &s, int *freq){
		int textLen, i, n = s.size();
		for (textLen = i = 0; i < n; ++i){
			textLen += freq[i] * (codeLen(s[i]) + 1);
		}
		return textLen;
	}
	//返回一套编码的文本长度 = freq*codeLen
	static int textLen(JCE::ArrayList<char*> &s, int *freq){
		int textLen, i, n = s.size();
		for (textLen = i = 0; i < n; ++i){
			textLen += freq[i] * codeLen(s[i]);
		}
		return textLen;
	}
	//检查文本是否前缀码 //string const codes[]-> 与用ArrayList存效果差不多?
	static bool isPrefixCodes(JCE::ArrayList<JCE::String> const &codes) {
		int n = codes.size();
		for (size_t i = 0; i < n; i++) {
			for (size_t j = 0; j != i && j < n; j++) {
				if (codes[j].find(codes[i]) == 0)//前缀冲突
					return false;
			}
		}
		return true;
	}
	//返回本huffman编码的带权路径长度 [路径长度]:从根结点到目标结点须经过的总边数(分支数 )(经过的结点总数-1)
	int wpl(){
		return WPL(huffRoot, 1);
	}
	//返回本huffman编码的(最短)文本长度 (空间占位 = textLen*1)
	int textLen(){
		return textLen(huffRoot, 1);
	}
	//判断是否最优编码
	bool isOptimalCoding(JCE::ArrayList<JCE::String> &codes, int *freq){
		//也可以比较wpl
		int textLen, i, len, n = codes.size();
		for (textLen = i = 0; i < n; ++i){
			len = codeLen(codes[i]);
			if (len > n - 1)/*n个结点最长编码长度为n-1*/
				return false;//效率优于直接计算并比较textLen
			textLen += freq[i] * len;
		}
		return textLen == this->textLen() ? true : false;
	}
	//返回本huffman编码文本的空间占用=codeLen*sizeof(字符)
	int memory(){
		return textLen()*sizeof(T);
	}
private:
	void destroy(HuTr &t){
		Position tmp = t;
		//可以直接调用 但引用不能传给父类
		BinTree<T>::destroy(tmp);
		t = NULL;
	}
	//返回给出的编码的前缀编码树 若所给的编码不是前缀码返回 NULL 若有频值则按频值依次填入 否则全部置1(有未知bug 若判断一套编码是否huffman编码)
	HuTr buildPreCodeTree(JCE::ArrayList<JCE::String> const &codes, int *freq = NULL){
		int n = codes.size(), sub = 0;
		HuTr rt = new HuTrNode(0, 0);
		for (int i = 0; i < n; i++){
			int len = codeLen(codes[i]);
			Position t = rt;
			for (int j = 0; j < len; j++){
				if (codes[i][j] == '1'){//1向左
					if (!t->Left)//为空 建结点
						t->Left = new HuTrNode(0, 0);
					else if (t->getValue() != 0){//不为空判断继续进行还是中止
						destroy(rt);
						return NULL;
					}
					t = t->Left;
				}
				else{//0向右
					if (!t->Right)
						t->Right = new HuTrNode(0, 0);
					else if (t->getValue() != 0){//建树途中遇见带权结点 ->该编码并非前缀码
						destroy(rt);
						return NULL;
					}//else do nothing
					t = t->Right;
				}
			}
			if (!t->isLeave())//若不是叶结点 则必不是前缀码
				return NULL;
			t->setValue(freq == NULL ? 1 : freq[sub++]);//赋权
		}
		return rt;
	}
	//带权路径长度(weightted path length)=sum(weight*depth)
	int WPL(Position t, int depth){
		if (!t->Left && !t->Right)
			return t->getValue()*depth;
		else/*否则一定有两个孩子*/
			return WPL(t->Left, depth + 1) + WPL(t->Right, depth + 1);
	}
	//(最短)文本长度 = freq[i]*codeLen
	int textLen(Position t, int depth){
		if (!t->Left && !t->Right)
			return t->getValue()*(depth - 1);//tips:depth(root) = 1
		else/*否则一定有两个孩子*/
			return textLen(t->Left, depth + 1) + textLen(t->Right, depth + 1);
	}
	//返回仅含大写英文句子的频率数组 改为map?
	JCE::ArrayList<int> frequency(JCE::String sentence){
		JCE::ArrayList<int> freq;
		freq.resize(26);
		for (unsigned i = 0; i < sentence.length(); ++i){
			if (sentence[i] == '_')
				freq[26]++;
			else{
				freq[sentence[i] - 'A']++;
			}
		}
	}
	//返回一个仅含大写英文字母和下划线的句子的哈夫曼编码所占内存,普通编码memory = sentence.length()*8
	int huffmanMemory(JCE::String sentence){
		static const int MAX_FREQ_SIZE = 128;
		int freq[MAX_FREQ_SIZE];
		memset(freq, 0, MAX_FREQ_SIZE * sizeof(int));

		for (unsigned i = 0; i < sentence.length(); ++i){
			if (sentence[i] == '_')
				freq[26]++;
			else{
				freq[sentence[i] - 'A']++;
			}
		}
		std::priority_queue<int, JCE::ArrayList<int>, JCE::greater<int> > q;

		for (int i = 0; i < MAX_FREQ_SIZE; ++i){
			if (freq[i] != 0)
				q.push(freq[i]);
		}
		int tmp = 0, mem = 0;
		while (q.size() > 1){
			tmp = q.top(); q.pop();
			tmp += q.top();
			q.pop();
			q.push(tmp);
			mem += tmp;//其实就是把越小的频率反复多加几次，越大的频率少加几次
		}
		return mem == 0 ? sentence.length() : mem;
	}
};

/*数组式huffman*/
class ArrayHuffman {
	struct HTNode {
		//权重
		int weight;
		//父结点
		int parent;
		//左孩子
		int lchild;
		//右孩子
		int rchild;
		//只用于sub方法查找是是否已使用的判断 注意此处借用了memset的初始化巧合
		bool isUsed;
	};
public:
	typedef HTNode *HuffmanTree;
	typedef char ** HuffmanCode;
protected:
	static void swap(int &s1, int &s2) {
		int tmp = s1;
		s1 = s2;
		s2 = tmp;
	}
	static void swapC(char &s1, char &s2) {
		char tmp = s1;
		s1 = s2;
		s2 = tmp;
	}
	//从1到upbound中找出father为0的节点赋给s1,s2,（为了保证答案唯一，请让s1的节点编号小于s2）
	static void SelectTwoMin(int upbound, HuffmanTree ht/*这tm是个数组!!!*/, int &s1, int &s2) {
		s1 = s2 = 0;
		//找出weight最小的两个的sub
		for (int i = 1; i < upbound; ++i) {
			if (ht[i].parent == 0) {
				if (ht[s1].weight > ht[i].weight)
					s1 = i;
			}
		}
		for (int i = 1; i < upbound; ++i) {
			if (ht[i].parent == 0) {
				if (ht[s2].weight > ht[i].weight && i != s1)
					s2 = i;
			}
		}
		if (s1 > s2) {
			swap(s1, s2);
		}
	}
	//查找并返回数组哈夫曼树中某个权值的结点的下标 不存在返回0
	static int sub(HuffmanTree ht, int wei, int n) {
		for (int i = 1; i < n; ++i) {
			if (!ht[i].isUsed && ht[i].weight == wei) {
				ht[i].isUsed = true;
				return i;
			}
		}
		return 0;
	}
public:
	//构造哈夫曼树ht 并 计算哈夫曼编码hc w为权值(频率)数组 n为权值个数 (需要手动free)
	static void HuffmanCoding(HuffmanTree &ht, HuffmanCode &hc, int *w, int n) {
		// 构造哈夫曼树(伪)
		ht = (HuffmanTree)malloc(sizeof(HTNode)*(2 * n));
		memset(ht, 0, (sizeof(HTNode)*(2 * n)));
		ht[0].weight = 1001;
		for (int i = 0; i < n; ++i) ht[i + 1].weight = w[i];
		//做n-1次合并 每次将权值最小的两颗树合并
		int size = n;
		for (int i = 1; i < n; ++i) {
			++size;
			//"pop" and "push"
			SelectTwoMin(size, ht, ht[size].lchild, ht[size].rchild);
			//计算新权值
			ht[size].weight = ht[ht[size].lchild].weight + ht[ht[size].rchild].weight;
			ht[ht[size].lchild].parent = ht[ht[size].rchild].parent = size;
		}
		// 计算编码
		hc = (HuffmanCode)malloc(sizeof(char*)*n);
		memset(hc, 0, (sizeof(char)*n));
		for (int i = 0; i < n; ++i) {
			hc[i] = (char*)malloc(sizeof(char)* n);
			memset(hc[i], 0, sizeof(char)* n);
			//n结点最长编码长度n - 1+'\0' = n 下标c为顶级结点下标size时终止
			for (int c = sub(ht, w[i], size), codeIndex = 0; c != size; c = ht[c].parent) {
				int parent = ht[c].parent;
				//hc[i][codeIndex++] = ht[parent].lchild == c ? '0' : '1';
				//左子树代表频度 < 右子树频度 编码为0
				hc[i][codeIndex++] = ht[parent].lchild == c ? '1' : '0';
			}
		}
		for (int i = 0; i < n; ++i) {
			//将每个编码逆转
			for (int lhs = 0, len = strlen(hc[i]); lhs < len / 2; ++lhs) {
				int rhs = len - lhs - 1;
				swapC(hc[i][lhs], hc[i][rhs]);
			}
		}
	}

	//返回编码长度
	static int codingLen(ArrayHuffman::HuffmanCode const hufCode, int n, int *weightList) {
		int sum = 0;
		for (int i = 0; i < n; ++i) {
			sum += weightList[i] * strlen(hufCode[i]);
		}
		return sum;
	}
};

//表达式树 @TODO 并未通过PTA测试(主要是后缀表达式计算时的负号问题 考虑添加前缀表达式计算)
class ExpressionTree : private BinTree<JCE::String>{
public:
	enum ExpressionTypeEnum{ 
		PREFIX_EXPRESSION,//前缀表达式
		INFIX_EXPRESSION,//中表达式
		POST_EXPRESSION,//后缀表达式
		ERROR_EXPRESSION//错误的表达式
	};
	//中缀表达式构建的表达式树不唯一
	ExpressionTree(string express, ExpressionTypeEnum type_){
		expression = express;
		type = type_;
	}
	//类型检查 检查完成后将类型置为willBe?
	bool check(ExpressionTypeEnum should, ExpressionTypeEnum willBe){
		if (type != should){
			return false;
		}
		else{
			if (should == ORDER_PREFIX_ROOT){
				int i = 0, countNum = 0, countOp = 0;
				char num[10] = { 0 };
				do{
					if (getNum(&i, expression.c_str(), num) != '\0')
						++countOp;
					else
						++countNum;
				} while (expression[i++] != '\0');
				//操作符个数检查:正确的二元前缀表达式中，用空格间隔的操作数必然比操作符多一个
				if (countNum - countOp != 1){
					type = ERROR_EXPRESSION;
					return false;
				}
				else{
					type = willBe;
					return true;
				}
			}
			else{
				_DEBUG_ERROR("后缀向前缀转换还没写");
				return false;
			}
		}
	}
	//返回下一个运算符号
	string nextOper(){
		static size_t ind = 0;
		string next;
		for (; ind < expression.size(); ind++){
			if (expression[ind] != ' ')
				next += expression[ind];
			else
				break;
		}
		ind++;
		return next;
	}
	/*
	1. 从左往右扫描串
	　2. 遇到操作符则递归构造树节点，当前操作符是根节点，并递归构造左右子节点
	 　3. 后序遍历当前结果，并返回
	  */
	//表达式转换  不允许不检查直接调用
	string prefixToPost(){
		string next = nextOper(), ans;
		if (!isdigit(next[0])){
			auto node = nodeCreater(next);//递归构造树
			node->Left = nodeCreater(prefixToPost());
			node->Right = nodeCreater(prefixToPost());
			postTraversal(node, [&](BT const bT){
				if (ans != "")
					ans += " " + bT->Data;
				else
					ans = bT->Data;
			});//后序遍历转换
			delete node->Left;
			delete node->Right;
			delete node;
			return ans;
		}
		else
			return next;
	}
	//transform
	string toPost(){
		if (type == PREFIX_EXPRESSION){
			if (check(PREFIX_EXPRESSION, POST_EXPRESSION))
				return expression = prefixToPost();
			else
				return "0";
		}
		else{
			_DEBUG_ERROR("方法调用错误: 只能是前缀向后缀转换");
			return "0";
		}
	}
	double eval(){
		if (type == POST_EXPRESSION){
			if (check(POST_EXPRESSION, POST_EXPRESSION))
				return evalPostfix();
			else
				return MAX_INT32;
		}
		else if (type == PREFIX_EXPRESSION){
			toPost();
			return eval();
		}
		else if (type == ERROR_EXPRESSION)
			return MAX_INT32;
		else{
			_DEBUG_ERROR("无法直接计算中缀表达式");
			return MAX_INT32;
		}
	}
	/*后缀表达式计算*/

	/*如果两个空格之间是操作数则返回该操作 否则将数字存于num中并返回'\0'*/
	char getNum(int *i, char const*ori, char *num){
		int sub = 0;
		while (1){
			if (ori[*i] == ' ' || ori[*i] == '\0'){
				num[sub] = '\0';
				if (sub == 1 && !isdigit(num[0]))
					return num[0];
				else
					return '\0';
			}
			else{
				num[sub++] = ori[(*i)++];
				continue;
			}
		}
	}
	//返回后缀表达式ans的值 不合法返回MAX_INT32
	double evalPostfix(){
		int i = 0;
		char c = 0;
		static char num[50] = { 0 };
		stack<double> ps;
		double po1, po2;
		do{
			if ((c = getNum(&i, expression.c_str(), num)) == '\0'){
				ps.push(atof(num));
			}
			else{
				if (ps.empty()){//运算符号缺少 或者多余
					return MAX_INT32;
				}
				po1 = ps.top();
				ps.pop();
				if (ps.empty()){
					return MAX_INT32;
				}
				po2 = ps.top(); ps.pop();
				switch (c){
				case '+':ps.push(po2 + po1); break;
				case '-':ps.push(po2 - po1); break;
				case '*':ps.push(po2 * po1); break;
				case '/':{
							 if (po1 - 0.0 == 0){//除以0错误
								 return MAX_INT32;
							 }
							 else
								 ps.push(po2 / po1); break;
				}
				default:break;
				}
			}
		} while (expression[i++] != 0);
		double re = ps.top(); ps.pop();
		if (ps.empty()){
			return re;
		}
		else{//运算数多余
			return MAX_INT32;
		}
	}
private:
	//BinTree base;
	string expression;
	//表达式当前类型
	ExpressionTypeEnum type;
};

#endif
