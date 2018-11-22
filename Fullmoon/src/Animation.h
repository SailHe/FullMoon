#ifndef __Animation_H
#define __Animation_H
#include"Sprite.h"

namespace EcologicEngine {

	//������: ��Լ�ڴ�+ʵ�����
	/*������ ��������Դ����ƶ���*/
	class AnimationManager {
		static GP Status pastStatus;
		static WCHAR buf[Constant::BUFFER_MAX_BIT]; WCHAR *const res = buf;/*·��*/
		static Pen* myPen;//��graphicControl����
		static GP Graphics *deviceGraphics_;//�豸���� ��graphicControl����(���ڵ���)
		static GP Graphics *deviceBufferGraphics_;//�豸���滭�� ��graphicControl����
		GP Graphics *graphics_ = nullptr;//�������� �ɸ����ָ��Ķ������


		ArrayList<ArrayList<GP Bitmap *>> animation;/*λͼ�� ���ô�surfaceTexture(��������)*/
		ArrayList<std::pair<size_t, size_t>> indexF;/*��ǰ֡/���֡frame index*/
		//�ͷ�λͼ���ڵ�����λͼ ���ÿ� λͼ���С����(��ʹ�õ�size��Ϊ0)
		void deleteAnimation() {
			FOR(i, 0, animation.size()) {
				FOR(j, 0, animation[i].size()) {
					delete animation[i][j];
					animation[i][j] = nullptr;
				}
			}
			animation.resize(0);
		}

	public:
		//���Խ��:Gdi+δ��ʼ��ǰ����Bitmap�ǿ��Ե�, ���ǻ����ʧ��(״̬��ʾ:δ��ʼ��)
		//deviceBufferGraphics_->DrawImage(&background, GP Rect(0,0,100,100));
		//������ʱ���ƻᱨ�� ��ʼ�������δ��ʼ��ǰ�����λͼû�д�����ҲûЧ��

		//ͼ�ο��� �����ȳ�ʼ����ſ�����ʹ�� �����豸�Դ滭��
		static GP Bitmap *graphicControl(GP Graphics *&deviceBufferGraphics, GP Size const &size) {
			static GdiplusStartupInput m_Gdistart;
			static ULONG_PTR m_GdiplusToken;
			static Bitmap *deviceBuffer = nullptr;
			if (deviceBufferGraphics == nullptr) {//û�г�ʼ��->��ͼ��ʼ��
				/*esyx��ʼ��*/
				srand((unsigned)time(nullptr));
				//initgraph(mainSprite->getWidth(), mainSprite->getHeight());
				//setbkmode(TRANSPARENT);//͸����setwritemode(R2_XORPEN);

				/*Gdiplush��ʼ�� initialization*/
				GdiplusStartup(&m_GdiplusToken, &m_Gdistart, nullptr);
				/*��ȡͼƬ���豸�������*/
				//deviceBuffer = new Bitmap(deviceBufferRes.c_str());
				deviceBuffer = new Bitmap(size.Width, size.Height);
				//@TODO �ر�hdc
				//deviceGraphics_ = GP Graphics::FromHDC(GetDC(GetHWnd()));
				deviceGraphics_ = GP Graphics::FromHDC(GetDC(GetActiveWindow()));
				deviceBufferGraphics = deviceBufferGraphics_ = GP Graphics::FromImage(deviceBuffer);
				myPen = new Pen(GP Color(255, 255, 0, 0));
			}
			else {//��ͼ��ֹ ��ͼж��
				delete deviceBufferGraphics_, myPen;
				deviceBufferGraphics = deviceBufferGraphics_ = nullptr;
				/*ж��Gdiplush ��֮����ʹ��Gdiplush �����Դж��Ҫ��֮ǰ����*/
				GdiplusShutdown(m_GdiplusToken);
			}
			return deviceBuffer;
		}

		//�����ȹ���֮������ʼ��
		AnimationManager() {}

		//Ĭ�ϵĻ������豸����
		//(�ļ�·��, ��Ӧ�����᷽���ϵ���֡��, ��Դͼ��startRowFrame��, startColFrame����ʼ����, �Ӽ��㿪ʼ�ĵ㲽������(��, ��))
		AnimationManager(PTCHAR resPath, int totalFrameXAxis, int totalFrameYAxis, int startRowFrame = 0, int startColFrame = 0, STEP stepDir = STEP_X_AXIS) {
			graphics_ = deviceBufferGraphics_;
			reLoad(resPath, totalFrameXAxis, totalFrameYAxis, startRowFrame, startColFrame, stepDir);
		}

		//��ֵ��ֵ :�������ֱ�λͼ �����������ֱ�λͼ (��֧��)
		AnimationManager& operator=(AnimationManager const &rhs) = delete;
		//��ֵ��ֵ :�������ֱ�λͼ ��������λͼָ�뽻��
		AnimationManager& operator=(AnimationManager &&rvalue) {
			indexF = std::move(rvalue.indexF);
			std::swap(graphics_, rvalue.graphics_);

			deleteAnimation();
			animation.resize(rvalue.animation.size());
			FOR(i, 0, rvalue.animation.size()) {
				animation[i].resize(rvalue.animation[i].size());
				FOR(j, 0, rvalue.animation[i].size()) {
					std::swap(animation[i][j], rvalue.animation[i][j]);
					//��ͼ���õ�Clone������������const ��������
					//partitionBitmap(0, 0, rhs.animation[i][j]->GetWidth(), rhs.animation[i][j]->GetHeight(), animation[i], rhs.animation[i][j]);
					//animation[i][j] = new GP Bitmap(*rhs.animation[i][j]);// ������ֵ
				}
			}
			return *this;
		}
		//��ֵ��������
		AnimationManager(AnimationManager const &rhs) = delete;
		//��ֵ�������� (��ʹ������ʱ��������Ҳֻ�ǽ������ڶ��Ϸ����ָ�� �������setȴ���ܴ�����ʱ��ֵ����)
		AnimationManager(AnimationManager &&rvalue) {
			*this = std::move(rvalue);
		}

		virtual ~AnimationManager() {
			graphics_ = nullptr;
			deleteAnimation();
		}

		/*(Ĭ�Ϻ�ɫ)���ػ��� ����͸��ֵ(��ʾ����ɫ�뱳��ɫ��ϵĳ̶�)���졢�̡���*/
		GP Pen* pen(BYTE a = 255, BYTE r = 0, BYTE g = 0, BYTE b = 0) {
			delete myPen;
			myPen = new GP Pen(GP Color(a, r, g, b));
			return myPen;
		}

		/*��������ԭ��(��ߴ���Ч�� ͬʱ���п��ǲ����õ����Ѷ�)*/

		/*�������� (����, ����, ���λ��)   ��������ʹ��*/
		void drawInRC(int rows, int cols, GP Point location) {
			graphics_->DrawImage(animation[rows][cols], location);
		}
		/*�������� (����, ����, ���λ��)   ��������ʹ��*/
		void drawInRC(int rows, int cols, Sprite const &location) {
			graphics_->DrawImage(animation[rows][cols], location.getRect());
		}
		/*�������� (����, ����, ����)*/
		void draw(int rows, int cols, GP Rect const &area) {
			graphics_->DrawImage(animation[rows][cols], area);
		}
		/*��Ż��� (λͼ���, ����) tips:ֻ֧�����0->�ҽ�Nһ�ֱ�ŷ�ʽ*/
		void draw(int num, Sprite const &area) {
			int h = animation.size(), w = 0;
			if (h == 0) {
				_DEBUG_ERROR("δװ���κ���Դ");
				return;
			}
			else {
				w = animation[0].size();
			}
			draw(num / w, num % w, area.getRect());
		}
		//�������� (�ɶ����� PS: ע��ʹ�õ�rSub��body�Դ���)
		void draw(Sprite &body) {
			static int baseRate = 1;//��׼���� ԭ��npc��5(��ֹ�����ٶȹ���) ���Ϊ1��ֹת��ʱ��ͼδ����
			RowsSub rSub = body.currentView();
			indexF[rSub].first %= animation[rSub].size()*baseRate;
			draw(rSub, indexF[rSub].first / baseRate, body.getRect());
			body.isDynamic() ? ++indexF[rSub].first
				: body.getStaticFrame() == ORIGIN_FRAME ? indexF[rSub].first = 0 : 0;
		}

		size_t getWidthA(size_t i = 0)const;
		size_t getHeightA(size_t i = 0)const;

		GP Graphics *getGraphics() {
			return graphics_;
		}
		//ָ����ͼ���� (��ֵ�޶�: ֻ�ܴ����ѱ������ͼ�ζ���)
		void setGraphics(GP Graphics *&graphics) {
			graphics_ = graphics;
		}
		//Ĭ�ϻ�ͼ����
		void setGraphics() {
			graphics_ = deviceBufferGraphics_;
		}

		//��ȡ��ͼԴ�б�
		int getOriginRowsSub(PTCHAR name) const {
			auto it = originSubTable.find(name);
			if (it != originSubTable.end()) {
				return it->second;
			}
			else {
				_DEBUG_ERROR("���ֲ�����");
				return -1;
			}
		}

		//�������� ������Դ��rSub(id)    <<<*���μ���ֻ֧�־��ȷָ�; ֮���Խ��з���ķָ�֡������ �е�ǰ�������ںܶ�ͼƬ��ֻ��һ�� �����ڶ���*>>>
		//(�ļ�·��, ��Ӧ�����᷽���ϵ���֡��, ��Դͼ��startRowFrame��, cols����ʼ����, �Ӽ��㿪ʼ�ĵ㲽������(��, ��)); ���û�б��� ��ô��ʹ�� ͼƬ��name��ȡԴrSub(xx\\name.xx)
		int reLoad(
			PTCHAR resPath, int totalFrameXAxis = 1, int totalFrameYAxis = 1
			, int startRowFrame = 0, int startColFrame = 0
			, int loadFX = -1, int loadFY = -1
			, STEP stepDir = STEP_X_AXIS
			, WCHAR const *alias = nullptr) {
			static std::map<GP Status, std::string> statusMap = {
				{ Ok, "OK" },
				{ GenericError, "GenericError" },
				{ InvalidParameter, "InvalidParameter" },
				{ OutOfMemory, "OutOfMemory" },
				{ ObjectBusy, "ObjectBusy" },
				{ InsufficientBuffer, "InsufficientBuffer" },
				{ NotImplemented, "NotImplemented" },
				{ Win32Error, "Win32Error" },
				{ WrongState, "WrongState" },
				{ Aborted, "Aborted" },
				{ FileNotFound, "FileNotFound" },
				{ ValueOverflow, "ValueOverflow" },
				{ AccessDenied, "AccessDenied" },
				{ UnknownImageFormat, "UnknownImageFormat" },
				{ FontFamilyNotFound, "FontFamilyNotFound" },
				{ FontStyleNotFound, "FontStyleNotFound" },
				{ NotTrueTypeFont, "NotTrueTypeFont" },
				{ UnsupportedGdiplusVersion, "UnsupportedGdiplusVersion" },
				{ GdiplusNotInitialized, "GdiplusNotInitialized" } };
			wcscpy_s(res + 4, Constant::BUFFER_MAX_BIT - 4, resPath);
			int rSub = loadResource(startRowFrame, startColFrame, totalFrameXAxis, totalFrameYAxis, loadFX, loadFY, stepDir);
			std::string errMesg = statusMap[pastStatus].c_str();
			assert(rSub >= 0);
			static std::wstring name;
			if (alias == nullptr) {
				int len = lstrlenW(res);
				int rDotIndex = len;
				for (int i = len; i > 0; --i) {
					if (res[i] == '.' && rDotIndex == len) {
						rDotIndex = i;
					}
					if (res[i] == '\\') {
						name = std::wstring(res + i + 1, res + rDotIndex);
						break;
					}
				}
			}
			else {
				name = std::wstring(alias);
			}
			if (DEBUG) {
				assert(originSubTable.find(name) == originSubTable.end());
			}
			originSubTable.insert(make_pair(name, rSub));
			return rSub;
		}

		//��������(��ʹ�� ͼƬ����alias��ȡԴrSub)
		int reLoad(
			PTCHAR resPath, std::wstring const &alias
			, int totalFrameXAxis = 1, int totalFrameYAxis = 1
			, int startRowFrame = 0, int startColFrame = 0
			, int loadFX = -1, int loadFY = -1
			, STEP stepDir = STEP_X_AXIS) {

			return reLoad(
				resPath, totalFrameXAxis, totalFrameYAxis
				, startRowFrame, startColFrame
				, loadFX, loadFY
				, stepDir, &alias[0]);
		}

	private:
		/*��ȡͼ���ĳһ���� (��ȡ����, �洢����(push_back), Դλͼ)*/
		static void partitionBitmap(int x, int y, int Width, int Height
			, ArrayList<GP Bitmap*> &animation, GP Bitmap &imageRes) {

			static RectF block = RectF();
			block.X = (REAL)x, block.Y = (REAL)y, block.Width = (REAL)Width, block.Height = (REAL)Height;
			//GP Image *enlargeImage = imageRes.Clone(block, PixelFormatDontCare)->GetThumbnailImage(enlargeProportion.Width, enlargeProportion.Height, NULL, NULL);
			animation.push_back(imageRes.Clone(block, PixelFormatDontCare));/*��¡ͼƬ�Ĳ���*/
			if (DEBUG) {
				//deviceGraphics_->DrawImage(animation.back(), 300, 300, 200, 200); //FOR DEBUG
			}
		}

		//newһ����sr������ͬ�ַ���WCHAR��ֵ������ ����(δʹ��)
		WCHAR *toWCHAR(WCHAR *&res, const char* sr, size_t len) {
			res = (WCHAR *)malloc(len * sizeof(WCHAR));
			res[len] = '\0';//wcharת��Ϊchar _MaxCount ���ַ���
			//mbstowcs(res, sr, len);
			//mbstowcs_s(size_t(10),res, sr, len);
			return res;
		}

		/*������Դ
		(ԭͼƬ�����������֡ Ĭ�ϱ�ʾ���к��еĵ�0֡��ʼ
		, x��y�����ϵ���֡��fx��fy �� ����������
		, x��y������Ҫ���ص�֡��-1���������ֹ���ʱ��ʾ����ʣ�����е�
		, ���Ȳ����ķ��� ��˵����һ�鶯���ڼ����ָ��ͼƬ���Ǻ��ŵĻ������ŵ�)
		 ���ش˴μ��صĶ���id(��ά�����������rSub)  �����س��� ���� -LastStatus*/
		int loadResource(
			int startRowFrame = 0, int startColFrame = 0
			, int totalFrameXAxis = 1, int totalFrameYAxis = 1
			, int loadFX = -1, int loadFY = -1
			, STEP stepDir = STEP_X_AXIS) {

			static int width = 0, height = 0;
			static std::wstring pastRes;
			//��ͼԴλͼ
			static std::shared_ptr<Bitmap> imageResource;
			if (pastRes == res) {
				// do nothing
			}
			else {
				imageResource.reset(new Bitmap(res));
				pastRes = res;
			}
			Bitmap &imageRes = *imageResource;

			pastStatus = imageRes.GetLastStatus();
			if (pastStatus != GP Status::Ok) {
				return -pastStatus;
			}
			width = imageRes.GetWidth() / totalFrameXAxis, height = imageRes.GetHeight() / totalFrameYAxis;/*֡��ʵ�ʿ��(��ԴͼΪ׼)*/
			if (width*totalFrameXAxis != imageRes.GetWidth()) {
				_DEBUG_ERROR("��Դ�ĺ�֡����������");
			}
			if (height*totalFrameYAxis != imageRes.GetHeight()) {
				_DEBUG_ERROR("��Դ����֡����������");
			}

			int forwardFrame = 0;//���������ϵ� ֡��
			int w = 0, h = 0;//���������ϵ� ֡���[����]
			int iRow = 0, iCol = 0;//��λ��(y) ��λ��(x) 

			//����ʣ����֡�� �Լ��ܼ��ص�֡��
			totalFrameXAxis -= startColFrame;
			totalFrameYAxis -= startRowFrame;
			if (loadFX < 0 || loadFX > totalFrameXAxis) {
				loadFX = totalFrameXAxis;
			}
			if (loadFY < 0 || loadFY > totalFrameYAxis) {
				loadFY = totalFrameYAxis;
			}

			/*���������ж�*/
			if (stepDir == STEP_X_AXIS) {
				forwardFrame = loadFX, w = width, h = height;
				iRow = startRowFrame * h, iCol = startColFrame * w;
			}
			else {
				forwardFrame = loadFY, w = height, h = width;
				iCol = startRowFrame * h, iRow = startColFrame * w;
			}

			int oldSize = animation.size();//Ϊ��֧��reLoad
			int increasedSize = loadFX + loadFY - forwardFrame;//�˴ν����ض����ĸ���(��Ҫ������ٸ�������)
			animation.resize(oldSize + increasedSize);
			indexF.resize(oldSize + increasedSize);

			for (int id = oldSize; id < oldSize + increasedSize; ++id, iRow += h) {//����(��)����
				indexF[id].second = forwardFrame;
				for (int f = forwardFrame, x = iCol; --f >= 0; x += w) {//֡(��)����
					if (stepDir == STEP_X_AXIS) {
						partitionBitmap(x, iRow, width, height, animation[id], imageRes);
					}
					else {
						//�ڲ�������Ϊy��ʱiRow��ʵ���������iCol x��ʵ��������y
						//����ͼ�����δ��� ��Ҫ����ⲽ������Ϊx��ʱ�����
						partitionBitmap(iRow, x, width, height, animation[id], imageRes);
					}
				}
			}
			return oldSize;
		}

		//std::map<std::wstring, SkId>������ <name id>
		std::map<std::wstring, SkId> originSubTable;
		//�Ŵ�ϵ��(������С) û��ʹ��
		static GP SizeF enlargeProportion;
	};

	/*��������  ��������(��Ϣ��� + ��Ϣչʾ)*/
	class WindowSprite : public Sprite {
		void alloc() {
			memset(windowTitle, 0, Constant::BUFFER_MAX_BIT * sizeof(WCHAR));
			memset(messageBuffer, 0, Constant::BUFFER_MAX_BIT * sizeof(WCHAR));
		}
	public:
		WindowSprite()
			:Sprite(Constant::mainCanvasSize) {
			alloc();
		}
		WindowSprite(Sprite const &sprite)
			:Sprite(sprite) {
			alloc();
		}
		//titleĬ����ʾ���Ϸ��в� Ĭ��������ͼ��С
		WindowSprite(WCHAR const *title)
			:Sprite(Constant::mainCanvasSize) {
			alloc();
			wcscpy_s(windowTitle, title);
		}

		WindowSprite &operator=(WindowSprite const &rhs) {
			alloc();
			wcscpy_s(messageBuffer, rhs.messageBuffer);
			wcscpy_s(windowTitle, rhs.windowTitle);
			return *this;
		}
		WindowSprite(WindowSprite const &rhs) {
			*this = rhs;
		}
		virtual ~WindowSprite() {
		}

		/*********************************������ͼ�λ��Ʒ���****************************/
		void drawPoint(GP Point const &p) {
			// esayX�������ص�
			// putpixel(p.X, p.Y, BLACK);
			// @TODO
			drawCircle(p, 1);
		}
		void drawLine(GP Point const &o, GP Point const &e) {
			messenger.getGraphics()->DrawLine(messenger.pen(255, 128, 0, 0), o, e);
		}
		//��������
		void drawRay(Ray const &ray, int length) {
			static LineSegment lineBuffer;
			lineBuffer = ray.toLineSegment(length);
			messenger.getGraphics()->DrawLine(messenger.pen(255, 128, 0, 0), lineBuffer.getStartPoint(), lineBuffer.getEndPoint());
		}
		void drawRectangle(GP Rect const &rect) {
			messenger.getGraphics()->DrawRectangle(messenger.pen(255, 0, 128, 0), rect);
		}
		void drawPolygon(ArrayList<GP Point> const &points) {
			messenger.getGraphics()->DrawPolygon(messenger.pen(), &points[0], points.size());
		}
		void drawEllipse(int left, int top, int adllWidth, int allHeight) {
			messenger.getGraphics()->DrawEllipse(messenger.pen(), left, top, adllWidth, allHeight);
		}
		//(���, adllWidth, allHeight)
		void drawEllipse(GP Point const &LT, int adllWidth, int allHeight) {
			drawEllipse(LT.X, LT.Y, adllWidth, allHeight);
		}
		//Բ�� �뾶
		void drawCircle(int cX, int cY, int radius) {
			//��Բ�Ĳ����������λ�� �� diameter(ֱ��)   esayX��Բ����: circle()
			messenger.getGraphics()->DrawEllipse(messenger.pen(255, 0, 128, 0), cX - radius, cY - radius, radius * 2, radius * 2);
		}
		void drawCircle(GP Point const &centre, int radius) {
			drawCircle(centre.X, centre.Y, radius);
		}
		void drawCircle(CircleShape const &circle) {
			drawCircle(circle.getCentre(), circle.getRadius());
		}
		void WindowSprite::text(WCHAR const *message, int left, int top, int fontSize, GP Color color)const {
			GP SolidBrush solidBrush(color);
			messenger.getGraphics()->DrawString(message, -1, font(fontSize), GP PointF((GP REAL)left, (GP REAL)top), &solidBrush);
		}
		//�ڱ���������ʾ�ı�
		void WindowSprite::text(WCHAR const *message, int fontSize, GP Color color)const {
			text(message, *this, fontSize, color);
		}
		//��ָ�����ο�����area����ʾ�ı���Ϣ(������ο��޷���ʾ�����ִ� ��ô�����Ĳ��ֻᱻ�ض� ������: �����С����������ǰ֪��)
		void WindowSprite::text(WCHAR const *message, Sprite const &area, int fontSize, GP Color color) const {
			GP SolidBrush solidBrush(color);
			messenger.getGraphics()->DrawString(message, -1, font(fontSize), area.tmpRectF(), &StringFormat(), &solidBrush);
		}
		void WindowSprite::text(WCHAR const *message, size_t len, Sprite const &area, int fontSize, GP Color color) const {
			GP SolidBrush solidBrush(color);
			messenger.getGraphics()->DrawString(message, len, font(fontSize), area.tmpRectF(), &StringFormat(), &solidBrush);
		}

		/*********************************��Чͼ�λ��Ʒ���****************************/
		void onLoding() {
			static Bitmap logo(L"res\\load\\dq.png");
			//���Ŷ���ScaleTransform; �ָ�ResetTransform;
			//messenger.getGraphics()->ScaleTransform(0.5f, 0.5f);
			messenger.getGraphics()->DrawImage(&logo
				, Rect(
					getWidth() / 2 - logo.GetWidth() / 2,
					getHeight() / 2 - logo.GetHeight() / 2,
					logo.GetWidth(), logo.GetHeight())
			);
		}
		/*�������Ч*/
		void shawHit() {
			messenger.draw(mouse);
		}
		bool massageBox(PTCHAR massage, PTCHAR tips = _T("tips")) {
			// ��һ����������Ϊ0����һ�������������Ĵ��ڵı�ţ��൱��˭�����Ĵ��ڣ�0Ϊϵͳ�������ڶ��������ǶԻ�������ݣ�
			// �����������ǶԻ���ı��⣬���ĸ������ǶԻ������ͣ����ó�0����
			//return MessageBox(GetHWnd(), massage, tips, MB_YESNO) == IDYES;
			return MessageBox(0, massage, tips, MB_YESNO) == IDYES;
		}
		//��ʾ����
		void drawNumber(int value) {
			//���ֵ�Դ�б�
			static size_t rSub = messenger.getOriginRowsSub(_T("number"));
			static int numW = messenger.getWidthA(rSub), numH = messenger.getHeightA(rSub);//һ�����ַ��ŵĿ��
			int aValue = abs(value);

			//������ʾ���ִ�С
			setSize(numW, numH);
			//��ʾ������
			if (value < 0) {
				//������ͼ
				messenger.draw(rSub, 12, getRect());
			}
			else if (value > 0) {
				//������ͼ
				messenger.draw(rSub, 11, getRect());
			}
			rightward(numW*totalBitOf(aValue));
			while (aValue > 0) {
				//��������
				messenger.draw(rSub, aValue % 10, getRect());
				aValue /= 10;
				rightward(-numW);
			}
		}
		//�Ի�
		void dialogue(WCHAR const *message) {
			static int rSub = messenger.getOriginRowsSub(_T("lableBack"));
			Sprite dialogueSprite = *this;
			dialogueSprite.rightward(dialogueSprite.getWidth() / 4);
			dialogueSprite.heighten(0.1);
			dialogueSprite.broaden(0.5);
			messenger.draw(rSub, 0, dialogueSprite.getRect());
			dialogueSprite.downward(dialogueSprite.getHeight() / 4);
			int len = lstrlenW(message);
			dialogueSprite.rightward(dialogueSprite.getWidth() / 3);
			int i = 0, lineLen = 13;
			while (len >= 0) {
				//һ����ʾ10������
				len -= lineLen;
				text(message + lineLen * i++, len > 0 ? lineLen : lineLen + len, dialogueSprite, 15, BLACK_);
				dialogueSprite.downward(dialogueSprite.getHeight() / 4);
			}
		}
		/*��ֵ��*/
		void speValBar(int value, int fullValue, GP Color color);
		/*��һ����ֵ�ı����嵽 ��Ϣ������*/
		void specificValueToBuffer(int value, int fullValue, std::wstring  const &str = L"");
		//���Ʋ˵�����
		void drawMenuBack() {
			static int rSub = messenger.getOriginRowsSub(_T("statusBack"));
			messenger.draw(rSub, 0, getRect());
		}
		//����һ����ť����(�����Զ�����)
		void drawButtonBack(Sprite const &buttonArea, PTSTR bottonName) {
			static int rSub = messenger.getOriginRowsSub(_T("buttonBack"));
			static Sprite buttonBackAreaTemp;
			messenger.draw(rSub, 0, buttonArea.getRect());
			buttonBackAreaTemp = buttonArea;
			//buttonBackAreaTemp.setLocation(buttonBackAreaTemp.getCentre());
			buttonBackAreaTemp.rightward((int)(Constant::mainCanvasSize.Width*0.015));//30-0.021
			buttonBackAreaTemp.downward((int)(Constant::mainCanvasSize.Height*0.020));//20-0.024
			text(bottonName, buttonBackAreaTemp, 15, BLACK_);
		}
		/*��Ʒ���� �����������б����е�������Ʒ ������Ʒ�ĸ��Ӵ���ItemsBox��*/
		void inventoryTabs(Package const &bag, ArrayList<Sprite> &ItemsBox);
		//��area��չʾһ��״ֵ̬(name: status/fullstatus)
		void drawStatusBar(size_t left, size_t top, std::wstring const &name, int statusValue, int statusFullValue, GP Color color = BLACK_) {
			specificValueToBuffer(statusValue, statusFullValue, name);
			//�����С����״̬�򱳾�����Ӧ
			text(messageBuffer, left + (int)(Constant::mainCanvasSize.Width*0.005), top, getWidth() / 24, color);
		}
		/*չʾ���Ϊid��һ����Ʒ*/
		void drawItem(size_t id, GP Point const &position) {
			messenger.drawInRC(id / 16, id % 16, position);
		}

		/***********************************���ܸ�������******************************************/
		static int totalBitOf(int number) {
			int top = 1;
			while (number /= 10)top++;
			return top;
		}
		//��ʾ����
		void shawTitle() {
			//�Ϸ��в�
			Sprite upCentre = *this;
			upCentre.downward((int)(getHeight()*0.1));
			upCentre.rightward(getWidth() / 4);
			text(windowTitle, upCentre, 10, BLACK_);
		}
		//����id�����Ʒ�����ؿ�
		size_t getWidthOfItem(size_t id) {
			return messenger.getWidthA(id / 16);
		}
		//����id�����Ʒ�����ؿ�
		size_t getHeightOfItem(size_t id) {
			return messenger.getHeightA(id / 16);
		}
		//չʾһ��ͼƬ��ȫͼ�Լ��ɵ������ ��������λ�����imageLimitRectList��
		void WindowSprite::imageSelect(LinkedList<RectF> &imageLimitRectList, bool emplace = true) {
			//��ID(�����û�������ȷ����ŷ�ʽ) ����ͼƬ������Դ��ַ
			static Bitmap image(L"res\\map\\grass.png");//Ұ���ͼ (maper�����м���)
			auto rect = getRect();
			Sub rows = 16, cols = 15;
			//һ��ѡ��Ԫ�Ĺ̶����
			int w = 16, h = 16;

			rect.X += 16, rect.Y += 16;
			rect.Width -= 32, rect.Height -= 32;

			REAL allW = (REAL)w*cols, allH = (REAL)h*rows;
			REAL fw = (REAL)w*(rect.Width / allW), fh = (REAL)h*(rect.Height / allH);
			messenger.getGraphics()->DrawImage(&image, rect);
			GP PointF &position = GP PointF((REAL)rect.X, (REAL)rect.Y);
			auto lambdaFun = [&]() {
				//��λ��
				RectF limitRect;
				FOR(r, 0, rows) {
					FOR(c, 0, cols) {
						//limitRect = GP RectF(position.X - 1, position.Y - 2, fw + 1, fh + 1);
						limitRect = GP RectF(position.X, position.Y, fw, fh);
						if (emplace) {
							imageLimitRectList.emplace_back(limitRect);
						}
						messenger.getGraphics()->DrawRectangle(messenger.pen(), limitRect);
						position.X += fw;
					}
					position.X = (REAL)rect.X;
					position.Y += fh;
				}
				return true;
			};
			lambdaFun();
		}

		/*�������ж�*/
		/*ѡ��ʽ�Ի���*/
		/*�ı�ʽ�Ի���*/
		static Sprite &getMouse() {
			return mouse;
		}
		static void setMouse(int x, int y) {
			mouse.setLocation(x, y);
			mouse.setDynamic(true);
			mouse.setCentre();
		}
		/*װ�غ�ʹ��->���ڴ����ظ�λͼʱ��ʹ�������ڴ����ģʽ*/
		static void loading() {
			messenger.setGraphics();
			bool moving = false;
			int rSub;
			//��Ʒ��
			rSub = messenger.reLoad(L"items.png", 16, 8);

			//״̬����
			rSub = messenger.reLoad(L"part\\statusBack.png");

			//��ť
			rSub = messenger.reLoad(L"part\\buttonBack.png");

			//���ֱ�ǩ����
			rSub = messenger.reLoad(L"part\\label.png", _T("lableBack"));

			//��Ʒ��ǩ
			rSub = messenger.reLoad(L"rfont\\4.png", L"itemLable");

			//����
			rSub = messenger.reLoad(L"part\\number.png", 13, 5);

			//��ͨ����
			rSub = messenger.reLoad(L"battle\\effect\\atk\\1.png", L"atk");

			//��Ч
			rSub = messenger.reLoad(L"battle\\effect\\boom\\dark.png", 8);

			rSub = messenger.reLoad(L"battle\\effect\\light\\light.png", 6);

			rSub = messenger.reLoad(L"battle\\effect\\bullet\\dark.png", L"darkbullet", 6); moving = true;

			mouse.setSize(messenger.getWidthA(rSub), messenger.getHeightA(rSub));
			mouse.setOriginView(rSub);

			rSub = messenger.reLoad(L"battle\\music.png", 12); moving = true;

			rSub = messenger.reLoad(L"battle\\effect\\gas.png", 4);

		}
	private:
		/*����·�����仭ˢ*/
		GP PathGradientBrush* pathBrush(BYTE a, BYTE r, BYTE g, BYTE b, GP Rect area) {
			// Create a path that consists of a single ellipse.
			static GraphicsPath path;
			path.AddRectangle(area);

			// Use the path to construct a brush.
			static PathGradientBrush pthGrBrush(&path);

			// Set the color at the center of the path to blue.
			pthGrBrush.SetCenterColor(GP Color(a, r, g, b));

			// Set the color along the entire boundary of the path to aqua.
			GP Color colors[] = { GP Color(a, 255, g, b) };
			int peopleCnt = 1;
			pthGrBrush.SetSurroundColors(colors, &peopleCnt);
			//messenger.getGraphics()->FillRectangle(&pthGrBrush, rect);
			//messenger.getGraphics()->FillRectangle(pathBrush(255, 0, 50, 255, area), rect);
			return &pthGrBrush;
		}
		GP Font const *font(int size)const {
			//static GP FontFamily fontFamily();//Courier New
			static GP Font* f = nullptr;
			delete f;
			return f = new GP Font(&GP FontFamily(L"Times New Roman"), (GP REAL)size, FontStyleRegular, UnitPixel);
		}
		//�����ͻ��Ƶ�����Ϣ ��Ȼ�����豸�Ļ����ϻ��Ƶ� �������������ϻ��� �Ƕ������ʵ�ֵ�
		static AnimationManager messenger;/*^_^ ���� ÷����*/
		// ��� ��ʼλ��(0, 0)
		static Sprite mouse;
		//PTSTR��WCHAR*
		WCHAR messageBuffer[Constant::BUFFER_MAX_BIT], windowTitle[Constant::BUFFER_MAX_BIT];
	};

	//ʱ���  (Ϊ�˱�֤ʱ�����Ψһ��: ���������븳ֵ �Ƽ�ʹ������ָ��)
	class Timestamp {
		static int ID;
		//ӵ����ID: ����Ψһ��
		int ownerID;
		//���һ�η�����Ϣ��ʱ��
		clock_t sendClock;
	public:
		//��֤ʱ�������Ч�� ��Ч����true
		bool isValid() {
			return ownerID >= 0;
		}
		//ʹʱ���������Ч
		void invalidate() {
			ownerID = -getOwnerID();
		}

		clock_t const &getSendClock()const {
			return sendClock;
		}
		int getOwnerID()const {
			return ownerID;
		}
		//�����ߵ���: ʹ�˿ǰ����Ķ���ʧЧ
		bool updateClock() {
			sendClock = clock();
			return sendClock == -1;//������ʱ�䲻��ȡ���򷵻�-1
		}

		Timestamp() {
			ownerID = ++ID;
			sendClock = clock();//�ó������������������ռ��CPU��ʱ��
		}

		Timestamp& operator=(Sprite const &rhs) = delete;
		Timestamp(Timestamp const &rhs);

		Timestamp& operator=(Timestamp &&rvalue) = delete;
		Timestamp(Timestamp &&rvalue) = delete;
	private:
		//��ΪID��Ψһ��, ���ԱȽϵ���ownerID?
		bool operator==(Timestamp const &rhs)const {
			return ownerID == rhs.ownerID;
		}
		bool operator<(Timestamp const &rhs) const {
			return ownerID < rhs.ownerID;
		}
		/*�����ķ�������*/
		bool operator!=(Timestamp const &rhs) const {
			return !(*this == rhs);
		}
		bool operator>=(Timestamp const &rhs) const {
			return !(*this < rhs);//��С�ڵȼ���>=
		}
		bool operator<=(Timestamp const &rhs) const {
			return (*this < rhs) || (*this == rhs);//С�ڻ���� �ȼ��� <=
		}
		bool operator>(Timestamp const &rhs) const {
			return !(*this <= rhs);//�� <= �ȼ��� >
		}
	};

}

#endif