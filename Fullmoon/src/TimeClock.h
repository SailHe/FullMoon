#pragma once


/*
* Timer1()  开始调用一次
* Timer2() 循环调用
* IsTimeOut() 循环调用检测时间差是否大于设置的值
*/

class TimeClock
{
public:
	TimeClock();
	TimeClock(double drtTime);
	~TimeClock() {}

	/*设置时间差, 调节速度 hf: dwMilliseconds(时间间隔 毫秒) 1000为一秒*/
	void SetDrtTime(double);

	/*更新 QPart1*/
	void Timer1();

	/*重置 t1 = t2*/
	void Init();

	/*比较两个计时时间差, 返回 true false 并重置当前时间差*/
	bool IsTimeOut();		// 时间差到

	//获取距离TimeOut的剩余时间(ms)
	double GetRemainingTime();

private:
	LARGE_INTEGER litmp;		// 定义该结构体的一个对象
	LONGLONG QPart1, QPart2;	// 用于获取结构体里面的 QuadPart 成员
	double dfFreq;				// 时钟频率
	double drtTime;				// 定义时间差, 决定物体运动速度
};