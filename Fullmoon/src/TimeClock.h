#pragma once


/*
* Timer1()  ��ʼ����һ��
* Timer2() ѭ������
* IsTimeOut() ѭ�����ü��ʱ����Ƿ�������õ�ֵ
*/

class TimeClock
{
public:
	TimeClock();
	TimeClock(double drtTime);
	~TimeClock() {}

	/*����ʱ���, �����ٶ� hf: dwMilliseconds(ʱ���� ����) 1000Ϊһ��*/
	void SetDrtTime(double);

	/*���� QPart1*/
	void Timer1();

	/*���� t1 = t2*/
	void Init();

	/*�Ƚ�������ʱʱ���, ���� true false �����õ�ǰʱ���*/
	bool IsTimeOut();		// ʱ��

	//��ȡ����TimeOut��ʣ��ʱ��(ms)
	double GetRemainingTime();

private:
	LARGE_INTEGER litmp;		// ����ýṹ���һ������
	LONGLONG QPart1, QPart2;	// ���ڻ�ȡ�ṹ������� QuadPart ��Ա
	double dfFreq;				// ʱ��Ƶ��
	double drtTime;				// ����ʱ���, ���������˶��ٶ�
};