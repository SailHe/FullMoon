#include "stdafx.h"
#include "constant.h"
#include "Game.h"
#include "../myLib/extendspace.h"
/**************************EsayX**********************************/
void triangle(int x, int y, int color){
	/*setlinecolor(color);// 设置画线颜色
	line(x, y, x + 10, y);// 画三角形的三条边
	line(x, y, x, y + 10);
	line(x + 10, y, x, y + 10);*/
}
void myCircle(int px = 0, int py = 0, int r = 30){
	/*int c;
	double rad;
	int x, y;
	for (rad = 0; rad < PI * 2; rad += 0.001){
		x = (int)(r * cos(rad) + px + 0.5);
		y = (int)(r * sin(rad) + py + 0.5);
		c = (int)(rad * 255 / (2 * PI) + 0.5);
		setlinecolor(RGB(0, 0, c));
		line(px, py, x, y);
	}*/
}
/*动态圆::动画的基本模式*/
void dyCircle(int &x, int &y, int r = 10, int background = 0){
	/*if (background == 0){
		setlinecolor(BLACK);// 绘制背景色圆: 擦除
		setfillcolor(BLACK);
	}
	else{
		setwritemode(R2_XORPEN);//XOR 绘图
		setlinecolor(RED);// 绘制可见圆: 画
		setfillcolor(GREEN | BLUE);
	}
	fillcircle(x, y, r);// 画
	Sleep(50);// 暂停
	fillcircle(x, y, r);// 擦
	*/
}
/**************************Gdi**********************************/
/*GDI画线*/
void GdiLine(Gdiplus::Point *a, Gdiplus::Point *b, HDC hdc){
	MoveToEx(hdc, a->X, a->Y, NULL);/*移动画笔位置*/
	LineTo(hdc, b->X, b->Y);
}
/**************************GdiPlus**********************************/
/*用GdiPlus加载一张png到image 需要开启GDI:startGdiShutdown()*/
void loadPng(Image *(&image), char const *name){
	int len = strlen(name);
	static WCHAR *wName = (WCHAR *)malloc(len*sizeof(WCHAR));
	wName[len] = NULL;
	/*wchar转换为char _MaxCount 是字符数*/
	size_t converted = 0;
	mbstowcs_s(&converted, wName, len + 1, name, _TRUNCATE);
	//wName是数组时
	//mbstowcs_s(&converted, wName, name, len);
	//mbstowcs(wName, name, len);
	/*加载本地 png*/
	image = Image::FromFile(wName);// L -> GDI+ 只支持 UNICODE 编码
}
/*将png格式图片从image以原样输出到hdc关联的窗口 要用FlushBatchDraw()显示绘图结果*/
void putPng(Image *image, int left, int top, HDC hdc){
	//graphics = Graphics::FromHDC(hdc);/*关联 hdc 与 graphics 对象*/
	/*调用绘图函数将 image 绘制hdc关联的窗口*/
	//graphics->DrawImage(image, left, top, image->GetWidth(), image->GetHeight());
}
/*EasyX过渡Gdi+绘图*/
void GdiPlusDemo(const char *name, int x, int y, double ratio, int width, int height){
	/*
	// gdi+ 初始化
	GdiplusStartupInput m_Gdistart;
	ULONG_PTR m_GdiplusToken;
	GdiplusStartup(&m_GdiplusToken, &m_Gdistart, NULL);
	// 定义关联 hdc 的 graphics 对象，用于调用各种绘图函数
	// 主窗口 hdc
	Graphics* graphics = Graphics::FromHDC(GetImageHDC());

	Image *image = nullptr;
	loadPng(image, "res\\world.png");
	// 将png打到背景上
	//putPng(image, player.position.X, player.position.Y, GetImageHDC(&background));

	height == 0 ? height = (int)(image->GetHeight()*ratio) : 0;
	width == 0 ? width = (int)(image->GetWidth()*ratio) : 0;
	graphics->DrawImage(image, x, y, width, height);//将 image 绘制到主窗口
	FlushBatchDraw();// 显示绘图结果
	// 释放 gdi+ 资源
	if (image != NULL)
		delete image;
	if (graphics != NULL)
		delete graphics;
	// 卸载gdi+ 这之前必须unload gdi+ 这之后不能使用gdi+
	GdiplusShutdown(m_GdiplusToken);
	*/
}

void cachedBitmap(GP Graphics &graphics){
	Bitmap bitmap(L"res\\world.png");
	UINT width = bitmap.GetWidth();
	UINT height = bitmap.GetHeight();
	CachedBitmap cBitmap(&bitmap, &graphics);
	int j, k;
	for (j = 0; j < 30; j += 10)
	for (k = 0; k < 1000; ++k)
		graphics.DrawImage(&bitmap, j, j / 2, width, height);
	for (j = 0; j < 30; j += 10)
	for (k = 0; k < 1000; ++k)
		graphics.DrawCachedBitmap(&cBitmap, j, 150 + j / 2);
}

void Game::demo(GP Graphics *deviceGraphics){
	//White, Green
	deviceGraphics->Clear(Color::Black);
	GP Graphics &myGraphics = *deviceGraphics;
	Bitmap image(L"res\\world.png");
	Bitmap &myBitmap = image;
	Pen myPen(Color(255, 0, 0, 255), 2);

	GP PointF p1(0.0, 0.0);
	GP PointF p2(100 * 6.0, 0.0);
	GP PointF p3(100 * 4.0, 100 * 4.0);
	GP PointF p(100 * 5.0, 100 * 2.5);
	myGraphics.DrawLine(&myPen, p1, p2);
	myGraphics.DrawLine(&myPen, p1, p3);
	myGraphics.DrawLine(&myPen, p2, p3);
	myGraphics.DrawEllipse(&myPen, p.X, p.Y, 10.0, 10.0);

	//myGraphics.DrawImage(plat_, 0, 0);
	//myGraphics.DrawImage(&image, image.GetWidth() + 100, 100, image.GetWidth(), image.GetHeight());
	
	//cachedBitmap(myGraphics);
	myGraphics.DrawLine(&myPen, 0, 0, 60, 30);
	//传递至DrawLine 方法的点(0, 0)和(160, 80)位于世界坐标空间
	//“世界变换”会将世界坐标转换为页面坐标  全局转换是指在X 方向平移100 个单位和在Y 方向平移50 个单位
	//myGraphics.TranslateTransform(-100.0f, -50.0f);
	myGraphics.DrawLine(&myPen, 0, 0, 60, 30);
	/*
	“页面变换”则是将页面坐标转换为设备坐标。
	页面坐标空间的原点一律位于工作区的左上角。此外，由于度量单位为像素，因此设备坐标和页面坐标是相同的
	*/

	//裁剪区域
	GP Rect rect(Constant::mainCanvasSize.Width / 4, Constant::mainCanvasSize.Height / 4
		, Constant::mainCanvasSize.Width / 2, Constant::mainCanvasSize.Height / 2);
	Region myRegion(rect);
	myGraphics.DrawRectangle(&myPen, 20, 30, 100, 50);
	//裁剪绘制
	myGraphics.SetClip(&myRegion, CombineModeReplace);
	//绘制一条被矩形裁剪的直线
	myGraphics.DrawLine(&myPen, 0, 0, 200, 200);

	GP GraphicsPath graphicsPath;
	GP Matrix matrix;
	graphicsPath.Transform(&matrix);
	//路径平直化
	graphicsPath.Flatten();//Page 32

	//平滑的线条
	//myGraphics.SetSmoothingMode(SmoothingModeAntiAlias);
	//myGraphics.DrawLine(&myPen, 60, 30, 100, 100);
	//deviceGraphics->DrawImage(&image, 0, 0);//将 image 绘制到主窗口
	//局部绘制以及缩放
	//myGraphics.DrawImage(&myBitmap, GP Rect(image.GetWidth(), image.GetHeight(), 300, 300), 24*4, 24*0, 23, 24, UnitPixel);
	StandardExtend::outputDebugFormat("移动已加载的地图的视野 %d\n", 10);
	int x = 0, y = 0;
	while (true){
		//a左
		if (KEY_DOWN_DETECTION(0x41)){
			++x;
		}
		//d右
		if (KEY_DOWN_DETECTION(0x44)){
			--x;
		}

		//w上
		if (KEY_DOWN_DETECTION(0x57)){
			++y;
		}
		//s下
		if (KEY_DOWN_DETECTION(0x53)){
			--y;
		}

		if (x < 0){
			x = 0;
		}
		if (y < 0){
			y = 0;
		}

		if (x >= (rect.Width/2)){
			x = rect.Width/2;
		}
		if (y >= (rect.Height/2)){
			y = rect.Height/2;
		}
		myGraphics.DrawImage(plat_, x, y);
	}
}

void Game::drawMap(GP Graphics &myGraphics){
	//裁剪区域
	static GP Rect rect(Constant::mainCanvasSize.Width / 4, Constant::mainCanvasSize.Height / 4
		, Constant::mainCanvasSize.Width / 2, Constant::mainCanvasSize.Height / 2);
	static Region myRegion(rect);
	//裁剪绘制
	myGraphics.SetClip(&myRegion, CombineModeReplace);

	static int x = 0, y = 0;

	/*//a
	if (KEY_DOWN_DETECTION(0x41)){
		x+=10;
	}
	//d
	if (KEY_DOWN_DETECTION(0x44)){
		--x;
	}

	//w
	if (KEY_DOWN_DETECTION(0x57)){
		++y;
	}
	//s
	if (KEY_DOWN_DETECTION(0x53)){
		--y;
	}

	if (x < 0){
		x = 0;
	}
	if (y < 0){
		y = 0;
	}

	if (x >= (rect.Width / 2)){
		x = rect.Width / 2;
	}
	if (y >= (rect.Height / 2)){
		y = rect.Height / 2;
	}*/
	myGraphics.DrawImage(plat_, x, y);
}