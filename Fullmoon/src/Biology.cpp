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

		/*����֮ǰ���¼���ײ ���������ڷǷ��ĵ�ͼλ�� ����������λ������*/
		static LinkedList<std::shared_ptr<CollisionBox>> impactResults;
		belongs->impactDetection(locationEvent(), impactResults);
		while (!impactResults.empty() || this->isMapBlock()) {
			//���·���λ��
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
		//���������RenderManager::cameraSprite��ô���������������Ȼ����� ���򲻻���
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
		messager.downward(messager.getHeight() / 21);//΢��
		messager.speValBar(attribute().getMP(), attribute().getFullMP(), BLUE_);
	}
	//��������
	bool Biology::die() {
		bool died = attribute().getFullHP() <= 0;
		return died;
	}
	//����ֵ����
	int Biology::stun() {
		bool st = attribute().getHP() <= 0;
		if (st) {
			status = CONK;
			attribute().lifetimeReduce();
			hold(116, 1);//�̶�����
			Sprite itemDropLocation = getBody();
			itemDropLocation.setLocation(itemDropLocation.getCentre());
			belongs->loadingPackage(itemDropLocation, attribute().getBag());
			//belongs->sendImpactEvent();//����ֵ
		}
		return st;
	}
	/*����*/
	void Biology::relive() {
		attribute().modifyHP(attribute().getFullHP());
		status = NORMAL;
	}
	void Biology::use(int id) {
		int fullHP = attribute().getFullHP();
		int HP = attribute().getHP();
		switch (id) {
		case 0:HP -= 10; break;//FOR DUBUG
		case 115:gem += rand() % 50; break;/*���� ����Ӧʵ��Ϊ��ԭ���˵ı�ʯ��*/
		case 116:HP += 50; break;/*�˺���:������1/2���ʵ��� �̶���ԭ��ɫ��1/3HP*/
		case 121:HP += rand() % 50 - 25; break;/*Ģ��  ҲӦ�л��ʼ���������*/
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
		//��Ŀ��: ����ײ������ӵ�һ��Fight�¼� ���¼���λ���ǵ�ǰ��ɫ�ķ����ϵĽ����ŵ�һ����λ
		EventImpactManager *ac = belongs;
		static Sprite nextBody;
		nextBody = getBody();
		auto dir = nextBody.getDir();
		//������һ����λ
		nextBody.downward((dir.y == 0 ? 0 :
			(dir.y > 0 ? 1 : -1)) * nextBody.getHeight());
		nextBody.rightward((dir.x == 0 ? 0 :
			(dir.x > 0 ? 1 : -1)) * nextBody.getWidth());
		auto f = std::shared_ptr<CollisionBox>(new Fight(body->getTimestamp(), nextBody, -attribute().getATK()));
		//����ǰ����������
		ac->sendImpactEvent(f);
	}

	TYPE Biology::getType() {
		return type;
	}

	void Biology::hold(size_t id, int cnt) {
		//@TODO ʵ�ֱ����㷨������Ʒ
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
		//������״̬�Ž���op��λ��Ӱ��
		bool travellingMotion = calcDirVector();
		if (getType() != MONSTER) {
			getBody().setDynamic(travellingMotion);
		}
		//û��ƫλ�Ƶ�����±ض����ᷢ���ƶ�
		if (travellingMotion && this->attribute().moveTime()) {
			move();
		}
		else {//û�е����ƶ�ʱ��->���ƶ�->ֻ��Ҫ����¼�
			collisionDetect();
		}
	}
	void Biology::rounds() {
		if (status == STATUS::CONK)//����״̬vanishʹ����ʧ
			return;
		shaw();
		//�ƶ�+�ƶ������з������¼����� (����: λ��, ս��, ��ײ, ������)
		walk();
	}

	bool People::action() {
		/*�����ж�*/
		if (stun()) {
			return false;
		}
		else {
			attribute().lvUp();
		}
		//Ѳ����ײ��
		//belongs->sendMessage().drawPolygon(patrolEvent()->toPolygon().getPoints());
		if (attribute().actionTime()) {
			operate();//��СAi������
		}
		rounds();
		return true;
	}
	void People::operate() {
		//����Ѳ���¼�
		updatePatrol();
		//Ѳ��Ȧ
		//belongs->sendMessage().drawCircle(patrolEvent()->toCircle());
		if (arriveTarget()) {
			//�����Է�Ӧ
			if (attribute().reactionTime()) {
				randomTarget();
			}
		}
		if (attribute().atkTime()) {
			atk();
		}
	}

	bool Player::action() {
		//@TODO���Խ���һ���ᵽECOsystem Ȼ������������Ժϲ�?
		if (stun()) {
			//�㱻�����˽���x���ָ�
			if (belongs->sendMessage().massageBox(_T("You Are Stun\n"))) {
				//some code
			}
			relive();
			//������ʧ���� ���� <= 0Ϊ���� Ȼ������ɾ��
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
			//չʾ���������
			belongs->sendMessage().shawHit();
		}
		rounds();
		if (editMode == ON) {
			belongs->edit(getBody(), editId);
		}
		return true;
	}
	void Player::operate() {
		//�ı�λ��ʸ��
		opKeyboard();
		if (menuMode == ON) {
			Logger::writeLine(_T("��Բ˵�״̬�����˸���"));
			menuStrip();
			if (status == QUIT) {
				return;
			}
		}
		else {
			//�ı�Ŀ��
			if (opMouse()) {
				shortestPathResult.reset();
				navigationQueue.clear();
			}
		}
		/*
		����Ѱ·�Ĵ���˼·
			1 ֻҪ��ֱ�ӳ�Ŀ�귽��ǰ��һ������ǰ�������뿪����״̬
			2 ���򣬳��Դ����������ƹ��ϰ�����������״̬(Ѱ·ģʽ?)
			3 һ����������״̬�����ı����ѡ������з���
			4 �趨������ж��ٲ����������ж�Ѱ·ʧ�ܣ����Ի�����������
		*/
		if (navigationQueue.size() == 0 && this->arriveTarget()) {
			//����Ŀ�ĵ�->�������->�˳�Ѱ·ģʽ
			shortestPathResult.reset();
		}
		else {
			if (shortestPathResult.isCalced()) {
				//Ѱ·ģʽ
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
		//AGI;//����Ļ���λ��
		static int d = 8*2;
		//б���ƶ�ʱ1��λ�ı���ϵ�� ��Ҫ����б���ƶ���ͬ��x=y�ľ�����ô ԭx=y����Ϊ->coefficient*x=coefficient*y
		static const double coefficient = sqrt(2) / 2;
		int dx = getBody().getDx(), dy = getBody().getDy();//��λ��


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
		if (KEY_DOWN_DETECTION(VK_TAB) && !KEY_DOWN_DETECTION(VK_MENU)) {//VK_MENU (alt��)
			//tab�����ӳ�
			static TimeClock tabdelay(300);
			if (tabdelay.IsTimeOut()) {
				cmd.push_back(9);
			}
		}

		while (!cmd.empty()) {
			if (opMode == ON) {
				switch (cmd.back()) {
				case 27:_getch(); break;
				case 'a':dx = -d; break;//�����ָ��תȦ*
				case 'd':dx = d; break;//��ʱ��*
				case 'w':dy = 0, setTarget(belongs->sendMessage().getMouse()); break;//ǰ�����ָ��*
				case 's':dy = d; break;//Զ�����ָ��*
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
			case WM_MOUSEMOVE:// ����ƶ�
				if (opMode == ON || //��ֹһ��ʼ����Ѱ��ģʽ*
					(opMode == OFF && //*��ֹѰ��������õ�Ŀ��ĸı�(Ѱ����;�����ֶ��Ҽ�ȡ��Ŀ���ǲ��ᱻ��nullptr��)*
						getTarget().equals(getBody()))) {
					belongs->sendMessage().setMouse(C.x, C.y);
				}break;
			case WM_LBUTTONDOWN:// �������
				if (opMode == ON) {
					atk();
				}
				else {
					belongs->sendMessage().setMouse(C.x, C.y);
					setTarget(belongs->sendMessage().getMouse());
					return true;
				}
				break;
			case WM_MBUTTONDOWN://�м�����
				if (opMode == ON) {
					opMode = OFF;
					setTarget();//�л�����������ǰ�����ƶ�
				}
				else {
					opMode = ON;
					belongs->sendMessage().setMouse(C.x, C.y);
				}break;
			case WM_LBUTTONUP://�������
				break;
			case WM_RBUTTONDOWN://�Ҽ�����
				if (opMode == OFF)//ȡ��Ŀ��*
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
		//������
		static int width = Constant::mainCanvasSize.Width, height = Constant::mainCanvasSize.Height;
		//��������
		static GP Point centre(width / 2, height / 2);

		displayer.setSize(width / 2, height / 2);
		displayer.setLocation(0, 0);
		displayer.setCentre(centre);

		displayer.drawMenuBack();
		//��ť
		Sprite backButton = displayer.getRect();
		backButton.enlargement(0.14);
		backButton.downward(-backButton.getHeight());
		Sprite statusButton = backButton;
		Sprite saveButton = backButton;
		Sprite itemButton = backButton;
		Sprite quitButton = backButton;
		Sprite editButton = backButton;

		editButton.rightward((int)(-width * 0.05));
		displayer.drawButtonBack(editButton, _T("�༭"));

		quitButton.rightward((int)(width*0.05));
		displayer.drawButtonBack(quitButton, _T("�˳�"));

		statusButton.rightward((int)(width*0.15));
		displayer.drawButtonBack(statusButton, _T("״̬"));

		itemButton.rightward((int)(width * 0.25));
		displayer.drawButtonBack(itemButton, _T("��Ʒ"));

		saveButton.rightward((int)(width * 0.35));
		displayer.drawButtonBack(saveButton, _T("����"));

		backButton.rightward((int)(width * 0.45));
		displayer.drawButtonBack(backButton, _T("����"));

		if (statuShow) {
			//״̬չʾ
			int x, y;
			auto it = displayer.pointIterator(displayer.getWidth(), (int)(Constant::mainCanvasSize.Width*0.025));
			it.iterate(x, y);
			it.iterate(x, y);
			displayer.drawStatusBar(x, y, _T("����ֵ: "), attribute().getEXP(), 100);
			it.iterate(x, y);
			displayer.drawStatusBar(x, y, _T("����ֵ: "), attribute().getHP(), attribute().getFullHP());
			it.iterate(x, y);
			displayer.drawStatusBar(x, y, _T("����ֵ: "), attribute().getMP(), attribute().getFullMP());
			it.iterate(x, y);
			displayer.drawStatusBar(x, y, _T("����ֵ: "), attribute().getATK(), attribute().getATK());
			it.iterate(x, y);
			displayer.drawStatusBar(x, y, _T("����ֵ: "), attribute().getDEF(), attribute().getDEF());
			it.iterate(x, y);
			displayer.drawStatusBar(x, y, _T("���ܵ�: "), attribute().getSP(), attribute().getSP());
		}

		LinkedList<GP RectF> editImageLimitRectList;
		if (editMode == ON && !itemClick && !statuShow) {
			displayer.imageSelect(editImageLimitRectList, true);
		}

		ArrayList<Sprite> itemsBox;
		if (itemClick) {
			//��Ʒ��ˢ��
			displayer.inventoryTabs(attribute().getBag(), itemsBox);
		}

		if (isMouseHit()) {
			MOUSEMSG C = getMouseCmd();

			switch (C.uMsg) {
			case WM_MOUSEMOVE:// ����ƶ�
				break;
			case WM_LBUTTONDOWN: {// �������
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
					//@TODO: Ŀǰֻ�ܱ����ͼ
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
					//������رյ�ͼ�༭
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
			case WM_MBUTTONDOWN://�м�����
				return;
			case WM_LBUTTONUP://�������
				break;
			case WM_RBUTTONDOWN://�Ҽ�����
				attribute().addItem(0, 1);
				break;
			default:break;
			}
		}
	}

}
