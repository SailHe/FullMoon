#ifndef __Game_H
#define __Game_H

#include "Ecosystem.h"
#include "../myLib/extendspace.h"

namespace ECE = EcologicEngine;

/*World 游戏控制器GameController*/
class Game{
	//using namespace std;
public:
	//构造->gameStart
	Game(HDC hdc, GP Size const &winSize_)
		:mainSprite(GP Rect(0, 0, winSize_.Width, winSize_.Height)){
		clock_t beginTime = clock();
		initialization(hdc);
		StandardExtend::outputDebugFormat("================\n\r");
		StandardExtend::outputDebugFormat("初始化耗时: %f s\n\r", StandardExtend::calcDiffClock(beginTime));
		StandardExtend::outputDebugFormat("================\n\r");
		//demo(deviceGraphics);
	}
	//析构->gameOver
	~Game(){
		delete mapGraphics;
		// 释放 gdi+ 资源(但是如果在那些静态变量被释放之前就结束GDI+的话只要有用到GDI+的都会释放错误)
		//建议在使用GDI+的最底层(Ani)加一个对象计数 若当前销毁的是最后一个对象那么就结束GDI+(同样地第一次构造的时候直接就可以自动启动)
		//ECE::AnimationManager::graphicControl();
		//EndBatchDraw();
		//cleardevice();
		//closegraph();
	}
	bool gameLoop(){
		clock_t beginTime = clock();
		static double pW = mainSprite.getWidth() / 960.0, pH = mainSprite.getHeight() / 540.0;
		static Gdiplus::RectF drawRect(0, 0, (REAL)(mainSprite.getWidth() * pW), (REAL)(mainSprite.getHeight() * pH));
		bool result = ecosystem.running(plat_);
		//FlushBatchDraw();
		deviceGraphics->DrawImage(deviceBuffer, mainSprite.getRect());
		deviceBufferGraphics->DrawImage(plat_, mainSprite.getRect());
		//Sleep(30);
		//static TimeClock gameRefresh = TimeClock(1000 / 60);while (!gameRefresh.IsTimeOut());
		//putimage(0, 0, &plat);

		//ECE::WindowSprite().dialogue(_T("--这里可以用--"));
		ECE::Logger::outPut();
		//ECE::Logger::eraseLine();
		ECE::Logger::clear();

		//drawMap(*deviceBufferGraphics);
		StandardExtend::outputDebugFormat("帧耗时: %f s\n\r", StandardExtend::calcDiffClock(beginTime));
		return result;
	}
	void mouseMasege(MOUSEMSG const &C){
		ecosystem.mouseMasege(C);
	}
	void reInitialization(HDC hdc){
		deviceGraphics = GP Graphics::FromHDC(hdc);
	}
private:
	void demo(GP Graphics *deviceGraphics);
	void drawMap(GP Graphics &myGraphics);
	//初始化
	void initialization(HDC hdc){
		//{ mainSprite.getWidth(), mainSprite.getHeight() }
		Constant::initialize(GP Size(mainSprite.getWidth(), mainSprite.getHeight()));
		//mainSprite = Constant::mainCanvasSize;
		deviceBuffer = ECE::AnimationManager::graphicControl(deviceBufferGraphics, Constant::mainCanvasSize);
		reInitialization(hdc);
		//ECE::AnimationManager back = ECE::AnimationManager(_T("platBack.jpg"), 1, 1);
		plat_ = new Bitmap({ Constant::mainCanvasSize.Width, Constant::mainCanvasSize.Height });
		//deviceGraphics->DrawImage(plat_, mainSprite.getRect());
		//putimage(0, 0, &plat);//图形引擎启动成功
		ECE::WindowSprite::loading();
		ECE::Biology::loading();
		/*graphics_->GetHDC();*///地图画板:关联plat的hdc的绘图对象
		mapGraphics = GP Graphics::FromImage(plat_);
		ECE::Atlas::loading(mapGraphics
			, (GP Rect(Constant::mainCanvasSize.Width / 4, Constant::mainCanvasSize.Height / 4, Constant::mainCanvasSize.Width / 2, Constant::mainCanvasSize.Height / 2))
			, mainSprite.getCentre());
		ECE::WindowSprite window = ECE::WindowSprite(Constant::mainCanvasSize);
		window.onLoding();
		//deviceGraphics->DrawLine(&GP Pen(GP Color(255, 0, 0, 0)), GP Point(0, 0), GP Point(mainSprite.getWidth(), mainSprite.getHeight()));
		deviceGraphics->DrawImage(deviceBuffer, mainSprite.getRect());
		ecosystem.loading(Constant::mainCanvasSize, 2);
		//music();//载入完毕
		//BeginBatchDraw();
		// 图片背景会比较卡
		// deviceBufferGraphics->DrawImage(&Bitmap(_T("res/platBack.jpg")), mainSprite.getRect());
		// 限制只在指定范围内绘制有效
		deviceGraphics->SetClip(mainSprite.getRect());
		deviceBufferGraphics->SetClip(mainSprite.getRect());
	}
	/*类型细节测试*/
	void testSprite(){
		ECE::Sprite a;//普通构造+
		ECE::Sprite b = ECE::Sprite();//普通构造+
		a.setRect(new GP Rect(20, 20, 20, 20));//右值设置
		a.setRect(a.getRect());//引用设置
		//a.setCentre();
		b = a;//左值赋值
		ECE::Sprite c = a;//左值拷贝构造+
		ECE::Sprite d = ECE::Sprite(c);//左值拷贝构造+
		//c.setPosition(50, 50);
		c = ECE::Sprite();//普通构造->temp->右值赋值
		b.~Sprite();
		b.~Sprite();//应该允许重复析构
		//a = b;//有指针域的话应该报段错误
		ECE::Sprite e = ECE::Sprite(ECE::Sprite(d));//左值拷贝构造->temp->右值拷贝构造
		/*一共构造了7个对象 调用构造方法7次 调用析构方法9次*/
		/*5个对象实体 2个临时对象(就是右值对象) 这之前自动析构2次+2次手动析构 这之后析构5次*/
	}
	/*单参构造类型测试*/ //classTest<Sprite::Iterator, GP Size>();
	template<class TestType, class ParaT> void classTest(){
		TestType a = TestType(ParaT());//普通构造+
		a = a;//左值赋值
		TestType b = TestType(a);//左值拷贝构造+
		b = TestType(ParaT());//普通构造->temp->右值赋值
		TestType c = TestType(TestType(a));//左值拷贝构造->temp->右值拷贝构造
		/*一共构造了5个对象 调用构造方法5次 调用析构方法5次*/
		/*3个对象实体 2个临时对象(就是右值对象) 这之前自动析构2次 这之后析构3次*/
	}

	void music();
	//设备显存  位图充当画布
	Bitmap *deviceBuffer = nullptr;
	Bitmap *plat_;//游戏地图实体
	ECE::Sprite mainSprite;//窗体body
	GP Graphics *deviceGraphics = nullptr, *deviceBufferGraphics = nullptr, *mapGraphics = nullptr;//GDI+图形对象 此对象由此类管理因此此变量并不多于
	/*莱姆(Namer)特尔(status) 菲尔德area 迷宫 梅兹maze 彼拉多build*/
	ECE::Ecosystem ecosystem;
};

#endif