#pragma once

#include"../stdafx.h"
#include"../ExtendSpace.h"

/*
const int DIR4x[] = { -1, 0, 1, 0 };
const int DIR4y[] = { 0, 1, 0, -1 };
extern const int Dir8[8][2];
extern const int Dir4[4][2];
*/
const int DIR84R[] = { 1, 0, -1, 0, 1, -1, -1, 1 };
const int DIR84C[] = { 0, 1, 0, -1, 1, 1, -1, -1 };
enum DirCountEnum { DIR4 = 4, DIR8 = 8 };
//在做加法运算时 它的first 与 second与习惯的表示xy rc的意义相同
typedef JCE::pair<int, int> DirectionVector;
//加了extern 会出现重定义问题
const DirectionVector DIR84[8] = {
	/*r轴正方向->第4123象限 便于调试 顺序不能随意更改*/
	{ 1, 0 }/*下*/,
	{ 0, 1 }/*右*/,
	{ -1, 0 }/*上*/,
	{ 0, -1 }/*左*/,

	{ 1, 1 }/*右下*/,
	{ 1, -1 }/*右上*/,
	{ -1, -1 }/*左上*/,
	{ -1, 1 }/*左下*/,
};


struct Point2Base {
	//结论：在没有涉及绘图时最好用SubTwain 何况图中存的顶点根本就不是点好嘛。。。

	//看坐标时习惯是先看列数(x轴), 看数组时实际操作时先看行数(原点在左上角不是重点!!主要是列数很多时候对不齐!!) 
	//先定义y: 1.方便调试 2.使用初始化列表时直接传入{ r, c }相当于Sub转化为Point2
	//tips：已知类型结构（暴露给外界）且没有定义任何构造函数 才可以使用初始化列表{} 若定义了任意构造函数则初始化列表会与构造函数的参数对应
	//即此时使用{x, y} 与Point2(x, y)等价 调试时Point2 y在前对应查看时先看的数组行 节约了时间

	int y, x;//y轴, x轴 对应的行列(r, c)
};

//下标对结构
struct SubTwain {
	int r;//行标
	int c;//列标
	const static int ERROR_SUB = -1;
	SubTwain() {
		r = c = -1;
	}
	SubTwain(int r, int c) {
		this->r = r;
		this->c = c;
	}/**/
	SubTwain(size_t r, size_t c) {
		this->r = r;
		this->c = c;
	}
	//用当前下标对作为限制下标 解析编号 构造一个点
	Point2Base const &toPoint2Base(int number) {
		static Point2Base point2Base;
		assert(number >= 0);
		//size_t max = max(r, c);
		point2Base.x = number % c;
		point2Base.y = number / c;
		return point2Base;
	}
	/**/
	/*可考虑将实现提出到一个文件这么一来就没必要Point2Base转换了*/
	SubTwain &operator = (Point2Base const &rhs) {
		r = rhs.y, c = rhs.x;
		return *this;
	}
	SubTwain(Point2Base const &rhs) {
		*this = rhs;
	}

	size_t number(size_t limitR, size_t limitC) {
		//小的那个才能作为编号的基准线?
		//size_t line = min(limitR, limitC);
		return r * limitC + c;
	}

	SubTwain operator- (DirectionVector const &rhs)const {
		//若只写a+b; 对a和b的实际值都无影响
		return{ r - rhs.first, c - rhs.second };
	}
	inline SubTwain &operator-= (DirectionVector const &rhs) {
		r += rhs.first;
		c += rhs.second;
		return *this;
	}
	SubTwain operator+ (DirectionVector const &rhs)const {
		return{ r + rhs.first, c + rhs.second };
	}
	inline SubTwain &operator+= (DirectionVector const &rhs) {
		r += rhs.first;
		c += rhs.second;
		return *this;
	}
	inline bool operator==(const SubTwain &rhs) const {
		return r == rhs.r && c == rhs.c;
	}
	inline bool operator!=(const SubTwain &rhs)const {
		return !((*this) == rhs);
	}
	inline bool operator<(const SubTwain &rhs) const {
		//先比较行又是方便调试
		return *this == rhs ? false :
			r == rhs.r ? c < rhs.c :
			r < rhs.r;
	}
	inline bool operator>(const SubTwain &rhs) const {
		return rhs < *this;
	}
	inline bool operator<=(const SubTwain &rhs) const {
		return !((*this) > rhs);
	}
	inline bool operator>=(const SubTwain &rhs) const {
		return !((*this) < rhs);
	}
};

//二维点结构
struct Point2 : public Point2Base {
	//特地使用构造函数时应该遵循数学习惯
	Point2(int cx, int ry) {
		x = cx, y = ry;
	}
	Point2() {}

	Point2 &operator=(SubTwain const &rhs) {
		y = rhs.r, x = rhs.c;
		return *this;
	}
	Point2(SubTwain const &rhs) {
		//传参时自动转换
		*this = rhs;
	}
	Point2 &operator + (DirectionVector const &rhs) {
		x += rhs.first;
		y += rhs.second;
		return *this;
	}
	bool operator==(const Point2 &rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point2 &rhs)const {
		return !((*this) == rhs);
	}
	bool operator<(const Point2 &rhs) const {
		return *this == rhs ? false :
			x == rhs.x ? y < rhs.y ://x轴优先（列）
			x < rhs.x;
	}
	bool operator>(const Point2 &rhs) const {
		return rhs < *this;
	}
	bool operator<=(const Point2&rhs) const {
		return !((*this) > rhs);
	}
	bool operator>=(const Point2&rhs) const {
		return !((*this) < rhs);
	}
};


/*
list<Edge> Edge 内有一个权值指针 指向邻接矩阵内的权 那么在编辑更改边权值时就可以不用额外操作list了
这么一来list的insert与erase前就不用遍历查找了 可是这背离了节约内存的初衷
*/

//跳表 未实现
//template<class T>
class SkipLists : public JCE::LinkedList<int> {
private:
	JCE::ArrayList<JCE::LinkedList<int>> indexLevel;//索引层list(或许索引层可以不用list实现)
};

//最短路结果集
struct ShortestPathResult {
	JCE::ArrayList<int> dist, path;
	size_t startId = 0, targetId = -1;

	//若已经计算过最短路并且没有到达目的地那么返回true
	bool isCalced() {
		return 0 <= targetId && path.size() > targetId && path[targetId] != -1;
	}
	//重置
	void reset() {
		targetId = -1;
	}
};

