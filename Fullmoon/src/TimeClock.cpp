#include "stdafx.h"
#include "TimeClock.h"

TimeClock::TimeClock()
{
	QueryPerformanceFrequency(&litmp);	// 获取时钟频率
	dfFreq = (double)litmp.QuadPart;

	QueryPerformanceCounter(&litmp);		// 获取时钟计数(频率与时间的乘积)
	QPart1 = litmp.QuadPart;
}

TimeClock::TimeClock(double drtTime){
	QueryPerformanceFrequency(&litmp);	// 获取时钟频率
	dfFreq = (double)litmp.QuadPart;

	QueryPerformanceCounter(&litmp);		// 获取时钟计数(频率与时间的乘积)
	QPart1 = litmp.QuadPart;
	SetDrtTime(drtTime);
}

void TimeClock::SetDrtTime(double t){
	drtTime = t;
}

void TimeClock::Timer1(){
	QueryPerformanceCounter(&litmp);		// 获取时钟计数(频率与时间的乘积)
	QPart1 = litmp.QuadPart;
}

void TimeClock::Init(){
	QueryPerformanceCounter(&litmp);
	QPart1 = litmp.QuadPart;
	QPart2 = litmp.QuadPart;
}

bool TimeClock::IsTimeOut(){
	QueryPerformanceCounter(&litmp);
	QPart2 = litmp.QuadPart;/*更新当前时间*/
	if (((double)(QPart2 - QPart1) * 1000) / dfFreq > drtTime){
		QPart1 = QPart2;/*重置起计时间==>当前时间差为0*/
		return true;
	}
	return false;
}

double TimeClock::GetRemainingTime(){
	QueryPerformanceCounter(&litmp);
	QPart2 = litmp.QuadPart;/*更新当前时间*/
	return drtTime - ((double)(QPart2 - QPart1) * 1000) / dfFreq;
}
