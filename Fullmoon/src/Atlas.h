#ifndef __Atlas_H
#define __Atlas_H
#include "AtlasUtil.h"
#include "CollisionEvent.h"

#include "../myLib/extendspace.h"

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
		static void loading(GP Graphics *mapGraphics, Sprite const &cameraArea, GP Point const &displayCentre) {
			RenderManager::cameraArea = cameraArea;
			displayArea = cameraArea;
			displayArea.setCentre(displayCentre);
			maper.setGraphics(mapGraphics);
			//野外地图
			maper.reLoad(L"map\\grass.png", 15, 16);
			//传送阵
			maper.reLoad(L"map\\teleporter.png", 1, 1);
			//野外地图2
			int rSub = maper.reLoad(L"world.png", 15, 7);

			// 地图画板只能绘制地图区域
			mapGraphics->SetClip(cameraArea.getRect());
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

		// 将地图移动到玩家所在的地方(渲染可见区域) 每次重绘整个视野 效率极低
		/*void renderDisplayMap() {
			renderDisplayMap(cameraArea);
		}*/
		void renderDisplayMap() {
			// 显示的位置(绝对坐标)
			int absoluteX, absoluteY;
			// 绝对坐标迭代器
			auto itDisplayArea = displayArea.pointIterator(Constant::GRID_CELL.Width, Constant::GRID_CELL.Height);

			// 渲染区域(位置跟随相机, 大小跟随展示区)
			Sprite renderArea;
			renderArea.setSize(displayArea.getRect().Width, displayArea.getRect().Height);
			renderArea.setCentre(RenderManager::cameraArea.getCentre());
			// 世界坐标: 用于(从地图数据中)确定渲染地图块值
			int x, y;
			auto itRenderCamera = renderArea.pointIterator(Constant::GRID_CELL.Width, Constant::GRID_CELL.Height);
			// n维数组的size()就是其第n维数组的大小
			// 即最先索引的一维 x y z分别是3 2 1维 (h w)或(x y)分别是2 1
			while (itRenderCamera.iterate(x, y)) {
				itDisplayArea.iterate(absoluteX, absoluteY);
				renderPlot(x, y, absoluteX, absoluteY);
			}
		}

		// 绘制视野
		void renderDisplayMap(Sprite cameraArea) {
			renderDisplayMap(cameraArea.getLocation(), displayArea.getLocation(), cameraArea.getSize());
		}

		// 将指定的渲染区域的数据 渲染至指定绝对区域 两者位置不同(世界, 绝对)但大小一致 由size决定
		// 返回display区域未发生变动的区域
		void renderDisplayMap(GP Point const &cameraLocation, GP Point const &displayLocation, GP Size const &size) {
			// @DEBUG 图片会对调试线产生遮挡 故挪至尾部
			// maper.getGraphics()->DrawRectangle(maper.pen(255, 255), GP Rect({ displayLocation .X-1, displayLocation.Y-1}, size));

			Sprite displayArea(size, displayLocation);
			Sprite cameraArea(size, cameraLocation);

			// 显示的位置(绝对坐标)
			int absoluteX, absoluteY;
			// 绝对坐标迭代器
			auto itDisplayArea = displayArea.pointIterator(Constant::GRID_CELL.Width, Constant::GRID_CELL.Height);

			// 渲染区域(位置跟随相机, 大小跟随展示区)
			Sprite renderArea;
			renderArea.setSize(displayArea.getRect().Width, displayArea.getRect().Height);
			renderArea.setCentre(cameraArea.getCentre());
			// 世界坐标: 用于(从地图数据中)确定渲染地图块值
			int x, y;
			auto itRenderCamera = renderArea.pointIterator(Constant::GRID_CELL.Width, Constant::GRID_CELL.Height);
			// n维数组的size()就是其第n维数组的大小
			// 即最先索引的一维 x y z分别是3 2 1维 (h w)或(x y)分别是2 1
			while (itRenderCamera.iterate(x, y)) {
				itDisplayArea.iterate(absoluteX, absoluteY);
				renderPlot(x, y, absoluteX, absoluteY);
			}
			maper.getGraphics()->DrawRectangle(maper.pen(255, 0, 255, 255), GP Rect(displayLocation, size));
		}

		// 屏幕视窗在地图中的位置
		static int leftTopX;
		static int oldLeftTopX;
		static int leftTopY;
		static int oldLeftTopY;
		
		// 绘制地图的方法
		void draw(Bitmap *imageBuffer) {
			static bool firstDraw = true;
			// 相机坐标来修正leftTopX值
			leftTopX = cameraArea.getLocation().X;
			leftTopY = cameraArea.getLocation().Y;
			//renderDisplayMap();return;
			if (firstDraw) {
				//renderDisplayMap(cameraArea.getLocation(), displayArea.getLocation(), cameraArea.getSize());
				renderDisplayMap();
				firstDraw = false;
			}
			else {
				drawBuffer(*maper.getGraphics(), imageBuffer);
				// drawCarmarkMap(*maper.getGraphics(), imageBuffer);
			}
		}

		// drawClipBuffer
		// 绘制更新的地图 并计算原地图中未更新的区域
		void drawUpdateMap(Sprite &unUpdateArea) {
			unUpdateArea = displayArea;
			if (oldLeftTopX == leftTopX) {
				// 屏幕没有发生横向卷动
			}
			else {
				int reRederWorldX = cameraArea.getLocation().X, reRederWorldY = cameraArea.getLocation().Y;
				int reRederAbsoluteX = displayArea.getLocation().X, reRederAbsoluteY = displayArea.getLocation().Y;
				// 向右
				if (leftTopX - oldLeftTopX > 0) {
					reRederWorldX += cameraArea.getWidth() - Constant::GRID_CELL.Width;
					reRederAbsoluteX += displayArea.getWidth() - Constant::GRID_CELL.Width;
				}
				// 向左
				else if (leftTopX - oldLeftTopX < 0) {
					// do nothing
				}
				Sprite temp = Sprite(GP Size(Constant::GRID_CELL.Width, displayArea.getHeight()), GP Point(reRederAbsoluteX, reRederAbsoluteY));
				unUpdateArea.clipByPointLeftTop(temp);
				renderDisplayMap(
					GP Point(reRederWorldX, reRederWorldY), temp.getLocation(), temp.getSize()
				);
			}
			if (oldLeftTopY == leftTopY) {
				// 屏幕没有发生纵向卷动
			}
			else {
				int reRederWorldX = cameraArea.getLocation().X, reRederWorldY = cameraArea.getLocation().Y;
				int reRederAbsoluteX = displayArea.getLocation().X, reRederAbsoluteY = displayArea.getLocation().Y;
				// 向下
				if (leftTopY - oldLeftTopY > 0) {
					reRederWorldY += cameraArea.getHeight() - Constant::GRID_CELL.Height;
					reRederAbsoluteY += displayArea.getHeight() - Constant::GRID_CELL.Height;
				}
				// 向上
				else if (leftTopY - oldLeftTopY < 0) {
					// do nothing
				}
				Sprite temp = Sprite(GP Size(displayArea.getWidth(), Constant::GRID_CELL.Height), GP Point(reRederAbsoluteX, reRederAbsoluteY));
				unUpdateArea.clipByPointLeftTop(temp);
				renderDisplayMap(
					GP Point(reRederWorldX, reRederWorldY), temp.getLocation(), temp.getSize()
				);
			}
			oldLeftTopX = leftTopX;
			oldLeftTopY = leftTopY;
		}

		// 重绘地图缓冲区的方法 Image 没有可用的方法
		void drawBuffer(GP Graphics &g, Bitmap *imageBuffer) {
			/*Logger::writeLine((std::wstring(_T("缓冲切割点位置"))
				+ std::to_wstring(bufClipX)).c_str());
			StandardExtend::outputDebugFormat(std::string("切割x=" + std::to_string(bufClipX)).c_str());
			StandardExtend::outputDebugFormat(std::string("玩家中心x=" + std::to_string(cameraArea.getCentre().X) + "\n\r").c_str());
			OutputDebugString((std::wstring(_T("两次切割点的差距=")) + std::to_wstring(abs(bufClipX - oldBufClipX))
				+ _T("\r\n")).c_str());*/
			// 原地图的绝对绘制坐标
			int abBufRenderX = oldLeftTopX- leftTopX, abBufRenderY = oldLeftTopY - leftTopY;
			g.DrawImage(imageBuffer, abBufRenderX, abBufRenderY);
			static Sprite renderArea;
			// 更新时没有的重渲染区域是需要渲染的区域
			drawUpdateMap(renderArea);
			// 计算原地图中的可重用区域
			// 将可重用区域渲染到渲染区

			//g.DrawImage(&Bitmap(_T("res/platBack.jpg")), unUpdateArea.getRect());
			
			// 设置无需重绘的区域为地图有效区
			//g.SetClip(unUpdateArea.getRect());
			// 贴上不需要重绘的区域
			/*auto temp = imageBuffer->Clone(displayArea.tmpRectF(), PixelFormatDontCare);
			g.DrawImage(temp, renderArea.getRect());
			delete temp;
			temp = nullptr;*/

			// 这是将一张图片以地图区左角为原点绘制的例子
			// g.DrawImage(&Bitmap(_T("res/logo.png")), displayArea.getLocation());

			// 恢复
			//g.SetClip(displayArea.getRect());
			//GP Region resultRegion;
			//g.GetClip(&resultRegion);
			//resultRegion.GetData();
		}

		// 卡马克卷轴方法画地图
		void drawCarmarkMap(GP Graphics &g, Image *imageBuffer) {
			/*
			// clipX屏幕分割点，用于在屏幕上进行选区，取值为(0,w]
			clipX += oldLeftTopX - leftTopX;
			if (clipX <= 0) {
				// 向右走的修正
				clipX += w;
			}
			else if (clipX > w) {
				// 向左走的修正
				clipX -= w;
			}

			// drawX当前的绘制点，用于图片缓冲区和屏幕对齐 取值为(-w,0]
			drawX += oldLeftTopX - leftTopX;
			if (drawX <= -w) {
				// 向右走的修正
				drawX += w;
			}
			else if (drawX > 0) {
				// 向左走的修正
				drawX -= w;
			}

			g.SetClip(Rect(0, 0, clipX, Constant::mainCanvasSize.Height), CombineModeReplace);
			//revise修正 g.DrawImage(imageBuffer, drawX, Data.REVISE_Y, 0);
			// 贴上不需要重绘的区域
			g.DrawImage(imageBuffer, drawX, 0);
			//renderPlot(drawX, 0);

			g.SetClip(Rect(clipX, 0, Constant::mainCanvasSize.Width - clipX, Constant::mainCanvasSize.Height), CombineModeReplace);
			//g.DrawImage(imageBuffer, Constant::mainCanvasSize.Width + drawX, Data.REVISE_Y, 0);// 贴上本次重绘的区域
			g.DrawImage(imageBuffer, Constant::mainCanvasSize.Width + drawX, 0);
			//renderPlot(Constant::mainCanvasSize.Width + drawX, 0);

			g.SetClip(Rect(0, 0, Constant::mainCanvasSize.Width, Constant::mainCanvasSize.Height), CombineModeReplace);

			oldLeftTopX = leftTopX;*/
		}


		//返回加载完成后生物的 左角位置
		GP Point loadPlat() {
			Sprite entrance, exit;
			loadRenderData(entrance, exit);
			/*int x, y;
			auto it = mapArea->pointIterator(Constant::GRID_CELL.Width, Constant::GRID_CELL.Height);
			//n维数组的size()就是其第n维数组的大小 即最先索引的一维 x y z分别是3 2 1维 (h w)或(x y)分别是2 1
			while (it.iterate(x, y)) {
				renderPlot(x, y);
			}*/
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
		//渲染x y处的地块到地图的对应位置(x, y)
		void renderPlot(int x, int y) {
			maper.draw(
				renderData.getVertexValue(Constant::rowSub(y), Constant::colSub(x))
				, Sprite(Constant::GRID_CELL, x, y)
			);
		}
		//读取渲染地图数据中x y处的地块数据 渲染对应的地块图形到绝对坐标(absoluteX, absoluteY)
		void renderPlot(int x, int y, int absoluteX, int absoluteY) {
			if (renderData.getLimitSub().r <= Constant::rowSub(y)
				|| renderData.getLimitSub().c <= Constant::colSub(x)
				|| Constant::rowSub(y) < 0 || Constant::colSub(x) < 0
				) {
				maper.drawInRC(19, 0, Sprite(Constant::GRID_CELL, absoluteX, absoluteY));
			}
			else {
				maper.draw(
					renderData.getVertexValue(Constant::rowSub(y)
					, Constant::colSub(x))
					, Sprite(Constant::GRID_CELL, absoluteX, absoluteY)
				);
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
		// 地图区域 (若只有一张地图 那么这个区域即代表了 世界地图)
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