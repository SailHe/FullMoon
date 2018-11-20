#include "Elf.h"
void draw(SYSTEMTIME ti, int x, int y, int len)
{
	double a_hour, a_min, a_sec;					// 时、分、秒针的弧度值
	int x_hour, y_hour, x_min, y_min, x_sec, y_sec;	// 时、分、秒针的末端位置
	int lenH = len*0.95, lenM = len*0.75, lenS = len*0.55;

	// 计算时、分、秒针的弧度值
	a_sec = ti.wSecond * 2 * PI / 60;
	a_min = ti.wMinute * 2 * PI / 60 + a_sec / 60;
	a_hour = ti.wHour * 2 * PI / 12 + a_min / 12;

	// 计算时、分、秒针的末端位置
	x_sec = x + (int)(lenH * sin(a_sec));
	y_sec = y - (int)(lenH * cos(a_sec));
	x_min = x + (int)(lenM * sin(a_min));
	y_min = y - (int)(lenM * cos(a_min));
	x_hour = x + (int)(lenS * sin(a_hour));
	y_hour = y - (int)(lenS * cos(a_hour));

	// 画时针
	setlinestyle(PS_SOLID, len*0.05, NULL);
	setlinecolor(WHITE);
	line(x, y, x_hour, y_hour);

	// 画分针
	setlinestyle(PS_SOLID, len*0.03, NULL);
	setlinecolor(LIGHTGRAY);
	line(x, y, x_min, y_min);

	// 画秒针
	setlinestyle(PS_SOLID, len*0.01, NULL);
	setlinecolor(RED);
	line(x, y, x_sec, y_sec);
	graphdefaults();
}
//圆盘刻度(圆心坐标 半径 刻度数目 颜色 宽 内 外延长)
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
// 表盘
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
// 钟表程序  任意键退出
void watch(int x, int y, int r){
	setwritemode(R2_XORPEN);//XOR 绘图
	BeginBatchDraw();
	watchFace(x, y, r);
	// 绘制表针
	SYSTEMTIME ti;				// 定义变量保存当前时间
	while (!kbhit()){
		GetLocalTime(&ti);		// 获取当前时间
		draw(ti, x, y, r);	// 画表针
		FlushBatchDraw();
		Sleep(1000);							// 延时 1 秒
		draw(ti, x, y, r);	// 擦表针（擦表针和画表针的过程是一样的）
	}
	watchFace(x, y, r);
	EndBatchDraw();
	getch();
}