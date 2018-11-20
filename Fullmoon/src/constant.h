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
	//��ʼ��
	static void initialize(GP Size const &mainCanvasSize){
		initialization = true;
		Constant::mainCanvasSize = mainCanvasSize;
	}
	//������ת��Ϊ���±�
	static Sub rowSub(int iRow){
		/*
		floor: �������Ա������������
		ceil : ��С���Ա������������
		round: �������뵽���ڽ�������
		������������ڵ�ͼ�赲�ж�ʱ�ﵽһ���Ӿ��ϵ�ƽ��
		*/
		//��������ÿ�м�ļ��(�߶�)=���±�
		return (int)round(iRow / (double)Constant::GRID_CELL.Height);
	}
	//���±�
	static Sub colSub(int iCol){
		return (int)round(iCol / (double)Constant::GRID_CELL.Width);
	}

private:
	static bool initialization;
};

#endif