///////////////////////////////////////////////////
// 程序名称：视觉错觉艺术2
// 编译环境：Visual C++ 6.0，EasyX 2013霜降版
// 作　　者：yangw80 <yw80@qq.com>
// 最后修改：2013-12-16
//
#include <graphics.h>
#include <conio.h>

// 定义常量
#define	UNIT		24
#define OFFSET_X	116
#define OFFSET_Y	36

// 画装饰花
void DrawUnit(int x, int y, int shape)
{
	// 设置颜色
	int n = ((shape & 1) ? 1 : 0) + ((shape & 2) ? 1 : 0) + ((shape & 4) ? 1 : 0);
	setlinecolor((n % 2 == 1) ? 0xaa00cc : WHITE);

	// 画枝干
	line(x - 5, y, x + 5, y);
	line(x, y - 5, x, y + 5);

	// 画叶子
	rectangle(x - 1, y - 4, x + 1, y - 2);
	rectangle(x - 1, y + 2, x + 1, y + 4);
	rectangle(x - 4, y - 1, x - 2, y + 1);
	rectangle(x + 2, y - 1, x + 4, y + 1);
}

// 主函数
void main()
{
	// 创建绘图窗体
	initgraph(640, 480);
	// 设置白色背景
	setbkcolor(WHITE);
	cleardevice();
	// 设置坐标原点
	setorigin(OFFSET_X, OFFSET_Y);

	int x, y;

	// 画格子背景
	for (y = 0; y < 17; y++)
	for (x = 0; x < 17; x++)
	{
		setfillcolor(((x + y) % 2) ? 0x83be3e : 0x01dcaa);
		solidrectangle(x * UNIT, y * UNIT, (x + 1) * UNIT - 1, (y + 1) * UNIT - 1);
	}

	// 画装饰花
	for (y = 0; y < 16; y++)
	{
		int shape = 8 - y % 8;
		for (x = 0; x < 16; x++)
			DrawUnit((x + 1) * UNIT, (y + 1) * UNIT, shape++);
	}

	// 按任意键退出
	getch();
	closegraph();
}