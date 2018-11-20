#ifndef __Constant_H
#define __Constant_H
#include "stdafx.h"
//#include <graphics.h>
#include "../Includes/turbo/graphics.h"
#include "gdi.h"

class Constant{
public:
	static const unsigned BUFFER_MAX_BIT = 1024;
	static const GP Size GRID_CELL;
	static const GP Point ZERO_POINT;
	static GP Size mainCanvasSize;
	//初始化
	static void initialize(GP Size const &mainCanvasSize){
		initialization = true;
		Constant::mainCanvasSize = mainCanvasSize;
	}
	//将行数转换为行下标
	static Sub rowSub(int iRow){
		/*
		floor: 不大于自变量的最大整数
		ceil : 不小于自变量的最大整数
		round: 四舍五入到最邻近的整数
		四舍五入可以在地图阻挡判断时达到一种视觉上的平衡
		*/
		//行数除以每行间的间隔(高度)=行下标
		return (int)round(iRow / (double)Constant::GRID_CELL.Height);
	}
	//列下标
	static Sub colSub(int iCol){
		return (int)round(iCol / (double)Constant::GRID_CELL.Width);
	}

private:
	static bool initialization;
};

#endif