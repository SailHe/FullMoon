#ifndef __AtlasUtil_H
#define __AtlasUtil_H

#include"ImpactDetection.h"
#include"../myLib/Graph/Graph.h"
#include"../myLib/Graph/TreeObject.h"

namespace EcologicEngine {

	/*渲染类 不建议做拷贝操作*/
	class RenderManager {
	protected:
		AvlTree<int> invalidIdTree;
		//传送点列表
		ArrayList<std::shared_ptr<CollisionBox>> transmissionList;
		GP Size size_;
		CoordinatesMap renderData;/*渲染地图主数据*/
		//重载渲染地图(position)
		void reloadRender(int x, int y, int id) {
			if (invalidIdTree.find(id)) {
				renderData.setValidVertex(Point2(Constant::colSub(x), Constant::rowSub(y)), id);
			}
			else {
				renderData.setInvalidVertex(Point2(Constant::colSub(x), Constant::rowSub(y)), id);
			}
		}
		void reloadRender(Sprite::Iterator &pointIterator, int id) {
			int x, y;
			while (pointIterator.iterate(x, y)) {
				reloadRender(x, y, id);
			}
		}
	public:
		RenderManager(Sprite const &mapBody)
			: size_(mapBody.colsLimit(), mapBody.rowsLimit())
			, renderData(size_.Height, size_.Width, DIR8) {
			invalidIdTree.insert(3);
			//invalidIdTree.insert(2);
		}
		bool loadRender(FILE *fp, Sprite &entrance, Sprite &exit) {
			if (!entrance.load(fp))
				return false;
			if (!exit.load(fp))
				return false;
			GP Size fileMapSize;
			if (fread(&fileMapSize, sizeof(GP Size), 1, fp) != 1)
				return false;
			int totalRows = fileMapSize.Height < size_.Height ? fileMapSize.Height : size_.Height;
			//行差
			int rowSub = fileMapSize.Height - size_.Height;
			int totalCols = fileMapSize.Width < size_.Width ? fileMapSize.Width : size_.Width;
			int colSub = fileMapSize.Width - size_.Width;
			byte renderId;
			for (int iRow = 0; iRow < totalRows; ++iRow) {
				for (int iCol = 0; iCol < totalCols; ++iCol) {
					if (!fread(&renderId, sizeof(byte), 1, fp))
						return false;
					else {
						if (invalidIdTree.find(renderId)) {//3代表无效的地块
							renderData.setInvalidVertex(Point2(iCol, iRow), renderId);
						}
						else {
							renderData.setValidVertex(Point2(iCol, iRow), renderId);
						}
					}
				}
				//当源地图大于现在需要显示的地图时 读取多余的列
				FOR(c, 0, colSub) {
					if (!fread(&renderId, sizeof(byte), 1, fp))
						return false;
				}
			}
			//当源地图大于现在需要显示的地图时 读取多余的行
			FOR(r, 0, rowSub) {
				FOR(c, 0, fileMapSize.Width) {
					if (!fread(&renderId, sizeof(byte), 1, fp))
						return false;
				}
			}
			return true;
		}
		bool loadRender_(FILE *fp, Sprite &entrance, Sprite &exit) {
			if (!entrance.load(fp))
				return false;
			if (!exit.load(fp))
				return false;
			GP Size fileMapSize;
			if (fread(&fileMapSize, sizeof(GP Size), 1, fp) != 1)
				return false;
			int totalRows = fileMapSize.Height < size_.Height ? fileMapSize.Height : size_.Height;
			//行差
			int rowSub = fileMapSize.Height - size_.Height;
			int totalCols = fileMapSize.Width < size_.Width ? fileMapSize.Width : size_.Width;
			int colSub = fileMapSize.Width - size_.Width;
			byte renderId;
			for (int iRow = 0; iRow < fileMapSize.Height; ++iRow) {
				for (int iCol = 0; iCol < fileMapSize.Width; ++iCol) {
					if (!fread(&renderId, sizeof(byte), 1, fp))
						return false;
					else {
						if (invalidIdTree.find(renderId)) {//3代表无效的地块
							renderData.setInvalidVertex(Point2(iCol, iRow), renderId);
						}
						else {
							renderData.setValidVertex(Point2(iCol, iRow), renderId);
						}
					}
				}
				//当源地图大于现在需要显示的地图时 读取多余的列
				FOR(c, 0, colSub) {
					if (!fread(&renderId, sizeof(byte), 1, fp))
						return false;
				}
			}
			//当源地图大于现在需要显示的地图时 读取多余的行
			FOR(r, 0, rowSub) {
				FOR(c, 0, fileMapSize.Width) {
					if (!fread(&renderId, sizeof(byte), 1, fp))
						return false;
				}
			}
			return true;
		}
		bool saveRender(FILE *fp) {
			FOR_ALL_OBJECT(transmissionList) {
				if (!element->save(fp)) {
					return false;
				}
			}
			if (!fwrite(&size_, sizeof(GP Size), 1, fp))
				return false;

			for (int iRow = 0; iRow < size_.Height; ++iRow) {
				for (int iCol = 0; iCol < size_.Width; ++iCol) {
					byte renderId = renderData.getVertexValue(iRow, iCol);
					if (!fwrite(&renderId, sizeof(byte), 1, fp))
						return false;
				}
			}
			return true;
		}

		// 计算并返回显示位置(绝对位置)
		static GP Point &calcDisplayLocation(GP Point &currentBodyLocation) {
			GP Point currentCameraLocation = RenderManager::cameraArea.getLocation();
			static GP Point currentDisplayLocation = RenderManager::displayArea.getLocation();
			int xSub = currentBodyLocation.X - currentCameraLocation.X, ySub = currentBodyLocation.Y - currentCameraLocation.Y;
			currentBodyLocation.X = currentDisplayLocation.X + xSub;
			currentBodyLocation.Y = currentDisplayLocation.Y + ySub;
			return currentBodyLocation;
		}

		static GP Point const &calcDisplayLocation(Sprite const &currentBody) {
			static GP Point currentBodyLocation;
			currentBodyLocation = currentBody.getLocation();
			calcDisplayLocation(currentBodyLocation);
			return currentBodyLocation;
		}

		// 玩家的真实视野(位置与玩家位置有关, 大小与展示区有关)
		static Sprite cameraArea;
		// 实际是屏幕的显示区域(展览区 绝对区域) 位置相当于屏幕的原点(但是绝对坐标) size就是屏幕的size
		static Sprite displayArea;
	};

	//事件碰撞管理器 (四叉树筛选->包围盒的初步碰撞判断->takeEffect精准判断)
	class EventImpactManager {
		//drama剧本 Script脚本 把…改编为剧本 Story=Message+time
	private:
		//接收者的接收能力范围 广播的影响范围
		//value用引用不行(实体的引用方法返回也是左值) 只能用指针 (用指针的右值引用将引发奇怪的错误)
		//左边用Body事件就行
		/*
		Base::iterator;//迭代器 = {}
		InsertValue = Base::value_type;//插入值 = {key, value}
		Key = Base::key_type;//实际键
		Message;//实际的值
		*/
		//碰撞树
		QuadTree impactTree;

		// STL中不允许 const @see https://stackoverflow.com/questions/35764948/xmemory-errors-in-c-project-after-migrating-vs-2012-to-vs-2015
		//LinkedList<std::shared_ptr<CollisionBox> const> eventRegistry;
		//事件注册表priority_queue<EventParcel>
		LinkedList<std::shared_ptr<CollisionBox>> eventRegistry;
		TimeClock refreshTime;
		//探测开始时的事件数
		int detectionCount = 0;
	public:

		EventImpactManager(Sprite const &window)
			: impactTree(window) {
			refreshTime.SetDrtTime(1000);
		}
		virtual ~EventImpactManager() {}

		//碰撞事件的总个数
		int eventSize() {
			return impactTree.size();
		}

		//刷新事件管理器 返回事件数量
		int refresh() {
			impactTree.refresh();
			if (DEBUG) {
				impactTree.forDebug();
			}
			return eventSize();
		}

		//发送一个需要碰撞检测的事件 发送成功返回true
		bool sendImpactEvent(std::shared_ptr<CollisionBox> &eventObj) {
			return impactTree.insert(eventObj);
		}

		//碰撞检测 (检测对象的碰撞盒, 碰撞结果列表, valid筛选器)  预留方法 暂未使用
		template<class Fun>
		void impactDetection(std::shared_ptr<CollisionBox> const &key,
			LinkedList<std::shared_ptr<CollisionBox>> &impacters, Fun customFiltrate) {
			detectionCount = eventSize();
			impactTree.retrieve(key, impacters);
			//对可能的碰撞者进行碰撞检验
			for (auto it = impacters.begin(); it != impacters.end();) {
				if (customFiltrate() && key->collide(**it)) {
					++it;
				}
				else {
					it = impacters.erase(it);
				}
			}
		}

		//碰撞检测 (检测对象的碰撞盒, 所有的碰撞结果列表)
		void impactDetection(std::shared_ptr<CollisionBox> const &key,
			LinkedList<std::shared_ptr<CollisionBox>> &impacteResults) {
			detectionCount = eventSize();
			impactTree.retrieve(key, impacteResults);
			//对可能的碰撞者进行碰撞检验
			for (auto it = impacteResults.begin(); it != impacteResults.end();) {
				if (key->collide(**it)) {
					++it;
				}
				else {
					it = impacteResults.erase(it);
				}
			}
		}
	};

	/*
	掉落物地图接口
	因为每张地图的掉落物要独立保留 所以没有合并到EventImpactManager 但是为什么不直接保留整个生态呢
	*/
	class DropMap {/*掉落物品(position, id)*/
		std::map<Sprite, int> dropItem;//因为要进行碰幢检测
		struct Item {
			int id = 0;
			Sprite body;
			int clock = 0;
			bool operator<(Item const &rhs)const {
				return clock < rhs.clock;
			}
		};
		//AvlTree<Item> dropTree;
	public:
		DropMap() {}
		DropMap(DropMap &&rvalue) {
			for (auto it = rvalue.dropItem.begin(); it != rvalue.dropItem.end(); ++it) {
				dropItem.insert(*std::move(it));
			}
		}
		virtual ~DropMap() {}
		/*将背包中的物品装载到掉落地图 (物品Sprite)*/
		void loadingItem(Sprite body, Package &diededBag) {
			diededBag.push_back(std::make_pair(116, 1));//固定掉落红心bug
			while (!diededBag.empty()) {
				int id = diededBag.back().first;
				if (--diededBag.back().second <= 0)
					diededBag.pop_back();
				dropItem.insert(std::make_pair(body, id));
			}
		}

		/*背包算法捡起物品(未实现)*/
		/*探测并返回返回方块处的物品 若没有物品返回-1*/
		int detectionItem(Sprite const &body) {
			//离自己最近的发生碰撞的body
			std::map<Sprite, int>::iterator it = dropItem.find(body);
			if (it != dropItem.end()) {
				int re = it->second;
				dropItem.erase(it);
				return re;
			}
			else
				return -1;
		}
		/*展示所有掉落物*/
		int nextId() {//BUG
			int id = dropItem.size() == 0 ? -1 :
				dropItem.begin()->second;
			if (id != -1)
				dropItem.erase(dropItem.begin());
			return id;
		}
	};

}

#endif