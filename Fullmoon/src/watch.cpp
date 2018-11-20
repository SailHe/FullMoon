#include "Elf.h"
void draw(SYSTEMTIME ti, int x, int y, int len)
{
	double a_hour, a_min, a_sec;					// ʱ���֡�����Ļ���ֵ
	int x_hour, y_hour, x_min, y_min, x_sec, y_sec;	// ʱ���֡������ĩ��λ��
	int lenH = len*0.95, lenM = len*0.75, lenS = len*0.55;

	// ����ʱ���֡�����Ļ���ֵ
	a_sec = ti.wSecond * 2 * PI / 60;
	a_min = ti.wMinute * 2 * PI / 60 + a_sec / 60;
	a_hour = ti.wHour * 2 * PI / 12 + a_min / 12;

	// ����ʱ���֡������ĩ��λ��
	x_sec = x + (int)(lenH * sin(a_sec));
	y_sec = y - (int)(lenH * cos(a_sec));
	x_min = x + (int)(lenM * sin(a_min));
	y_min = y - (int)(lenM * cos(a_min));
	x_hour = x + (int)(lenS * sin(a_hour));
	y_hour = y - (int)(lenS * cos(a_hour));

	// ��ʱ��
	setlinestyle(PS_SOLID, len*0.05, NULL);
	setlinecolor(WHITE);
	line(x, y, x_hour, y_hour);

	// ������
	setlinestyle(PS_SOLID, len*0.03, NULL);
	setlinecolor(LIGHTGRAY);
	line(x, y, x_min, y_min);

	// ������
	setlinestyle(PS_SOLID, len*0.01, NULL);
	setlinecolor(RED);
	line(x, y, x_sec, y_sec);
	graphdefaults();
}
//Բ�̶̿�(Բ������ �뾶 �̶���Ŀ ��ɫ �� �� ���ӳ�)
void watchScale(int ox, int oy, int r, int count = 0, int colour = WHITE, int width = 4, int in = 2, int out = 2){
	setlinestyle(PS_SOLID, r*(width / 100.0), NULL);
	setlinecolor(colour);
	int x1, y1, x2, y2;
	int radian = 0;
	int mod = count == 0 ? 1 : 2 * PI*r / count;
	in = 100 - in;
	out = 100 + out;
	for (double rad = 0; rad < 2 * PI; rad += 0.0001){
		x1 = (int)(r*(in / 100.0) * cos(rad) + ox);
		y1 = (int)(r*(in / 100.0) * sin(rad) + oy);

		x2 = (int)((out / 100.0)*r * cos(rad) + ox);
		y2 = (int)((out / 100.0)*r * sin(rad) + oy);
		radian = rad*r;
		if (radian % mod == 0)
			line(x1, y1, x2, y2), ++radian;
	}
	setlinestyle(PS_SOLID, 1, NULL);
	setlinecolor(WHITE);
}
// ����
void watchFace(int ox, int oy, int r){
	settextstyle(r*0.1, 0, _T(" "));
	outtextxy(ox - r*0.20, oy + r*0.3, _T("HelloWord"));//logo

	setlinecolor(WHITE);
	circle(ox, oy, r*0.95);
	setlinecolor(GREEN);
	circle(ox, oy, r);
	setlinecolor(CYAN);
	setlinestyle(PS_SOLID, 4, NULL);
	circle(ox, oy, r*1.05);

	watchScale(ox, oy, r, 3, BLUE);
	watchScale(ox, oy, r, 12, YELLOW, 2);
	watchScale(ox, oy, r*0.95, 120, RED, 1, 0);
	graphdefaults();
}
// �ӱ����  ������˳�
void watch(int x, int y, int r){
	setwritemode(R2_XORPEN);//XOR ��ͼ
	BeginBatchDraw();
	watchFace(x, y, r);
	// ���Ʊ���
	SYSTEMTIME ti;				// ����������浱ǰʱ��
	while (!kbhit()){
		GetLocalTime(&ti);		// ��ȡ��ǰʱ��
		draw(ti, x, y, r);	// ������
		FlushBatchDraw();
		Sleep(1000);							// ��ʱ 1 ��
		draw(ti, x, y, r);	// �����루������ͻ�����Ĺ�����һ���ģ�
	}
	watchFace(x, y, r);
	EndBatchDraw();
	getch();
}