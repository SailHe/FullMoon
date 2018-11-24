#ifndef __Game_H
#define __Game_H

#include "Ecosystem.h"
#include "../myLib/extendspace.h"

namespace ECE = EcologicEngine;

/*World ��Ϸ������GameController*/
class Game{
	//using namespace std;
public:
	//����->gameStart
	Game(HDC hdc, GP Size const &winSize_)
		:mainSprite(GP Rect(0, 0, winSize_.Width, winSize_.Height)){
		clock_t beginTime = clock();
		initialization(hdc);
		StandardExtend::outputDebugFormat("================\n\r");
		StandardExtend::outputDebugFormat("��ʼ����ʱ: %f s\n\r", StandardExtend::calcDiffClock(beginTime));
		StandardExtend::outputDebugFormat("================\n\r");
		//demo(deviceGraphics);
	}
	//����->gameOver
	~Game(){
		delete mapGraphics;
		// �ͷ� gdi+ ��Դ(�����������Щ��̬�������ͷ�֮ǰ�ͽ���GDI+�Ļ�ֻҪ���õ�GDI+�Ķ����ͷŴ���)
		//������ʹ��GDI+����ײ�(Ani)��һ��������� ����ǰ���ٵ������һ��������ô�ͽ���GDI+(ͬ���ص�һ�ι����ʱ��ֱ�ӾͿ����Զ�����)
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

		//ECE::WindowSprite().dialogue(_T("--���������--"));
		ECE::Logger::outPut();
		//ECE::Logger::eraseLine();
		ECE::Logger::clear();

		//drawMap(*deviceBufferGraphics);
		StandardExtend::outputDebugFormat("֡��ʱ: %f s\n\r", StandardExtend::calcDiffClock(beginTime));
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
	//��ʼ��
	void initialization(HDC hdc){
		//{ mainSprite.getWidth(), mainSprite.getHeight() }
		Constant::initialize(GP Size(mainSprite.getWidth(), mainSprite.getHeight()));
		//mainSprite = Constant::mainCanvasSize;
		deviceBuffer = ECE::AnimationManager::graphicControl(deviceBufferGraphics, Constant::mainCanvasSize);
		reInitialization(hdc);
		//ECE::AnimationManager back = ECE::AnimationManager(_T("platBack.jpg"), 1, 1);
		plat_ = new Bitmap({ Constant::mainCanvasSize.Width, Constant::mainCanvasSize.Height });
		//deviceGraphics->DrawImage(plat_, mainSprite.getRect());
		//putimage(0, 0, &plat);//ͼ�����������ɹ�
		ECE::WindowSprite::loading();
		ECE::Biology::loading();
		/*graphics_->GetHDC();*///��ͼ����:����plat��hdc�Ļ�ͼ����
		mapGraphics = GP Graphics::FromImage(plat_);
		ECE::Atlas::loading(mapGraphics
			, (GP Rect(Constant::mainCanvasSize.Width / 4, Constant::mainCanvasSize.Height / 4, Constant::mainCanvasSize.Width / 2, Constant::mainCanvasSize.Height / 2))
			, mainSprite.getCentre());
		ECE::WindowSprite window = ECE::WindowSprite(Constant::mainCanvasSize);
		window.onLoding();
		//deviceGraphics->DrawLine(&GP Pen(GP Color(255, 0, 0, 0)), GP Point(0, 0), GP Point(mainSprite.getWidth(), mainSprite.getHeight()));
		deviceGraphics->DrawImage(deviceBuffer, mainSprite.getRect());
		ecosystem.loading(Constant::mainCanvasSize, 2);
		//music();//�������
		//BeginBatchDraw();
		// ͼƬ������ȽϿ�
		// deviceBufferGraphics->DrawImage(&Bitmap(_T("res/platBack.jpg")), mainSprite.getRect());
		// ����ֻ��ָ����Χ�ڻ�����Ч
		deviceGraphics->SetClip(mainSprite.getRect());
		deviceBufferGraphics->SetClip(mainSprite.getRect());
	}
	/*����ϸ�ڲ���*/
	void testSprite(){
		ECE::Sprite a;//��ͨ����+
		ECE::Sprite b = ECE::Sprite();//��ͨ����+
		a.setRect(new GP Rect(20, 20, 20, 20));//��ֵ����
		a.setRect(a.getRect());//��������
		//a.setCentre();
		b = a;//��ֵ��ֵ
		ECE::Sprite c = a;//��ֵ��������+
		ECE::Sprite d = ECE::Sprite(c);//��ֵ��������+
		//c.setPosition(50, 50);
		c = ECE::Sprite();//��ͨ����->temp->��ֵ��ֵ
		b.~Sprite();
		b.~Sprite();//Ӧ�������ظ�����
		//a = b;//��ָ����Ļ�Ӧ�ñ��δ���
		ECE::Sprite e = ECE::Sprite(ECE::Sprite(d));//��ֵ��������->temp->��ֵ��������
		/*һ��������7������ ���ù��췽��7�� ������������9��*/
		/*5������ʵ�� 2����ʱ����(������ֵ����) ��֮ǰ�Զ�����2��+2���ֶ����� ��֮������5��*/
	}
	/*���ι������Ͳ���*/ //classTest<Sprite::Iterator, GP Size>();
	template<class TestType, class ParaT> void classTest(){
		TestType a = TestType(ParaT());//��ͨ����+
		a = a;//��ֵ��ֵ
		TestType b = TestType(a);//��ֵ��������+
		b = TestType(ParaT());//��ͨ����->temp->��ֵ��ֵ
		TestType c = TestType(TestType(a));//��ֵ��������->temp->��ֵ��������
		/*һ��������5������ ���ù��췽��5�� ������������5��*/
		/*3������ʵ�� 2����ʱ����(������ֵ����) ��֮ǰ�Զ�����2�� ��֮������3��*/
	}

	void music();
	//�豸�Դ�  λͼ�䵱����
	Bitmap *deviceBuffer = nullptr;
	Bitmap *plat_;//��Ϸ��ͼʵ��
	ECE::Sprite mainSprite;//����body
	GP Graphics *deviceGraphics = nullptr, *deviceBufferGraphics = nullptr, *mapGraphics = nullptr;//GDI+ͼ�ζ��� �˶����ɴ��������˴˱�����������
	/*��ķ(Namer)�ض�(status) �ƶ���area �Թ� ÷��maze ������build*/
	ECE::Ecosystem ecosystem;
};

#endif