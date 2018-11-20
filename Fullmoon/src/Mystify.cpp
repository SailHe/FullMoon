// 程序名称：变幻线
// 编译环境：VC6 + EasyX v20100825
// 编 写 者：zh@easyx.cn
// 编写日期：2010-11-25
//
#include <graphics.h>
#include <conio.h>
#include <time.h>
#define WIDTH   640
#define HEIGHT  480
#define MAXSTEP 9



// 节点（多个连续的节点环，组成一个顶点）
struct POINTNODE
{
	POINT       pos;    // 位置
	POINTNODE*  next;   // 指向下一个节点的指针
};



// 顶点（四个顶点，组成一个多边形）
class VERTEX
{
public:
	POINTNODE*  m_head;     // 头结点的指针
	POINT       m_step;     // 移动步长

	// 构造函数
	VERTEX()
	{
		m_head = NULL;
	}

	// 析构函数
	~VERTEX()
	{
		if (m_head != NULL)
			delete[] m_head;
	}

	// 初始化环链
	void Init(int count)
	{
		// 初始化前进方向
		m_step.x = ((rand() % 2) * 2 - 1) * (rand() % MAXSTEP + 1);
		m_step.y = ((rand() % 2) * 2 - 1) * (rand() % MAXSTEP + 1);

		// 初始化节点环
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

	// 获取头部节点坐标
	POINT GetHead()
	{
		return m_head->pos;
	}

	// 获取尾部节点坐标
	POINT GetTail()
	{
		return m_head->next->pos;
	}

	// 移动顶点
	void Move()
	{
		m_head->next->pos.x = m_head->pos.x + m_step.x;
		m_head->next->pos.y = m_head->pos.y + m_step.y;
		m_head = m_head->next;

		// 判断顶点是否越界
		if (m_head->pos.x < 0)           { m_head->pos.x = -m_head->pos.x;             m_step.x = rand() % MAXSTEP + 1; }
		else if (m_head->pos.x >= WIDTH)  { m_head->pos.x -= m_head->pos.x - WIDTH + 1; m_step.x = -rand() % MAXSTEP - 1; }
		if (m_head->pos.y < 0)           { m_head->pos.y = -m_head->pos.y;             m_step.y = rand() % MAXSTEP + 1; }
		else if (m_head->pos.y >= HEIGHT) { m_head->pos.y -= m_head->pos.y - HEIGHT + 1;    m_step.y = -rand() % MAXSTEP - 1; }
	}
};



// 多边形
class POLYGON
{
private:
	COLORREF    m_color;        // 多边形的颜色
	VERTEX      m_vertex[4];    // 构成多边形的四个顶点

public:
	// 构造函数
	POLYGON(int lines)
	{
		// 初始化颜色
		m_color = HSLtoRGB(float(rand() % 360), 1.0, 0.5);

		// 初始化四个顶点
		for (int i = 0; i<4; i++)
			m_vertex[i].Init(lines);
	}

	// 移动多边形
	void Move()
	{
		int i;

		// 擦掉多边形的尾部
		setcolor(BLACK);
		moveto(m_vertex[3].GetTail().x, m_vertex[3].GetTail().y);
		for (i = 0; i<4; i++)
			lineto(m_vertex[i].GetTail().x, m_vertex[i].GetTail().y);

		// 移动每个顶点
		for (i = 0; i<4; i++)
			m_vertex[i].Move();

		// 画多边形的头部
		setcolor(m_color);
		moveto(m_vertex[3].GetHead().x, m_vertex[3].GetHead().y);
		for (i = 0; i<4; i++)
			lineto(m_vertex[i].GetHead().x, m_vertex[i].GetHead().y);

		// 有 1% 的概率更换颜色
		if (rand() % 100 == 0)
			m_color = HSLtoRGB(float(rand() % 360), 1.0, 0.5);
	}
};



// 主函数
void main()
{
	// 初始化绘图窗口
	initgraph(WIDTH, HEIGHT);

	// 初始化随机种子
	srand((unsigned)time(NULL));

	// 定义两个多边形对象，分别有 7 层线和 12 层线
	POLYGON s1(7), s2(12);

	// 移动多边形，按任意键退出
	while (!kbhit())
	{
		s1.Move();
		s2.Move();
		Sleep(20);
	}

	// 关闭绘图窗口
	closegraph();
}