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
//�����ӷ�����ʱ ����first �� second��ϰ�ߵı�ʾxy rc��������ͬ
typedef JCE::pair<int, int> DirectionVector;
//����extern ������ض�������
const DirectionVector DIR84[8] = {
	/*r��������->��4123���� ���ڵ��� ˳�����������*/
	{ 1, 0 }/*��*/,
	{ 0, 1 }/*��*/,
	{ -1, 0 }/*��*/,
	{ 0, -1 }/*��*/,

	{ 1, 1 }/*����*/,
	{ 1, -1 }/*����*/,
	{ -1, -1 }/*����*/,
	{ -1, 1 }/*����*/,
};


struct Point2Base {
	//���ۣ���û���漰��ͼʱ�����SubTwain �ο�ͼ�д�Ķ�������Ͳ��ǵ�������

	//������ʱϰ�����ȿ�����(x��), ������ʱʵ�ʲ���ʱ�ȿ�����(ԭ�������Ͻǲ����ص�!!��Ҫ�������ܶ�ʱ��Բ���!!) 
	//�ȶ���y: 1.������� 2.ʹ�ó�ʼ���б�ʱֱ�Ӵ���{ r, c }�൱��Subת��ΪPoint2
	//tips����֪���ͽṹ����¶����磩��û�ж����κι��캯�� �ſ���ʹ�ó�ʼ���б�{} �����������⹹�캯�����ʼ���б���빹�캯���Ĳ�����Ӧ
	//����ʱʹ��{x, y} ��Point2(x, y)�ȼ� ����ʱPoint2 y��ǰ��Ӧ�鿴ʱ�ȿ��������� ��Լ��ʱ��

	int y, x;//y��, x�� ��Ӧ������(r, c)
};

//�±�Խṹ
struct SubTwain {
	int r;//�б�
	int c;//�б�
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
	//�õ�ǰ�±����Ϊ�����±� ������� ����һ����
	Point2Base const &toPoint2Base(int number) {
		static Point2Base point2Base;
		assert(number >= 0);
		//size_t max = max(r, c);
		point2Base.x = number % c;
		point2Base.y = number / c;
		return point2Base;
	}
	/**/
	/*�ɿ��ǽ�ʵ�������һ���ļ���ôһ����û��ҪPoint2Baseת����*/
	SubTwain &operator = (Point2Base const &rhs) {
		r = rhs.y, c = rhs.x;
		return *this;
	}
	SubTwain(Point2Base const &rhs) {
		*this = rhs;
	}

	size_t number(size_t limitR, size_t limitC) {
		//С���Ǹ�������Ϊ��ŵĻ�׼��?
		//size_t line = min(limitR, limitC);
		return r * limitC + c;
	}

	SubTwain operator- (DirectionVector const &rhs)const {
		//��ֻдa+b; ��a��b��ʵ��ֵ����Ӱ��
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
		//�ȱȽ������Ƿ������
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

//��ά��ṹ
struct Point2 : public Point2Base {
	//�ص�ʹ�ù��캯��ʱӦ����ѭ��ѧϰ��
	Point2(int cx, int ry) {
		x = cx, y = ry;
	}
	Point2() {}

	Point2 &operator=(SubTwain const &rhs) {
		y = rhs.r, x = rhs.c;
		return *this;
	}
	Point2(SubTwain const &rhs) {
		//����ʱ�Զ�ת��
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
			x == rhs.x ? y < rhs.y ://x�����ȣ��У�
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
list<Edge> Edge ����һ��Ȩֵָ�� ָ���ڽӾ����ڵ�Ȩ ��ô�ڱ༭���ı�Ȩֵʱ�Ϳ��Բ��ö������list��
��ôһ��list��insert��eraseǰ�Ͳ��ñ��������� �����ⱳ���˽�Լ�ڴ�ĳ���
*/

//���� δʵ��
//template<class T>
class SkipLists : public JCE::LinkedList<int> {
private:
	JCE::ArrayList<JCE::LinkedList<int>> indexLevel;//������list(������������Բ���listʵ��)
};

//���·�����
struct ShortestPathResult {
	JCE::ArrayList<int> dist, path;
	size_t startId = 0, targetId = -1;

	//���Ѿ���������·����û�е���Ŀ�ĵ���ô����true
	bool isCalced() {
		return 0 <= targetId && path.size() > targetId && path[targetId] != -1;
	}
	//����
	void reset() {
		targetId = -1;
	}
};

