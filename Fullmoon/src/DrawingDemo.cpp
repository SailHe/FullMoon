#include "stdafx.h"
#include "constant.h"
#include "Game.h"
#include "../myLib/extendspace.h"
/**************************EsayX**********************************/
void triangle(int x, int y, int color){
	/*setlinecolor(color);// ���û�����ɫ
	line(x, y, x + 10, y);// �������ε�������
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
/*��̬Բ::�����Ļ���ģʽ*/
void dyCircle(int &x, int &y, int r = 10, int background = 0){
	/*if (background == 0){
		setlinecolor(BLACK);// ���Ʊ���ɫԲ: ����
		setfillcolor(BLACK);
	}
	else{
		setwritemode(R2_XORPEN);//XOR ��ͼ
		setlinecolor(RED);// ���ƿɼ�Բ: ��
		setfillcolor(GREEN | BLUE);
	}
	fillcircle(x, y, r);// ��
	Sleep(50);// ��ͣ
	fillcircle(x, y, r);// ��
	*/
}
/**************************Gdi**********************************/
/*GDI����*/
void GdiLine(Gdiplus::Point *a, Gdiplus::Point *b, HDC hdc){
	MoveToEx(hdc, a->X, a->Y, NULL);/*�ƶ�����λ��*/
	LineTo(hdc, b->X, b->Y);
}
/**************************GdiPlus**********************************/
/*��GdiPlus����һ��png��image ��Ҫ����GDI:startGdiShutdown()*/
void loadPng(Image *(&image), char const *name){
	int len = strlen(name);
	static WCHAR *wName = (WCHAR *)malloc(len*sizeof(WCHAR));
	wName[len] = NULL;
	/*wcharת��Ϊchar _MaxCount ���ַ���*/
	size_t converted = 0;
	mbstowcs_s(&converted, wName, len + 1, name, _TRUNCATE);
	//wName������ʱ
	//mbstowcs_s(&converted, wName, name, len);
	//mbstowcs(wName, name, len);
	/*���ر��� png*/
	image = Image::FromFile(wName);// L -> GDI+ ֻ֧�� UNICODE ����
}
/*��png��ʽͼƬ��image��ԭ�������hdc�����Ĵ��� Ҫ��FlushBatchDraw()��ʾ��ͼ���*/
void putPng(Image *image, int left, int top, HDC hdc){
	//graphics = Graphics::FromHDC(hdc);/*���� hdc �� graphics ����*/
	/*���û�ͼ������ image ����hdc�����Ĵ���*/
	//graphics->DrawImage(image, left, top, image->GetWidth(), image->GetHeight());
}
/*EasyX����Gdi+��ͼ*/
void GdiPlusDemo(const char *name, int x, int y, double ratio, int width, int height){
	/*
	// gdi+ ��ʼ��
	GdiplusStartupInput m_Gdistart;
	ULONG_PTR m_GdiplusToken;
	GdiplusStartup(&m_GdiplusToken, &m_Gdistart, NULL);
	// ������� hdc �� graphics �������ڵ��ø��ֻ�ͼ����
	// ������ hdc
	Graphics* graphics = Graphics::FromHDC(GetImageHDC());

	Image *image = nullptr;
	loadPng(image, "res\\world.png");
	// ��png�򵽱�����
	//putPng(image, player.position.X, player.position.Y, GetImageHDC(&background));

	height == 0 ? height = (int)(image->GetHeight()*ratio) : 0;
	width == 0 ? width = (int)(image->GetWidth()*ratio) : 0;
	graphics->DrawImage(image, x, y, width, height);//�� image ���Ƶ�������
	FlushBatchDraw();// ��ʾ��ͼ���
	// �ͷ� gdi+ ��Դ
	if (image != NULL)
		delete image;
	if (graphics != NULL)
		delete graphics;
	// ж��gdi+ ��֮ǰ����unload gdi+ ��֮����ʹ��gdi+
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
	//������DrawLine �����ĵ�(0, 0)��(160, 80)λ����������ռ�
	//������任���Ὣ��������ת��Ϊҳ������  ȫ��ת����ָ��X ����ƽ��100 ����λ����Y ����ƽ��50 ����λ
	//myGraphics.TranslateTransform(-100.0f, -50.0f);
	myGraphics.DrawLine(&myPen, 0, 0, 60, 30);
	/*
	��ҳ��任�����ǽ�ҳ������ת��Ϊ�豸���ꡣ
	ҳ������ռ��ԭ��һ��λ�ڹ����������Ͻǡ����⣬���ڶ�����λΪ���أ�����豸�����ҳ����������ͬ��
	*/

	//�ü�����
	GP Rect rect(Constant::mainCanvasSize.Width / 4, Constant::mainCanvasSize.Height / 4
		, Constant::mainCanvasSize.Width / 2, Constant::mainCanvasSize.Height / 2);
	Region myRegion(rect);
	myGraphics.DrawRectangle(&myPen, 20, 30, 100, 50);
	//�ü�����
	myGraphics.SetClip(&myRegion, CombineModeReplace);
	//����һ�������βü���ֱ��
	myGraphics.DrawLine(&myPen, 0, 0, 200, 200);

	GP GraphicsPath graphicsPath;
	GP Matrix matrix;
	graphicsPath.Transform(&matrix);
	//·��ƽֱ��
	graphicsPath.Flatten();//Page 32

	//ƽ��������
	//myGraphics.SetSmoothingMode(SmoothingModeAntiAlias);
	//myGraphics.DrawLine(&myPen, 60, 30, 100, 100);
	//deviceGraphics->DrawImage(&image, 0, 0);//�� image ���Ƶ�������
	//�ֲ������Լ�����
	//myGraphics.DrawImage(&myBitmap, GP Rect(image.GetWidth(), image.GetHeight(), 300, 300), 24*4, 24*0, 23, 24, UnitPixel);
	StandardExtend::outputDebugFormat("�ƶ��Ѽ��صĵ�ͼ����Ұ %d\n", 10);
	int x = 0, y = 0;
	while (true){
		//a��
		if (KEY_DOWN_DETECTION(0x41)){
			++x;
		}
		//d��
		if (KEY_DOWN_DETECTION(0x44)){
			--x;
		}

		//w��
		if (KEY_DOWN_DETECTION(0x57)){
			++y;
		}
		//s��
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
	//�ü�����
	static GP Rect rect(Constant::mainCanvasSize.Width / 4, Constant::mainCanvasSize.Height / 4
		, Constant::mainCanvasSize.Width / 2, Constant::mainCanvasSize.Height / 2);
	static Region myRegion(rect);
	//�ü�����
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