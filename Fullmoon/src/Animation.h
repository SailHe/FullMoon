#ifndef __Animation_H
#define __Animation_H
#include"Sprite.h"

namespace EcologicEngine {

	//管理类: 节约内存+实体管理
	/*动画类 管理动画资源与绘制动画*/
	class AnimationManager {
		static GP Status pastStatus;
		static WCHAR buf[Constant::BUFFER_MAX_BIT]; WCHAR *const res = buf;/*路径*/
		static Pen* myPen;//由graphicControl管理
		static GP Graphics *deviceGraphics_;//设备画板 由graphicControl管理(用于调试)
		static GP Graphics *deviceBufferGraphics_;//设备缓存画板 由graphicControl管理
		GP Graphics *graphics_ = nullptr;//独立画板 由给予该指针的对象管理


		ArrayList<ArrayList<GP Bitmap *>> animation;/*位图组 备用词surfaceTexture(表面纹理)*/
		ArrayList<std::pair<size_t, size_t>> indexF;/*当前帧/最大帧frame index*/
		//释放位图组内的所有位图 并置空 位图组大小不变(已使用的size置为0)
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
		//测试结果:Gdi+未初始化前定义Bitmap是可以的, 但是会加载失败(状态显示:未初始化)
		//deviceBufferGraphics_->DrawImage(&background, GP Rect(0,0,100,100));
		//但若此时绘制会报错 初始化后绘制未初始化前定义的位图没有错误但是也没效果

		//图形控制 必须先初始化后才可正常使用 返回设备显存画板
		static GP Bitmap *graphicControl(GP Graphics *&deviceBufferGraphics, GP Size const &size) {
			static GdiplusStartupInput m_Gdistart;
			static ULONG_PTR m_GdiplusToken;
			static Bitmap *deviceBuffer = nullptr;
			if (deviceBufferGraphics == nullptr) {//没有初始化->绘图初始化
				/*esyx初始化*/
				srand((unsigned)time(nullptr));
				//initgraph(mainSprite->getWidth(), mainSprite->getHeight());
				//setbkmode(TRANSPARENT);//透明的setwritemode(R2_XORPEN);

				/*Gdiplush初始化 initialization*/
				GdiplusStartup(&m_GdiplusToken, &m_Gdistart, nullptr);
				/*获取图片的设备场景句柄*/
				//deviceBuffer = new Bitmap(deviceBufferRes.c_str());
				deviceBuffer = new Bitmap(size.Width, size.Height);
				//@TODO 关闭hdc
				//deviceGraphics_ = GP Graphics::FromHDC(GetDC(GetHWnd()));
				deviceGraphics_ = GP Graphics::FromHDC(GetDC(GetActiveWindow()));
				deviceBufferGraphics = deviceBufferGraphics_ = GP Graphics::FromImage(deviceBuffer);
				myPen = new Pen(GP Color(255, 255, 0, 0));
			}
			else {//绘图终止 绘图卸载
				delete deviceBufferGraphics_, myPen;
				deviceBufferGraphics = deviceBufferGraphics_ = nullptr;
				/*卸载Gdiplush 这之后不能使用Gdiplush 因此资源卸载要在之前处理*/
				GdiplusShutdown(m_GdiplusToken);
			}
			return deviceBuffer;
		}

		//可以先构造之后必须初始化
		AnimationManager() {}

		//默认的画板是设备画板
		//(文件路径, 对应坐标轴方向上的总帧数, 从源图第startRowFrame行, startColFrame处开始计算, 从计算开始的点步进方向(横, 纵))
		AnimationManager(PTCHAR resPath, int totalFrameXAxis, int totalFrameYAxis, int startRowFrame = 0, int startColFrame = 0, STEP stepDir = STEP_X_AXIS) {
			graphics_ = deviceBufferGraphics_;
			reLoad(resPath, totalFrameXAxis, totalFrameYAxis, startRowFrame, startColFrame, stepDir);
		}

		//左值赋值 :析构左手边位图 拷贝构造右手边位图 (不支持)
		AnimationManager& operator=(AnimationManager const &rhs) = delete;
		//右值赋值 :析构左手边位图 将左右手位图指针交换
		AnimationManager& operator=(AnimationManager &&rvalue) {
			indexF = std::move(rvalue.indexF);
			std::swap(graphics_, rvalue.graphics_);

			deleteAnimation();
			animation.resize(rvalue.animation.size());
			FOR(i, 0, rvalue.animation.size()) {
				animation[i].resize(rvalue.animation[i].size());
				FOR(j, 0, rvalue.animation[i].size()) {
					std::swap(animation[i][j], rvalue.animation[i][j]);
					//裂图调用的Clone方法不允许处理const 不允许拷贝
					//partitionBitmap(0, 0, rhs.animation[i][j]->GetWidth(), rhs.animation[i][j]->GetHeight(), animation[i], rhs.animation[i][j]);
					//animation[i][j] = new GP Bitmap(*rhs.animation[i][j]);// 不允许赋值
				}
			}
			return *this;
		}
		//左值拷贝构造
		AnimationManager(AnimationManager const &rhs) = delete;
		//右值拷贝构造 (即使传入临时对象我们也只是交换了在堆上分配的指针 但是域的set却不能传入临时右值引用)
		AnimationManager(AnimationManager &&rvalue) {
			*this = std::move(rvalue);
		}

		virtual ~AnimationManager() {
			graphics_ = nullptr;
			deleteAnimation();
		}

		/*(默认黑色)返回画笔 α半透明值(表示该颜色与背景色混合的程度)、红、绿、蓝*/
		GP Pen* pen(BYTE a = 255, BYTE r = 0, BYTE g = 0, BYTE b = 0) {
			delete myPen;
			myPen = new GP Pen(GP Color(a, r, g, b));
			return myPen;
		}

		/*参数最少原则(提高传参效率 同时还有考虑参数得到的难度)*/

		/*基本绘制 (行数, 列数, 左角位置)   仅供测试使用*/
		void drawInRC(int rows, int cols, GP Point location) {
			graphics_->DrawImage(animation[rows][cols], location);
		}
		/*基本绘制 (行数, 列数, 左角位置)   仅供测试使用*/
		void drawInRC(int rows, int cols, Sprite const &location) {
			graphics_->DrawImage(animation[rows][cols], location.getRect());
		}
		/*基本绘制 (行数, 列数, 区域)*/
		void draw(int rows, int cols, GP Rect const &area) {
			graphics_->DrawImage(animation[rows][cols], area);
		}
		/*编号绘制 (位图编号, 区域) tips:只支持左角0->右角N一种编号方式*/
		void draw(int num, Sprite const &area) {
			int h = animation.size(), w = 0;
			if (h == 0) {
				_DEBUG_ERROR("未装载任何资源");
				return;
			}
			else {
				w = animation[0].size();
			}
			draw(num / w, num % w, area.getRect());
		}
		//动画绘制 (可动区域 PS: 注意使用的rSub是body自带的)
		void draw(Sprite &body) {
			static int baseRate = 1;//基准速率 原先npc是5(防止播放速度过快) 玩家为1防止转向时贴图未更改
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
		//指定绘图画板 (左值限定: 只能传入已被管理的图形对象)
		void setGraphics(GP Graphics *&graphics) {
			graphics_ = graphics;
		}
		//默认绘图画板
		void setGraphics() {
			graphics_ = deviceBufferGraphics_;
		}

		//获取贴图源行标
		int getOriginRowsSub(PTCHAR name) const {
			auto it = originSubTable.find(name);
			if (it != originSubTable.end()) {
				return it->second;
			}
			else {
				_DEBUG_ERROR("名字不存在");
				return -1;
			}
		}

		//增量加载 返回资源的rSub(id)    <<<*单次加载只支持均匀分割; 之所以将列方向的分割帧数置于 行的前面是由于很多图片都只有一行 但存在多列*>>>
		//(文件路径, 对应坐标轴方向上的总帧数, 从源图第startRowFrame行, cols处开始计算, 从计算开始的点步进方向(横, 纵)); 如果没有别名 那么需使用 图片名name获取源rSub(xx\\name.xx)
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

		//别名加载(需使用 图片别名alias获取源rSub)
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
		/*截取图像的某一部分 (截取区域, 存储容器(push_back), 源位图)*/
		static void partitionBitmap(int x, int y, int Width, int Height
			, ArrayList<GP Bitmap*> &animation, GP Bitmap &imageRes) {

			static RectF block = RectF();
			block.X = (REAL)x, block.Y = (REAL)y, block.Width = (REAL)Width, block.Height = (REAL)Height;
			//GP Image *enlargeImage = imageRes.Clone(block, PixelFormatDontCare)->GetThumbnailImage(enlargeProportion.Width, enlargeProportion.Height, NULL, NULL);
			animation.push_back(imageRes.Clone(block, PixelFormatDontCare));/*克隆图片的部分*/
			if (DEBUG) {
				//deviceGraphics_->DrawImage(animation.back(), 300, 300, 200, 200); //FOR DEBUG
			}
		}

		//new一个与sr具有相同字符的WCHAR赋值并返回 慎用(未使用)
		WCHAR *toWCHAR(WCHAR *&res, const char* sr, size_t len) {
			res = (WCHAR *)malloc(len * sizeof(WCHAR));
			res[len] = '\0';//wchar转换为char _MaxCount 是字符数
			//mbstowcs(res, sr, len);
			//mbstowcs_s(size_t(10),res, sr, len);
			return res;
		}

		/*加载资源
		(原图片行列虚拟起点帧 默认表示从行和列的第0帧开始
		, x和y方向上的总帧数fx和fy 即 列数和行数
		, x和y方向需要加载的帧数-1或输入数字过大时表示加载剩余所有的
		, 优先步进的方向 简单说就是一组动画在即将分割的图片中是横排的还是纵排的)
		 返回此次加载的动画id(二维数组的行索引rSub)  若加载出错 返回 -LastStatus*/
		int loadResource(
			int startRowFrame = 0, int startColFrame = 0
			, int totalFrameXAxis = 1, int totalFrameYAxis = 1
			, int loadFX = -1, int loadFY = -1
			, STEP stepDir = STEP_X_AXIS) {

			static int width = 0, height = 0;
			static std::wstring pastRes;
			//裂图源位图
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
			width = imageRes.GetWidth() / totalFrameXAxis, height = imageRes.GetHeight() / totalFrameYAxis;/*帧的实际宽高(以源图为准)*/
			if (width*totalFrameXAxis != imageRes.GetWidth()) {
				_DEBUG_ERROR("资源的横帧必须能整除");
			}
			if (height*totalFrameYAxis != imageRes.GetHeight()) {
				_DEBUG_ERROR("资源的纵帧必须能整除");
			}

			int forwardFrame = 0;//步进方向上的 帧数
			int w = 0, h = 0;//步进方向上的 帧宽高[虚拟]
			int iRow = 0, iCol = 0;//行位置(y) 列位置(x) 

			//计算剩余总帧数 以及总加载的帧数
			totalFrameXAxis -= startColFrame;
			totalFrameYAxis -= startRowFrame;
			if (loadFX < 0 || loadFX > totalFrameXAxis) {
				loadFX = totalFrameXAxis;
			}
			if (loadFY < 0 || loadFY > totalFrameYAxis) {
				loadFY = totalFrameYAxis;
			}

			/*步进方向判断*/
			if (stepDir == STEP_X_AXIS) {
				forwardFrame = loadFX, w = width, h = height;
				iRow = startRowFrame * h, iCol = startColFrame * w;
			}
			else {
				forwardFrame = loadFY, w = height, h = width;
				iCol = startRowFrame * h, iRow = startColFrame * w;
			}

			int oldSize = animation.size();//为了支持reLoad
			int increasedSize = loadFX + loadFY - forwardFrame;//此次将加载动画的个数(即要储存多少个数组行)
			animation.resize(oldSize + increasedSize);
			indexF.resize(oldSize + increasedSize);

			for (int id = oldSize; id < oldSize + increasedSize; ++id, iRow += h) {//动画(行)步进
				indexF[id].second = forwardFrame;
				for (int f = forwardFrame, x = iCol; --f >= 0; x += w) {//帧(列)步进
					if (stepDir == STEP_X_AXIS) {
						partitionBitmap(x, iRow, width, height, animation[id], imageRes);
					}
					else {
						//在步进方向为y轴时iRow的实际意义就是iCol x的实际意义是y
						//若试图理解这段代码 需要先理解步进方向为x轴时的情况
						partitionBitmap(iRow, x, width, height, animation[id], imageRes);
					}
				}
			}
			return oldSize;
		}

		//std::map<std::wstring, SkId>索引表 <name id>
		std::map<std::wstring, SkId> originSubTable;
		//放大系数(或是缩小) 没有使用
		static GP SizeF enlargeProportion;
	};

	/*窗框精灵类  交互功能(消息输出 + 信息展示)*/
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
		//title默认显示于上方中部 默认是主地图大小
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

		/*********************************基本的图形绘制方法****************************/
		void drawPoint(GP Point const &p) {
			// esayX绘制像素点
			// putpixel(p.X, p.Y, BLACK);
			// @TODO
			drawCircle(p, 1);
		}
		void drawLine(GP Point const &o, GP Point const &e) {
			messenger.getGraphics()->DrawLine(messenger.pen(255, 128, 0, 0), o, e);
		}
		//绘制射线
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
		//(左角, adllWidth, allHeight)
		void drawEllipse(GP Point const &LT, int adllWidth, int allHeight) {
			drawEllipse(LT.X, LT.Y, adllWidth, allHeight);
		}
		//圆心 半径
		void drawCircle(int cX, int cY, int radius) {
			//椭圆的参数的是左角位置 和 diameter(直径)   esayX的圆绘制: circle()
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
		//在本区域内显示文本
		void WindowSprite::text(WCHAR const *message, int fontSize, GP Color color)const {
			text(message, *this, fontSize, color);
		}
		//在指定矩形框区域area内显示文本消息(如果矩形框无法显示整个字串 那么超出的部分会被截断 不好用: 字体大小往往难以提前知晓)
		void WindowSprite::text(WCHAR const *message, Sprite const &area, int fontSize, GP Color color) const {
			GP SolidBrush solidBrush(color);
			messenger.getGraphics()->DrawString(message, -1, font(fontSize), area.tmpRectF(), &StringFormat(), &solidBrush);
		}
		void WindowSprite::text(WCHAR const *message, size_t len, Sprite const &area, int fontSize, GP Color color) const {
			GP SolidBrush solidBrush(color);
			messenger.getGraphics()->DrawString(message, len, font(fontSize), area.tmpRectF(), &StringFormat(), &solidBrush);
		}

		/*********************************特效图形绘制方法****************************/
		void onLoding() {
			static Bitmap logo(L"res\\load\\dq.png");
			//缩放动画ScaleTransform; 恢复ResetTransform;
			//messenger.getGraphics()->ScaleTransform(0.5f, 0.5f);
			messenger.getGraphics()->DrawImage(&logo
				, Rect(
					getWidth() / 2 - logo.GetWidth() / 2,
					getHeight() / 2 - logo.GetHeight() / 2,
					logo.GetWidth(), logo.GetHeight())
			);
		}
		/*鼠标点击特效*/
		void shawHit() {
			messenger.draw(mouse);
		}
		bool massageBox(PTCHAR massage, PTCHAR tips = _T("tips")) {
			// 第一个参数设置为0，第一个参数是依赖的窗口的编号，相当于谁弹出的窗口，0为系统弹出，第二个参数是对话框的内容，
			// 第三个参数是对话框的标题，第四个参数是对话框类型，设置成0即可
			//return MessageBox(GetHWnd(), massage, tips, MB_YESNO) == IDYES;
			return MessageBox(0, massage, tips, MB_YESNO) == IDYES;
		}
		//显示数字
		void drawNumber(int value) {
			//数字的源行标
			static size_t rSub = messenger.getOriginRowsSub(_T("number"));
			static int numW = messenger.getWidthA(rSub), numH = messenger.getHeightA(rSub);//一个数字符号的宽度
			int aValue = abs(value);

			//设置显示数字大小
			setSize(numW, numH);
			//显示正负号
			if (value < 0) {
				//负号贴图
				messenger.draw(rSub, 12, getRect());
			}
			else if (value > 0) {
				//正号贴图
				messenger.draw(rSub, 11, getRect());
			}
			rightward(numW*totalBitOf(aValue));
			while (aValue > 0) {
				//绘制数字
				messenger.draw(rSub, aValue % 10, getRect());
				aValue /= 10;
				rightward(-numW);
			}
		}
		//对话
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
				//一行显示10个文字
				len -= lineLen;
				text(message + lineLen * i++, len > 0 ? lineLen : lineLen + len, dialogueSprite, 15, BLACK_);
				dialogueSprite.downward(dialogueSprite.getHeight() / 4);
			}
		}
		/*比值条*/
		void speValBar(int value, int fullValue, GP Color color);
		/*将一个比值文本缓冲到 消息缓冲区*/
		void specificValueToBuffer(int value, int fullValue, std::wstring  const &str = L"");
		//绘制菜单背景
		void drawMenuBack() {
			static int rSub = messenger.getOriginRowsSub(_T("statusBack"));
			messenger.draw(rSub, 0, getRect());
		}
		//绘制一个按钮背景(文字自动居中)
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
		/*物品罗列 在区域内罗列背包中的所有物品 将有物品的格子传入ItemsBox中*/
		void inventoryTabs(Package const &bag, ArrayList<Sprite> &ItemsBox);
		//在area处展示一个状态值(name: status/fullstatus)
		void drawStatusBar(size_t left, size_t top, std::wstring const &name, int statusValue, int statusFullValue, GP Color color = BLACK_) {
			specificValueToBuffer(statusValue, statusFullValue, name);
			//字体大小根据状态框背景自适应
			text(messageBuffer, left + (int)(Constant::mainCanvasSize.Width*0.005), top, getWidth() / 24, color);
		}
		/*展示编号为id的一个物品*/
		void drawItem(size_t id, GP Point const &position) {
			messenger.drawInRC(id / 16, id % 16, position);
		}

		/***********************************功能辅助方法******************************************/
		static int totalBitOf(int number) {
			int top = 1;
			while (number /= 10)top++;
			return top;
		}
		//显示标题
		void shawTitle() {
			//上方中部
			Sprite upCentre = *this;
			upCentre.downward((int)(getHeight()*0.1));
			upCentre.rightward(getWidth() / 4);
			text(windowTitle, upCentre, 10, BLACK_);
		}
		//返回id编号物品的像素宽
		size_t getWidthOfItem(size_t id) {
			return messenger.getWidthA(id / 16);
		}
		//返回id编号物品的像素宽
		size_t getHeightOfItem(size_t id) {
			return messenger.getHeightA(id / 16);
		}
		//展示一张图片的全图以及可点击区域 将所有限位框加入imageLimitRectList中
		void WindowSprite::imageSelect(LinkedList<RectF> &imageLimitRectList, bool emplace = true) {
			//的ID(根据用户的设置确定编号方式) 传入图片名或资源地址
			static Bitmap image(L"res\\map\\grass.png");//野外地图 (maper处已有加载)
			auto rect = getRect();
			Sub rows = 16, cols = 15;
			//一个选择单元的固定宽高
			int w = 16, h = 16;

			rect.X += 16, rect.Y += 16;
			rect.Width -= 32, rect.Height -= 32;

			REAL allW = (REAL)w*cols, allH = (REAL)h*rows;
			REAL fw = (REAL)w*(rect.Width / allW), fh = (REAL)h*(rect.Height / allH);
			messenger.getGraphics()->DrawImage(&image, rect);
			GP PointF &position = GP PointF((REAL)rect.X, (REAL)rect.Y);
			auto lambdaFun = [&]() {
				//限位框
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

		/*区域点击判断*/
		/*选择式对话框*/
		/*文本式对话框*/
		static Sprite &getMouse() {
			return mouse;
		}
		static void setMouse(int x, int y) {
			mouse.setLocation(x, y);
			mouse.setDynamic(true);
			mouse.setCentre();
		}
		/*装载后使用->存在大量重复位图时可使用这种内存管理模式*/
		static void loading() {
			messenger.setGraphics();
			bool moving = false;
			int rSub;
			//物品表
			rSub = messenger.reLoad(L"items.png", 16, 8);

			//状态背景
			rSub = messenger.reLoad(L"part\\statusBack.png");

			//按钮
			rSub = messenger.reLoad(L"part\\buttonBack.png");

			//文字标签背景
			rSub = messenger.reLoad(L"part\\label.png", _T("lableBack"));

			//物品标签
			rSub = messenger.reLoad(L"rfont\\4.png", L"itemLable");

			//数字
			rSub = messenger.reLoad(L"part\\number.png", 13, 5);

			//普通攻击
			rSub = messenger.reLoad(L"battle\\effect\\atk\\1.png", L"atk");

			//特效
			rSub = messenger.reLoad(L"battle\\effect\\boom\\dark.png", 8);

			rSub = messenger.reLoad(L"battle\\effect\\light\\light.png", 6);

			rSub = messenger.reLoad(L"battle\\effect\\bullet\\dark.png", L"darkbullet", 6); moving = true;

			mouse.setSize(messenger.getWidthA(rSub), messenger.getHeightA(rSub));
			mouse.setOriginView(rSub);

			rSub = messenger.reLoad(L"battle\\music.png", 12); moving = true;

			rSub = messenger.reLoad(L"battle\\effect\\gas.png", 4);

		}
	private:
		/*返回路径渐变画刷*/
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
		//此类型绘制的是消息 必然是在设备的画板上绘制的 而在其它画板上绘制 是动画类该实现的
		static AnimationManager messenger;/*^_^ 先驱 梅辛杰*/
		// 鼠标 初始位置(0, 0)
		static Sprite mouse;
		//PTSTR或WCHAR*
		WCHAR messageBuffer[Constant::BUFFER_MAX_BIT], windowTitle[Constant::BUFFER_MAX_BIT];
	};

	//时间戳  (为了保证时间戳的唯一性: 不允许拷贝与赋值 推荐使用智能指针)
	class Timestamp {
		static int ID;
		//拥有者ID: 具有唯一性
		int ownerID;
		//最后一次发送消息的时间
		clock_t sendClock;
	public:
		//验证时间戳的有效性 有效返回true
		bool isValid() {
			return ownerID >= 0;
		}
		//使时间戳对象无效
		void invalidate() {
			ownerID = -getOwnerID();
		}

		clock_t const &getSendClock()const {
			return sendClock;
		}
		int getOwnerID()const {
			return ownerID;
		}
		//发送者调用: 使此壳包裹的对象失效
		bool updateClock() {
			sendClock = clock();
			return sendClock == -1;//若挂钟时间不可取，则返回-1
		}

		Timestamp() {
			ownerID = ++ID;
			sendClock = clock();//该程序从启动到函数调用占用CPU的时间
		}

		Timestamp& operator=(Sprite const &rhs) = delete;
		Timestamp(Timestamp const &rhs);

		Timestamp& operator=(Timestamp &&rvalue) = delete;
		Timestamp(Timestamp &&rvalue) = delete;
	private:
		//因为ID是唯一的, 所以比较的是ownerID?
		bool operator==(Timestamp const &rhs)const {
			return ownerID == rhs.ownerID;
		}
		bool operator<(Timestamp const &rhs) const {
			return ownerID < rhs.ownerID;
		}
		/*派生的符号重载*/
		bool operator!=(Timestamp const &rhs) const {
			return !(*this == rhs);
		}
		bool operator>=(Timestamp const &rhs) const {
			return !(*this < rhs);//不小于等价于>=
		}
		bool operator<=(Timestamp const &rhs) const {
			return (*this < rhs) || (*this == rhs);//小于或等于 等价于 <=
		}
		bool operator>(Timestamp const &rhs) const {
			return !(*this <= rhs);//不 <= 等价于 >
		}
	};

}

#endif