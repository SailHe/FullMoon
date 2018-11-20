#include <graphics.h>
#include <conio.h>
#include "Biology.h"

// 将图片转换为马赛克效果:
//作 者：krissi <zh@easyx.cn>
// 参数：
//		pimg: 待处理的 IMAGE 对象指针
//		tilesize: 马赛克的尺寸
//		startx: 马赛克的平铺起始位置 x 坐标
//		starty: 马赛克的平铺起始位置 y 坐标
void Mosaic(IMAGE *pimg, int tilesize, int startx, int starty)
{
	int	width = pimg->getwidth();		// 图像的宽
	int	height = pimg->getheight();	// 图像的高
	int	redsum;			// 红色值的和
	int	greensum;		// 绿色值的和
	int	bluesum;		// 蓝色值的和
	int	count;			// 每个小方块内的像素数量
	int	color;			// 每个像素的颜色
	int x, y, tx, ty;	// 循环变量

	// 获取指向显存的指针
	DWORD* pMem = GetImageBuffer(pimg);

	// 求出左上角第一个方块的坐标
	startx = (startx % tilesize == 0 ? 0 : startx % tilesize - tilesize);
	starty = (starty % tilesize == 0 ? 0 : starty % tilesize - tilesize);

	// 处理每一个小方块
	for (y = starty; y < height; y += tilesize)
	for (x = startx; x < width; x += tilesize)
	{
		// 清空累加值
		redsum = greensum = bluesum = count = 0;

		// 求小方块的红、绿、蓝颜色值的和
		for (ty = min(y + tilesize, height) - 1; ty >= max(y, 0); ty--)
		for (tx = min(x + tilesize, width) - 1; tx >= max(x, 0); tx--)
		{
			color = pMem[ty * width + tx];
			redsum += GetRValue(color);
			greensum += GetGValue(color);
			bluesum += GetBValue(color);
			count++;
		}

		// 求红、绿、蓝颜色的平均值
		redsum /= count;
		greensum /= count;
		bluesum /= count;

		// 设置小方块内的每个像素为平均颜色值
		color = RGB(redsum, greensum, bluesum);
		for (ty = min(y + tilesize, height) - 1; ty >= max(y, 0); ty--)
		for (tx = min(x + tilesize, width) - 1; tx >= max(x, 0); tx--)
			pMem[ty * width + tx] = color;
	}
}

/*马赛克穿越动画 第一次只有加载 没有马赛克*/
void DisplayArea::mosaicImage(GP Bitmap const *img, Graphics *graphics){
	//GP Bitmap *timg;
	for (int i = 2; i < 200; ++i){
		///*timg = *img;
		//Mosaic(timg, i, 320, 240);
		//graphics->DrawImage(timg, 0, 0);
		//putimage(0, 0, &timg);
		//FlushBatchDraw();
	}
}
