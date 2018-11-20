#ifndef __Biology_H
#define __Biology_H
//生态区域->生物->生态系统
#include "Ecologic.h"

/*生物基类: 种种迹象都表明生物类不是子画面 包涵一个子画面*/
class Biology{
private:
	std::shared_ptr<Body> body;
	//使用智能指针的原因: 生物死亡后 其内存不会马上销毁 会在所有生物进行目标有效判定(即一轮)后再销毁(防止了二次销毁) 而使用智能指针方便了随机的target
	//时间戳控制器: 即使生物死亡 只要有生物持有此时间戳 那么就可以判断 数据的有效性 其发出的除位置外的事件仍应当有效
	TYPE type;/*对象类型*/
	static AnimationManager surface;//外观
protected:
	static int obCnt;//对象总数
	DisplayArea *belongs;//所属地图
	ShortestPathResult shortestPathResult;
	//导航队列: 每次会pop最前面的点作为当前目标点 然后做偏移
	LinkedList<GP Point> navigationQueue;
	//精灵的绘制位置与精灵的实际位置有所不同 简单来说系统会将实际位置的地图读取到相机所在的地方进行渲染
	std::shared_ptr<Body> drawBody;
	int bypassCount = -1;
	STATUS status = NORMAL;
	//std::shared_ptr<Attribute>
	Attribute *attribute_;//子类可修改 外部可读取
	int friendliness;//友好度
	int gem;/*宝石(货币)*/
	//对子类暴露可修改的属性(相当于一个可继承的域)
	Attribute &attribute(){
		return *attribute_;
	}
	
	Body &getBody(){
		return *body;
	}
	/****用于子类注册贴图*****/
	static AnimationManager &getSurface(){
		return surface;
	}
public:
	//(贴图对象 所属生态)
	Biology(DisplayArea *, TYPE, GP Size const &);
	virtual ~Biology(){
		body->invalidate();
		body->getTimestamp()->invalidate();
		delete attribute_;
		attribute_ = nullptr;
	}
	/*Biology& operator=(Biology const &){}//delete
	//完美转发(perfect forward):怎么传进来怎么传给别人
	//Biology& operator=(Biology &&rvalue);
	Biology(Biology const &){}// = delete;
	Biology(Biology &&rvalue);*/

	void alloc(){
		//attribute_.reset(new Attribute());
		//targetBuffer.reset(size);//主要是宽高
		//targetBuffer->setParent(belongs->getBody());
		attribute_ = new Attribute(new TargetLocation(getBody()));
	}
	/*生物的复制 定义为繁殖行为所需 暂时先这样*/
	Biology &operator=(Biology const &rhs){
		*attribute_ = *rhs.attribute_;
		return *this;
	}
	Biology(Biology const &rhs){
		alloc();
		*this = rhs;
	}
	Biology &operator=(Biology &&rvalue){
		std::swap(attribute_, rvalue.attribute_);
		return *this;
	}
	Biology(Biology &&rvalue){
		alloc();
		*this = std::move(rvalue);
	}

	
	static void loading(){
		surface.setGraphics();
		surface.reLoad(_T("game\\player\\nan.png"), 3, 4);
		surface.reLoad(_T("game\\player\\si.png"), 3, 4);
		surface.reLoad(_T("game\\player\\li.png"), 3, 4);
		for (int i = 0; i < 3; ++i){
			static TCHAR s[1024] = { (L"npc\\0.png") };
			s[4] = i + '0';//%3
			surface.reLoad(s, 3, 4);
		}
		// 共15列每列3个 monsters00-monsters214 同一张图片的批量裂图在底层有优化 不会多次重复加载同一张图
		FOR(col, 0, 15){
			std::wstring alias(_T("monsters"));
			//每次循环加载一列的3个动画 每个动画2帧(y轴步进最多步进2帧)
			wsprintf(&alias[7], _T("%d%d"), 0, col);
			surface.reLoad(_T("monsters.png"), alias, 16, 6, 0, col, 1, 2, STEP_Y_AXIS);

			wsprintf(&alias[7], _T("%d%d"), 1, col);
			surface.reLoad(_T("monsters.png"), alias, 16, 6, 2, col, 1, 2, STEP_Y_AXIS);

			wsprintf(&alias[7], _T("%d%d"), 2, col);
			surface.reLoad(_T("monsters.png"), alias, 16, 6, 4, col, 1, 2, STEP_Y_AXIS);
		}
	}
	//返回对象识别号码
	int getID()const{
		return body->getOwnerID();
	}
	/*角色绘图*/
	virtual void shaw();
	
	/*显示 血蓝条,经验,等级*/
	virtual void shawBar(GP Rect const &rect);
	//行动:行为的组合 若该生物在此次行动过程中死亡 返回false
	virtual bool action() = 0;
	virtual bool die();
	Attribute const &getAttribute(){
		return attribute();
	}
	STATUS getStatus(){
		return status;
	}
	virtual int getObCnt(){
		return obCnt;
	}
	int getAtkRange();
	TYPE getType();

	//返回当前位置事件
	std::shared_ptr<CollisionBox> locationEvent(){
		return body;
	}
	//返回子画面
	Sprite const &getSprite()const{
		return *body;
	}
	//与目标发生碰撞 或没有目标 返回true
	bool arriveTarget(){
		return attribute().getTarget()->collide(getBody());
	}
	TargetLocation const &getTarget();
	//随机目标
	void randomTarget(){
		attribute().randomTarget();
	}
	void setTarget(Biology const &);
	void setTarget(TargetLocation const &);
	//置空目标-> 之后调用arriveTarget()保证返回true
	void setTarget(){
		attribute().setTarget(body);
	}
	/*返回与任意对象的的直线距离*/
	double dis2ToBiology(BioPointer);
	/*返回与目标的实际距离 未实现*/

	//初始化完成后 需要注册的一些事件 该方法会在所有生物都初始化完成后被依次调用
	virtual void registration(){
		//do something
	}
	void setBelongs(DisplayArea *belongs){
		this->belongs = belongs;
	}
protected:
	virtual void atk();
	void relive();
	/*物品id*/
	virtual void use(int);
	//virtual void escape();virtual void def();
	/*昏迷*/
	virtual int stun();
	//操作对象的行为
	virtual void operate() = 0;
	//计算偏位移 若计算后 有偏位移返回 true
	bool calcDirVector(){
		if (getBody().haveTravellingMotion()){
			//优先清算非目标偏位移: 键盘操作 其它移动效果
			setTarget();
			return true;
		}else{
			if (arriveTarget()){
				setTarget();
				return false;
			}
			else{//未到达目标
				getBody().updateD(*attribute().getTarget());//更新向目标的偏移矢量
				return true;
			}
		}
	}
	/*巡视*/
	void rounds();
	//行走
	void walk();
	//如果生物当前body被地图阻挡返回true
	virtual bool isMapBlock(){
		// !=0 表示有 阻力 即阻挡
		return belongs->block(getBody()) != 0;
	}
	/*碰撞检测 与 事件处理*/
	bool collisionDetect(){
		static LinkedList<std::shared_ptr<CollisionBox>> impactResults;
		belongs->impactDetection(locationEvent(), impactResults);
		bool obstruct = isMapBlock();//地图阻挡
		while (!impactResults.empty()){
			//进入takeEffect方法后 事件会根据用户自动判断是否生效, 产生什么效果以及是否阻挡 因此不能截断
			obstruct = impactResults.front()->takeEffect(attribute(), *locationEvent())
				|| obstruct;
			impactResults.pop_front();
		}
		return obstruct;
	}
	//绕行
	void bypass(){
		//若绕行超时就换方向
		if (attribute().bypassTime()){
			++bypassCount;
		}
		Offset newDir;
		Offset dir = getBody().getD();
		//每次绕行选择不同方向 选择方向后只要没有被阻挡那就一直是那个方向 只能选择相对与前进方向的左右方向绕行
		switch (dir.mainView()){
		case View::BACK: {
							 //原前进方向为上下方向占主要时 选择左右方向绕行
							 if (bypassCount % 2 == 0){
								 newDir.x = abs(dir.x) + 4;
							 }
							 else{
								 newDir.x = -(abs(dir.x) + 4);
							 }
							 newDir.y = -abs(dir.y + 4);
							 break;
		}case View::FRONT:{
							 //原前进方向为上下方向占主要时 选择左右方向绕行
							 if (bypassCount % 2 == 0){
								 newDir.x = abs(dir.x) + 4;
							 }
							 else{
								 newDir.x = -(abs(dir.x) + 4);
							 }
							 newDir.y = abs(dir.y) + 4;
							 break;
		}case View::LEFT: {
			if (bypassCount % 2 == 0){
				newDir.y = abs(dir.y) + 4;
			}
			else{
				newDir.y = -abs(dir.y + 4);
			}
			newDir.x = -(abs(dir.x) + 4);
			break;
		}case View::RIGHT:{
			if (bypassCount % 2 == 0){
				newDir.y = abs(dir.y) + 4;
			}
			else{
				newDir.y = -abs(dir.y + 4);
			}
			newDir.x = abs(dir.x) + 4;
			break;
		}default:
			break;
		}
		getBody().setD(newDir.x, newDir.y);
	}
	//移动 (obstruct: 阻塞 用于实现行走阻挡效果) block 障碍物 块 broadcast广播
	//ps::阻碍是未知的只能在子类 或 在外部实现
	virtual void move(){
		if (status == BLOCKING){
			status = BYPASS;
		}
		if (status == BYPASS){
			bypass();
		}
		//getBody().motion();//位置变化
		getBody().motionX();
		bool xCollision = collisionDetect();
		if (xCollision){
			//存在已知障碍物 不产生实际位移
			//getBody().revertMotion();
			getBody().revertMotionX();
			status = BLOCKING;
		}
		getBody().motionY();
		bool yCollision = collisionDetect();
		if (yCollision){
			getBody().revertMotionY();
			status = BLOCKING;
		}

		//若两个轴都已经恢复正常 或 绕行次数大于3次(已尝试了前方的2个斜向方向)
		if (!xCollision && !yCollision || bypassCount >= 2){
			//正常移动
			status = NORMAL;
			bypassCount = 0;
		}
		getBody().updateDir();
	}
	void hold(size_t id, int cnt);
	//计算导航队列(队列保证front是距离发起者最近的点)
	void calcNavigationQueue(LinkedList<GP Point> &navigationQueue){
		static SubTwain limitSub(belongs->getBody()->rowsLimit(), belongs->getBody()->colsLimit());
		size_t &targetId = shortestPathResult.targetId;
		auto startCentre = getBody().getCentre();
		SubTwain startSub = SubTwain(Constant::rowSub(startCentre.Y), Constant::colSub(startCentre.X));
		
		//从终点的path下标向上一直推到起点(假设终点是左上角 即编号为0的点)
		int parentId = shortestPathResult.path[targetId];
		if (parentId == -1){
			belongs->sendMessage().dialogue(_T("无法到达!"));
			setTarget();
			return;
		}
		Point2Base point2Base = limitSub.toPoint2Base(targetId);
		GP Point nowP, pastP = { point2Base.x*Constant::GRID_CELL.Width, point2Base.y*Constant::GRID_CELL.Height };
		navigationQueue.push_front(pastP);
		for (; parentId != shortestPathResult.startId; parentId = shortestPathResult.path[parentId]){
			point2Base = limitSub.toPoint2Base(parentId);
			nowP = { point2Base.x*Constant::GRID_CELL.Width, point2Base.y*Constant::GRID_CELL.Height };
			navigationQueue.push_front(nowP);
			//belongs->sendMessage().drawLine(nowP, pastP);
			pastP = nowP;
		}
	}
	void shawPath(){
		Sprite targetBody = getBody();
		targetBody.setCentre(navigationQueue.front());
		setTarget(targetBody);
		if (navigationQueue.empty()){
			return;
		}
		if (this->arriveTarget()){
			navigationQueue.pop_front();
		}
		if (navigationQueue.empty()){
			return;
		}
		GP Point nowP, pastP = navigationQueue.front();
		auto i = navigationQueue.begin();
		for (++i; i != navigationQueue.end(); ++i){
			nowP = *i;
			belongs->sendMessage().drawLine(nowP, pastP);
			pastP = nowP;
		}
	}
};

/*人物类*/
class People :public Biology{
public:
	People(DisplayArea* be, Sub figureSub = -1, TYPE type = PEOPLE)
		//size 不应由外界设定 应该是根据某些属性 在贴图的基础上作出调整 因此外界只能指定贴图
		: Biology(be, type, GP Size(19, 32)){
		//共5个非玩家贴图
		if (0 <= figureSub && figureSub <= 5){
			//Do Nothing
		}
		else{
			figureSub = (rand() % 5) + 1;;
		}
		//每个贴图4个视图 [1*4, 5*4]
		getBody().setOriginView(figureSub * 4);
		//需要知道贴图后才能完全复制
		drawBody.reset(new Body(getBody()));
		getBody().setPastView(View(rand() % ViewCount));//随机合法视图
	}
	//返回是否行动(若昏迷就不可行动)
	virtual bool action() override;
	virtual void operate() override;
	void registration() override {
		if (getType() != PLAYER){
			int v = attribute().getVisibility() / 2;
			patrolBox.reset(new PatrolEvent(getBody().getTimestamp(), v, attribute()));
			updatePatrol();
			//注册巡逻事件
			belongs->sendImpactEvent(patrolEvent());
		}
	}
private:
	std::shared_ptr<CollisionBox> patrolEvent() const {
		return patrolBox;
	}
	void updatePatrol(){
		patrolBox->setPatrolCircle(getBody().getCentre());
	}
	std::shared_ptr<PatrolEvent> patrolBox;
};

/*玩家类*/
class Player :public People{
	MODE opMode;//鼠标模式/寻径模式 的开关 连击效果并不容易触发:理论上触发后5s内按上次连击的任意键都会连击
	//地图编辑模式开关
	MODE editMode;
	Sub editId = 0;
	MODE menuMode;
	MOUSEMSG mouseCmd;
	bool haveMouseCmd = false;
public:
	//0号贴图是玩家
	Player(DisplayArea* be)
		: People(be, 0, PLAYER){
		editMode = OFF;
		menuMode = OFF;
		getBody().setPastView(FRONT);
		attribute().AGI = 5;
		/*初始化操作模式*/
		//if (WindowSprite(GetHWnd(), "鼠标移动操作模式?", "tips", MB_YESNO) == IDYES)opMode = ON, mousePosition = position;
		//else opMode = OFF;
		attribute().inertiaClock.SetDrtTime(0);
		drawBody->setCentre(GP Point(Constant::mainCanvasSize.Width / 2, Constant::mainCanvasSize.Height / 2));
	}
	/*角色绘图*/
	virtual void shaw(){
		RenderManager::cameraSprite.setCentre(getBody().getCentre());
		GP Point location = drawBody->getCentre();
		*drawBody = getBody();
		drawBody->setCentre(location);
		getSurface().draw(*drawBody);
		shawBar(drawBody->getRect());
	}
	virtual bool isMapBlock() override{
		return editMode == ON ? false : belongs->block(getBody()) != 0;
	}
	void setLocation(GP Point const &location){
		getBody().setLocation(location);
	}
	bool action()override;
	void operate()override;
	//菜单控件
	void menuStrip();
	void judgeDoubleHit(ArrayList<char> &cmd, ArrayList<char> &lastCmd);
	void opKeyboard();
	//若存在改变目标的操作返回true
	bool opMouse();
	void setMouseCmd(MOUSEMSG const &mouseCmd){
		haveMouseCmd = true;
		this->mouseCmd = mouseCmd;
	}
	bool isMouseHit(){
		return haveMouseCmd;
	}
	MOUSEMSG const &getMouseCmd(){
		haveMouseCmd = false;
		return this->mouseCmd;
	}
private:
};

/*怪物类*/
class Monster :public Biology{
public:
	Monster(DisplayArea* be, Sub figureRowSub = -1, Sub figureColSub = -1)
		:Biology(be, MONSTER, GP Size(40, 48)){
		//为了造成停顿效果 静止时播放上一帧
		getBody().setStaticFrame(PAST_FRAME);
		getBody().setPastView(BACK);
		//Monster的贴图只有一个面
		getBody().setmaxView(BACK);
		if (figureRowSub < 0 || figureRowSub > 2 || figureColSub < 0 || figureColSub > 14){
			getBody().setOriginView(getRandomSub());//0号贴图是玩家
		}
		else{
			getBody().setOriginView(getFigureSub(figureRowSub, figureColSub));//0号贴图是玩家
		}
		//需要知道贴图后才能完全复制
		drawBody.reset(new Body(getBody()));
	}
	bool action()override{
		/*击倒判断*/
		if (stun()){
			return false;
		}
		else{
			attribute().lvUp();
		}
		//嗑药
		if (attribute().getHP() < attribute().getFullHP() / 2){
			if (attribute().getBag().size() > 0){
				use(attribute().updateItemCount(0, -1));
			}
			else{
				//Run Away
			}
		}
		//巡逻碰撞盒
		//belongs->sendMessage().drawPolygon(patrolEvent()->toPolygon().getPoints());
		bool isActionTime = attribute().actionTime();
		if (isActionTime){
			operate();//减小Ai操作率
		}
		getBody().setDynamic(isActionTime);
		rounds();
		return true;
	}
private:
	Sub getFigureSub(Sub figureRowSub, Sub figureColSub){
		std::wstring alias(_T("monsters"));
		wsprintf(&alias[7], _T("%d%d"), figureRowSub, figureColSub);
		return Biology::getSurface().getOriginRowsSub(&alias[0]);
	}
	//获取一个随机贴图编号
	Sub getRandomSub(){
		std::wstring alias(_T("monsters"));
		wsprintf(&alias[7], _T("%d%d"), 0, rand() % 15);
		return Biology::getSurface().getOriginRowsSub(&alias[0]);
	}

	void operate() override {
		if (arriveTarget()){
			//若可以反应
			if (attribute().reactionTime()) {
				randomTarget();
			}
		}
		if (attribute().atkTime()){
			atk();
		}
	}
};

#endif
