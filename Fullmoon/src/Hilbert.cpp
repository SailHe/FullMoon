// �������ƣ�Hilbert (ϣ������)���� (���� L ϵͳ����)
// ���뻷����Visual C++ 6.0��EasyX 2011���ݰ�
// ����޸ģ�2011-3-7
//
#include <graphics.h>
#include <conio.h>

int g_len;	// Hilbert ���ߵĵ�λ����

// �ݹ���� Hilbert ����
void hilbert(LPCTSTR cmd, int level)
{
	static BYTE d = 0;								// ����
	static POINT c[4] = { 1, 0, 0, -1, -1, 0, 0, 1 };	// �����Ӧ����ϵ��

	if (level < 0) return;

	// ���������ַ���
	int i = 0;
	while (cmd[i])
	{
		switch (cmd[i++])
		{
		case '+':	d = (d + 1) & 3;							break;
		case '-':	d = (d - 1) & 3;							break;
		case 'X':	hilbert("+YF-XFX-FY+", level - 1);			break;
		case 'Y':	hilbert("-XF+YFY+FX-", level - 1);			break;
		case 'F':	linerel(c[d].x * g_len, c[d].y * g_len);	break;
		}
	}
}

// ������
void main()
{
	// ���û�ͼ����
	initgraph(800, 600);						// ���ô��ڴ�С
	outtextxy(20, 550, "�� 1��8 ��ʾ��ͬ����� Hilbert ���ߣ��� ESC �˳���");
	rectangle(143, 23, 657, 537);				// ���Ʊ�ʾ��Χ�ľ��ο�
	setorigin(144, 24);							// ����ԭ������
	setcolor(RED);								// ������ɫ
	setfillstyle(BLACK);

	int level = '5';							// ���ó�ʼ����

	do
	{
		if (level >= '1' && level <= '8')		// ������ 1��8
		{
			level -= '0';						// ת��Ϊ��Ӧ������ֵ
			bar(0, 0, 511, 511);				// ��ջ�ͼ��
			g_len = 512 >> level;				// ���㵥λ����
			moveto(g_len / 2, 512 - g_len / 2);	// �趨���

			hilbert("X", level);				// �ݹ���� Hilbert ����
		}
	} while ((level = getch()) != 27);			// �� ESC �˳�

	closegraph();
}