///////////////////////////////////////////////////
// �������ƣ��Ӿ��������2
// ���뻷����Visual C++ 6.0��EasyX 2013˪����
// �������ߣ�yangw80 <yw80@qq.com>
// ����޸ģ�2013-12-16
//
#include <graphics.h>
#include <conio.h>

// ���峣��
#define	UNIT		24
#define OFFSET_X	116
#define OFFSET_Y	36

// ��װ�λ�
void DrawUnit(int x, int y, int shape)
{
	// ������ɫ
	int n = ((shape & 1) ? 1 : 0) + ((shape & 2) ? 1 : 0) + ((shape & 4) ? 1 : 0);
	setlinecolor((n % 2 == 1) ? 0xaa00cc : WHITE);

	// ��֦��
	line(x - 5, y, x + 5, y);
	line(x, y - 5, x, y + 5);

	// ��Ҷ��
	rectangle(x - 1, y - 4, x + 1, y - 2);
	rectangle(x - 1, y + 2, x + 1, y + 4);
	rectangle(x - 4, y - 1, x - 2, y + 1);
	rectangle(x + 2, y - 1, x + 4, y + 1);
}

// ������
void main()
{
	// ������ͼ����
	initgraph(640, 480);
	// ���ð�ɫ����
	setbkcolor(WHITE);
	cleardevice();
	// ��������ԭ��
	setorigin(OFFSET_X, OFFSET_Y);

	int x, y;

	// �����ӱ���
	for (y = 0; y < 17; y++)
	for (x = 0; x < 17; x++)
	{
		setfillcolor(((x + y) % 2) ? 0x83be3e : 0x01dcaa);
		solidrectangle(x * UNIT, y * UNIT, (x + 1) * UNIT - 1, (y + 1) * UNIT - 1);
	}

	// ��װ�λ�
	for (y = 0; y < 16; y++)
	{
		int shape = 8 - y % 8;
		for (x = 0; x < 16; x++)
			DrawUnit((x + 1) * UNIT, (y + 1) * UNIT, shape++);
	}

	// ��������˳�
	getch();
	closegraph();
}