//碰撞检测
#ifndef __ImpactDetection_H
#define __ImpactDetection_H
#include "Animation.h"
#include "Attribute.h"

//每个可碰撞对象都必须可转换为包围盒对象
class BoundingBox{
protected:
	clock_t createClock = clock();//对象生成时间
	//体型对象对应一个实体, 只有实体才能获取时间戳
	shared_ptr<Timestamp> timestamp;
	bool obstruct = false;//阻挡效果 碰撞不一定阻挡(为什么不整合Body和ImpacttShell的理由)
	TimeClock durationClock;/*持续时间 超过该时间后无效*/
	/*
	LARGE_INTEGER litmp;		// 定义该结构体的一个对象
	LONGLONG QPart1, QPart2;	// 用于获取结构体里面的 QuadPart 成员
	gameRefresh.SetDrtTime(30);
	QueryPerformanceCounter(&litmp);
	QPart2 = litmp.QuadPart;//更新当前时间
	*/

	//附带阻挡效果的事件返回true
	bool obstructEvent()const{
		return obstruct;
	}
public:

	BoundingBox(shared_ptr<Timestamp> const &timestamp)
		:timestamp(timestamp){
	}

	int getOwnerID() const {
		return timestamp->getOwnerID();
	}
	//使这一个碰撞盒对象无效(不再参与碰撞检测)
	virtual void invalidate(){
		createClock = -createClock;
	}
	virtual bool isValid() {
		//发送对象存在 事件对象在发送后生成
		return timestamp->isValid() && createClock >= timestamp->getSendClock();
	}

	//生效: 用于事件接收者接收事件的效果 存在阻挡返回true(传入属性包进行效果计算)
	virtual bool takeEffect(Attribute &originBioAttribute, BoundingBox const &triggererBox){
		return obstruct;
	}

	//每多一种碰撞盒必须在包围盒管理类(此类)中 注册相应的包围盒转换方法 尽量外切
	virtual PolygonShape toPolygon() const = 0;

	virtual CircleShape toCircle() const = 0;

	virtual Sprite const &toSprite() const = 0;
};

//矩形碰撞盒: 能进行碰撞判定的子画面
class CollisionBox : public Sprite, public BoundingBox{
public:
	using CollisionBoxList = LinkedList<shared_ptr<CollisionBox>>;

	CollisionBox(shared_ptr<Timestamp> const &timestamp, int h = 0, int w = 0)
		:BoundingBox(timestamp){
		this->setSize(w, h);
	}
	//盒子的位置可以待定 但大小必须知道
	CollisionBox(shared_ptr<Timestamp> const &timestamp, GP Size const &size)
		:BoundingBox(timestamp), Sprite(size){
	}

	// 按照给定的矩形象限四分线 切割本碰撞盒 返回切割后的包围盒组
	void carve(GP Point &centre, CollisionBoxList &results) const {
		GP Rect rect;//不能static
		rect = this->getRect();
		CollisionBoxList temp;
		int	dX = centre.X - rect.X;
		int	dY = centre.Y - rect.Y;
		bool carveY = dX > 0 && dX < rect.Width;//纵切
		bool carveX = dY > 0 && dY < rect.Height;//横切transect

		// 切割XY方向
		if (carveX && carveY) {
			//xy切割一次后不会再递归地xy切割 而只有xy切割会影响centre的值 故centre可以是引用
			int cyTemp = centre.Y;
			centre.Y = rect.Y;
			carve(centre, temp);//现将原矩形纵向切为两部分
			centre.Y = cyTemp;
			centre.X = rect.X;
			while (!temp.empty()) {//再依次将两部分横向切割
				temp.back()->carve(centre, results);
				temp.pop_back();
			}
		}// 只切割X方向
		else if (carveX) {
			results.push_back(newNode(timestamp, rect.X, rect.Y, rect.Width, dY));
			results.push_back(newNode(timestamp, rect.X, centre.Y, rect.Width, rect.Height - dY));
		}// 只切割Y方向
		else if (carveY) {
			results.push_back(newNode(timestamp, rect.X, rect.Y, dX, rect.Height));
			results.push_back(newNode(timestamp, centre.X, rect.Y, rect.Width - dX, rect.Height));
		}
	};

	void setTriggerDistrict(shared_ptr<PolygonShape> triggerDistrict){
		this->triggerDistrict = triggerDistrict;
	}

	virtual PolygonShape toPolygon() const override{
		return PolygonShape(getRect());
	}

	virtual CircleShape toCircle() const override {
		static GP Point centre;
		int w = getHeight(), h = getWidth();
		double r = sqrt(pow(w, 2) + pow(h, 2)) / 2;
		return CircleShape(getCentre(centre), (int)(r));
	}
	virtual Sprite const &toSprite() const override{
		return *this;
	}
protected:
private:
	shared_ptr<CollisionBox> newNode(shared_ptr<Timestamp> const &timestamp,
		int x = 0, int y = 0, int h = 0, int w = 0)const{
		auto temp = new CollisionBox(timestamp, w, h);
		temp->setLocation(x, y);
		return shared_ptr<CollisionBox>(temp);
	}
	//实际碰撞触发区域
	shared_ptr<PolygonShape> triggerDistrict;
};

//包围盒组
using CollisionBoxList = CollisionBox::CollisionBoxList;

//矩形碰撞盒管理用四叉树结点
class QuadTreeNode{
public:
	//子象限枚举 (保证各子象限的int值可以直接作为数组索引)
	enum QuadrantEnum{
		//未定义
		UNDEFINED = -3, 
		//父象限(不完全属于此象限)
		FATHER = -2, 
		//此象限交界处
		JUNCTION = -1, 
		ONE = 0, 
		TWO = 1, 
		THREE = 2, 
		FOUR = 3 
	};
	using NodesArray = vector<shared_ptr<QuadTreeNode>>;//如果需要支持赋值和拷贝操作的话 不能用shared_ptr<>管理指针
	
	//new一个CollisionBox此对象会管理
	QuadTreeNode(Sprite *&&bounds, QuadTreeNode *parent = nullptr, int level = 0)
		: bounds(bounds), parent(parent), level(level){
		if (level < MAX_LEVELS){//优先分裂
			split();
		}
	}
	~QuadTreeNode(){}

	//返回此结点下所有参与碰撞的碰撞盒数
	int counter(){
		return count;
	}

	//分裂: 使分离生成子象限
	void split() {
		GP Point centre;
		static GP Rect rect;
		bounds->getCentre(centre);
		rect = bounds->getRect();
		auto x = rect.X;
		auto y = rect.Y;
		//在奇数分割时加1 避免出现边缘搜索时包含判断的错误
		auto sWidth = rect.Width % 2 == 0 ? rect.Width / 2 : rect.Width / 2 + 1;//象限四分后的宽高
		auto sHeight = rect.Height % 2 == 0 ? rect.Height / 2 : rect.Height / 2 + 1;
		/*auto sWidth = rect.Width / 2;//象限四分后的宽高
		auto sHeight = rect.Height / 2;*/
		this->quadrants.push_back(newNode(this, centre.X, y, sWidth, sHeight));
		this->quadrants.push_back(newNode(this, x, y, sWidth, sHeight));
		this->quadrants.push_back(newNode(this, x, centre.Y, sWidth, sHeight));
		this->quadrants.push_back(newNode(this, centre.X, centre.Y, sWidth, sHeight));
	}

	//forDebug
	int calcSize() const {
		int sizeCounter = this->objects.size();
		for (auto it = quadrants.begin(); it != quadrants.end(); ++it) {
			sizeCounter += (*it)->calcSize();
		}
		if (count != sizeCounter){
			_DEBUG_ERROR("四叉树内碰撞盒 动态数量字段 与 实际计算数量 不符!");
		}
		return sizeCounter;
	}

	shared_ptr<QuadTreeNode> newNode(QuadTreeNode *parent, int x, int y, int w, int h)const{
		return shared_ptr<QuadTreeNode>(new QuadTreeNode(new Sprite(GP Rect(x, y, w, h)), parent, level + 1));
	}

	//是根结点返回true
	bool isRoot()const{
		return parent == nullptr;
	}

	//内部检索
	void retrieveIn(shared_ptr<CollisionBox> const &collisionBox,
		CollisionBoxList &results)const{

		if (count == 0){
			return;//当前象限的总结点数为0
		}
		else{
			//进行查询
		}

		//append添加途经的所有有效碰撞盒;(由于批量刷新这一特性可能导致碰撞树中存在无效的碰撞盒) assign分配赋值
		for (auto it = objects.begin(); it != objects.end(); ++it){
			if ((*it)->isValid()){
				results.push_back(*it);
			}
		}

		if (DEBUG){
			static WindowSprite s;
			//s.drawRectangle(collisionBox->getRect());
			s.drawRectangle(bounds->getRect());
		}

		//查询子象限
		if (quadrants.empty()){
			//没有子象限 直接返回
		}
		else{
			QuadrantEnum index = getQuadrantIndex(collisionBox);
			if (index == QuadrantEnum::FATHER){
				_DEBUG_ERROR("错误: 检索途中不可能出现父画面的碰撞盒");
			}
			else if (index == JUNCTION){
				static GP Point centre;
				CollisionBoxList carveBoxes;//分裂的盒子
				collisionBox->carve(bounds->getCentre(centre), carveBoxes);

				for (auto it = carveBoxes.begin(); it != carveBoxes.end(); ++it){
					index = getQuadrantIndex(*it, false);//分裂的盒子小必属于子象限 不用检查
					quadrants[index]->retrieveIn(*it, results);
				}
			}
			else{
				quadrants[index]->retrieveIn(collisionBox, results);
			}
		}
	}

	//内部刷新
	void refreshIn(CollisionBoxList &overflowList){
		//遍历碰撞对象
		for (auto it = objects.begin(); it != objects.end();){
			auto collisionBox = *it;

			if (collisionBox->isValid()){
				QuadrantEnum index = getQuadrantIndex(collisionBox, true);
				if (index == JUNCTION){//交界处 不变动
					++it;
				}
				else{
					//删除后重新插入
					eraseBoundingBox(it);
					if (isRoot()){
						overflowList.push_back(collisionBox);
					}
					else{
						insert(collisionBox, parent, index);
					}
				}
			}
			else {//清除无效结点
				auto p = parent;
				//将父画面的计数都减一
				while (p != nullptr){
					--p->count;
					p = p->parent;
				}
				eraseBoundingBox(it);
			}
		}

		// 递归刷新子象限
		for (auto it = quadrants.begin(); it != quadrants.end(); ++it) {
			if ((*it)->count != 0){
				(*it)->refreshIn(overflowList);
			}
		}
	}

	//返回碰撞盒的子象限索引
	QuadrantEnum getQuadrantIndex(shared_ptr<CollisionBox> const &collisionBox,
		bool checkIsInner = true)const{
			static GP Point centre;
			static GP Rect rect;
			bounds->getCentre(centre);
			rect = collisionBox->getRect();
			bool onTop = rect.Y + rect.Height <= centre.Y;;
			bool onBottom = rect.Y >= centre.Y;
			bool onLeft = rect.X + rect.Width <= centre.X;
			bool onRight = rect.X >= centre.X;

			if (checkIsInner && !this->bounds->contains(*collisionBox)) {
				return FATHER;//溢出此象限界限
			}
			else{
				if (onTop) {
					if (onRight) {
						return ONE;
					}
					else if (onLeft) {
						return TWO;
					}
				}
				else if (onBottom) {
					if (onLeft) {
						return THREE;
					}
					else if (onRight) {
						return FOUR;
					}
				}
				//位于交界处
				return JUNCTION;
			}
		}

	//删除此象限的it结点 it自动更新
	void eraseBoundingBox(CollisionBoxList::iterator &it){
			--count;
			it = objects.erase(it);
		}

	//内部递归插入接口 (parent非nullptr 已知index时可以不重复计算)
	bool insert(shared_ptr<CollisionBox> const &collisionBox,
		QuadTreeNode *parent, QuadrantEnum index = UNDEFINED){
			static bool fromSub = false;
			if (fromSub){
				//来自子结点的插入有可能还会向上追溯 不增加
			}
			else{
				//++count;
			}

			if (quadrants.empty()){
				_DEBUG_ERROR("插入碰撞盒过小 当前的层数无法区分 建议增加最大层数");
				return false;
			}
			else{
				index = index == UNDEFINED ? this->getQuadrantIndex(collisionBox) : index;
				if (index == QuadrantEnum::JUNCTION){
					if (fromSub){//追溯结束 确认添加
						++count;
						fromSub = false;
					}
					else{
						++count;
						//正常添加
					}
					//位于此象限
					objects.push_back(collisionBox);
				}
				else if (index == QuadrantEnum::FATHER){
					if (isRoot()){
						//++count;
						//不能将溢出的碰撞盒 放在Root象限: 参见insert的外部接口处的解释
						//objects.push_back(collisionBox);
						return false;
					}
					else{
						//向父画面插入(用于刷新)
						fromSub = true;
						--parent->count;//减少一个该象限已经删除的该结点
						parent->insert(collisionBox, parent->parent);
					}
				}
				else{
					//位于子象限
					++count;
					return quadrants[index]->insert(collisionBox, this);
				}
			}
			return true;
		}
	
private:
	//min 9, 平均15次, max 21
	static const int MAX_OBJECTS = 50;//主要是担心分割线处分布了过多碰撞盒 (最合适象限插入)
	static const int MAX_LEVELS = 7;
	int count = 0;//子结点数
	CollisionBoxList objects;//存储碰撞盒
	NodesArray quadrants;//存储四个子象限节点
	int level;//该节点的深度，根节点的默认深度为0
	QuadTreeNode *parent = nullptr;//空表示root
	shared_ptr<Sprite> bounds;//界限: 该节点对应的象限在屏幕上的范围，bounds是一个矩形
};

//四叉树(场景管理 皮影戏)
class QuadTree{
public:
	QuadTree(Sprite const &bounds)
		:root(new Sprite(bounds)){
	}
	//插入一个碰撞盒
	bool insert(shared_ptr<CollisionBox> const &collisionBox){
		//解决插入象限溢出(同时在出现可能的溢出错误时不报错 也导致了一些bug难以发现) 
		//如果采用直接插在Root象限的话: 碰撞盒原属于Root象限的父象限 当它进入了Root的某个子象限再返回时Root的"父象限"时会引发: 从Root删除->插入"父象限"->迭代 的无限刷新
		if (!root.insert(collisionBox, nullptr)){
			overflowList.push_back(collisionBox);
			return true;
		}
		if (overflowList.size() > 60-(size_t)(Constant::mainCanvasSize.Width*0.015)){
			//强行报错 不允许溢出碰撞盒数量过多 此时既有可能是出现了bug
			_DEBUG_ERROR("插入了过多的溢出Root画面的子画面 请检查是否出现BUG 或者进行优化");
		}
		return true;
	}
	//返回所有参与碰撞的碰撞盒数
	int size(){
		return root.counter() + overflowList.size();
	}

	//检索: 返回可能碰撞的最小集合
	void retrieve(shared_ptr<CollisionBox> const &collisionBox, CollisionBoxList &results) const {
		root.retrieveIn(collisionBox, results);
		//results.splice(results.end(), CollisionBoxList(overflowList)}); //concat: 合并两个集合(保留后者)
		FOR (it, overflowList.begin(), overflowList.end()){
			if ((*it)->isValid()){
				results.push_back(*it);
			}
		}
	}

	//动态刷新
	void refresh(){
		for (CollisionBoxList::iterator iter = overflowList.begin(); iter != overflowList.end();){
			if (root.insert(*iter, nullptr)){
				iter = overflowList.erase(iter);
			}
			else{
				++iter;
			}
		}
		root.refreshIn(overflowList);
	}

	void forDebug() const {
		int cntBufferForDebug = root.calcSize();
	}
private:
	CollisionBoxList overflowList;
	QuadTreeNode root;
};

#endif