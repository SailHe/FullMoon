#ifndef __FULLMOON_STDAFX_H
#define __FULLMOON_STDAFX_H
#include"../myLib/stdafx.h"
#include <thread>
//#define interface class
#define implements public
//地块贴图实际大小为16(GRID有实际效果的可选大小只有16 8 4 2)
#define GRID 8*2
//技能id
#define SkId BYTE

//using namespace std;
//using namespace std::placeholders;
//可变数组
template<typename T>using Varrays = std::vector<T>;
//数组表
template<typename T>using ArrayList = std::vector<T>;
//邻接表
template<typename T>using LinkedList = std::list<T>;

//背包类
using Package = std::vector<std::pair<int, int>>;
typedef int Sub;//下标类型
typedef Sub RowsSub;//行标类型
typedef Sub ColsSub;//列标类型
typedef __int64 I64;
//_ASSERTE(_CrtCheckMemory());//内存检查



/*与Biology相关的三种类型:人物 怪物 管理员*/
enum TYPE { PLAYER, PEOPLE, MANAGER, MONSTER };
//正常 异常 昏迷(死亡用成员方法判断) 退出 被阻挡 绕行状态
enum STATUS { NORMAL, ABNORMAL, CONK, QUIT, BLOCKING, BYPASS };
/*视图: up(向上)->背影, down->正面, left->左, right->右  ViewCount:视图个数*/
enum View { BACK, FRONT, LEFT, RIGHT, ViewCount };
//用于指定动画静止时是播放哪一帧(起始帧, 上一帧)
enum StaticFrameEnum { ORIGIN_FRAME, PAST_FRAME };
//步进方向
enum STEP { STEP_X_AXIS, STEP_Y_AXIS };
//模式
enum MODE { ON, OFF, UNDEFINED };
//constant.h
//默认视图(此值转换为int必须是0)
const View defaultView = BACK;

/*
SHORT GetKeyState(int nVirtKey);// virtual-key code
作用：返回键的状态，按下、释放或锁定（down、up or toggled）
参数：虚拟键代码（VK_)。如果是字母a-z、A-Z 或数字0-9， 则为其对应的ASCII码（比如字母O的ASCII码为十六进制的0x4F）
返回值：返回码的高位显示当前是否有键被按下，低位（0位）则显示NumLock、CapsLock、ScrollLock的状态（ON或OFF，为ON时键盘指示灯亮）。即高位为1，返回值小于0，说明有键按下；最低位为1表示处于锁定（ON）状态
*/
#define KEY_DOWN_DETECTION(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? true : false) 
#define KEY_UP_DETECTION(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? false : true)

struct MOUSEMSG
{
	UINT uMsg;      // 当前鼠标消息
	int mkCtrl;     // Ctrl 键是否按下
	int mkShift;    // Shift 键是否按下
	int mkLButton;  // 鼠标左键是否按下
	int mkMButton;  // 鼠标中键是否按下
	int mkRButton;  // 鼠标右键是否按下
	int x;          // 当前鼠标 x 坐标
	int y;          // 当前鼠标 y 坐标
	int wheel;      // 鼠标滚轮滚动值
};
#endif