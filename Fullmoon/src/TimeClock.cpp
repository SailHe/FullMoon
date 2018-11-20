#include "stdafx.h"
#include "TimeClock.h"

TimeClock::TimeClock()
{
	QueryPerformanceFrequency(&litmp);	// ��ȡʱ��Ƶ��
	dfFreq = (double)litmp.QuadPart;

	QueryPerformanceCounter(&litmp);		// ��ȡʱ�Ӽ���(Ƶ����ʱ��ĳ˻�)
	QPart1 = litmp.QuadPart;
}

TimeClock::TimeClock(double drtTime){
	QueryPerformanceFrequency(&litmp);	// ��ȡʱ��Ƶ��
	dfFreq = (double)litmp.QuadPart;

	QueryPerformanceCounter(&litmp);		// ��ȡʱ�Ӽ���(Ƶ����ʱ��ĳ˻�)
	QPart1 = litmp.QuadPart;
	SetDrtTime(drtTime);
}

void TimeClock::SetDrtTime(double t){
	drtTime = t;
}

void TimeClock::Timer1(){
	QueryPerformanceCounter(&litmp);		// ��ȡʱ�Ӽ���(Ƶ����ʱ��ĳ˻�)
	QPart1 = litmp.QuadPart;
}

void TimeClock::Init(){
	QueryPerformanceCounter(&litmp);
	QPart1 = litmp.QuadPart;
	QPart2 = litmp.QuadPart;
}

bool TimeClock::IsTimeOut(){
	QueryPerformanceCounter(&litmp);
	QPart2 = litmp.QuadPart;/*���µ�ǰʱ��*/
	if (((double)(QPart2 - QPart1) * 1000) / dfFreq > drtTime){
		QPart1 = QPart2;/*�������ʱ��==>��ǰʱ���Ϊ0*/
		return true;
	}
	return false;
}

double TimeClock::GetRemainingTime(){
	QueryPerformanceCounter(&litmp);
	QPart2 = litmp.QuadPart;/*���µ�ǰʱ��*/
	return drtTime - ((double)(QPart2 - QPart1) * 1000) / dfFreq;
}
