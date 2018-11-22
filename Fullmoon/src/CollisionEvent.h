#ifndef __Collision_H
#define __Collision_H
//自定义碰撞事件
#include"ImpactDetection.h"
//_T()
#include <tchar.h>

namespace EcologicEngine {

	/***************************************** 事件 *******************************************/
	//故事(Story)是按时间顺序讲述的事件.   情节(Plot)是对事件的陌生化(创造性变形): 事情发展的具体经过

	/*消息事件 {字段: 消息 + 展示框}*/
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
			//不对自己产生阻挡效果
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
		//文字消息
		WCHAR messages[1024];
		//Sprite Message::dialogueDistrict = Sprite(GP Rect(100, 100, 550, 30));
		WindowSprite dialogDistrict = WindowSprite(_T("Message dialog"));//消息 展示会话框
	};

	//传送区域
	class TransmissionEvent : public Message {
	public:
		//碰撞盒是该巡逻圈的外接矩形
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
				messagePlush(_T("触发了"));
				messagePlush(getOwnerID());
				messagePlush(_T("设下的传送阵"));
				if (triggererAttribute.getCurrentEcoregionsIndex() == nextEcoregionsIndex) {
					messagePlush(_T(", 但是这个传送阵的目的地就是当前区域."));
				}
				else {
					if (shouldReset) {
						transmissionTime.Init();
						shouldReset = false;
					}
					dialogDistrict.drawCircle(patrolCircle);
					messagePlush(_T(", 将在"));
					messagePlush((int)transmissionTime.GetRemainingTime() / 1000);
					messagePlush(_T("s后传送"));
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

		//用碰撞盒 设置巡逻圈
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
		//巡逻圈
		CircleShape patrolCircle;
	};

	//巡逻事件
	class PatrolEvent : public Message {
	public:
		//碰撞盒是该巡逻圈的外接矩形
		PatrolEvent(std::shared_ptr<Timestamp> const &timestamp, int radius, Attribute &ownerAttribute)
			:Message(timestamp, GP Size(2 * radius, 2 * radius))
			, ownerAttribute(ownerAttribute) {
			setRect(GP Rect(100, 100, 550, 30));
			patrolCircle.setRadius(radius);
		}

		virtual bool takeEffect(Attribute &triggererAttribute, BoundingBox const &triggererBox) override {
			if (getOwnerID() != triggererBox.getOwnerID() && triggererBox.toPolygon().collide(patrolCircle)) {
				//如果进入了RenderManager::cameraSprite那么将计算出绘制区域然后绘制 否则不绘制
				if (RenderManager::cameraSprite.collide(dialogDistrict)) {
					GP Point centreC = patrolCircle.getCentre();
					patrolCircle.setCentre(RenderManager::calcDisplayLocation(centreC));
					dialogDistrict.drawCircle(patrolCircle);
				}
				setMessage();
				messagePlush(triggererBox.getOwnerID());
				messagePlush(_T("进入了"));
				messagePlush(getOwnerID());
				messagePlush(_T("的巡逻范围"));
				ownerAttribute.setTarget(triggererBox.toSprite());
				dialogDistrict.dialogue(messages);
			}
			return false;
		}

		virtual CircleShape toCircle() const override {
			return getPatrolCircle();
		}

		//用碰撞盒 设置巡逻圈
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
		//巡逻圈
		CircleShape patrolCircle;
		Attribute &ownerAttribute;
		//触发回调 没法传入闭包lambda
		//void(*callBack) (Sprite const &target) = nullptr;
	};


	//四个视图的体型碰撞事件 为了方便调试 暂时继承Message
	class Body :public Message {
		/*virtual bool takeEffect(int id, Attribute &triggererAttribute)override{
		return obstructEvent();
		}*/
	public:
		RowsSub currentView() override {
			//通过前进方向判断 这次的视图 决定要播放的动画(位图组)
			if (dir_->x != 0 || dir_->y != 0) {/*初始化时的随机朝向*/
				lastView = abs(dir_->x) > abs(dir_->y) ?
					(dir_->x <= 0 ? LEFT : RIGHT) :/*withX*/
					(dir_->y <= 0 ? BACK : FRONT);/*withY*/
			}
			return originView + (lastView > maxView ? 0 : lastView);
		}

		//体型碰撞盒必须有时间戳(验证有效性) 大小(用于随机位置)
		Body(std::shared_ptr<Timestamp> const &timestamp, GP Size const &size,
			std::shared_ptr<Sprite const> const &parent)
			: Message(timestamp, size) {
			//体型默认具有阻挡效果
			this->obstruct = true;
			setParent(parent);
			randomLocation();
			durationClock.SetDrtTime(3000);
			setTriggerDistrict(std::shared_ptr<PolygonShape>(new PolygonShape()));
		}
		//持有此体型的实体只需要在需要返回体型的地方返回一个const就可以避免外界使用自己的时间戳了 而自己仍可以使用
		std::shared_ptr<Timestamp> &getTimestamp() {
			return timestamp;
		}
		virtual ~Body() {
		}
		void setmaxView(View maxView) {
			this->maxView = maxView;
		}
	private:
		//默认4个视图
		int maxView = ViewCount;
	};

	/*时效性战斗事件 实现伤害叠加是难点(同一个时间有多个伤害在同一位置:优先队列)*/
	class Fight : public Message {

	public:
		//战斗事件未设置父画面
		Fight(std::shared_ptr<Timestamp> const &timestamp, Sprite const &location, int value)
			: Message(timestamp, GP Size(location.getWidth(), location.getHeight())) {
			//伤害默认没有阻挡效果
			this->obstruct = false;
			this->value = value;
			durationClock.SetDrtTime(100);//0.1s
			setLocation(location);
			dialogDistrict.setSize(location.getWidth(), location.getHeight());
			dialogDistrict.setLocation(location);
		}
		virtual bool isValid() override {
			//发送对象存在 事件对象在发送后生成 在事件持续时间内
			return timestamp->isValid() && createClock >= timestamp->getSendClock()
				&& !durationClock.IsTimeOut();
		}
		//显示普通伤害效果
		void shawAtk() {
			//如果进入了RenderManager::cameraSprite那么将计算出绘制区域然后绘制 否则不绘制
			if (!RenderManager::cameraSprite.collide(dialogDistrict)) {
				return;
			}
			dialogDistrict.setLocation(RenderManager::calcDisplayLocation(*this));
			dialogDistrict.drawNumber(value);
		}
	private:
		//伤害不针对自己 恢复针对所有对象 当前不实现恢复效果
		virtual bool takeEffect(Attribute &triggererAttribute, BoundingBox const &triggererBox)override {
			if (triggererBox.getOwnerID() != getOwnerID()) {
				/*物理 法术 恢复  若是攻击行为则显示伤害 或是显示提示文字*/
				triggererAttribute.modifyHP(value);//@TODO 实现会心
				shawAtk();
				//生效后变失效
				invalidate();
				triggererAttribute.atkCountSub();
			}
			else {
				//事件持续时间截止后失效
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
		//行为影响值: <0表示Damage(伤害) 否则表示Recover(恢复)
		int value = 0;
	};

	/*时效性物品事件*/
	class Item : public Message {
		size_t ID = 0;
		virtual bool takeEffect(Attribute &triggererAttribute, BoundingBox const &triggererBox)override {
			triggererAttribute.addItem(ID, 1);
			invalidate();
			return obstructEvent();
		}
	public:
		//物品位置 物品编号
		Item(std::shared_ptr<Timestamp> &timestamp, Sprite const &location, size_t id)
			: Message(timestamp, GP Size(dialogDistrict.getWidthOfItem(id), dialogDistrict.getHeightOfItem(id))) {

			//物品默认没有阻挡效果
			this->obstruct = false;
			ID = id;
			durationClock.SetDrtTime(15000);//持续15s
			setLocation(location);
			dialogDistrict.setSize(dialogDistrict.getWidthOfItem(id), dialogDistrict.getHeightOfItem(id));
			dialogDistrict.setLocation(location);
		}
	public:
		//显示该物品
		void shawItem() {
			static GP Point location;
			getRect().GetLocation(&location);
			dialogDistrict.drawItem(ID, location);
		}
		//物品有效性检查
		virtual bool isValid()override {
			//发送对象存在 事件对象在发送后生成 在事件持续时间内
			return timestamp->isValid() && createClock >= timestamp->getSendClock()
				&& !durationClock.IsTimeOut();
		}
	};

}

#endif