#ifndef __Collision_H
#define __Collision_H
//�Զ�����ײ�¼�
#include"ImpactDetection.h"
//_T()
#include <tchar.h>

namespace EcologicEngine {

	/***************************************** �¼� *******************************************/
	//����(Story)�ǰ�ʱ��˳�������¼�.   ���(Plot)�Ƕ��¼���İ����(�����Ա���): ���鷢չ�ľ��徭��

	/*��Ϣ�¼� {�ֶ�: ��Ϣ + չʾ��}*/
	class Message : public CollisionBox {
	public:
		void collisionDemo() {
			CircleShape c = CircleShape({ 100, 100 }, 50);
			Sprite sp;
			sp.setSize(c.getRadius() * 2, c.getRadius() * 2);
			sp.setCentre(c.getCentre());
			PolygonShape p = PolygonShape(sp.getRect());
			/*p.addPoint(0, 0);
			p.addPoint(100, 0);
			p.addPoint(100, 100);
			p.addPoint(0, 100);*/
			static Ray ray = Ray(c.getCentre(), Direction(1, 1));
			dialogDistrict.drawRay(ray, 100);
			dialogDistrict.drawPolygon(p.getPoints());
			dialogDistrict.drawLine(c.getCentre(), { 110, 110 });
			dialogDistrict.drawCircle(c.getCentre(), c.getRadius());

			//dialogDistrict.drawPolygon(toPolygon().getPoints());
			dialogDistrict.drawCircle(toCircle());
			if (toPolygon().collide(p)) {
				dialogDistrict.text(_T("pp"), 10, RED_);
			}
			if (toPolygon().collide(c)) {
				dialogDistrict.text(_T("pc"), 10, GREEN_);
			}
			if (ray.collide(toPolygon())) {
				dialogDistrict.text(_T("rl"), 10, BLUE_);
			}
			if (ray.collide(toCircle())) {
				dialogDistrict.text(_T("rc"), 10, BLUE_);
			}
		}
		virtual bool takeEffect(Attribute &triggererAttribute, BoundingBox const &triggererBox)override {
			//collisionDemo();
			dialogDistrict.setRect(getRect());
			dialogDistrict.downward(32);
			dialogDistrict.text(messages, 10, BLACK_);
			//�����Լ������赲Ч��
			return triggererBox.getOwnerID() == getOwnerID() ? false : obstructEvent();
		}
		Message(std::shared_ptr<Timestamp> const &timestamp, GP Size const &size)
			: CollisionBox(timestamp, size) {
			memset(messages, 0, sizeof(WCHAR) * 1024);
		}
		virtual ~Message() {}

		void setMessage(WCHAR *message = _T("")) {
			wcscpy_s(this->messages, message);
		}
		void setMessage(int num) {
			wsprintf(messages, _T(" %d"), num);
		}
		void messagePlush(WCHAR const *message = _T("")) {
			wsprintf(this->messages + wcslen(messages), message);
		}
		void messagePlush(int num) {
			wsprintf(messages + wcslen(messages), _T("%d"), num);
		}
	protected:
		//������Ϣ
		WCHAR messages[1024];
		//Sprite Message::dialogueDistrict = Sprite(GP Rect(100, 100, 550, 30));
		WindowSprite dialogDistrict = WindowSprite(_T("Message dialog"));//��Ϣ չʾ�Ự��
	};

	//��������
	class TransmissionEvent : public Message {
	public:
		//��ײ���Ǹ�Ѳ��Ȧ����Ӿ���
		TransmissionEvent(std::shared_ptr<Timestamp> const &timestamp, int radius, size_t nextEcoregionsIndex)
			:Message(timestamp, GP Size(2 * radius, 2 * radius)) {
			patrolCircle.setRadius(radius);
			this->nextEcoregionsIndex = nextEcoregionsIndex;
			transmissionTime.SetDrtTime(10000);
		}

		virtual bool takeEffect(Attribute &triggererAttribute, BoundingBox const &triggererBox) override {
			if (getOwnerID() != triggererBox.getOwnerID() && triggererBox.toPolygon().collide(patrolCircle)) {
				setMessage();
				messagePlush(triggererBox.getOwnerID());
				messagePlush(_T("������"));
				messagePlush(getOwnerID());
				messagePlush(_T("���µĴ�����"));
				if (triggererAttribute.getCurrentEcoregionsIndex() == nextEcoregionsIndex) {
					messagePlush(_T(", ��������������Ŀ�ĵؾ��ǵ�ǰ����."));
				}
				else {
					if (shouldReset) {
						transmissionTime.Init();
						shouldReset = false;
					}
					dialogDistrict.drawCircle(patrolCircle);
					messagePlush(_T(", ����"));
					messagePlush((int)transmissionTime.GetRemainingTime() / 1000);
					messagePlush(_T("s����"));
					if (transmissionTime.IsTimeOut()) {
						triggererAttribute.setCurrentEcoregionsIndex(nextEcoregionsIndex);
						shouldReset = true;
					}
				}
				dialogDistrict.dialogue(messages);
			}
			return false;
		}

		virtual CircleShape toCircle() const override {
			return getCircle();
		}

		//����ײ�� ����Ѳ��Ȧ
		void setCircle(GP Point const &centre, int radius = 0) {
			if (radius != 0) {
				this->patrolCircle.setRadius(radius);
				this->setSize(radius, radius);
			}
			this->patrolCircle.setCentre(centre);
			this->setCentre(centre);
		}

		CircleShape const &getCircle() const {
			return patrolCircle;
		}
	private:
		bool shouldReset = true;
		TimeClock transmissionTime;
		size_t nextEcoregionsIndex;
		//Ѳ��Ȧ
		CircleShape patrolCircle;
	};

	//Ѳ���¼�
	class PatrolEvent : public Message {
	public:
		//��ײ���Ǹ�Ѳ��Ȧ����Ӿ���
		PatrolEvent(std::shared_ptr<Timestamp> const &timestamp, int radius, Attribute &ownerAttribute)
			:Message(timestamp, GP Size(2 * radius, 2 * radius))
			, ownerAttribute(ownerAttribute) {
			setRect(GP Rect(100, 100, 550, 30));
			patrolCircle.setRadius(radius);
		}

		virtual bool takeEffect(Attribute &triggererAttribute, BoundingBox const &triggererBox) override {
			if (getOwnerID() != triggererBox.getOwnerID() && triggererBox.toPolygon().collide(patrolCircle)) {
				//���������RenderManager::cameraSprite��ô���������������Ȼ����� ���򲻻���
				if (RenderManager::cameraSprite.collide(dialogDistrict)) {
					GP Point centreC = patrolCircle.getCentre();
					patrolCircle.setCentre(RenderManager::calcDisplayLocation(centreC));
					dialogDistrict.drawCircle(patrolCircle);
				}
				setMessage();
				messagePlush(triggererBox.getOwnerID());
				messagePlush(_T("������"));
				messagePlush(getOwnerID());
				messagePlush(_T("��Ѳ�߷�Χ"));
				ownerAttribute.setTarget(triggererBox.toSprite());
				dialogDistrict.dialogue(messages);
			}
			return false;
		}

		virtual CircleShape toCircle() const override {
			return getPatrolCircle();
		}

		//����ײ�� ����Ѳ��Ȧ
		void setPatrolCircle(GP Point const &centre, int radius = 0) {
			if (radius != 0) {
				this->patrolCircle.setRadius(radius);
				this->setSize(getRadius(), getRadius());
			}
			this->patrolCircle.setCentre(centre);
			this->setCentre(centre);
		}

		CircleShape const &getPatrolCircle() const {
			return patrolCircle;
		}
	private:
		//Ѳ��Ȧ
		CircleShape patrolCircle;
		Attribute &ownerAttribute;
		//�����ص� û������հ�lambda
		//void(*callBack) (Sprite const &target) = nullptr;
	};


	//�ĸ���ͼ��������ײ�¼� Ϊ�˷������ ��ʱ�̳�Message
	class Body :public Message {
		/*virtual bool takeEffect(int id, Attribute &triggererAttribute)override{
		return obstructEvent();
		}*/
	public:
		RowsSub currentView() override {
			//ͨ��ǰ�������ж� ��ε���ͼ ����Ҫ���ŵĶ���(λͼ��)
			if (dir_->x != 0 || dir_->y != 0) {/*��ʼ��ʱ���������*/
				lastView = abs(dir_->x) > abs(dir_->y) ?
					(dir_->x <= 0 ? LEFT : RIGHT) :/*withX*/
					(dir_->y <= 0 ? BACK : FRONT);/*withY*/
			}
			return originView + (lastView > maxView ? 0 : lastView);
		}

		//������ײ�б�����ʱ���(��֤��Ч��) ��С(�������λ��)
		Body(std::shared_ptr<Timestamp> const &timestamp, GP Size const &size,
			std::shared_ptr<Sprite const> const &parent)
			: Message(timestamp, size) {
			//����Ĭ�Ͼ����赲Ч��
			this->obstruct = true;
			setParent(parent);
			randomLocation();
			durationClock.SetDrtTime(3000);
			setTriggerDistrict(std::shared_ptr<PolygonShape>(new PolygonShape()));
		}
		//���д����͵�ʵ��ֻ��Ҫ����Ҫ�������͵ĵط�����һ��const�Ϳ��Ա������ʹ���Լ���ʱ����� ���Լ��Կ���ʹ��
		std::shared_ptr<Timestamp> &getTimestamp() {
			return timestamp;
		}
		virtual ~Body() {
		}
		void setmaxView(View maxView) {
			this->maxView = maxView;
		}
	private:
		//Ĭ��4����ͼ
		int maxView = ViewCount;
	};

	/*ʱЧ��ս���¼� ʵ���˺��������ѵ�(ͬһ��ʱ���ж���˺���ͬһλ��:���ȶ���)*/
	class Fight : public Message {

	public:
		//ս���¼�δ���ø�����
		Fight(std::shared_ptr<Timestamp> const &timestamp, Sprite const &location, int value)
			: Message(timestamp, GP Size(location.getWidth(), location.getHeight())) {
			//�˺�Ĭ��û���赲Ч��
			this->obstruct = false;
			this->value = value;
			durationClock.SetDrtTime(100);//0.1s
			setLocation(location);
			dialogDistrict.setSize(location.getWidth(), location.getHeight());
			dialogDistrict.setLocation(location);
		}
		virtual bool isValid() override {
			//���Ͷ������ �¼������ڷ��ͺ����� ���¼�����ʱ����
			return timestamp->isValid() && createClock >= timestamp->getSendClock()
				&& !durationClock.IsTimeOut();
		}
		//��ʾ��ͨ�˺�Ч��
		void shawAtk() {
			//���������RenderManager::cameraSprite��ô���������������Ȼ����� ���򲻻���
			if (!RenderManager::cameraSprite.collide(dialogDistrict)) {
				return;
			}
			dialogDistrict.setLocation(RenderManager::calcDisplayLocation(*this));
			dialogDistrict.drawNumber(value);
		}
	private:
		//�˺�������Լ� �ָ�������ж��� ��ǰ��ʵ�ָֻ�Ч��
		virtual bool takeEffect(Attribute &triggererAttribute, BoundingBox const &triggererBox)override {
			if (triggererBox.getOwnerID() != getOwnerID()) {
				/*���� ���� �ָ�  ���ǹ�����Ϊ����ʾ�˺� ������ʾ��ʾ����*/
				triggererAttribute.modifyHP(value);//@TODO ʵ�ֻ���
				shawAtk();
				//��Ч���ʧЧ
				invalidate();
				triggererAttribute.atkCountSub();
			}
			else {
				//�¼�����ʱ���ֹ��ʧЧ
				if (durationClock.IsTimeOut()) {
					invalidate();
					triggererAttribute.atkCountSub();
				}
				else {
					//Do Nothing 
				}
			}
			return obstructEvent();
		}
		//��ΪӰ��ֵ: <0��ʾDamage(�˺�) �����ʾRecover(�ָ�)
		int value = 0;
	};

	/*ʱЧ����Ʒ�¼�*/
	class Item : public Message {
		size_t ID = 0;
		virtual bool takeEffect(Attribute &triggererAttribute, BoundingBox const &triggererBox)override {
			triggererAttribute.addItem(ID, 1);
			invalidate();
			return obstructEvent();
		}
	public:
		//��Ʒλ�� ��Ʒ���
		Item(std::shared_ptr<Timestamp> &timestamp, Sprite const &location, size_t id)
			: Message(timestamp, GP Size(dialogDistrict.getWidthOfItem(id), dialogDistrict.getHeightOfItem(id))) {

			//��ƷĬ��û���赲Ч��
			this->obstruct = false;
			ID = id;
			durationClock.SetDrtTime(15000);//����15s
			setLocation(location);
			dialogDistrict.setSize(dialogDistrict.getWidthOfItem(id), dialogDistrict.getHeightOfItem(id));
			dialogDistrict.setLocation(location);
		}
	public:
		//��ʾ����Ʒ
		void shawItem() {
			static GP Point location;
			getRect().GetLocation(&location);
			dialogDistrict.drawItem(ID, location);
		}
		//��Ʒ��Ч�Լ��
		virtual bool isValid()override {
			//���Ͷ������ �¼������ڷ��ͺ����� ���¼�����ʱ����
			return timestamp->isValid() && createClock >= timestamp->getSendClock()
				&& !durationClock.IsTimeOut();
		}
	};

}

#endif