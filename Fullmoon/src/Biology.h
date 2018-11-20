#ifndef __Biology_H
#define __Biology_H
//��̬����->����->��̬ϵͳ
#include "Ecologic.h"

/*�������: ���ּ��󶼱��������಻���ӻ��� ����һ���ӻ���*/
class Biology{
private:
	std::shared_ptr<Body> body;
	//ʹ������ָ���ԭ��: ���������� ���ڴ治���������� ���������������Ŀ����Ч�ж�(��һ��)��������(��ֹ�˶�������) ��ʹ������ָ�뷽���������target
	//ʱ���������: ��ʹ�������� ֻҪ��������д�ʱ��� ��ô�Ϳ����ж� ���ݵ���Ч�� �䷢���ĳ�λ������¼���Ӧ����Ч
	TYPE type;/*��������*/
	static AnimationManager surface;//���
protected:
	static int obCnt;//��������
	DisplayArea *belongs;//������ͼ
	ShortestPathResult shortestPathResult;
	//��������: ÿ�λ�pop��ǰ��ĵ���Ϊ��ǰĿ��� Ȼ����ƫ��
	LinkedList<GP Point> navigationQueue;
	//����Ļ���λ���뾫���ʵ��λ��������ͬ ����˵ϵͳ�Ὣʵ��λ�õĵ�ͼ��ȡ��������ڵĵط�������Ⱦ
	std::shared_ptr<Body> drawBody;
	int bypassCount = -1;
	STATUS status = NORMAL;
	//std::shared_ptr<Attribute>
	Attribute *attribute_;//������޸� �ⲿ�ɶ�ȡ
	int friendliness;//�Ѻö�
	int gem;/*��ʯ(����)*/
	//�����౩¶���޸ĵ�����(�൱��һ���ɼ̳е���)
	Attribute &attribute(){
		return *attribute_;
	}
	
	Body &getBody(){
		return *body;
	}
	/****��������ע����ͼ*****/
	static AnimationManager &getSurface(){
		return surface;
	}
public:
	//(��ͼ���� ������̬)
	Biology(DisplayArea *, TYPE, GP Size const &);
	virtual ~Biology(){
		body->invalidate();
		body->getTimestamp()->invalidate();
		delete attribute_;
		attribute_ = nullptr;
	}
	/*Biology& operator=(Biology const &){}//delete
	//����ת��(perfect forward):��ô��������ô��������
	//Biology& operator=(Biology &&rvalue);
	Biology(Biology const &){}// = delete;
	Biology(Biology &&rvalue);*/

	void alloc(){
		//attribute_.reset(new Attribute());
		//targetBuffer.reset(size);//��Ҫ�ǿ��
		//targetBuffer->setParent(belongs->getBody());
		attribute_ = new Attribute(new TargetLocation(getBody()));
	}
	/*����ĸ��� ����Ϊ��ֳ��Ϊ���� ��ʱ������*/
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
		// ��15��ÿ��3�� monsters00-monsters214 ͬһ��ͼƬ��������ͼ�ڵײ����Ż� �������ظ�����ͬһ��ͼ
		FOR(col, 0, 15){
			std::wstring alias(_T("monsters"));
			//ÿ��ѭ������һ�е�3������ ÿ������2֡(y�Ჽ����ಽ��2֡)
			wsprintf(&alias[7], _T("%d%d"), 0, col);
			surface.reLoad(_T("monsters.png"), alias, 16, 6, 0, col, 1, 2, STEP_Y_AXIS);

			wsprintf(&alias[7], _T("%d%d"), 1, col);
			surface.reLoad(_T("monsters.png"), alias, 16, 6, 2, col, 1, 2, STEP_Y_AXIS);

			wsprintf(&alias[7], _T("%d%d"), 2, col);
			surface.reLoad(_T("monsters.png"), alias, 16, 6, 4, col, 1, 2, STEP_Y_AXIS);
		}
	}
	//���ض���ʶ�����
	int getID()const{
		return body->getOwnerID();
	}
	/*��ɫ��ͼ*/
	virtual void shaw();
	
	/*��ʾ Ѫ����,����,�ȼ�*/
	virtual void shawBar(GP Rect const &rect);
	//�ж�:��Ϊ����� ���������ڴ˴��ж����������� ����false
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

	//���ص�ǰλ���¼�
	std::shared_ptr<CollisionBox> locationEvent(){
		return body;
	}
	//�����ӻ���
	Sprite const &getSprite()const{
		return *body;
	}
	//��Ŀ�귢����ײ ��û��Ŀ�� ����true
	bool arriveTarget(){
		return attribute().getTarget()->collide(getBody());
	}
	TargetLocation const &getTarget();
	//���Ŀ��
	void randomTarget(){
		attribute().randomTarget();
	}
	void setTarget(Biology const &);
	void setTarget(TargetLocation const &);
	//�ÿ�Ŀ��-> ֮�����arriveTarget()��֤����true
	void setTarget(){
		attribute().setTarget(body);
	}
	/*�������������ĵ�ֱ�߾���*/
	double dis2ToBiology(BioPointer);
	/*������Ŀ���ʵ�ʾ��� δʵ��*/

	//��ʼ����ɺ� ��Ҫע���һЩ�¼� �÷��������������ﶼ��ʼ����ɺ����ε���
	virtual void registration(){
		//do something
	}
	void setBelongs(DisplayArea *belongs){
		this->belongs = belongs;
	}
protected:
	virtual void atk();
	void relive();
	/*��Ʒid*/
	virtual void use(int);
	//virtual void escape();virtual void def();
	/*����*/
	virtual int stun();
	//�����������Ϊ
	virtual void operate() = 0;
	//����ƫλ�� ������� ��ƫλ�Ʒ��� true
	bool calcDirVector(){
		if (getBody().haveTravellingMotion()){
			//���������Ŀ��ƫλ��: ���̲��� �����ƶ�Ч��
			setTarget();
			return true;
		}else{
			if (arriveTarget()){
				setTarget();
				return false;
			}
			else{//δ����Ŀ��
				getBody().updateD(*attribute().getTarget());//������Ŀ���ƫ��ʸ��
				return true;
			}
		}
	}
	/*Ѳ��*/
	void rounds();
	//����
	void walk();
	//������ﵱǰbody����ͼ�赲����true
	virtual bool isMapBlock(){
		// !=0 ��ʾ�� ���� ���赲
		return belongs->block(getBody()) != 0;
	}
	/*��ײ��� �� �¼�����*/
	bool collisionDetect(){
		static LinkedList<std::shared_ptr<CollisionBox>> impactResults;
		belongs->impactDetection(locationEvent(), impactResults);
		bool obstruct = isMapBlock();//��ͼ�赲
		while (!impactResults.empty()){
			//����takeEffect������ �¼�������û��Զ��ж��Ƿ���Ч, ����ʲôЧ���Լ��Ƿ��赲 ��˲��ܽض�
			obstruct = impactResults.front()->takeEffect(attribute(), *locationEvent())
				|| obstruct;
			impactResults.pop_front();
		}
		return obstruct;
	}
	//����
	void bypass(){
		//�����г�ʱ�ͻ�����
		if (attribute().bypassTime()){
			++bypassCount;
		}
		Offset newDir;
		Offset dir = getBody().getD();
		//ÿ������ѡ��ͬ���� ѡ�����ֻҪû�б��赲�Ǿ�һֱ���Ǹ����� ֻ��ѡ�������ǰ����������ҷ�������
		switch (dir.mainView()){
		case View::BACK: {
							 //ԭǰ������Ϊ���·���ռ��Ҫʱ ѡ�����ҷ�������
							 if (bypassCount % 2 == 0){
								 newDir.x = abs(dir.x) + 4;
							 }
							 else{
								 newDir.x = -(abs(dir.x) + 4);
							 }
							 newDir.y = -abs(dir.y + 4);
							 break;
		}case View::FRONT:{
							 //ԭǰ������Ϊ���·���ռ��Ҫʱ ѡ�����ҷ�������
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
	//�ƶ� (obstruct: ���� ����ʵ�������赲Ч��) block �ϰ��� �� broadcast�㲥
	//ps::�谭��δ֪��ֻ�������� �� ���ⲿʵ��
	virtual void move(){
		if (status == BLOCKING){
			status = BYPASS;
		}
		if (status == BYPASS){
			bypass();
		}
		//getBody().motion();//λ�ñ仯
		getBody().motionX();
		bool xCollision = collisionDetect();
		if (xCollision){
			//������֪�ϰ��� ������ʵ��λ��
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

		//�������ᶼ�Ѿ��ָ����� �� ���д�������3��(�ѳ�����ǰ����2��б����)
		if (!xCollision && !yCollision || bypassCount >= 2){
			//�����ƶ�
			status = NORMAL;
			bypassCount = 0;
		}
		getBody().updateDir();
	}
	void hold(size_t id, int cnt);
	//���㵼������(���б�֤front�Ǿ��뷢��������ĵ�)
	void calcNavigationQueue(LinkedList<GP Point> &navigationQueue){
		static SubTwain limitSub(belongs->getBody()->rowsLimit(), belongs->getBody()->colsLimit());
		size_t &targetId = shortestPathResult.targetId;
		auto startCentre = getBody().getCentre();
		SubTwain startSub = SubTwain(Constant::rowSub(startCentre.Y), Constant::colSub(startCentre.X));
		
		//���յ��path�±�����һֱ�Ƶ����(�����յ������Ͻ� �����Ϊ0�ĵ�)
		int parentId = shortestPathResult.path[targetId];
		if (parentId == -1){
			belongs->sendMessage().dialogue(_T("�޷�����!"));
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

/*������*/
class People :public Biology{
public:
	People(DisplayArea* be, Sub figureSub = -1, TYPE type = PEOPLE)
		//size ��Ӧ������趨 Ӧ���Ǹ���ĳЩ���� ����ͼ�Ļ������������� ������ֻ��ָ����ͼ
		: Biology(be, type, GP Size(19, 32)){
		//��5���������ͼ
		if (0 <= figureSub && figureSub <= 5){
			//Do Nothing
		}
		else{
			figureSub = (rand() % 5) + 1;;
		}
		//ÿ����ͼ4����ͼ [1*4, 5*4]
		getBody().setOriginView(figureSub * 4);
		//��Ҫ֪����ͼ�������ȫ����
		drawBody.reset(new Body(getBody()));
		getBody().setPastView(View(rand() % ViewCount));//����Ϸ���ͼ
	}
	//�����Ƿ��ж�(�����ԾͲ����ж�)
	virtual bool action() override;
	virtual void operate() override;
	void registration() override {
		if (getType() != PLAYER){
			int v = attribute().getVisibility() / 2;
			patrolBox.reset(new PatrolEvent(getBody().getTimestamp(), v, attribute()));
			updatePatrol();
			//ע��Ѳ���¼�
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

/*�����*/
class Player :public People{
	MODE opMode;//���ģʽ/Ѱ��ģʽ �Ŀ��� ����Ч���������״���:�����ϴ�����5s�ڰ��ϴ��������������������
	//��ͼ�༭ģʽ����
	MODE editMode;
	Sub editId = 0;
	MODE menuMode;
	MOUSEMSG mouseCmd;
	bool haveMouseCmd = false;
public:
	//0����ͼ�����
	Player(DisplayArea* be)
		: People(be, 0, PLAYER){
		editMode = OFF;
		menuMode = OFF;
		getBody().setPastView(FRONT);
		attribute().AGI = 5;
		/*��ʼ������ģʽ*/
		//if (WindowSprite(GetHWnd(), "����ƶ�����ģʽ?", "tips", MB_YESNO) == IDYES)opMode = ON, mousePosition = position;
		//else opMode = OFF;
		attribute().inertiaClock.SetDrtTime(0);
		drawBody->setCentre(GP Point(Constant::mainCanvasSize.Width / 2, Constant::mainCanvasSize.Height / 2));
	}
	/*��ɫ��ͼ*/
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
	//�˵��ؼ�
	void menuStrip();
	void judgeDoubleHit(ArrayList<char> &cmd, ArrayList<char> &lastCmd);
	void opKeyboard();
	//�����ڸı�Ŀ��Ĳ�������true
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

/*������*/
class Monster :public Biology{
public:
	Monster(DisplayArea* be, Sub figureRowSub = -1, Sub figureColSub = -1)
		:Biology(be, MONSTER, GP Size(40, 48)){
		//Ϊ�����ͣ��Ч�� ��ֹʱ������һ֡
		getBody().setStaticFrame(PAST_FRAME);
		getBody().setPastView(BACK);
		//Monster����ͼֻ��һ����
		getBody().setmaxView(BACK);
		if (figureRowSub < 0 || figureRowSub > 2 || figureColSub < 0 || figureColSub > 14){
			getBody().setOriginView(getRandomSub());//0����ͼ�����
		}
		else{
			getBody().setOriginView(getFigureSub(figureRowSub, figureColSub));//0����ͼ�����
		}
		//��Ҫ֪����ͼ�������ȫ����
		drawBody.reset(new Body(getBody()));
	}
	bool action()override{
		/*�����ж�*/
		if (stun()){
			return false;
		}
		else{
			attribute().lvUp();
		}
		//�ҩ
		if (attribute().getHP() < attribute().getFullHP() / 2){
			if (attribute().getBag().size() > 0){
				use(attribute().updateItemCount(0, -1));
			}
			else{
				//Run Away
			}
		}
		//Ѳ����ײ��
		//belongs->sendMessage().drawPolygon(patrolEvent()->toPolygon().getPoints());
		bool isActionTime = attribute().actionTime();
		if (isActionTime){
			operate();//��СAi������
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
	//��ȡһ�������ͼ���
	Sub getRandomSub(){
		std::wstring alias(_T("monsters"));
		wsprintf(&alias[7], _T("%d%d"), 0, rand() % 15);
		return Biology::getSurface().getOriginRowsSub(&alias[0]);
	}

	void operate() override {
		if (arriveTarget()){
			//�����Է�Ӧ
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
