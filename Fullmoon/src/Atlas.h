#ifndef __Atlas_H
#define __Atlas_H
#include "AtlasUtil.h"
#include "CollisionEvent.h"

namespace EcologicEngine {

	// 碰撞群
	using Colony = std::map<int, CollisionBox>;
	/*
	生态区域 :剧本 掉落物 群体 具有通信, 渲染功能
	在构造一个生态后应当自动渲染 然后提供剧请通信和物品通信的接口
	*/
	/*
	无论如何，总是先考虑对象的组合，然后再考虑继承。参见Effective C++ (有virtual才需要继承)
	如果有多种方式可以有效地表达类之间的关系，那么应该选择一种最弱的关系，那将最大程度地降低耦合性，获得最高的灵活性。
	*/
	// 地图集(Atlas)
	class Atlas :RenderManager, public EventImpactManager {
	public:
		Atlas(Sprite const &mapArea)
			: mapArea(new Sprite(mapArea)), EventImpactManager(mapArea)
			, areaWindow(WindowSprite(_T("区域消息窗口")))
			, RenderManager(mapArea)
			, timestamp(new Timestamp()) {
			// 如果在生成生物前没有初始化渲染地图的话那么将会引起初始化位置的错误
			loadPlat();
			areaWindow.setRect(mapArea.getRect());
		}

		Atlas &operator=(Atlas &&rvalue) {
			mapArea = std::move(rvalue.mapArea);
			return *this;
		}

		//装载背包中的物品 (ownerID: 是原拥有者的id 此处用于区分物品)
		void loadingPackage(Sprite const &location, Package &diededBag) {
			while (!diededBag.empty()) {
				int id = diededBag.back().first;
				if (--diededBag.back().second <= 0)
					diededBag.pop_back();
				//此时间戳用于掉落物实体与碰撞盒之间的联系
				dropItem.emplace_back(std::shared_ptr<Item>(new Item(std::shared_ptr<Timestamp>(new Timestamp()), location, id)));
				//将新生成的事件的副本作为一个碰撞事件加入碰撞树(时间戳一致)
				sendImpactEvent(std::shared_ptr<CollisionBox>(dropItem.back()));
			}
		}
		//绘制所有掉落物(若发现已失效物品将会清除)
		void drawDropItem() {
			for (auto it = dropItem.begin(); it != dropItem.end(); ) {
				if ((*it)->isValid()) {
					(*it)->shawItem();
					++it;
				}
				else {
					it = dropItem.erase(it);
				}
			}
		}

		WindowSprite &sendMessage() {
			return areaWindow;
		}
		std::shared_ptr<Sprite const> getMapArea() {
			return mapArea;
		}
		//装载数据(屏幕显示区域的Size与摄像机相同)
		static void loading(GP Graphics *mapGraphics, Sprite const &cameraSprite, GP Point const &displayCentre) {
			RenderManager::cameraSprite = cameraSprite;
			displayArea = cameraSprite;
			displayArea.setCentre(displayCentre);
			maper.setGraphics(mapGraphics);
			maper.reLoad(L"map\\grass.png", 15, 16);//野外地图
			maper.reLoad(L"map\\teleporter.png", 1, 1);//传送阵
			int rSub = maper.reLoad(L"world.png", 15, 7);//野外地图2
		}

		void mosaicTransition(Bitmap const *plat) {
			mosaicImage(plat, maper.getGraphics());
		}

		void getFileByname(char *fileName, char *openMode) {
			errno_t err;
			//如果成功返回0，失败则返回相应的错误代码。
			if ((err = fopen_s(&fp, fileName, openMode)) == 0) {
				return;
			}
			_DEBUG_ERROR("文件打开失败");
		}
		bool saveRenderData() {
			bool st = false;
			getFileByname("map0.dat", "wb");
			st = saveRender(fp);
			std::fclose(fp);
			return st;
		}
		bool loadRenderData(Sprite &entrance, Sprite &exit) {
			bool st = false;
			getFileByname("map0.dat", "rb");
			st = loadRender(fp, entrance, exit);
			std::fclose(fp);
			return st;
		}

		// 将地图移动到玩家所在的地方(渲染可见区域)
		void renderDisplayMap() {
			//告知世界坐标区域 渲染对应区域的地图到plat //, Bitmap *&displayPlat
			int x, y;
			int mapX, mapY;
			//渲染位置(显示区域)
			Sprite mapRenderSprite = displayArea;
			auto mapIter = mapRenderSprite.pointIterator(Constant::GRID_CELL.Width, Constant::GRID_CELL.Height);

			//渲染来源
			Sprite renderOriginSprite = RenderManager::cameraSprite;
			renderOriginSprite.setSize(mapRenderSprite.getRect().Width, mapRenderSprite.getRect().Height);
			renderOriginSprite.setCentre(RenderManager::cameraSprite.getCentre());
			auto it = renderOriginSprite.pointIterator(Constant::GRID_CELL.Width, Constant::GRID_CELL.Height);
			// n维数组的size()就是其第n维数组的大小 即最先索引的一维 x y z分别是3 2 1维 (h w)或(x y)分别是2 1
			while (it.iterate(x, y)) {
				mapIter.iterate(mapX, mapY);
				renderPlot(x, y, mapX, mapY);
			}
		}


		// 地图数组
		//byte** MapData;

		// 屏幕视窗在地图中的位置
		static int leftTopX;
		static int oldLeftTopX;
		static int leftTopY;

		//byte lightTime;
		//byte param;

		// 每张地图的属性
		// 地图元素组成的单元格大小
		//static int cellWidth = 8;
		// 地图元素组成的单元格大小
		//static int cellHeight = 8;
		// 地图中x方向单元格的个数，此参数决定地图的大小，每张地图都有不同的尺寸
		//int mapWNum;
		// 地图中y方向单元格的个数，此参数决定地图的大小，每张地图都有不同的尺寸
		//int mapHNum;

		// 从第1张地图到第5张地图的mapWNum 96、192、64、64、96

		// 缓冲区图片尺寸
		int w = RenderManager::cameraSprite.getLocation().X + 500, h = w;
		// 屏幕分割点，用于在屏幕上进行选区，取值为(0,w]
		int clipX = 0;
		// 屏幕绘制点，用于图片缓冲区和屏幕对齐，取值为(-w,0]
		int drawX = 0;
		// 上一次缓冲区分割点，和屏幕分割点有一定的关系，取值为[0,w)
		int oldBufClipX = RenderManager::cameraSprite.getLocation().X;
		// 缓冲区分割点，和屏幕分割点有一定的关系，取值为[0,w)
		int bufClipX = oldBufClipX;
		// 缓冲区绘制点，用于在缓冲区上绘制地图信息，取值为[0,w)，注意此数为离散值，i*cellWidth
		int bufDrawX = 0;

		// 缓冲区
		//Image *imageBuffer = nullptr;
		//Graphics *bg;// 缓冲区的画笔
		//Engine eg;


		// 绘制地图的方法 Graphics &g
		void draw(Image *imageBuffer) {
			// bg
			drawBuffer(*maper.getGraphics());
			drawCarmarkMap(*maper.getGraphics(), imageBuffer);
		}

		// 重绘地图缓冲区的方法
		void drawBuffer(GP Graphics &g) {
			leftTopX = RenderManager::displayArea.getLocation().X;
			if (oldLeftTopX == leftTopX) {
				// 如果屏幕没有发生卷动则直接返回
				Logger::writeLine(_T("----------没有发生卷动------------"));
				return;
			}

			// bufClipX缓冲区分割点，和屏幕分割点有一定的关系，取值为[0,w)
			bufClipX += leftTopX - oldLeftTopX;
			if (bufClipX < 0) {
				// 向左走的修正
				bufClipX += w;
			}
			else if (bufClipX > w) {
				// 向右走的修正
				bufClipX -= w;
			}

			if (leftTopX - oldLeftTopX > 0) {
				// 向右走
				// System.out.println("→");
				if (abs(oldBufClipX - bufClipX) > 200) {
					// oldBufClipX和bufClipX在缓冲区的两端！
					// 设置两个选区，分别填充！
					g.SetClip(Rect(oldBufClipX, 0, abs(oldBufClipX - w), h));
					drawClipBuffer(*maper.getGraphics());
					g.SetClip(Rect(0, 0, bufClipX, h));
					drawClipBuffer(*maper.getGraphics());
				}
				else {
					g.SetClip(Rect(oldBufClipX, 0, abs(bufClipX - oldBufClipX), h));
					drawClipBuffer(*maper.getGraphics());

				}

			}
			else {
				// 向左走
				// System.out.println("←");
				if (abs(oldBufClipX - bufClipX) > 200) {
					// oldBufClipX和bufClipX在缓冲区的两端！
					// 设置两个选区，分别填充！
					g.SetClip(Rect(0, 0, oldBufClipX, h));
					drawClipBuffer(*maper.getGraphics());
					g.SetClip(Rect(bufClipX, 0, abs(bufClipX - w), h));
					drawClipBuffer(*maper.getGraphics());
				}
				else {
					g.SetClip(Rect(bufClipX, 0, abs(bufClipX - oldBufClipX), h));
					drawClipBuffer(*maper.getGraphics());
				}
			}

			g.SetClip(Rect(0, 0, w, h));

			oldBufClipX = bufClipX;

		}

		//绘制缓冲区中选区的地图
		void drawClipBuffer(GP Graphics &g) {

			// 求出起始格子的索引号
			int iStart = 0;// 计算y方向（纵向）的起始地图单元格索引

			int jStart = 0;
			if (leftTopX - oldLeftTopX > 0) {// 向右
				jStart = (leftTopX + w) / Constant::GRID_CELL.Width; // 计算x方向（横向）的起始地图单元格索引
			}
			else if (leftTopX - oldLeftTopX < 0) {// 向左
				jStart = leftTopX / Constant::GRID_CELL.Width;
			}

			// 求出绘制格子的y方向数目和x方向数目
			int iNum = 21;// 纵向（y方向）可以显示的地图单元格数目
			int jNum = 1;// 横向（x方向）可以显示的地图单元格数目

			// 求出在缓冲区的绘制起始点的坐标
			bufDrawX = bufClipX / Constant::GRID_CELL.Width * Constant::GRID_CELL.Width;

			// 根据上一次的绘制情况，修正索引号
			if (oldBufClipX / Constant::GRID_CELL.Width != bufClipX / Constant::GRID_CELL.Width) {
				if (leftTopX - oldLeftTopX > 0) {// 向右
					jStart -= 1;
					jNum += 1;
					bufDrawX -= Constant::GRID_CELL.Width;
					// System.out.println("修正");
				}
				else if (leftTopX - oldLeftTopX < 0) {// 向左
					// jStart+=1;
					jNum += 1;

				}

			}
			SubTwain subs = renderData.getLimitSub();
			// System.out.println(jStart);
			// System.out.println(bufDrawX);
			// 防止数组越界
			if (iStart + iNum >= subs.r) {// Y轴方向修正
				iNum = subs.r - 1 - iStart;
			}
			if (jStart + jNum >= subs.c) {// X轴方向修正
				jNum = subs.c - 1 - jStart;
			}

			// 使用双缓冲绘制地图
			for (int i = iStart; i <= iStart + iNum; i++) {
				for (int j = jStart; j <= jStart + jNum; j++) { // 双循环遍历整个地图数组
					if (renderData.getVertexValue(i, j) >= 0) { // 地图数组中元素大于等于０表示存在图块
						//maper.draw(renderData.getVertexValue(Constant::rowSub(y), Constant::colSub(x)), Sprite(Constant::GRID_CELL, mapX, mapY));
						renderPlot(
							(j - jStart) * Constant::GRID_CELL.Width + bufDrawX, // 用-jStart修正成为屏幕内坐标X
							(i - iStart) * Constant::GRID_CELL.Height // 用-iStart修正成为屏幕内坐标Y
						);
					}
				}
			}
			// System.out.println("========================");
		}

		//卡马克卷轴方法画地图
		void drawCarmarkMap(GP Graphics &g, Image *imageBuffer) {

			// clipX屏幕分割点，用于在屏幕上进行选区，取值为(0,w]
			clipX += oldLeftTopX - leftTopX;
			if (clipX <= 0) {// 向右走的修正
				clipX += w;
			}
			else if (clipX > w) {// 向左走的修正
				clipX -= w;
			}

			// drawX当前的绘制点，用于图片缓冲区和屏幕对齐 取值为(-w,0]
			drawX += oldLeftTopX - leftTopX;
			if (drawX <= -w) {// 向右走的修正
				drawX += w;
			}
			else if (drawX > 0) {// 向左走的修正
				drawX -= w;
			}

			g.SetClip(Rect(0, 0, clipX, Constant::mainCanvasSize.Height), CombineModeReplace);
			//revise修正 g.DrawImage(imageBuffer, drawX, Data.REVISE_Y, 0);// 贴上不需要重绘的区域
			// 贴上不需要重绘的区域
			g.DrawImage(imageBuffer, drawX, 0);
			//renderPlot(drawX, 0);

			g.SetClip(Rect(clipX, 0, Constant::mainCanvasSize.Width - clipX, Constant::mainCanvasSize.Height), CombineModeReplace);
			//g.DrawImage(imageBuffer, Constant::mainCanvasSize.Width + drawX, Data.REVISE_Y, 0);// 贴上本次重绘的区域
			g.DrawImage(imageBuffer, Constant::mainCanvasSize.Width + drawX, 0);
			//renderPlot(Constant::mainCanvasSize.Width + drawX, 0);

			g.SetClip(Rect(0, 0, Constant::mainCanvasSize.Width, Constant::mainCanvasSize.Height), CombineModeReplace);

			oldLeftTopX = leftTopX;
		}


		//返回加载完成后生物的 左角位置
		GP Point loadPlat() {
			Sprite entrance, exit;
			loadRenderData(entrance, exit);
			int x, y;
			auto it = mapArea->pointIterator(Constant::GRID_CELL.Width, Constant::GRID_CELL.Height);
			//n维数组的size()就是其第n维数组的大小 即最先索引的一维 x y z分别是3 2 1维 (h w)或(x y)分别是2 1
			while (it.iterate(x, y)) {
				renderPlot(x, y);
			}
			//传送阵所在行标 列标
			int transferSub = 18, transferCol = 4;
			entrance.setSize(maper.getWidthA(transferSub), maper.getHeightA(transferSub));
			//entrance.setCentre({ 32 + 5, 24 + 5 });
			exit.setSize(maper.getWidthA(transferSub), maper.getHeightA(transferSub));
			//exit.setCentre({ mapArea->getWidth() - 32 - 5, mapArea->getHeight() - 24 - 25 });
			std::shared_ptr<TransmissionEvent> left, right;
			left.reset(new TransmissionEvent(timestamp, 1, 0));
			right.reset(new TransmissionEvent(timestamp, 1, 1));
			left->setCircle(entrance.getCentre(), entrance.getRadius());
			right->setCircle(exit.getCentre(), entrance.getRadius());
			transmissionList.emplace_back(left);
			transmissionList.emplace_back(right);
			//渲染出口入口 (传送阵图片)
			maper.draw(transferSub, transferCol, entrance.getRect());
			maper.draw(transferSub, transferCol, exit.getRect());
			//maper.draw(16, 0, entrance.getRect());
			//maper.draw(16, 0, exit.getRect());

			return GP Point(entrance.getRect().X, entrance.getRect().Y);
		}

		//编辑生态:使body处的地图重新渲染为渲染号为id的地块
		void edit(Sprite const &body, int id) {
			auto it = body.pointIterator(Constant::GRID_CELL.Width, Constant::GRID_CELL.Height);
			reloadRender(it, id);
			renderPlot(it);
		}

		//地图阻力(以此地图为父画面的子画面 未完全实现 详见记事)
		int block(Sprite const &nextBody) {
			if (!mapArea->contains(nextBody))
				return true;//走出地图
			else {
				auto it = nextBody.pointIterator(Constant::GRID_CELL.Width, Constant::GRID_CELL.Height);
				int x, y;
				while (it.iterate(x, y)) {
					int mapCellData = renderData.getVertexValue(Constant::rowSub(y), Constant::colSub(x));
					assert(mapCellData >= 0);
					//无效->不能通过->阻挡->返回true
					if (!renderData.isVertexValid(Constant::rowSub(y), Constant::colSub(x))) {
						//阻挡地块
						return true;
					}
					else {
						continue;
					}
				}
				//不存在阻挡地块
				return false;
			}
		}

		//寻路
		bool pickUpFollow(Sprite const &body, Sprite const &target, ShortestPathResult &result) {
			static Graph &parityGraph = renderData.parityGraph();
			static SubTwain limitSub(mapArea->rowsLimit(), mapArea->colsLimit());
			ArrayList<int> &dist = result.dist, &path = result.path;

			//@TODO 加入体型的判断(仅仅是一个点有可能体型无法通过)
			auto startRect = body.getRect();
			auto startCentre = body.getCentre();
			//SubTwain startSub = SubTwain(Constant::rowSub(startCentre.Y), Constant::colSub(startCentre.X));
			SubTwain startSub = SubTwain(Constant::rowSub(startRect.GetTop()), Constant::colSub(startRect.GetLeft()));
			size_t startId = startSub.number(limitSub.r, limitSub.c);

			auto endRect = target.getRect();
			auto endCentre = target.getCentre();
			SubTwain endSub = SubTwain(Constant::rowSub(endCentre.Y), Constant::colSub(endCentre.X));
			//SubTwain endSub = SubTwain(Constant::rowSub(endRect.GetTop()), Constant::colSub(endRect.GetLeft()));
			size_t targetId = endSub.number(limitSub.r, limitSub.c);

			result.startId = startId;
			result.targetId = targetId;
			parityGraph.setDestVertex(targetId);

			return parityGraph.shortestPath(startId, dist, path);
		}

		// 注册地图事件
		void registration() {
			// 传送事件
			FOR(it, transmissionList.begin(), transmissionList.end()) {
				this->sendImpactEvent(*it);
			}
		}

		// 处理地图逻辑
		bool run();

	private:
		//渲染x y处的地块到地图的对应位置
		void renderPlot(int x, int y) {
			maper.draw(renderData.getVertexValue(Constant::rowSub(y), Constant::colSub(x)), Sprite(Constant::GRID_CELL, x, y));
		}
		//读取x y处的渲染地块 渲染到mapX, mapY
		void renderPlot(int x, int y, int mapX, int mapY) {
			if (renderData.getLimitSub().r <= Constant::rowSub(y)
				|| renderData.getLimitSub().c <= Constant::colSub(x)
				|| Constant::rowSub(y) < 0 || Constant::colSub(x) < 0
				) {
				maper.drawInRC(19, 0, Sprite(Constant::GRID_CELL, mapX, mapY));
			}
			else {
				maper.draw(renderData.getVertexValue(Constant::rowSub(y), Constant::colSub(x)), Sprite(Constant::GRID_CELL, mapX, mapY));
			}
		}
		//渲染一个体型块大小的地块
		void renderPlot(Sprite::Iterator &pointIterator) {
			int x, y;
			while (pointIterator.iterate(x, y)) {
				renderPlot(x, y);
			}
		}

		void mosaicImage(Bitmap const *, Graphics*);
		
		std::shared_ptr<Timestamp> timestamp;
		
		// 地图资源管理员实体
		static AnimationManager maper;
		// 展览区
		std::shared_ptr<Sprite const> mapArea;
		Colony colony;
		// 生态消息窗口
		WindowSprite areaWindow;
		// 每次使用后会手动关闭无需析构
		FILE *fp = nullptr;
		// 物品实体集, 用于物品的显示(用掉落时间作为索引 只需要遍历, 增删操作 而且增删的时候不需要查重)
		ArrayList<std::shared_ptr<Item>> dropItem;
	};

}
#endif