#include "Biology.h"
namespace EcologicEngine {

	AnimationManager Biology::surface;
	int Biology::obCnt = 0;
	
	Biology::Biology(EcologicEngine::Atlas* belongs, TYPE type, GP Size const &size)
		: belongs(belongs), type(type)
		, body(new Body(std::shared_ptr<Timestamp>(new Timestamp()), size, belongs->getMapArea())) {
		alloc();
		if (DEBUG) {
			body->setMessage(getID());
		}

		/*若和之前的事件碰撞 或者生成在非法的地图位置 则重新生成位置数据*/
		static LinkedList<std::shared_ptr<CollisionBox>> impactResults;
		belongs->impactDetection(locationEvent(), impactResults);
		while (!impactResults.empty() || this->isMapBlock()) {
			//重新分配位置
			body->randomLocation();
			belongs->impactDetection(locationEvent(), impactResults);
		}
		belongs->sendImpactEvent(locationEvent());
		setTarget();
		friendliness = 0;
		gem = 0;
		obCnt++;
	}

	void Biology::shaw() {
		//如果进入了RenderManager::cameraSprite那么将计算出绘制区域然后绘制 否则不绘制
		if (RenderManager::cameraArea.collide(getBody())) {
			*drawBody = getBody();
			drawBody->setLocation(RenderManager::calcDisplayLocation(getBody()));

			body->takeEffect(attribute(), *drawBody);//*locationEvent()
			surface.draw(*drawBody);
			shawBar(drawBody->getRect());
		}
	}

	void Biology::shawBar(GP Rect const &rect) {
		WindowSprite &messager = belongs->sendMessage();
		messager.setRect(rect);
		messager.speValBar(attribute().getHP(), attribute().getFullHP(), RED_);
		messager.downward(messager.getHeight() / 21);//微调
		messager.speValBar(attribute().getMP(), attribute().getFullMP(), BLUE_);
	}
	//寿命归零
	bool Biology::die() {
		bool died = attribute().getFullHP() <= 0;
		return died;
	}
	//生命值归零
	int Biology::stun() {
		bool st = attribute().getHP() <= 0;
		if (st) {
			status = CONK;
			attribute().lifetimeReduce();
			hold(116, 1);//固定掉落
			Sprite itemDropLocation = getBody();
			itemDropLocation.setLocation(itemDropLocation.getCentre());
			belongs->loadingPackage(itemDropLocation, attribute().getBag());
			//belongs->sendImpactEvent();//经验值
		}
		return st;
	}
	/*复活*/
	void Biology::relive() {
		attribute().modifyHP(attribute().getFullHP());
		status = NORMAL;
	}
	void Biology::use(int id) {
		int fullHP = attribute().getFullHP();
		int HP = attribute().getHP();
		switch (id) {
		case 0:HP -= 10; break;//FOR DUBUG
		case 115:gem += rand() % 50; break;/*包裹 最终应实现为加原主人的宝石数*/
		case 116:HP += 50; break;/*人红心:物死后1/2机率掉落 固定加原角色的1/3HP*/
		case 121:HP += rand() % 50 - 25; break;/*蘑菇  也应有机率加其它属性*/
		}
		if (HP > fullHP)
			HP = fullHP;
		attribute().modifyHP(HP);
		//if (MP > fullMP)MP = fullMP;
	}

	void Biology::atk() {
		if (attribute().getAtkCount() > 3) {
			return;
		}
		//总目标: 向碰撞树中添加的一个Fight事件 该事件的位置是当前角色的方向上的紧邻着的一个身位
		EventImpactManager *ac = belongs;
		static Sprite nextBody;
		nextBody = getBody();
		auto dir = nextBody.getDir();
		//计算下一个身位
		nextBody.downward((dir.y == 0 ? 0 :
			(dir.y > 0 ? 1 : -1)) * nextBody.getHeight());
		nextBody.rightward((dir.x == 0 ? 0 :
			(dir.x > 0 ? 1 : -1)) * nextBody.getWidth());
		auto f = std::shared_ptr<CollisionBox>(new Fight(body->getTimestamp(), nextBody, -attribute().getATK()));
		//向正前方发动攻击
		ac->sendImpactEvent(f);
	}

	TYPE Biology::getType() {
		return type;
	}

	void Biology::hold(size_t id, int cnt) {
		//@TODO 实现背包算法捡起物品
		attribute().addItem(id, cnt);
	}

	int Biology::getAtkRange() {
		//return AGI + 5;
		return attribute().getVisibility() - 5;
	}

	void Biology::setTarget(Biology const &target) {
		attribute().setTarget(target.body);
	}
	void Biology::setTarget(TargetLocation const &target) {
		attribute().setTarget(target);
	}
	TargetLocation const &Biology::getTarget() {
		return *attribute().getTarget();
	}
	double Biology::dis2ToBiology(BioPointer tarB) {
		return tarB != nullptr ? getBody().distance2(tarB->getBody()) : infinity;
	}
	void Biology::walk() {
		//非绕行状态才接收op的位移影响
		bool travellingMotion = calcDirVector();
		if (getType() != MONSTER) {
			getBody().setDynamic(travellingMotion);
		}
		//没有偏位移的情况下必定不会发生移动
		if (travellingMotion && this->attribute().moveTime()) {
			move();
		}
		else {//没有到达移动时间->不移动->只需要检测事件
			collisionDetect();
		}
	}
	void Biology::rounds() {
		if (status == STATUS::CONK)//昏迷状态vanish使其消失
			return;
		shaw();
		//移动+移动过程中发生的事件处理 (清算: 位移, 战斗, 碰撞, 掉落物)
		walk();
	}

	bool People::action() {
		/*击倒判断*/
		if (stun()) {
			return false;
		}
		else {
			attribute().lvUp();
		}
		//巡逻碰撞盒
		//belongs->sendMessage().drawPolygon(patrolEvent()->toPolygon().getPoints());
		if (attribute().actionTime()) {
			operate();//减小Ai操作率
		}
		rounds();
		return true;
	}
	void People::operate() {
		//更新巡逻事件
		updatePatrol();
		//巡逻圈
		//belongs->sendMessage().drawCircle(patrolEvent()->toCircle());
		if (arriveTarget()) {
			//若可以反应
			if (attribute().reactionTime()) {
				randomTarget();
			}
		}
		if (attribute().atkTime()) {
			atk();
		}
	}

	bool Player::action() {
		//@TODO可以将这一段提到ECOsystem 然后这个方法可以合并?
		if (stun()) {
			//你被击倒了将在x秒后恢复
			if (belongs->sendMessage().massageBox(_T("You Are Stun\n"))) {
				//some code
			}
			relive();
			//击倒损失寿命 寿命 <= 0为死亡 然后永久删除
			if (die()) {
				belongs->sendMessage().massageBox(_T("You Are Die\n"));
				return false;
			}
		}
		else {
			attribute().lvUp();
		}
		operate();
		if (status == QUIT) {
			return false;
		}
		if (!arriveTarget()) {
			//展示鼠标点击动画
			belongs->sendMessage().shawHit();
		}
		rounds();
		if (editMode == ON) {
			belongs->edit(getBody(), editId);
		}
		return true;
	}
	void Player::operate() {
		//改变位移矢量
		opKeyboard();
		if (menuMode == ON) {
			Logger::writeLine(_T("你对菜单状态进行了更改"));
			menuStrip();
			if (status == QUIT) {
				return;
			}
		}
		else {
			//改变目标
			if (opMouse()) {
				shortestPathResult.reset();
				navigationQueue.clear();
			}
		}
		/*
		单步寻路的大致思路
			1 只要能直接朝目标方向前进一步，就前进，并离开绕行状态
			2 否则，尝试从左右两边绕过障碍，进入绕行状态(寻路模式?)
			3 一旦进入绕行状态，不改变最初选择的绕行方向
			4 设定最多绕行多少步，超过则判定寻路失败，可以换用其它方法
		*/
		if (navigationQueue.size() == 0 && this->arriveTarget()) {
			//到达目的地->导航完毕->退出寻路模式
			shortestPathResult.reset();
		}
		else {
			if (shortestPathResult.isCalced()) {
				//寻路模式
				shawPath();
			}
			else {
				this->belongs->pickUpFollow(getBody(), getTarget(), this->shortestPathResult);
				calcNavigationQueue(navigationQueue);
			}
		}
	}
	void Player::opKeyboard() {
		static ArrayList<char> cmd;
		//AGI;//对象的基本位移
		static int d = 8*2;
		//斜向移动时1单位的比例系数 即要想在斜向移动等同于x=y的距离那么 原x=y将变为->coefficient*x=coefficient*y
		static const double coefficient = sqrt(2) / 2;
		int dx = getBody().getDx(), dy = getBody().getDy();//分位移


		if (KEY_DOWN_DETECTION(0x41)) {
			cmd.push_back('a');
		}
		if (KEY_DOWN_DETECTION(0x44)) {
			cmd.push_back('d');
		}
		if (KEY_DOWN_DETECTION(0x53)) {
			cmd.push_back('s');
		}
		if (KEY_DOWN_DETECTION(0x57)) {
			cmd.push_back('w');
		}
		if (KEY_DOWN_DETECTION(0x4A)) {
			cmd.push_back('j');
		}
		/*
		auto temp = (GetAsyncKeyState(VK_TAB));
		if (temp == -32767){
			cmd.push_back(9);
		}*/
		if (KEY_DOWN_DETECTION(VK_TAB) && !KEY_DOWN_DETECTION(VK_MENU)) {//VK_MENU (alt键)
			//tab按键延迟
			static TimeClock tabdelay(300);
			if (tabdelay.IsTimeOut()) {
				cmd.push_back(9);
			}
		}

		while (!cmd.empty()) {
			if (opMode == ON) {
				switch (cmd.back()) {
				case 27:_getch(); break;
				case 'a':dx = -d; break;//绕鼠标指针转圈*
				case 'd':dx = d; break;//逆时针*
				case 'w':dy = 0, setTarget(belongs->sendMessage().getMouse()); break;//前往鼠标指针*
				case 's':dy = d; break;//远离鼠标指针*
				default:break;
				}
			}
			else {
				switch (cmd.back()) {
				case 27:_getch(); break;
				case 'a':dx = -d; break;//LEFT*
				case 'd':dx = +d; break;//RIGHT*
				case 'w':dy = -d; break;//BACK*
				case 's':dy = +d; break;//FRONT*
				case 'j': {
					atk();
					break;
				}
				case 9: {
					menuMode = menuMode == ON ? OFF : ON;
					break;
				}
				default:break;
				}
			}
			cmd.pop_back();
		}
		if (dx != 0 && dy != 0) {
			dx *= coefficient;
			dy *= coefficient;
		}
		getBody().setD(dx, dy);
	}
	bool Player::opMouse() {
		if (isMouseHit()) {
			MOUSEMSG C = getMouseCmd();
			switch (C.uMsg) {
			case WM_MOUSEMOVE:// 鼠标移动
				if (opMode == ON || //防止一开始即是寻径模式*
					(opMode == OFF && //*防止寻径点击设置的目标的改变(寻径中途除非手动右键取消目标是不会被置nullptr的)*
						getTarget().equals(getBody()))) {
					belongs->sendMessage().setMouse(C.x, C.y);
				}break;
			case WM_LBUTTONDOWN:// 左键按下
				if (opMode == ON) {
					atk();
				}
				else {
					belongs->sendMessage().setMouse(C.x, C.y);
					setTarget(belongs->sendMessage().getMouse());
					return true;
				}
				break;
			case WM_MBUTTONDOWN://中键按下
				if (opMode == ON) {
					opMode = OFF;
					setTarget();//切换后在左键点击前不能移动
				}
				else {
					opMode = ON;
					belongs->sendMessage().setMouse(C.x, C.y);
				}break;
			case WM_LBUTTONUP://左键弹起
				break;
			case WM_RBUTTONDOWN://右键按下
				if (opMode == OFF)//取消目标*
					setTarget();
				break;
			default:break;
			}
		}
		return false;
	}

	void Player::menuStrip() {
		static WindowSprite displayer;
		static bool statuShow = false;
		static bool itemClick = false;
		//画面宽高
		static int width = Constant::mainCanvasSize.Width, height = Constant::mainCanvasSize.Height;
		//画面中心
		static GP Point centre(width / 2, height / 2);

		displayer.setSize(width / 2, height / 2);
		displayer.setLocation(0, 0);
		displayer.setCentre(centre);

		displayer.drawMenuBack();
		//按钮
		Sprite backButton = displayer.getRect();
		backButton.enlargement(0.14);
		backButton.downward(-backButton.getHeight());
		Sprite statusButton = backButton;
		Sprite saveButton = backButton;
		Sprite itemButton = backButton;
		Sprite quitButton = backButton;
		Sprite editButton = backButton;

		editButton.rightward((int)(-width * 0.05));
		displayer.drawButtonBack(editButton, _T("编辑"));

		quitButton.rightward((int)(width*0.05));
		displayer.drawButtonBack(quitButton, _T("退出"));

		statusButton.rightward((int)(width*0.15));
		displayer.drawButtonBack(statusButton, _T("状态"));

		itemButton.rightward((int)(width * 0.25));
		displayer.drawButtonBack(itemButton, _T("物品"));

		saveButton.rightward((int)(width * 0.35));
		displayer.drawButtonBack(saveButton, _T("保存"));

		backButton.rightward((int)(width * 0.45));
		displayer.drawButtonBack(backButton, _T("返回"));

		if (statuShow) {
			//状态展示
			int x, y;
			auto it = displayer.pointIterator(displayer.getWidth(), (int)(Constant::mainCanvasSize.Width*0.025));
			it.iterate(x, y);
			it.iterate(x, y);
			displayer.drawStatusBar(x, y, _T("经验值: "), attribute().getEXP(), 100);
			it.iterate(x, y);
			displayer.drawStatusBar(x, y, _T("健康值: "), attribute().getHP(), attribute().getFullHP());
			it.iterate(x, y);
			displayer.drawStatusBar(x, y, _T("气力值: "), attribute().getMP(), attribute().getFullMP());
			it.iterate(x, y);
			displayer.drawStatusBar(x, y, _T("攻击值: "), attribute().getATK(), attribute().getATK());
			it.iterate(x, y);
			displayer.drawStatusBar(x, y, _T("防御值: "), attribute().getDEF(), attribute().getDEF());
			it.iterate(x, y);
			displayer.drawStatusBar(x, y, _T("技能点: "), attribute().getSP(), attribute().getSP());
		}

		LinkedList<GP RectF> editImageLimitRectList;
		if (editMode == ON && !itemClick && !statuShow) {
			displayer.imageSelect(editImageLimitRectList, true);
		}

		ArrayList<Sprite> itemsBox;
		if (itemClick) {
			//物品栏刷新
			displayer.inventoryTabs(attribute().getBag(), itemsBox);
		}

		if (isMouseHit()) {
			MOUSEMSG C = getMouseCmd();

			switch (C.uMsg) {
			case WM_MOUSEMOVE:// 鼠标移动
				break;
			case WM_LBUTTONDOWN: {// 左键按下
				displayer.drawMenuBack();
				int clickId = -1;
				FOR_ALL_OBJECT(editImageLimitRectList) {
					++clickId;
					if (element.Contains(C.x, C.y)) {
						editId = clickId;
						break;
					}
				}

				FOR_ALL_OBJECT(itemsBox) {
					if (element.isClick(C.x, C.y)) {
						size_t id = attribute().updateItemCount(element.currentView(), -1);
						use(id);
					}
				}

				if (quitButton.isClick(C.x, C.y)) {
					status = QUIT;
					break;
				}
				if (backButton.isClick(C.x, C.y)) {
					menuMode = OFF;
					break;
				}
				else if (saveButton.isClick(C.x, C.y)) {
					//@TODO: 目前只能保存地图
					belongs->saveRenderData();
					break;
				}
				else if (statusButton.isClick(C.x, C.y)) {
					statuShow = true;
					itemClick = false;
				}
				else if (itemButton.isClick(C.x, C.y)) {
					itemClick = true;
					statuShow = false;
				}
				else if (editButton.isClick(C.x, C.y)) {
					//开启或关闭地图编辑
					if (editMode == ON) {
						editMode = OFF;
					}
					else if (editMode == OFF) {
						editMode = ON;
						itemClick = false;
						statuShow = false;
					}
					return;
				}
				else {
				}
				break;
			}
			case WM_MBUTTONDOWN://中键按下
				return;
			case WM_LBUTTONUP://左键弹起
				break;
			case WM_RBUTTONDOWN://右键按下
				attribute().addItem(0, 1);
				break;
			default:break;
			}
		}
	}

}
