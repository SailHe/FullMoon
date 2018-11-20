/////////////////////////////////////////////////////////////////////////
// 程序名称：将图片进行模糊处理
// 编译环境：Visual C++ 6.0 / 2010，EasyX 20130114(beta)
// 作　　者：krissi <zh@easyx.cn>
// 最后修改：2013-1-19
//
#include <graphics.h>
#include <conio.h>				

// 将图片进行模糊处理
void Blur(IMAGE *pimg)
{
	DWORD*	pMem = GetImageBuffer(pimg);

	COLORREF color;
	int	r, g, b;
	int	num = 0;
	int	width = pimg->getwidth();
	int	height = pimg->getheight();


	// 计算 9 格方向系数
	int	cell[9] = { -(width + 1), -width, -(width - 1), -1, 0, 1, width - 1, width, width + 1 };

	// 逐个像素点读取计算
	for (int i = width * height - 1; i >= 0; i--)
	{
		// 累加周围 9 格颜色值
		for (int n = 0, m = 0; n < 9; n++)
		{
			// 位置定位
			num = i + cell[n];

			// 判断位置值是否越界
			if (num < 0 || num >= width * height)
			{
				color = RGB(0, 0, 0);		// 将越界像素赋值为0
				m++;						// 统计越界像素数
			}
			else
				color = pMem[num];

			// 累加颜色值
			r += GetRValue(color);
			g += GetGValue(color);
			b += GetBValue(color);
		}
		int m = 0;
		// 将平均值赋值该像素
		pMem[i] = RGB(r / (9 - m), g / (9 - m), b / (9 - m));
		r = g = b = 0;
	}
}

// 主函数
void main()
{
	// 初始化绘图环境
	initgraph(640, 480);

	// 获取图像
	IMAGE img;
	loadimage(&img, _T("res\\jpg.jpg"));

	// 显示原始图像
	putimage(0, 0, &img);

	// 任意键执行
	getch();

	// 模糊处理 10 次
	for (int m = 0; m < 5; m++)
		Blur(&img);

	// 显示处理后的图像
	putimage(0, 0, &img);

	// 任意键关闭绘图环境
	getch();
	closegraph();
}