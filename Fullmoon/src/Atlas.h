#ifndef __Atlas_H
#define __Atlas_H
//��ͼ��
#include"ImpactDetection.h"
//#include"E:\Projects\myLib\Graph.h"
//#include"E:\Projects\myLib\TreeObject.h"
//#include "E:\Projects\Source\Repos\archives-algorithm\DSAUtilityExtension\src\Graph\Graph.h"
//#include "E:\Projects\Source\Repos\archives-algorithm\DSAUtilityExtension\src\Graph\TreeObject.h"
#include"../myLib/Graph/Graph.h"
#include"../myLib/Graph/TreeObject.h"

/*��Ⱦ�� ����������������*/
class RenderManager{
protected:
	AvlTree<int> invalidIdTree;
	//���͵��б�
	ArrayList<shared_ptr<CollisionBox>> transmissionList;
	GP Size size_;
	CoordinatesMap renderData;/*��Ⱦ��ͼ������*/
	//������Ⱦ��ͼ(position)
	void reloadRender(int x, int y, int id){
		if (invalidIdTree.find(id)){
			renderData.setValidVertex(Point2(Constant::colSub(x), Constant::rowSub(y)), id);
		}
		else{
			renderData.setInvalidVertex(Point2(Constant::colSub(x), Constant::rowSub(y)), id);
		}
	}
	void reloadRender(Sprite::Iterator &pointIterator, int id){
		int x, y;
		while (pointIterator.iterate(x, y)){
			reloadRender(x, y, id);
		}
	}
public:
	RenderManager(Sprite const &mapBody)
		: size_(mapBody.colsLimit(), mapBody.rowsLimit())
		, renderData(size_.Height, size_.Width, DIR8){
		invalidIdTree.insert(3);
		//invalidIdTree.insert(2);
	}
	bool loadRender(FILE *fp, Sprite &entrance, Sprite &exit){
		if (!entrance.load(fp))
			return false;
		if (!exit.load(fp))
			return false;
		GP Size fileMapSize;
		if (fread(&fileMapSize, sizeof(GP Size), 1, fp) != 1)
			return false;
		int totalRows = fileMapSize.Height < size_.Height ? fileMapSize.Height : size_.Height;
		//�в�
		int rowSub = fileMapSize.Height - size_.Height;
		int totalCols = fileMapSize.Width < size_.Width ? fileMapSize.Width : size_.Width;
		int colSub = fileMapSize.Width - size_.Width;
		byte renderId;
		for (int iRow = 0; iRow < totalRows; ++iRow){
			for (int iCol = 0; iCol < totalCols; ++iCol){
				if (!fread(&renderId, sizeof(byte), 1, fp))
					return false;
				else{
					if (invalidIdTree.find(renderId)){//3������Ч�ĵؿ�
						renderData.setInvalidVertex(Point2(iCol, iRow), renderId);
					}
					else{
						renderData.setValidVertex(Point2(iCol, iRow), renderId);
					}
				}
			}
			//��Դ��ͼ����������Ҫ��ʾ�ĵ�ͼʱ ��ȡ�������
			FOR (c, 0, colSub){
				if (!fread(&renderId, sizeof(byte), 1, fp))
					return false;
			}
		}
		//��Դ��ͼ����������Ҫ��ʾ�ĵ�ͼʱ ��ȡ�������
		FOR(r, 0, rowSub){
			FOR(c, 0, fileMapSize.Width){
				if (!fread(&renderId, sizeof(byte), 1, fp))
					return false;
			}
		}
		return true;
	}
	bool loadRender_(FILE *fp, Sprite &entrance, Sprite &exit){
		if (!entrance.load(fp))
			return false;
		if (!exit.load(fp))
			return false;
		GP Size fileMapSize;
		if (fread(&fileMapSize, sizeof(GP Size), 1, fp) != 1)
			return false;
		int totalRows = fileMapSize.Height < size_.Height ? fileMapSize.Height : size_.Height;
		//�в�
		int rowSub = fileMapSize.Height - size_.Height;
		int totalCols = fileMapSize.Width < size_.Width ? fileMapSize.Width : size_.Width;
		int colSub = fileMapSize.Width - size_.Width;
		byte renderId;
		for (int iRow = 0; iRow < fileMapSize.Height; ++iRow){
			for (int iCol = 0; iCol < fileMapSize.Width; ++iCol){
				if (!fread(&renderId, sizeof(byte), 1, fp))
					return false;
				else{
					if (invalidIdTree.find(renderId)){//3������Ч�ĵؿ�
						renderData.setInvalidVertex(Point2(iCol, iRow), renderId);
					}
					else{
						renderData.setValidVertex(Point2(iCol, iRow), renderId);
					}
				}
			}
			//��Դ��ͼ����������Ҫ��ʾ�ĵ�ͼʱ ��ȡ�������
			FOR(c, 0, colSub){
				if (!fread(&renderId, sizeof(byte), 1, fp))
					return false;
			}
		}
		//��Դ��ͼ����������Ҫ��ʾ�ĵ�ͼʱ ��ȡ�������
		FOR(r, 0, rowSub){
			FOR(c, 0, fileMapSize.Width){
				if (!fread(&renderId, sizeof(byte), 1, fp))
					return false;
			}
		}
		return true;
	}
	bool saveRender(FILE *fp){
		FOR_ALL_OBJECT(transmissionList){
			if (!element->save(fp)){
				return false;
			}
		}
		if (!fwrite(&size_, sizeof(GP Size), 1, fp))
			return false;

		for (int iRow = 0; iRow < size_.Height; ++iRow){
			for (int iCol = 0; iCol < size_.Width; ++iCol){
				byte renderId = renderData.getVertexValue(iRow, iCol);
				if (!fwrite(&renderId, sizeof(byte), 1, fp))
					return false;
			}
		}
		return true;
	}

	static GP Point &calcDisplayLocation(GP Point &currentBodyLocation){
		GP Point currentCameraLocation = RenderManager::cameraSprite.getLocation();
		static GP Point currentDisplayLocation = RenderManager::displaySprite.getLocation();
		int xSub = currentBodyLocation.X - currentCameraLocation.X, ySub = currentBodyLocation.Y - currentCameraLocation.Y;
		currentBodyLocation.X = currentDisplayLocation.X + xSub;
		currentBodyLocation.Y = currentDisplayLocation.Y + ySub;
		return currentBodyLocation;
	}

	static GP Point const &calcDisplayLocation(Sprite const &currentBody){
		static GP Point currentBodyLocation;
		currentBodyLocation = currentBody.getLocation();
		calcDisplayLocation(currentBodyLocation);
		return currentBodyLocation;
	}

	//��ҵ���ʵ��Ұ
	static Sprite cameraSprite;
	//ʵ������Ļ����ʾ����
	static Sprite displaySprite;
};

//�¼���ײ������ (�Ĳ���ɸѡ->��Χ�еĳ�����ײ�ж�->takeEffect��׼�ж�)
class EventImpactManager{
	//drama�籾 Script�ű� �ѡ��ı�Ϊ�籾 Story=Message+time
private:
	//�����ߵĽ���������Χ �㲥��Ӱ�췶Χ
	//value�����ò���(ʵ������÷�������Ҳ����ֵ) ֻ����ָ�� (��ָ�����ֵ���ý�������ֵĴ���)
	//�����Body�¼�����
	/*Base::iterator;//������ = {}
	InsertValue = Base::value_type;//����ֵ = {key, value}
	Key = Base::key_type;//ʵ�ʼ�
	Message;//ʵ�ʵ�ֵ*/
	QuadTree impactTree;//��ײ��

	// STL�в����� const @see https://stackoverflow.com/questions/35764948/xmemory-errors-in-c-project-after-migrating-vs-2012-to-vs-2015
	//list<shared_ptr<CollisionBox> const> eventRegistry;//�¼�ע���priority_queue<EventParcel>
	list<shared_ptr<CollisionBox>> eventRegistry;//�¼�ע���priority_queue<EventParcel>
	TimeClock refreshTime;
	int detectionCount = 0;//̽�⿪ʼʱ���¼���
public:

	EventImpactManager(Sprite const &window)
		: impactTree(window){
		refreshTime.SetDrtTime(1000);
	}
	virtual ~EventImpactManager(){}

	//��ײ�¼����ܸ���
	int eventSize(){
		return impactTree.size();
	}

	//ˢ���¼������� �����¼�����
	int refresh(){
		impactTree.refresh();
		if (DEBUG){
			impactTree.forDebug();
		}
		return eventSize();
	}

	//����һ����Ҫ��ײ�����¼� ���ͳɹ�����true
	bool sendImpactEvent(shared_ptr<CollisionBox> &eventObj){
		return impactTree.insert(eventObj);
	}

	//��ײ��� (���������ײ��, ��ײ����б�, validɸѡ��)  Ԥ������ ��δʹ��
	template<class Fun>
	void impactDetection(shared_ptr<CollisionBox> const &key, 
		list<shared_ptr<CollisionBox>> &impacters, Fun customFiltrate){
		detectionCount = eventSize();
		impactTree.retrieve(key, impacters);
		//�Կ��ܵ���ײ�߽�����ײ����
		for (auto it = impacters.begin(); it != impacters.end();){
			if (customFiltrate() && key->collide(**it)){
				++it;
			}
			else{
				it = impacters.erase(it);
			}
		}
	}

	//��ײ��� (���������ײ��, ���е���ײ����б�)
	void impactDetection(shared_ptr<CollisionBox> const &key,
		list<shared_ptr<CollisionBox>> &impacteResults){
		detectionCount = eventSize();
		impactTree.retrieve(key, impacteResults);
		//�Կ��ܵ���ײ�߽�����ײ����
		for (auto it = impacteResults.begin(); it != impacteResults.end();){
			if (key->collide(**it)){
				++it;
			}
			else{
				it = impacteResults.erase(it);
			}
		}
	}
};

/*
�������ͼ�ӿ�
��Ϊÿ�ŵ�ͼ�ĵ�����Ҫ�������� ����û�кϲ���EventImpactManager ����Ϊʲô��ֱ�ӱ���������̬��
*/
class DropMap{/*������Ʒ(position, id)*/
	map<Sprite, int> dropItem;//��ΪҪ�����������
	struct Item{
		int id = 0;
		Sprite body;
		int clock = 0;
		bool operator<(Item const &rhs)const{
			return clock < rhs.clock;
		}
	};
	//AvlTree<Item> dropTree;
public:
	DropMap(){}
	DropMap(DropMap &&rvalue){
		for (auto it = rvalue.dropItem.begin(); it != rvalue.dropItem.end(); ++it){
			dropItem.insert(*std::move(it));
		}
	}
	virtual ~DropMap(){}
	/*�������е���Ʒװ�ص������ͼ (��ƷSprite)*/
	void loadingItem(Sprite body, Package &diededBag){
		diededBag.push_back(make_pair(116, 1));//�̶��������bug
		while (!diededBag.empty()){
			int id = diededBag.back().first;
			if (--diededBag.back().second <= 0)
				diededBag.pop_back();
			dropItem.insert(make_pair(body, id));
		}
	}

	/*�����㷨������Ʒ(δʵ��)*/
	/*̽�Ⲣ���ط��ط��鴦����Ʒ ��û����Ʒ����-1*/
	int detectionItem(Sprite const &body){
		//���Լ�����ķ�����ײ��body
		map<Sprite, int>::iterator it = dropItem.find(body);
		if (it != dropItem.end()){
			int re = it->second;
			dropItem.erase(it);
			return re;
		}
		else
			return -1;
	}
	/*չʾ���е�����*/
	int nextId(){//BUG
		int id = dropItem.size() == 0 ? -1 :
			dropItem.begin()->second;
		if (id != -1)
			dropItem.erase(dropItem.begin());
		return id;
	}
};

#endif