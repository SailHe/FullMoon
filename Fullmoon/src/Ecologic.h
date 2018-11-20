#ifndef __Ecologic_H
#define __Ecologic_H
#include "Atlas.h"
#include "CollisionEvent.h"
//Ⱥ����
using Colony = map<int, CollisionBox>;
using BiologyManager = shared_ptr<Biology>;
/*
��̬���� :�籾 ������ Ⱥ�� ����ͨ��, ��Ⱦ����
�ڹ���һ����̬��Ӧ���Զ���Ⱦ Ȼ���ṩ����ͨ�ź���Ʒͨ�ŵĽӿ�
*/
/*
������Σ������ȿ��Ƕ������ϣ�Ȼ���ٿ��Ǽ̳С��μ�Effective C++ (��virtual����Ҫ�̳�)
����ж��ַ�ʽ������Ч�ر�����֮��Ĺ�ϵ����ôӦ��ѡ��һ�������Ĺ�ϵ���ǽ����̶ȵؽ�������ԣ������ߵ�����ԡ�
Ecoregion_
*/
class DisplayArea :RenderManager, public EventImpactManager{
public:
	DisplayArea(Sprite const &mapBody, size_t bioCount)
		: ecoregionMapBody(new Sprite(mapBody)), EventImpactManager(mapBody)
		, ecoregionWindow(WindowSprite(_T("xx��̬")))
		, RenderManager(mapBody)
		, timestamp(new Timestamp()){
		//�������������ǰû�г�ʼ����Ⱦ��ͼ�Ļ���ô���������ʼ��λ�õĴ���
		loadPlat();
		ecoregionWindow.setRect(mapBody.getRect());
		createColony(bioCount);//������Ϊsize
		registration();
	}
	
	DisplayArea &operator=(DisplayArea &&rvalue){
		ecoregionMapBody = std::move(rvalue.ecoregionMapBody);
		return *this;
	}

	//װ�ر����е���Ʒ (ownerID: ��ԭӵ���ߵ�id �˴�����������Ʒ)
	void loadingPackage(Sprite const &location, Package &diededBag){
		while (!diededBag.empty()){
			int id = diededBag.back().first;
			if (--diededBag.back().second <= 0)
				diededBag.pop_back();
			//��ʱ������ڵ�����ʵ������ײ��֮�����ϵ
			dropItem.emplace_back(shared_ptr<Item>(new Item(shared_ptr<Timestamp>(new Timestamp()), location, id)));
			//�������ɵ��¼��ĸ�����Ϊһ����ײ�¼�������ײ��(ʱ���һ��)
			sendImpactEvent(shared_ptr<CollisionBox>(dropItem.back()));
		}
	}
	//�������е�����(��������ʧЧ��Ʒ�������)
	void drawDropItem(){
		for (auto it = dropItem.begin(); it != dropItem.end(); ){
			if ((*it)->isValid()){
				(*it)->shawItem();
				++it;
			}
			else{
				it = dropItem.erase(it);
			}
		}
	}

	WindowSprite &sendMessage(){
		return ecoregionWindow;
	}
	shared_ptr<Sprite const> getBody(){
		return ecoregionMapBody;
	}
	//װ������(��Ļ��ʾ�����Size���������ͬ)
	static void loading(GP Graphics *mapGraphics, Sprite const &cameraSprite, GP Point const &displayCentre){
		RenderManager::cameraSprite = cameraSprite;
		displaySprite = cameraSprite;
		displaySprite.setCentre(displayCentre);
		maper.setGraphics(mapGraphics);
		maper.reLoad(L"map\\grass.png", 15, 16);//Ұ���ͼ
		maper.reLoad(L"map\\teleporter.png", 1, 1);//������
		int rSub = maper.reLoad(L"world.png", 15, 7);//Ұ���ͼ2
	}

	void mosaicTransition(Bitmap const *plat){
		mosaicImage(plat, maper.getGraphics());
	}
	
	void getFileByname(char *fileName, char *openMode){
		errno_t err;
		//����ɹ�����0��ʧ���򷵻���Ӧ�Ĵ�����롣
		if ((err = fopen_s(&fp, fileName, openMode)) == 0){
			return;
		}
		_DEBUG_ERROR("�ļ���ʧ��");
	}
	bool saveRenderData(){
		bool st = false;
		getFileByname("map0.dat", "wb");
		st = saveRender(fp);
		std::fclose(fp);
		return st;
	}
	bool loadRenderData(Sprite &entrance, Sprite &exit){
		bool st = false;
		getFileByname("map0.dat", "rb");
		st = loadRender(fp, entrance, exit);
		std::fclose(fp);
		return st;
	}

	//����ͼ�ƶ���������ڵĵط�(��Ⱦ�ɼ�����)
	void renderDisplayMap(){
		//��֪������������ ��Ⱦ��Ӧ����ĵ�ͼ��plat //, Bitmap *&displayPlat
		int x, y;
		int mapX, mapY;
		//��Ⱦλ��(��ʾ����)
		Sprite mapRenderSprite = displaySprite;
		auto mapIter = mapRenderSprite.pointIterator(Constant::GRID_CELL.Width, Constant::GRID_CELL.Height);

		//��Ⱦ��Դ
		Sprite renderOriginSprite = RenderManager::cameraSprite;
		renderOriginSprite.setSize(mapRenderSprite.getRect().Width, mapRenderSprite.getRect().Height);
		renderOriginSprite.setCentre(RenderManager::cameraSprite.getCentre());
		auto it = renderOriginSprite.pointIterator(Constant::GRID_CELL.Width, Constant::GRID_CELL.Height);
		//nά�����size()�������nά����Ĵ�С ������������һά x y z�ֱ���3 2 1ά (h w)��(x y)�ֱ���2 1
		while (it.iterate(x, y)){
			mapIter.iterate(mapX, mapY);
			renderPlot(x, y, mapX, mapY);
		}
	}


	//byte** MapData;// ��ͼ����

	static int leftTopX;// ��Ļ�Ӵ��ڵ�ͼ�е�λ��
	static int oldLeftTopX;
	static int leftTopY;

	//byte lightTime;
	//byte param;

	// ÿ�ŵ�ͼ������
	//static int cellWidth = 8; // ��ͼԪ����ɵĵ�Ԫ���С
	//static int cellHeight = 8; // ��ͼԪ����ɵĵ�Ԫ���С
	//int mapWNum; // ��ͼ��x����Ԫ��ĸ������˲���������ͼ�Ĵ�С��ÿ�ŵ�ͼ���в�ͬ�ĳߴ�
	//int mapHNum; // ��ͼ��y����Ԫ��ĸ������˲���������ͼ�Ĵ�С��ÿ�ŵ�ͼ���в�ͬ�ĳߴ�

	// �ӵ�1�ŵ�ͼ����5�ŵ�ͼ��mapWNum 96��192��64��64��96

	int w = RenderManager::cameraSprite.getLocation().X + 500, h = w;// ������ͼƬ�ߴ�
	int clipX = 0;// ��Ļ�ָ�㣬��������Ļ�Ͻ���ѡ����ȡֵΪ(0,w]
	int drawX = 0;// ��Ļ���Ƶ㣬����ͼƬ����������Ļ���룬ȡֵΪ(-w,0]
	int oldBufClipX = RenderManager::cameraSprite.getLocation().X;// ��һ�λ������ָ�㣬����Ļ�ָ����һ���Ĺ�ϵ��ȡֵΪ[0,w)
	int bufClipX = oldBufClipX;// �������ָ�㣬����Ļ�ָ����һ���Ĺ�ϵ��ȡֵΪ[0,w)
	int bufDrawX = 0;// ���������Ƶ㣬�����ڻ������ϻ��Ƶ�ͼ��Ϣ��ȡֵΪ[0,w)��ע�����Ϊ��ɢֵ��i*cellWidth

	//Image *imageBuffer = nullptr;// ������
	//Graphics *bg;// �������Ļ���
	//Engine eg;


	// ���Ƶ�ͼ�ķ��� Graphics &g
	void draw(Image *imageBuffer) {
		drawBuffer(*maper.getGraphics());// bg
		drawCarmarkMap(*maper.getGraphics(), imageBuffer);
	}

	// �ػ��ͼ�������ķ���
	void drawBuffer(Graphics &g) {
		leftTopX = RenderManager::displaySprite.getLocation().X;
		if (oldLeftTopX == leftTopX) {// �����Ļû�з���������ֱ�ӷ���
			return;
		}

		// bufClipX�������ָ�㣬����Ļ�ָ����һ���Ĺ�ϵ��ȡֵΪ[0,w)
		bufClipX += leftTopX - oldLeftTopX;
		if (bufClipX < 0) {// �����ߵ�����
			bufClipX += w;
		}
		else if (bufClipX > w) {// �����ߵ�����
			bufClipX -= w;
		}

		if (leftTopX - oldLeftTopX > 0) {// ������
			// System.out.println("��");
			if (abs(oldBufClipX - bufClipX) > 200) {// oldBufClipX��bufClipX�ڻ����������ˣ�
				// ��������ѡ�����ֱ���䣡
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
		else {// ������
			// System.out.println("��");
			if (abs(oldBufClipX - bufClipX) > 200) {// oldBufClipX��bufClipX�ڻ����������ˣ�
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

	//���ƻ�������ѡ���ĵ�ͼ
	 void drawClipBuffer(Graphics &g) {

		// �����ʼ���ӵ�������
		int iStart = 0;// ����y�������򣩵���ʼ��ͼ��Ԫ������

		int jStart = 0;
		if (leftTopX - oldLeftTopX > 0) {// ����
			jStart = (leftTopX + w) / Constant::GRID_CELL.Width; // ����x���򣨺��򣩵���ʼ��ͼ��Ԫ������
		}
		else if (leftTopX - oldLeftTopX < 0) {// ����
			jStart = leftTopX / Constant::GRID_CELL.Width;
		}

		// ������Ƹ��ӵ�y������Ŀ��x������Ŀ
		int iNum = 21;// ����y���򣩿�����ʾ�ĵ�ͼ��Ԫ����Ŀ
		int jNum = 1;// ����x���򣩿�����ʾ�ĵ�ͼ��Ԫ����Ŀ

		// ����ڻ������Ļ�����ʼ�������
		bufDrawX = bufClipX / Constant::GRID_CELL.Width * Constant::GRID_CELL.Width;

		// ������һ�εĻ������������������
		if (oldBufClipX / Constant::GRID_CELL.Width != bufClipX / Constant::GRID_CELL.Width) {
			if (leftTopX - oldLeftTopX > 0) {// ����
				jStart -= 1;
				jNum += 1;
				bufDrawX -= Constant::GRID_CELL.Width;
				// System.out.println("����");
			}
			else if (leftTopX - oldLeftTopX < 0) {// ����
				// jStart+=1;
				jNum += 1;

			}

		}
		SubTwain subs = renderData.getLimitSub();
		// System.out.println(jStart);
		// System.out.println(bufDrawX);
		// ��ֹ����Խ��
		if (iStart + iNum >= subs.r) {// Y�᷽������
			iNum = subs.r - 1 - iStart;
		}
		if (jStart + jNum >= subs.c) {// X�᷽������
			jNum = subs.c - 1 - jStart;
		}

		// ʹ��˫������Ƶ�ͼ
		for (int i = iStart; i <= iStart + iNum; i++) {
			for (int j = jStart; j <= jStart + jNum; j++) { // ˫ѭ������������ͼ����
				if (renderData.getVertexValue(i, j) >= 0) { // ��ͼ������Ԫ�ش��ڵ��ڣ���ʾ����ͼ��
					//maper.draw(renderData.getVertexValue(Constant::rowSub(y), Constant::colSub(x)), Sprite(Constant::GRID_CELL, mapX, mapY));
					renderPlot(
						(j - jStart) * Constant::GRID_CELL.Width + bufDrawX, // ��-jStart������Ϊ��Ļ������X
						(i - iStart) * Constant::GRID_CELL.Height // ��-iStart������Ϊ��Ļ������Y
						);
				}
			}
		}
		// System.out.println("========================");
	}

	//�����˾��᷽������ͼ
	 void drawCarmarkMap(Graphics &g, Image *imageBuffer) {
		
		// clipX��Ļ�ָ�㣬��������Ļ�Ͻ���ѡ����ȡֵΪ(0,w]
		clipX += oldLeftTopX - leftTopX;
		if (clipX <= 0) {// �����ߵ�����
			clipX += w;
		}
		else if (clipX > w) {// �����ߵ�����
			clipX -= w;
		}

		// drawX��ǰ�Ļ��Ƶ㣬����ͼƬ����������Ļ���� ȡֵΪ(-w,0]
		drawX += oldLeftTopX - leftTopX;
		if (drawX <= -w) {// �����ߵ�����
			drawX += w;
		}
		else if (drawX > 0) {// �����ߵ�����
			drawX -= w;
		}

		g.SetClip(Rect(0, 0, clipX, Constant::mainCanvasSize.Height), CombineModeReplace);
		//revise���� g.DrawImage(imageBuffer, drawX, Data.REVISE_Y, 0);// ���ϲ���Ҫ�ػ������
		g.DrawImage(imageBuffer, drawX, 0);// ���ϲ���Ҫ�ػ������
		//renderPlot(drawX, 0);

		g.SetClip(Rect(clipX, 0, Constant::mainCanvasSize.Width - clipX, Constant::mainCanvasSize.Height), CombineModeReplace);
		//g.DrawImage(imageBuffer, Constant::mainCanvasSize.Width + drawX, Data.REVISE_Y, 0);// ���ϱ����ػ������
		g.DrawImage(imageBuffer, Constant::mainCanvasSize.Width + drawX, 0);
		//renderPlot(Constant::mainCanvasSize.Width + drawX, 0);

		g.SetClip(Rect(0, 0, Constant::mainCanvasSize.Width, Constant::mainCanvasSize.Height), CombineModeReplace);

		oldLeftTopX = leftTopX;
	}



	//���ؼ�����ɺ������ ���λ��
	GP Point loadPlat(){
		Sprite entrance, exit;
		loadRenderData(entrance, exit);
		int x, y;
		auto it = ecoregionMapBody->pointIterator(Constant::GRID_CELL.Width, Constant::GRID_CELL.Height);
		//nά�����size()�������nά����Ĵ�С ������������һά x y z�ֱ���3 2 1ά (h w)��(x y)�ֱ���2 1
		while (it.iterate(x, y)){
			renderPlot(x, y);
		}
		//�����������б� �б�
		int transferSub = 18, transferCol = 4;
		entrance.setSize(maper.getWidthA(transferSub), maper.getHeightA(transferSub));
		//entrance.setCentre({ 32 + 5, 24 + 5 });
		exit.setSize(maper.getWidthA(transferSub), maper.getHeightA(transferSub));
		//exit.setCentre({ ecoregionMapBody->getWidth() - 32 - 5, ecoregionMapBody->getHeight() - 24 - 25 });
		shared_ptr<TransmissionEvent> left, right;
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
	void registration();
	
	//�༭��̬:ʹbody���ĵ�ͼ������ȾΪ��Ⱦ��Ϊid�ĵؿ�
	void edit(Sprite const &body, int id){
		auto it = body.pointIterator(Constant::GRID_CELL.Width, Constant::GRID_CELL.Height);
		reloadRender(it, id);
		renderPlot(it);
	}

	//��ͼ����(�Դ˵�ͼΪ��������ӻ��� δ��ȫʵ�� �������)
	int block(Sprite const &nextBody){
		if (!ecoregionMapBody->contains(nextBody))
			return true;//�߳���ͼ
		else{
			auto it = nextBody.pointIterator(Constant::GRID_CELL.Width, Constant::GRID_CELL.Height);
			int x, y;
			while (it.iterate(x, y)){
				int mapCellData = renderData.getVertexValue(Constant::rowSub(y), Constant::colSub(x));
				assert(mapCellData >= 0);
				//��Ч->����ͨ��->�赲->����true
				if (!renderData.isVertexValid(Constant::rowSub(y), Constant::colSub(x))){
					//�赲�ؿ�
					return true;
				}
				else{
					continue;
				}
			}
			//�������赲�ؿ�
			return false;
		}
	}

	//Ѱ·
	bool pickUpFollow(Sprite const &body, Sprite const &target, ShortestPathResult &result){
		static Graph &parityGraph = renderData.parityGraph();
		static SubTwain limitSub(ecoregionMapBody->rowsLimit(), ecoregionMapBody->colsLimit());
		vector<int> &dist = result.dist, &path = result.path;

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

	//չʾ����̬
	bool shawEcologic();

private:
	//��Ⱦx y���ĵؿ鵽��ͼ�Ķ�Ӧλ��
	void renderPlot(int x, int y){
		maper.draw(renderData.getVertexValue(Constant::rowSub(y), Constant::colSub(x)), Sprite(Constant::GRID_CELL, x, y));
	}
	//��ȡx y������Ⱦ�ؿ� ��Ⱦ��mapX, mapY
	void renderPlot(int x, int y, int mapX, int mapY){
		if (renderData.getLimitSub().r <= Constant::rowSub(y)
			|| renderData.getLimitSub().c <= Constant::colSub(x)
			|| Constant::rowSub(y) < 0 || Constant::colSub(x) < 0
			){
			maper.drawInRC(19, 0, Sprite(Constant::GRID_CELL, mapX, mapY));
		}
		else{
			maper.draw(renderData.getVertexValue(Constant::rowSub(y), Constant::colSub(x)), Sprite(Constant::GRID_CELL, mapX, mapY));
		}
	}
	//��Ⱦһ�����Ϳ��С�ĵؿ�
	void renderPlot(Sprite::Iterator &pointIterator){
		int x, y;
		while (pointIterator.iterate(x, y)){
			renderPlot(x, y);
		}
	}

	void mosaicImage(Bitmap const *, Graphics*);
	//����������̬Be�Ĵ�СΪbioCount������Ⱥ ���������(�����Ϊ�� ��ô��������Ĭ�Ͻ���������ڴ���̬��)
	void createColony(size_t bioCount);

	shared_ptr<Timestamp> timestamp;
	//����̬�����г�Ա <body��Ϣ, ʵ��(��̬��Ҫ��ָ��)> ����ջ����������ڴ����� new���ڶ�������
	map<int, BiologyManager> member;
	static AnimationManager maper;//��ͼ��Դ����Աʵ��
	//��̬�����С
	shared_ptr<Sprite const> ecoregionMapBody;
	Colony colony;
	//��̬��Ϣ����
	WindowSprite ecoregionWindow;
	//ÿ��ʹ�ú���ֶ��ر���������
	FILE *fp = nullptr;
	vector<shared_ptr<Item>> dropItem;// ��Ʒʵ�弯, ������Ʒ����ʾ(�õ���ʱ����Ϊ���� ֻ��Ҫ����, ��ɾ���� ������ɾ��ʱ����Ҫ����)
};


#endif