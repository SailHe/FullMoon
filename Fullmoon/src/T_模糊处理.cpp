/////////////////////////////////////////////////////////////////////////
// �������ƣ���ͼƬ����ģ������
// ���뻷����Visual C++ 6.0 / 2010��EasyX 20130114(beta)
// �������ߣ�krissi <zh@easyx.cn>
// ����޸ģ�2013-1-19
//
#include <graphics.h>
#include <conio.h>				

// ��ͼƬ����ģ������
void Blur(IMAGE *pimg)
{
	DWORD*	pMem = GetImageBuffer(pimg);

	COLORREF color;
	int	r, g, b;
	int	num = 0;
	int	width = pimg->getwidth();
	int	height = pimg->getheight();


	// ���� 9 ����ϵ��
	int	cell[9] = { -(width + 1), -width, -(width - 1), -1, 0, 1, width - 1, width, width + 1 };

	// ������ص��ȡ����
	for (int i = width * height - 1; i >= 0; i--)
	{
		// �ۼ���Χ 9 ����ɫֵ
		for (int n = 0, m = 0; n < 9; n++)
		{
			// λ�ö�λ
			num = i + cell[n];

			// �ж�λ��ֵ�Ƿ�Խ��
			if (num < 0 || num >= width * height)
			{
				color = RGB(0, 0, 0);		// ��Խ�����ظ�ֵΪ0
				m++;						// ͳ��Խ��������
			}
			else
				color = pMem[num];

			// �ۼ���ɫֵ
			r += GetRValue(color);
			g += GetGValue(color);
			b += GetBValue(color);
		}
		int m = 0;
		// ��ƽ��ֵ��ֵ������
		pMem[i] = RGB(r / (9 - m), g / (9 - m), b / (9 - m));
		r = g = b = 0;
	}
}

// ������
void main()
{
	// ��ʼ����ͼ����
	initgraph(640, 480);

	// ��ȡͼ��
	IMAGE img;
	loadimage(&img, _T("res\\jpg.jpg"));

	// ��ʾԭʼͼ��
	putimage(0, 0, &img);

	// �����ִ��
	getch();

	// ģ������ 10 ��
	for (int m = 0; m < 5; m++)
		Blur(&img);

	// ��ʾ������ͼ��
	putimage(0, 0, &img);

	// ������رջ�ͼ����
	getch();
	closegraph();
}