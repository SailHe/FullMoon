#ifndef __FULLMOON_STDAFX_H
#define __FULLMOON_STDAFX_H
#include"../myLib/stdafx.h"
#include <thread>
//#define interface class
#define implements public
//�ؿ���ͼʵ�ʴ�СΪ16(GRID��ʵ��Ч���Ŀ�ѡ��Сֻ��16 8 4 2)
#define GRID 8*2
//����id
#define SkId BYTE

//using namespace std;
//using namespace std::placeholders;
//�ɱ�����
template<typename T>using Varrays = std::vector<T>;
//�����
template<typename T>using ArrayList = std::vector<T>;
//�ڽӱ�
template<typename T>using LinkedList = std::list<T>;

//������
using Package = std::vector<std::pair<int, int>>;
typedef int Sub;//�±�����
typedef Sub RowsSub;//�б�����
typedef Sub ColsSub;//�б�����
typedef __int64 I64;
//_ASSERTE(_CrtCheckMemory());//�ڴ���



/*��Biology��ص���������:���� ���� ����Ա*/
enum TYPE { PLAYER, PEOPLE, MANAGER, MONSTER };
//���� �쳣 ����(�����ó�Ա�����ж�) �˳� ���赲 ����״̬
enum STATUS { NORMAL, ABNORMAL, CONK, QUIT, BLOCKING, BYPASS };
/*��ͼ: up(����)->��Ӱ, down->����, left->��, right->��  ViewCount:��ͼ����*/
enum View { BACK, FRONT, LEFT, RIGHT, ViewCount };
//����ָ��������ֹʱ�ǲ�����һ֡(��ʼ֡, ��һ֡)
enum StaticFrameEnum { ORIGIN_FRAME, PAST_FRAME };
//��������
enum STEP { STEP_X_AXIS, STEP_Y_AXIS };
//ģʽ
enum MODE { ON, OFF, UNDEFINED };
//constant.h
//Ĭ����ͼ(��ֵת��Ϊint������0)
const View defaultView = BACK;

/*
SHORT GetKeyState(int nVirtKey);// virtual-key code
���ã����ؼ���״̬�����¡��ͷŻ�������down��up or toggled��
��������������루VK_)���������ĸa-z��A-Z ������0-9�� ��Ϊ���Ӧ��ASCII�루������ĸO��ASCII��Ϊʮ�����Ƶ�0x4F��
����ֵ��������ĸ�λ��ʾ��ǰ�Ƿ��м������£���λ��0λ������ʾNumLock��CapsLock��ScrollLock��״̬��ON��OFF��ΪONʱ����ָʾ������������λΪ1������ֵС��0��˵���м����£����λΪ1��ʾ����������ON��״̬
*/
#define KEY_DOWN_DETECTION(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? true : false) 
#define KEY_UP_DETECTION(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? false : true)

struct MOUSEMSG
{
	UINT uMsg;      // ��ǰ�����Ϣ
	int mkCtrl;     // Ctrl ���Ƿ���
	int mkShift;    // Shift ���Ƿ���
	int mkLButton;  // �������Ƿ���
	int mkMButton;  // ����м��Ƿ���
	int mkRButton;  // ����Ҽ��Ƿ���
	int x;          // ��ǰ��� x ����
	int y;          // ��ǰ��� y ����
	int wheel;      // �����ֹ���ֵ
};
#endif