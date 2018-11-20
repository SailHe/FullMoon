// �������ƣ������
// ���뻷����VC6 + EasyX v20100825
// �� д �ߣ�zh@easyx.cn
// ��д���ڣ�2010-11-25
//
#include <graphics.h>
#include <conio.h>
#include <time.h>
#define WIDTH   640
#define HEIGHT  480
#define MAXSTEP 9



// �ڵ㣨��������Ľڵ㻷�����һ�����㣩
struct POINTNODE
{
	POINT       pos;    // λ��
	POINTNODE*  next;   // ָ����һ���ڵ��ָ��
};



// ���㣨�ĸ����㣬���һ������Σ�
class VERTEX
{
public:
	POINTNODE*  m_head;     // ͷ����ָ��
	POINT       m_step;     // �ƶ�����

	// ���캯��
	VERTEX()
	{
		m_head = NULL;
	}

	// ��������
	~VERTEX()
	{
		if (m_head != NULL)
			delete[] m_head;
	}

	// ��ʼ������
	void Init(int count)
	{
		// ��ʼ��ǰ������
		m_step.x = ((rand() % 2) * 2 - 1) * (rand() % MAXSTEP + 1);
		m_step.y = ((rand() % 2) * 2 - 1) * (rand() % MAXSTEP + 1);

		// ��ʼ���ڵ㻷
		m_head = new POINTNODE[count];
		m_head[0].pos.x = rand() % WIDTH;
		m_head[0].pos.y = rand() % HEIGHT;
		m_head[0].next = &m_head[count - 1];
		for (int i = 1; i <= count; i++)
		{
			m_head[i].pos.x = m_head[i - 1].pos.x - m_step.x;
			m_head[i].pos.y = m_head[i - 1].pos.y - m_step.y;
			m_head[i].next = &m_head[i - 1];
		}
	}

	// ��ȡͷ���ڵ�����
	POINT GetHead()
	{
		return m_head->pos;
	}

	// ��ȡβ���ڵ�����
	POINT GetTail()
	{
		return m_head->next->pos;
	}

	// �ƶ�����
	void Move()
	{
		m_head->next->pos.x = m_head->pos.x + m_step.x;
		m_head->next->pos.y = m_head->pos.y + m_step.y;
		m_head = m_head->next;

		// �ж϶����Ƿ�Խ��
		if (m_head->pos.x < 0)           { m_head->pos.x = -m_head->pos.x;             m_step.x = rand() % MAXSTEP + 1; }
		else if (m_head->pos.x >= WIDTH)  { m_head->pos.x -= m_head->pos.x - WIDTH + 1; m_step.x = -rand() % MAXSTEP - 1; }
		if (m_head->pos.y < 0)           { m_head->pos.y = -m_head->pos.y;             m_step.y = rand() % MAXSTEP + 1; }
		else if (m_head->pos.y >= HEIGHT) { m_head->pos.y -= m_head->pos.y - HEIGHT + 1;    m_step.y = -rand() % MAXSTEP - 1; }
	}
};



// �����
class POLYGON
{
private:
	COLORREF    m_color;        // ����ε���ɫ
	VERTEX      m_vertex[4];    // ���ɶ���ε��ĸ�����

public:
	// ���캯��
	POLYGON(int lines)
	{
		// ��ʼ����ɫ
		m_color = HSLtoRGB(float(rand() % 360), 1.0, 0.5);

		// ��ʼ���ĸ�����
		for (int i = 0; i<4; i++)
			m_vertex[i].Init(lines);
	}

	// �ƶ������
	void Move()
	{
		int i;

		// ��������ε�β��
		setcolor(BLACK);
		moveto(m_vertex[3].GetTail().x, m_vertex[3].GetTail().y);
		for (i = 0; i<4; i++)
			lineto(m_vertex[i].GetTail().x, m_vertex[i].GetTail().y);

		// �ƶ�ÿ������
		for (i = 0; i<4; i++)
			m_vertex[i].Move();

		// ������ε�ͷ��
		setcolor(m_color);
		moveto(m_vertex[3].GetHead().x, m_vertex[3].GetHead().y);
		for (i = 0; i<4; i++)
			lineto(m_vertex[i].GetHead().x, m_vertex[i].GetHead().y);

		// �� 1% �ĸ��ʸ�����ɫ
		if (rand() % 100 == 0)
			m_color = HSLtoRGB(float(rand() % 360), 1.0, 0.5);
	}
};



// ������
void main()
{
	// ��ʼ����ͼ����
	initgraph(WIDTH, HEIGHT);

	// ��ʼ���������
	srand((unsigned)time(NULL));

	// ������������ζ��󣬷ֱ��� 7 ���ߺ� 12 ����
	POLYGON s1(7), s2(12);

	// �ƶ�����Σ���������˳�
	while (!kbhit())
	{
		s1.Move();
		s2.Move();
		Sleep(20);
	}

	// �رջ�ͼ����
	closegraph();
}