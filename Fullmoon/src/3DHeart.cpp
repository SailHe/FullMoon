/////////////////////////////////////////////////////////
// �������ƣ��������Ϧ���� ���� 3D Heart
// ���뻷����Visual Studio 2017��EasyX 20151015(beta)
// ��    �ߣ�Werelone <werelone.cn>
// ����޸ģ�2017-8-19
// ��Ŀ���ͣ�Win32 Console Application
//

#include <easyx.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
using namespace std;

// �궨����Ļ��Ⱥ͸߶�
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// �� �ṹ��
typedef struct Point
{
	int x = 0;
	int y = 0;
	Point(int x, int y){
		this->x = x;
		this->y = y;
	}
	Point() = default;
};

// ���� �ṹ��
// width��height �ֱ��������ӵĿ�͸�
// x��y ��ʾ���ӵ���ά���꣨z ������Ϊ 0��
// color ��ʾ�����ӵ���ɫ
typedef struct Atom
{
	static const int width = 2;
	static const int height = 4;
	int x = 0;
	int y = 0;
	COLORREF color;
};


// ȫ�ֱ�������

vector<Atom> _atomGroup;	// ����Ⱥ
vector<Atom> _wordGroup;	// ��������Ⱥ

double _angle_x = 0;		// �� x ����ת�Ƕ�
double _angle_y = 0;		// �� y ����ת�Ƕ�

MOUSEMSG _lastMsg;			// ��¼�ϴλ�ͼʱ�������Ϣ
MOUSEMSG _nowMsg;			// ��¼��ǰ�����Ϣ


// ��������

void InitQuietHeart();		// ������ֹ����������Ⱥ�������ã����Ա�����
void BeatHeart();			// �������������࣬ÿ����һ�θú���������һ֡
void InitBlessingWord();	// ����ף����������Ⱥ
void Rotate();				// ������ת�Ƕ�
void Draw();				// ������άͶӰ
Point GetConvertPoint(int x, int y);	// ����ά����ת��ΪͶӰ��Ļ�ͼ��������


int main3DHeart()
{
	initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);						// ��ʼ������
	setorigin(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);				// ����ԭ��Ϊ��Ļ����
	SetWindowText(GetHWnd(), _T("�������Ϧ���� ���� 3D Heart"));	// ���ô��ڱ���

	BeatHeart();			// ��������������
	InitBlessingWord();		// ��������
	Draw();					// ����һ�λ�ͼ
	bool beating = true;	// ���������ı���
	
	while (true)			// ������ѭ��
	{
		while (MouseHit())	// ������������Ϣ
		{
			MOUSEMSG msg = GetMouseMsg();
			// ����������ʱ����¼���λ��
			if (msg.uMsg == WM_LBUTTONDOWN)
			{
				_lastMsg = msg;
			}
			// ���������£�����������ƶ�ʱ��������ת
			if (msg.mkLButton && msg.uMsg == WM_MOUSEMOVE)
			{
				beating = false;	// ��תͼ��ʱ��ֹͣ����
				_nowMsg = msg;		// ��ȡ��ǰ���λ��
				Rotate();			// ͨ���Ա���һ�λ�ͼʱ�����λ�ã�������ת
				_lastMsg = _nowMsg;	// ��¼����һ�ε����λ��
				Draw();				// ����һ�λ�ͼ
				circle(0, 0, 2);	// ��������ԭ��
			} 
			else if (msg.mkRButton)
			{
				beating = false;	// ����Ҽ�����ʱ��ֹͣ����
			}
			else
			{
				beating = true;		// ���������������
			}
		}
		// ��������
		if (beating)
		{
			BeatHeart();
			Draw();
		}
		// ��ʱ
		Sleep(16);
	}
	// �رջ�ͼ����
	closegraph();
	return 0;
}


// ��������ʱ�ĸ�������
float f(float x, float y, float z)
{
	float a = x * x + 9.0f / 4.0f * y * y + z * z - 1;
	return a * a * a - x * x * z * z * z - 9.0f / 80.0f * y * y * z * z * z;
}

// ��������ʱ�ĸ�������
float h(float x, float z)
{
	for (float y = 1.0f; y >= 0.0f; y -= 0.001f)
	{
		if (f(x, y, z) <= 0.0f)
		{
			return y;
		}
	}
	return 0.0f;
}

void InitQuietHeart()
{
	// �����������Ⱥ
	int _x = 0;
	int _y = -30;
	for (float z = 1.5f; z > -1.5f; z -= 0.05f)
	{
		_x = -60;
		for (float x = -1.5f; x < 1.5f; x += 0.025f)
		{
			float v = f(x, 0.0f, z);
			if (v <= 0.0f)
			{
				// ��������
				Atom a;
				// ������������
				a.x = _x;
				a.y = _y;
				// �����λ����ɫ
				float y0 = h(x, z);
				float ny = 0.01f;
				float nx = h(x + ny, z) - y0;
				float nz = h(x, z + ny) - y0;
				float nd = 1.0f / sqrtf(nx * nx + ny * ny + nz * nz);
				float d = (nx + ny - nz) * nd * 0.5f + 0.5f;
				switch ((int)(d * 5.0f))
				{
				case 0: a.color = 0x111133; break;
				case 1: a.color = 0x111144; break;
				case 2: a.color = 0x111155; break;
				case 3: a.color = 0x111177; break;
				case 4: a.color = 0x111199; break;
				case 5: a.color = 0x1111aa; break;
				case 6: a.color = 0x2222cc; break;
				case 7: a.color = 0x2222ee; break;
				case 8: a.color = 0x3333ff; break;
				}
				_atomGroup.push_back(a);
			}
			_x++;
		}
		_y++;
	}
}

void BeatHeart()
{
	// ʱ����� t
	static float t = 0.0f;
	t += (t > 100) ? -100 : 0.1f;
	// һϵ�и�������...
	float s = sinf(t);
	float a = s * s * s * s * 0.2f;
	// �������Ⱥ
	_atomGroup.clear();
	// �������Ⱥ
	int _x = 0;
	int _y = -20;
	for (float z = 1.3f; z > -1.2f; z -= 0.05f)
	{
		float tz = z * (1.2f - a);
		_x = -60;
		for (float x = -1.5f; x < 1.5f; x += 0.025f)
		{
			float tx = x * (1.2f + a);
			float v = f(tx, 0.0f, tz);
			if (v <= 0.0f)
			{
				// ��������
				Atom atom;
				// ������������
				atom.x = _x;
				atom.y = _y;
				// ����������ɫ
				// һϵ�и�������...
				float y0 = h(tx, tz);
				float ny = 0.01f;
				float nx = h(tx + ny, tz) - y0;
				float nz = h(tx, tz + ny) - y0;
				float nd = 1.0f / sqrtf(nx * nx + ny * ny + nz * nz);
				float d = (nx + ny - nz) * nd * 0.5f + 0.5f;
				switch ((int)(d * 5.0f))
				{
				case 0: atom.color = 0x111133; break;
				case 1: atom.color = 0x111144; break;
				case 2: atom.color = 0x111155; break;
				case 3: atom.color = 0x111177; break;
				case 4: atom.color = 0x111199; break;
				case 5: atom.color = 0x1111aa; break;
				case 6: atom.color = 0x2222cc; break;
				case 7: atom.color = 0x2222ee; break;
				case 8: atom.color = 0x3333ff; break;
				}
				// ��ӽ�����Ⱥ
				_atomGroup.push_back(atom);
			}
			_x++;
		}
		_y++;
	}
}

void InitBlessingWord()
{
	char blessing[7][34] = {	// ����Ϧ���֡�����
		{ 0,0,0,0,0,0,0, 0,0, 0,0,0,1,0,0,0, 0,0, 0,1,0,0,1,0,0, 0,0, 0,0,0,0,1,0,0 },
		{ 0,0,1,0,0,0,1, 0,0, 0,0,1,1,1,0,0, 0,0, 0,1,0,1,1,1,1, 0,0, 0,1,1,1,0,0,0 },
		{ 0,0,1,0,0,1,0, 0,0, 0,1,1,0,0,1,0, 0,0, 0,1,0,0,1,0,1, 0,0, 1,0,0,1,0,0,0 },
		{ 0,0,1,1,1,0,0, 0,0, 1,0,0,1,0,1,0, 0,0, 1,1,1,1,1,1,1, 0,0, 1,1,1,1,1,1,1 },
		{ 0,1,1,0,0,0,1, 0,0, 0,0,0,0,1,0,0, 0,0, 0,1,0,0,1,0,0, 0,0, 0,0,0,1,0,0,0 },
		{ 1,0,0,1,1,1,0, 0,0, 0,0,0,1,0,0,0, 0,0, 0,1,0,1,0,1,0, 0,0, 0,1,0,1,0,1,0 },
		{ 0,0,0,0,0,0,0, 0,0, 0,0,1,0,0,0,0, 0,0, 0,1,1,0,0,0,1, 0,0, 1,0,0,1,0,0,1 }
	};
	// �������Ⱥ
	for (int i = 0; i < 7; i++)
	{
		for (int j = -17; j < 17; j++)
		{
			if (blessing[i][j+17] == 1)
			{
				Atom a;
				a.x = j;
				a.y = 30 + i;
				a.color = WHITE;
				_wordGroup.push_back(a);
			}
		}
	}
}

void Rotate()
{
	// ������ת�Ƕ�
	_angle_x += (_nowMsg.x - _lastMsg.x) / 100.0;
	_angle_y += (_nowMsg.y - _lastMsg.y) / 100.0;
	// ��ֹ�Ƕȹ�����С
	if (fabs(_angle_x) > 2 * M_PI)
	{
		_angle_x > 0 ? _angle_x -= 2 * M_PI : _angle_x += 2 * M_PI;
	}
	if (fabs(_angle_y) > 2 * M_PI)
	{
		_angle_y > 0 ? _angle_y -= 2 * M_PI : _angle_y += 2 * M_PI;
	}
}

void Draw()
{
	static Point p;
	// ����������ͼ
	BeginBatchDraw();
	// ��յ�ǰ����
	cleardevice();
	// ��������Ⱥ
	for (Atom a : _atomGroup)
	{
		// ������ɫ
		setfillcolor(a.color);
		// ��ȡͶӰ����
		p = GetConvertPoint(a.x, a.y);
		// ��������
		solidrectangle(p.x - Atom::width / 2, p.y - Atom::height / 2, p.x + Atom::width / 2, p.y + Atom::height / 2);
	}
	// ������������Ⱥ
	setfillcolor(WHITE);
	for (Atom a : _wordGroup)
	{
		// ��ȡͶӰ����
		p = GetConvertPoint(a.x, a.y);
		// ��������
		solidrectangle(p.x - 1, p.y - 1, p.x + 1, p.y + 1);
	}
	// ֹͣ������ͼ
	EndBatchDraw();
}

Point GetConvertPoint(int x, int y)
{
	static int x0, y0;
	// ��������Կ��
	x *= Atom::width;
	y *= Atom::height;
	// ����ת������
	x0 = int(x * cos(_angle_x) + y * sin(_angle_y) * sin(_angle_x));
	y0 = int(y * cos(_angle_y) + x * sin(_angle_x) * sin(_angle_y));
	return Point( x0, y0 );
}
