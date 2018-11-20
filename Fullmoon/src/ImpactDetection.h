//��ײ���
#ifndef __ImpactDetection_H
#define __ImpactDetection_H
#include "Animation.h"
#include "Attribute.h"

//ÿ������ײ���󶼱����ת��Ϊ��Χ�ж���
class BoundingBox{
protected:
	clock_t createClock = clock();//��������ʱ��
	//���Ͷ����Ӧһ��ʵ��, ֻ��ʵ����ܻ�ȡʱ���
	shared_ptr<Timestamp> timestamp;
	bool obstruct = false;//�赲Ч�� ��ײ��һ���赲(Ϊʲô������Body��ImpacttShell������)
	TimeClock durationClock;/*����ʱ�� ������ʱ�����Ч*/
	/*
	LARGE_INTEGER litmp;		// ����ýṹ���һ������
	LONGLONG QPart1, QPart2;	// ���ڻ�ȡ�ṹ������� QuadPart ��Ա
	gameRefresh.SetDrtTime(30);
	QueryPerformanceCounter(&litmp);
	QPart2 = litmp.QuadPart;//���µ�ǰʱ��
	*/

	//�����赲Ч�����¼�����true
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
	//ʹ��һ����ײ�ж�����Ч(���ٲ�����ײ���)
	virtual void invalidate(){
		createClock = -createClock;
	}
	virtual bool isValid() {
		//���Ͷ������ �¼������ڷ��ͺ�����
		return timestamp->isValid() && createClock >= timestamp->getSendClock();
	}

	//��Ч: �����¼������߽����¼���Ч�� �����赲����true(�������԰�����Ч������)
	virtual bool takeEffect(Attribute &originBioAttribute, BoundingBox const &triggererBox){
		return obstruct;
	}

	//ÿ��һ����ײ�б����ڰ�Χ�й�����(����)�� ע����Ӧ�İ�Χ��ת������ ��������
	virtual PolygonShape toPolygon() const = 0;

	virtual CircleShape toCircle() const = 0;

	virtual Sprite const &toSprite() const = 0;
};

//������ײ��: �ܽ�����ײ�ж����ӻ���
class CollisionBox : public Sprite, public BoundingBox{
public:
	using CollisionBoxList = LinkedList<shared_ptr<CollisionBox>>;

	CollisionBox(shared_ptr<Timestamp> const &timestamp, int h = 0, int w = 0)
		:BoundingBox(timestamp){
		this->setSize(w, h);
	}
	//���ӵ�λ�ÿ��Դ��� ����С����֪��
	CollisionBox(shared_ptr<Timestamp> const &timestamp, GP Size const &size)
		:BoundingBox(timestamp), Sprite(size){
	}

	// ���ո����ľ��������ķ��� �и��ײ�� �����и��İ�Χ����
	void carve(GP Point &centre, CollisionBoxList &results) const {
		GP Rect rect;//����static
		rect = this->getRect();
		CollisionBoxList temp;
		int	dX = centre.X - rect.X;
		int	dY = centre.Y - rect.Y;
		bool carveY = dX > 0 && dX < rect.Width;//����
		bool carveX = dY > 0 && dY < rect.Height;//����transect

		// �и�XY����
		if (carveX && carveY) {
			//xy�и�һ�κ󲻻��ٵݹ��xy�и� ��ֻ��xy�и��Ӱ��centre��ֵ ��centre����������
			int cyTemp = centre.Y;
			centre.Y = rect.Y;
			carve(centre, temp);//�ֽ�ԭ����������Ϊ������
			centre.Y = cyTemp;
			centre.X = rect.X;
			while (!temp.empty()) {//�����ν������ֺ����и�
				temp.back()->carve(centre, results);
				temp.pop_back();
			}
		}// ֻ�и�X����
		else if (carveX) {
			results.push_back(newNode(timestamp, rect.X, rect.Y, rect.Width, dY));
			results.push_back(newNode(timestamp, rect.X, centre.Y, rect.Width, rect.Height - dY));
		}// ֻ�и�Y����
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
	//ʵ����ײ��������
	shared_ptr<PolygonShape> triggerDistrict;
};

//��Χ����
using CollisionBoxList = CollisionBox::CollisionBoxList;

//������ײ�й������Ĳ������
class QuadTreeNode{
public:
	//������ö�� (��֤�������޵�intֵ����ֱ����Ϊ��������)
	enum QuadrantEnum{
		//δ����
		UNDEFINED = -3, 
		//������(����ȫ���ڴ�����)
		FATHER = -2, 
		//�����޽��紦
		JUNCTION = -1, 
		ONE = 0, 
		TWO = 1, 
		THREE = 2, 
		FOUR = 3 
	};
	using NodesArray = vector<shared_ptr<QuadTreeNode>>;//�����Ҫ֧�ָ�ֵ�Ϳ��������Ļ� ������shared_ptr<>����ָ��
	
	//newһ��CollisionBox�˶�������
	QuadTreeNode(Sprite *&&bounds, QuadTreeNode *parent = nullptr, int level = 0)
		: bounds(bounds), parent(parent), level(level){
		if (level < MAX_LEVELS){//���ȷ���
			split();
		}
	}
	~QuadTreeNode(){}

	//���ش˽�������в�����ײ����ײ����
	int counter(){
		return count;
	}

	//����: ʹ��������������
	void split() {
		GP Point centre;
		static GP Rect rect;
		bounds->getCentre(centre);
		rect = bounds->getRect();
		auto x = rect.X;
		auto y = rect.Y;
		//�������ָ�ʱ��1 ������ֱ�Ե����ʱ�����жϵĴ���
		auto sWidth = rect.Width % 2 == 0 ? rect.Width / 2 : rect.Width / 2 + 1;//�����ķֺ�Ŀ��
		auto sHeight = rect.Height % 2 == 0 ? rect.Height / 2 : rect.Height / 2 + 1;
		/*auto sWidth = rect.Width / 2;//�����ķֺ�Ŀ��
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
			_DEBUG_ERROR("�Ĳ�������ײ�� ��̬�����ֶ� �� ʵ�ʼ������� ����!");
		}
		return sizeCounter;
	}

	shared_ptr<QuadTreeNode> newNode(QuadTreeNode *parent, int x, int y, int w, int h)const{
		return shared_ptr<QuadTreeNode>(new QuadTreeNode(new Sprite(GP Rect(x, y, w, h)), parent, level + 1));
	}

	//�Ǹ���㷵��true
	bool isRoot()const{
		return parent == nullptr;
	}

	//�ڲ�����
	void retrieveIn(shared_ptr<CollisionBox> const &collisionBox,
		CollisionBoxList &results)const{

		if (count == 0){
			return;//��ǰ���޵��ܽ����Ϊ0
		}
		else{
			//���в�ѯ
		}

		//append���;����������Ч��ײ��;(��������ˢ����һ���Կ��ܵ�����ײ���д�����Ч����ײ��) assign���丳ֵ
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

		//��ѯ������
		if (quadrants.empty()){
			//û�������� ֱ�ӷ���
		}
		else{
			QuadrantEnum index = getQuadrantIndex(collisionBox);
			if (index == QuadrantEnum::FATHER){
				_DEBUG_ERROR("����: ����;�в����ܳ��ָ��������ײ��");
			}
			else if (index == JUNCTION){
				static GP Point centre;
				CollisionBoxList carveBoxes;//���ѵĺ���
				collisionBox->carve(bounds->getCentre(centre), carveBoxes);

				for (auto it = carveBoxes.begin(); it != carveBoxes.end(); ++it){
					index = getQuadrantIndex(*it, false);//���ѵĺ���С������������ ���ü��
					quadrants[index]->retrieveIn(*it, results);
				}
			}
			else{
				quadrants[index]->retrieveIn(collisionBox, results);
			}
		}
	}

	//�ڲ�ˢ��
	void refreshIn(CollisionBoxList &overflowList){
		//������ײ����
		for (auto it = objects.begin(); it != objects.end();){
			auto collisionBox = *it;

			if (collisionBox->isValid()){
				QuadrantEnum index = getQuadrantIndex(collisionBox, true);
				if (index == JUNCTION){//���紦 ���䶯
					++it;
				}
				else{
					//ɾ�������²���
					eraseBoundingBox(it);
					if (isRoot()){
						overflowList.push_back(collisionBox);
					}
					else{
						insert(collisionBox, parent, index);
					}
				}
			}
			else {//�����Ч���
				auto p = parent;
				//��������ļ�������һ
				while (p != nullptr){
					--p->count;
					p = p->parent;
				}
				eraseBoundingBox(it);
			}
		}

		// �ݹ�ˢ��������
		for (auto it = quadrants.begin(); it != quadrants.end(); ++it) {
			if ((*it)->count != 0){
				(*it)->refreshIn(overflowList);
			}
		}
	}

	//������ײ�е�����������
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
				return FATHER;//��������޽���
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
				//λ�ڽ��紦
				return JUNCTION;
			}
		}

	//ɾ�������޵�it��� it�Զ�����
	void eraseBoundingBox(CollisionBoxList::iterator &it){
			--count;
			it = objects.erase(it);
		}

	//�ڲ��ݹ����ӿ� (parent��nullptr ��֪indexʱ���Բ��ظ�����)
	bool insert(shared_ptr<CollisionBox> const &collisionBox,
		QuadTreeNode *parent, QuadrantEnum index = UNDEFINED){
			static bool fromSub = false;
			if (fromSub){
				//�����ӽ��Ĳ����п��ܻ�������׷�� ������
			}
			else{
				//++count;
			}

			if (quadrants.empty()){
				_DEBUG_ERROR("������ײ�й�С ��ǰ�Ĳ����޷����� ��������������");
				return false;
			}
			else{
				index = index == UNDEFINED ? this->getQuadrantIndex(collisionBox) : index;
				if (index == QuadrantEnum::JUNCTION){
					if (fromSub){//׷�ݽ��� ȷ�����
						++count;
						fromSub = false;
					}
					else{
						++count;
						//�������
					}
					//λ�ڴ�����
					objects.push_back(collisionBox);
				}
				else if (index == QuadrantEnum::FATHER){
					if (isRoot()){
						//++count;
						//���ܽ��������ײ�� ����Root����: �μ�insert���ⲿ�ӿڴ��Ľ���
						//objects.push_back(collisionBox);
						return false;
					}
					else{
						//�򸸻������(����ˢ��)
						fromSub = true;
						--parent->count;//����һ���������Ѿ�ɾ���ĸý��
						parent->insert(collisionBox, parent->parent);
					}
				}
				else{
					//λ��������
					++count;
					return quadrants[index]->insert(collisionBox, this);
				}
			}
			return true;
		}
	
private:
	//min 9, ƽ��15��, max 21
	static const int MAX_OBJECTS = 50;//��Ҫ�ǵ��ķָ��ߴ��ֲ��˹�����ײ�� (��������޲���)
	static const int MAX_LEVELS = 7;
	int count = 0;//�ӽ����
	CollisionBoxList objects;//�洢��ײ��
	NodesArray quadrants;//�洢�ĸ������޽ڵ�
	int level;//�ýڵ����ȣ����ڵ��Ĭ�����Ϊ0
	QuadTreeNode *parent = nullptr;//�ձ�ʾroot
	shared_ptr<Sprite> bounds;//����: �ýڵ��Ӧ����������Ļ�ϵķ�Χ��bounds��һ������
};

//�Ĳ���(�������� ƤӰϷ)
class QuadTree{
public:
	QuadTree(Sprite const &bounds)
		:root(new Sprite(bounds)){
	}
	//����һ����ײ��
	bool insert(shared_ptr<CollisionBox> const &collisionBox){
		//��������������(ͬʱ�ڳ��ֿ��ܵ��������ʱ������ Ҳ������һЩbug���Է���) 
		//�������ֱ�Ӳ���Root���޵Ļ�: ��ײ��ԭ����Root���޵ĸ����� ����������Root��ĳ���������ٷ���ʱRoot��"������"ʱ������: ��Rootɾ��->����"������"->���� ������ˢ��
		if (!root.insert(collisionBox, nullptr)){
			overflowList.push_back(collisionBox);
			return true;
		}
		if (overflowList.size() > 60-(size_t)(Constant::mainCanvasSize.Width*0.015)){
			//ǿ�б��� �����������ײ���������� ��ʱ���п����ǳ�����bug
			_DEBUG_ERROR("�����˹�������Root������ӻ��� �����Ƿ����BUG ���߽����Ż�");
		}
		return true;
	}
	//�������в�����ײ����ײ����
	int size(){
		return root.counter() + overflowList.size();
	}

	//����: ���ؿ�����ײ����С����
	void retrieve(shared_ptr<CollisionBox> const &collisionBox, CollisionBoxList &results) const {
		root.retrieveIn(collisionBox, results);
		//results.splice(results.end(), CollisionBoxList(overflowList)}); //concat: �ϲ���������(��������)
		FOR (it, overflowList.begin(), overflowList.end()){
			if ((*it)->isValid()){
				results.push_back(*it);
			}
		}
	}

	//��̬ˢ��
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