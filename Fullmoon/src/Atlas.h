#ifndef __Atlas_H
#define __Atlas_H
#include "AtlasUtil.h"
#include "CollisionEvent.h"

#include "../myLib/extendspace.h"

namespace EcologicEngine {

	// ��ײȺ
	using Colony = std::map<int, CollisionBox>;
	/*
	��̬���� :�籾 ������ Ⱥ�� ����ͨ��, ��Ⱦ����
	�ڹ���һ����̬��Ӧ���Զ���Ⱦ Ȼ���ṩ����ͨ�ź���Ʒͨ�ŵĽӿ�
	*/
	/*
	������Σ������ȿ��Ƕ������ϣ�Ȼ���ٿ��Ǽ̳С��μ�Effective C++ (��virtual����Ҫ�̳�)
	����ж��ַ�ʽ������Ч�ر����֮��Ĺ�ϵ����ôӦ��ѡ��һ�������Ĺ�ϵ���ǽ����̶ȵؽ�������ԣ������ߵ�����ԡ�
	*/
	// ��ͼ��(Atlas)
	class Atlas :RenderManager, public EventImpactManager {
	public:
		Atlas(Sprite const &mapArea)
			: mapArea(new Sprite(mapArea)), EventImpactManager(mapArea)
			, areaWindow(WindowSprite(_T("������Ϣ����")))
			, RenderManager(mapArea)
			, timestamp(new Timestamp()) {
			// �������������ǰû�г�ʼ����Ⱦ��ͼ�Ļ���ô���������ʼ��λ�õĴ���
			loadPlat();
			areaWindow.setRect(mapArea.getRect());
		}

		Atlas &operator=(Atlas &&rvalue) {
			mapArea = std::move(rvalue.mapArea);
			return *this;
		}

		//װ�ر����е���Ʒ (ownerID: ��ԭӵ���ߵ�id �˴�����������Ʒ)
		void loadingPackage(Sprite const &location, Package &diededBag) {
			while (!diededBag.empty()) {
				int id = diededBag.back().first;
				if (--diededBag.back().second <= 0)
					diededBag.pop_back();
				//��ʱ������ڵ�����ʵ������ײ��֮�����ϵ
				dropItem.emplace_back(std::shared_ptr<Item>(new Item(std::shared_ptr<Timestamp>(new Timestamp()), location, id)));
				//�������ɵ��¼��ĸ�����Ϊһ����ײ�¼�������ײ��(ʱ���һ��)
				sendImpactEvent(std::shared_ptr<CollisionBox>(dropItem.back()));
			}
		}
		//�������е�����(��������ʧЧ��Ʒ�������)
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
		//װ������(��Ļ��ʾ�����Size���������ͬ)
		static void loading(GP Graphics *mapGraphics, Sprite const &cameraArea, GP Point const &displayCentre) {
			RenderManager::cameraArea = cameraArea;
			displayArea = cameraArea;
			displayArea.setCentre(displayCentre);
			maper.setGraphics(mapGraphics);
			maper.reLoad(L"map\\grass.png", 15, 16);//Ұ���ͼ
			maper.reLoad(L"map\\teleporter.png", 1, 1);//������
			int rSub = maper.reLoad(L"world.png", 15, 7);//Ұ���ͼ2
		}

		void mosaicTransition(Bitmap const *plat) {
			mosaicImage(plat, maper.getGraphics());
		}

		void getFileByname(char *fileName, char *openMode) {
			errno_t err;
			//����ɹ�����0��ʧ���򷵻���Ӧ�Ĵ�����롣
			if ((err = fopen_s(&fp, fileName, openMode)) == 0) {
				return;
			}
			_DEBUG_ERROR("�ļ���ʧ��");
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

		// ����ͼ�ƶ���������ڵĵط�(��Ⱦ�ɼ�����) ÿ���ػ�������Ұ Ч�ʼ���
		void renderDisplayMap() {
			renderDisplayMap(cameraArea);
		}

		// ������Ұ
		void renderDisplayMap(Sprite cameraArea) {
			renderDisplayMap(cameraArea.getLocation(), displayArea.getLocation(), cameraArea.getSize());
		}

		// ��ָ������Ⱦ��������� ��Ⱦ��ָ���������� ����λ�ò�ͬ(����, ����)����Сһ�� ��size����
		void renderDisplayMap(GP Point cameraLocation, GP Point displayLocation, GP Size size) {

			// @DEBUG ͼƬ��Ե����߲����ڵ� ��Ų��β��
			// maper.getGraphics()->DrawRectangle(maper.pen(255, 255), GP Rect({ displayLocation .X-1, displayLocation.Y-1}, size));

			Sprite displayArea(size, displayLocation);
			Sprite cameraArea(size, cameraLocation);

			// ��ʾ��λ��(��������)
			int absoluteX, absoluteY;
			// �������������
			auto itDisplayArea = displayArea.pointIterator(Constant::GRID_CELL.Width, Constant::GRID_CELL.Height);

			// ��Ⱦ����(λ�ø������, ��С����չʾ��)
			Sprite renderArea;
			renderArea.setSize(displayArea.getRect().Width, displayArea.getRect().Height);
			renderArea.setCentre(cameraArea.getCentre());
			// ��������: ����(�ӵ�ͼ������)ȷ����Ⱦ��ͼ��ֵ
			int x, y;
			auto itRenderCamera = renderArea.pointIterator(Constant::GRID_CELL.Width, Constant::GRID_CELL.Height);
			// nά�����size()�������nά����Ĵ�С
			// ������������һά x y z�ֱ���3 2 1ά (h w)��(x y)�ֱ���2 1
			while (itRenderCamera.iterate(x, y)) {
				itDisplayArea.iterate(absoluteX, absoluteY);
				renderPlot(x, y, absoluteX, absoluteY);
			}
			maper.getGraphics()->DrawRectangle(maper.pen(255, 0, 255, 255), GP Rect(displayLocation, size));
		}


		// ��ͼ����
		//byte** MapData;

		// ��Ļ�Ӵ��ڵ�ͼ�е�λ��
		static int leftTopX;
		static int oldLeftTopX;
		static int leftTopY;
		static int oldLeftTopY;
		// ������ͼƬ�ߴ�
		int w = RenderManager::cameraArea.getWidth(), h = RenderManager::cameraArea.getHeight();
		// ��Ļ�ָ�㣬��������Ļ�Ͻ���ѡ����ȡֵΪ(0,w]
		int clipX = 0;
		// ��Ļ���Ƶ㣬����ͼƬ����������Ļ���룬ȡֵΪ(-w,0]
		int drawX = 0;
		// ��һ�λ������ָ�㣬����Ļ�ָ����һ���Ĺ�ϵ��ȡֵΪ[0,w)
		int oldBufClipX = RenderManager::cameraArea.getLocation().X;
		// �������ָ�㣬����Ļ�ָ����һ���Ĺ�ϵ��ȡֵΪ[0,w)
		int bufClipX = oldBufClipX;
		// ���������Ƶ㣬�����ڻ������ϻ��Ƶ�ͼ��Ϣ��ȡֵΪ[0,w)��ע�����Ϊ��ɢֵ��i*cellWidth
		int bufDrawX = 0;

		// ������
		//Image *imageBuffer = nullptr;
		//Graphics *bg;// �������Ļ���
		//Engine eg;


		// ���Ƶ�ͼ�ķ��� Graphics &g
		void draw(Image *imageBuffer) {
			static bool firstDraw = true;
			setScreenMode();
			if (firstDraw) {
				renderDisplayMap();
				firstDraw = false;
			}
			else {
				drawBuffer(*maper.getGraphics());
				// drawCarmarkMap(*maper.getGraphics(), imageBuffer);
			}
		}

		// ���ƻ�������ѡ���ĵ�ͼ
		void drawClipBuffer(GP Graphics &g) {
			
		}

		// �ػ��ͼ�������ķ���
		void drawBuffer(GP Graphics &g) {
			if (oldLeftTopX == leftTopX) {
				// ��Ļû�з��������
			}
			else {
				int reRederWorldX = cameraArea.getLocation().X, reRederWorldY = cameraArea.getLocation().Y;
				int reRederAbsoluteX = displayArea.getLocation().X, reRederAbsoluteY = displayArea.getLocation().Y;
				// ����
				if (leftTopX - oldLeftTopX > 0) {
					reRederWorldX += cameraArea.getWidth() - Constant::GRID_CELL.Width;
					reRederAbsoluteX += displayArea.getWidth() - Constant::GRID_CELL.Width;
				}
				// ����
				else if (leftTopX - oldLeftTopX < 0) {
					// do nothing
				}
				renderDisplayMap(
					GP Point(reRederWorldX, reRederWorldY)
					, GP Point(reRederAbsoluteX, reRederAbsoluteY)
					, GP Size(Constant::GRID_CELL.Width, displayArea.getHeight())
				);
			}

			if (oldLeftTopY == leftTopY) {
				// ��Ļû�з��������
			}
			else {
				int reRederWorldX = cameraArea.getLocation().X, reRederWorldY = cameraArea.getLocation().Y;
				int reRederAbsoluteX = displayArea.getLocation().X, reRederAbsoluteY = displayArea.getLocation().Y;
				// ����
				if (leftTopY - oldLeftTopY > 0) {
					reRederWorldY += cameraArea.getHeight() - Constant::GRID_CELL.Height;
					reRederAbsoluteY += displayArea.getHeight() - Constant::GRID_CELL.Height;
				}
				// ����
				else if (leftTopY - oldLeftTopY < 0) {
					// do nothing
				}
				renderDisplayMap(
					GP Point(reRederWorldX, reRederWorldY)
					, GP Point(reRederAbsoluteX, reRederAbsoluteY)
					, GP Size(displayArea.getWidth(), Constant::GRID_CELL.Height)
				);
			}

			//drawClipBuffer(*maper.getGraphics());
			oldLeftTopX = leftTopX;
			oldLeftTopY = leftTopY;
			return;


			// bufClipX�������ָ�㣬����Ļ�ָ����һ���Ĺ�ϵ��ȡֵΪ[0,w)
			bufClipX += leftTopX - oldLeftTopX;
			if (bufClipX < 0) {
				// �����ߵ�����
				bufClipX += w;
			}
			else if (bufClipX > w) {
				// �����ߵ�����
				bufClipX -= w;
			}

			Logger::writeLine((std::wstring(_T("�����и��λ��"))
				+ std::to_wstring(bufClipX)).c_str());
			StandardExtend::outputDebugFormat(std::string("�и�x=" + std::to_string(bufClipX)).c_str());
			StandardExtend::outputDebugFormat(std::string("�������x=" + std::to_string(cameraArea.getCentre().X) + "\n\r").c_str());
			OutputDebugString((std::wstring(_T("�����и��Ĳ��=")) + std::to_wstring(abs(bufClipX - oldBufClipX))
				+ _T("\r\n")).c_str());
			if (leftTopX - oldLeftTopX > 0) {
				// ������
				StandardExtend::outputDebugFormat("��\n\r");
				if (abs(oldBufClipX - bufClipX) > 200) {
					// oldBufClipX��bufClipX�ڻ����������ˣ�
					// ��������ѡ�����ֱ���䣡
					g.SetClip(Rect(oldBufClipX, 0, abs(oldBufClipX - w), h));
					drawClipBuffer(*maper.getGraphics());
					g.SetClip(Rect(0, 0, bufClipX, h));
					drawClipBuffer(*maper.getGraphics());
				}
				else {
					//g.SetClip(Rect(oldBufClipX, 0, abs(bufClipX - oldBufClipX), h));
					drawClipBuffer(*maper.getGraphics());

				}

			}
			else {
				// ������
				StandardExtend::outputDebugFormat("��\n\r");
				if (abs(oldBufClipX - bufClipX) > 200) {
					// oldBufClipX��bufClipX�ڻ����������ˣ�
					// ��������ѡ�����ֱ���䣡
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

		//������㷨
		void setScreenMode() {
			// �������������leftTopXֵ
			leftTopX = cameraArea.getLocation().X;
			leftTopY = cameraArea.getLocation().Y;
		}

		// ����˾��᷽������ͼ
		void drawCarmarkMap(GP Graphics &g, Image *imageBuffer) {

			// clipX��Ļ�ָ�㣬��������Ļ�Ͻ���ѡ����ȡֵΪ(0,w]
			clipX += oldLeftTopX - leftTopX;
			if (clipX <= 0) {
				// �����ߵ�����
				clipX += w;
			}
			else if (clipX > w) {
				// �����ߵ�����
				clipX -= w;
			}

			// drawX��ǰ�Ļ��Ƶ㣬����ͼƬ����������Ļ���� ȡֵΪ(-w,0]
			drawX += oldLeftTopX - leftTopX;
			if (drawX <= -w) {
				// �����ߵ�����
				drawX += w;
			}
			else if (drawX > 0) {
				// �����ߵ�����
				drawX -= w;
			}

			g.SetClip(Rect(0, 0, clipX, Constant::mainCanvasSize.Height), CombineModeReplace);
			//revise���� g.DrawImage(imageBuffer, drawX, Data.REVISE_Y, 0);
			// ���ϲ���Ҫ�ػ������
			g.DrawImage(imageBuffer, drawX, 0);
			//renderPlot(drawX, 0);

			g.SetClip(Rect(clipX, 0, Constant::mainCanvasSize.Width - clipX, Constant::mainCanvasSize.Height), CombineModeReplace);
			//g.DrawImage(imageBuffer, Constant::mainCanvasSize.Width + drawX, Data.REVISE_Y, 0);// ���ϱ����ػ������
			g.DrawImage(imageBuffer, Constant::mainCanvasSize.Width + drawX, 0);
			//renderPlot(Constant::mainCanvasSize.Width + drawX, 0);

			g.SetClip(Rect(0, 0, Constant::mainCanvasSize.Width, Constant::mainCanvasSize.Height), CombineModeReplace);

			oldLeftTopX = leftTopX;
		}


		//���ؼ�����ɺ������ ���λ��
		GP Point loadPlat() {
			Sprite entrance, exit;
			loadRenderData(entrance, exit);
			int x, y;
			auto it = mapArea->pointIterator(Constant::GRID_CELL.Width, Constant::GRID_CELL.Height);
			//nά�����size()�������nά����Ĵ�С ������������һά x y z�ֱ���3 2 1ά (h w)��(x y)�ֱ���2 1
			while (it.iterate(x, y)) {
				renderPlot(x, y);
			}
			//�����������б� �б�
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
			//��Ⱦ������� (������ͼƬ)
			maper.draw(transferSub, transferCol, entrance.getRect());
			maper.draw(transferSub, transferCol, exit.getRect());
			//maper.draw(16, 0, entrance.getRect());
			//maper.draw(16, 0, exit.getRect());

			return GP Point(entrance.getRect().X, entrance.getRect().Y);
		}

		//�༭��̬:ʹbody���ĵ�ͼ������ȾΪ��Ⱦ��Ϊid�ĵؿ�
		void edit(Sprite const &body, int id) {
			auto it = body.pointIterator(Constant::GRID_CELL.Width, Constant::GRID_CELL.Height);
			reloadRender(it, id);
			renderPlot(it);
		}

		//��ͼ����(�Դ˵�ͼΪ��������ӻ��� δ��ȫʵ�� �������)
		int block(Sprite const &nextBody) {
			if (!mapArea->contains(nextBody))
				return true;//�߳���ͼ
			else {
				auto it = nextBody.pointIterator(Constant::GRID_CELL.Width, Constant::GRID_CELL.Height);
				int x, y;
				// @ TODO
				return false;
				while (it.iterate(x, y)) {
					int mapCellData = renderData.getVertexValue(Constant::rowSub(y), Constant::colSub(x));
					assert(mapCellData >= 0);
					//��Ч->����ͨ��->�赲->����true
					if (!renderData.isVertexValid(Constant::rowSub(y), Constant::colSub(x))) {
						//�赲�ؿ�
						return true;
					}
					else {
						continue;
					}
				}
				//�������赲�ؿ�
				return false;
			}
		}

		//Ѱ·
		bool pickUpFollow(Sprite const &body, Sprite const &target, ShortestPathResult &result) {
			static Graph &parityGraph = renderData.parityGraph();
			static SubTwain limitSub(mapArea->rowsLimit(), mapArea->colsLimit());
			ArrayList<int> &dist = result.dist, &path = result.path;

			//@TODO �������͵��ж�(������һ�����п��������޷�ͨ��)
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

		// ע���ͼ�¼�
		void registration() {
			// �����¼�
			FOR(it, transmissionList.begin(), transmissionList.end()) {
				this->sendImpactEvent(*it);
			}

			// ע���ͼ�㷨λ�� ���ݽ�ɫ�ĳ�ʼλ�ó�ʼ�� oldLeftTopX
			setScreenMode();
			oldLeftTopX = leftTopX;
		}

		// �����ͼ�߼�
		bool run();

	private:
		//��Ⱦx y���ĵؿ鵽��ͼ�Ķ�Ӧλ��(x, y)
		void renderPlot(int x, int y) {
			maper.draw(
				renderData.getVertexValue(Constant::rowSub(y), Constant::colSub(x))
				, Sprite(Constant::GRID_CELL, x, y)
			);
		}
		//��ȡ��Ⱦ��ͼ������x y���ĵؿ����� ��Ⱦ��Ӧ�ĵؿ�ͼ�ε���������(absoluteX, absoluteY)
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
		//��Ⱦһ�����Ϳ��С�ĵؿ�
		void renderPlot(Sprite::Iterator &pointIterator) {
			int x, y;
			while (pointIterator.iterate(x, y)) {
				renderPlot(x, y);
			}
		}

		void mosaicImage(Bitmap const *, Graphics*);
		
		std::shared_ptr<Timestamp> timestamp;
		
		// ��ͼ��Դ����Աʵ��
		static AnimationManager maper;
		// ��ͼ���� (��ֻ��һ�ŵ�ͼ ��ô������򼴴����� �����ͼ)
		std::shared_ptr<Sprite const> mapArea;
		Colony colony;
		// ��̬��Ϣ����
		WindowSprite areaWindow;
		// ÿ��ʹ�ú���ֶ��ر���������
		FILE *fp = nullptr;
		// ��Ʒʵ�弯, ������Ʒ����ʾ(�õ���ʱ����Ϊ���� ֻ��Ҫ����, ��ɾ���� ������ɾ��ʱ����Ҫ����)
		ArrayList<std::shared_ptr<Item>> dropItem;
	};

}
#endif