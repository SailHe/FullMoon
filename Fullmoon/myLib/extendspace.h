#pragma once

#ifndef _STANDARD_H_
#define _STANDARD_H_
#include "stdafx.h"
#include "DSAUEdll.h"

/*
std::setfill(char ch)        Fill whitespace with 'ch'
setprecision(int n)     Set floating - point precision to 'n'
std::setw(int w)             Read or write value to 'w' characters
setbase(int b)          Output integers in base 'b'(only 'b' is 8 / 10 / 16 could the function work)
*/

//un used
#define PI_14 3.14159265358979//14λ��
#define MileByF(feet) ((feet)/5280.0)//5280 feet in a mile.
#define FootByI(inches) ((inches)/12.0)//12 inches in a foot.
#define HourByM(minutes) ((minutes)/60.0)//60 minutes in an hour.
#define MinuteByS(seconds) ((seconds)/60.0)//60 seconds in a minute.
#define FurlongByM(meters) ((meters)/201.168)//201.168 meters in a furlong(mile).
#define CALC_ARRAY_LEN(array, length) { length =  sizeof(array) / sizeof(array[0]); }//�����鳤��

const double EPSINON = 1e-15;
const double EPS = 1e-9;
const double EPS_DOUBLE = 1e-6;
const double PI = 3.14159265;//��λ��
const double PIA = acos(-1.0);//���ű�����

typedef int Sub;//�±�����
typedef Sub RowsSub;//�б�����
typedef Sub ColsSub;//�б�����
typedef __int64 I64;


#define ARRAY_TEMPLATE template<class T, class Iterator>

//��׼��չ: ���������׼�����������ģ��(iterateϵ��)ֻ������������ ������ʵ��ʹ��
namespace StandardExtend{
	const int MAX_R = 1024;
	const int MAX_C = 1024;
	//using namespace std;
	//using namespace JCE;

	//��ΪСд�ַ�
	DSAUTILITYEXTENSION_API char toLowerAlph(char c);
	DSAUTILITYEXTENSION_API int toIntNum(char alphOrCharNum);
	//10ToA(Alph)
	DSAUTILITYEXTENSION_API char toUppercaseAscllChar(int num);
	//'A' == 'a' == '0' ... 'J' == 'j' == '9' �Դ�����
	DSAUTILITYEXTENSION_API bool isAa0Equal(char a, char b);

	// =====DATE
	//�����������ʱ����λ��Minite��
	DSAUTILITYEXTENSION_API double calcDifftime(time_t startTime);
	//�����������ʱ�Ӳ��λ��s��
	DSAUTILITYEXTENSION_API double calcDiffClock(clock_t startClock);
	//���ڲ���Function ���Զ����Function��ִ��ʱ��(��λ��s)
	template<class Function>
	void testAndDiffClock(Function fun, JCE::String const &name = "") {
		std::clock_t startClock = clock();
		fun();
		std::cout << name + "ִ��ʱ�䣺" << calcDiffClock(startClock) << std::endl;
	}

	template<class T>
	bool testAndOut(JCE::String const &name, T realValue, T expectValue) {
		assert(realValue == expectValue);
		std::cout << name + " ʵ��ֵ: " << realValue << "; ����ֵ: " << expectValue << std::endl;
		return true;
	}

	//�����ж�
	DSAUTILITYEXTENSION_API int isIntercalary(int year);
	//�������(��, ��)�ǲ���(��)�еĵڼ��� ��: �� ��-��-�� ת��Ϊ ����
	//���� (year, month, day ��ǰ��, ��, ��)
	//��������
	DSAUTILITYEXTENSION_API int calcNumberOfDays(int year, int month, int day);
	//����t1��t2��ķ���ʱ���min, ����h��min������ڲ���ָ����
	//t��ʽ805 (��ʾ08:05)
	DSAUTILITYEXTENSION_API int timeNumDiffrence(int t1, int t2, int *t_h, int *t_min);

	//���stdin������ֱ������end�ַ�
	DSAUTILITYEXTENSION_API void refreshStdin(char end);
	// �����滻Ϊ: destContainer.resize(origin.end() - origin.begin());
	// std::copy(origin.begin(), origin.end(), destContainer.begin());
	template<class T, class Iterator>
	JCE::ArrayList<T> toArrayList(Iterator left, Iterator right) {
		JCE::ArrayList<T> result;
		result.reserve(right - left);
		iterate(left, right, [&](Iterator it) {
			result.push_back(*it);
		});
		return result;
	}

	template<class T>
	//left <= value < right  EG: inRange(minEle, element, maxEle+1); -> element [minEle, maxEle]
	bool inRange(T left, T value, T right){
		return left <= value && value < right;
	}

	template<class Iterator, class Fun>
	void iterate(Iterator left, Iterator right, Fun visit){
		while (left != right){
			visit(left);
			++left;
		}
	}

	template<class Iterator>
	//coutFillChar��ʾ����ַ� �൱��printf��%02d �е�'0' coutWidth��ʾ���е�2
	void outPutIterable(Iterator left, Iterator right, JCE::SizeType coutWidth = 0, char coutFillChar = '\0', char intervalCahr = ' '){
		std::cout << std::setfill(coutFillChar);
		int count = -1;
		iterate(left, right, [&](Iterator left){
			char c = (++count == 0 ? '\0' : intervalCahr);
			std::cout << c << std::setw(coutWidth) << *left;
		});
		puts("");
	}

	template<class Iterator>
	//lineWrap �������л��� С��0 ��ʾ�����������
	void outPutIterable(Iterator left, Iterator right, JCE::SizeType coutWidth, char coutFillChar, JCE::SizeType lineWrap){
		std::cout << std::setfill(coutFillChar);
		if (left == right) {
			return;
		}
		//����left����
		std::cout << std::setiosflags(std::ios::left);
		std::cout << std::setw(coutWidth) << *left;
		++left;
		int c = 1;
		lineWrap = lineWrap < 0 ? right - left : lineWrap;
		iterate(left, right, [&](Iterator left){
			++c;
			if (c % lineWrap == 0){
				//���һ��
				//std::cout << " " << std::setw(coutWidth) << *left << std::endl;
				std::cout << std::setw(coutWidth) << *left << std::endl;
			}
			else if (c % lineWrap == 1){
				//��һ��
				std::cout << std::setw(coutWidth) << *left;
			}
			else{
				//�м��
				//std::cout << " " << std::setw(coutWidth) << *left;
				std::cout << std::setw(coutWidth) << *left;
			}
		});
		//���һ��û�л������ﻻ��
		if (c % lineWrap != 0){
			puts("");
		}
		//ȡ�����뷽ʽ
		std::cout << std::resetiosflags(std::ios::left);
	}

	template<class Iterator, class Fun>
	//lineWrapFun �����Ƿ���  bool(* lineWrapFun)(Iterator left, Iterator right)
	void outPutIterable(Iterator left, Iterator right, JCE::SizeType coutWidth, char coutFillChar, Fun lineWrapFun){
		std::cout << std::setfill(coutFillChar);
		//����left����
		std::cout << std::setiosflags(std::ios::left);
		int c = 0;
		bool isPastWrap = false;
		//lineWrapFun = nullptr == lineWrapFun ? [&](){return right - left; } : lineWrapFun;
		iterate(left, right, [&](Iterator left){
			++c;
			isPastWrap = lineWrapFun(left);
			if (1 == c){//��һ��
				std::cout << std::setw(coutWidth) << *left;
			}
			else{
				//�м�&���
				std::cout << std::setw(coutWidth) << *left;
			}
			if (isPastWrap){//���һ��
				std::cout << std::setw(coutWidth) << std::endl;
				c = 0;
			}
		});
		//���һ��û�л������ﻻ��
		if (!isPastWrap){
			puts("");
		}
		//ȡ�����뷽ʽ
		std::cout << std::resetiosflags(std::ios::left);
	}


	template<class T>
	void outPut2DArrayList(JCE::ArrayList<JCE::ArrayList<T>> const &arrayList2D, char coutFillChar = '0', JCE::SizeType coutWidth = 2){
		JCE::SizeType rows = arrayList2D.size();
		for (JCE::SizeType r = 0; r < rows; ++r){
			outPutIterable(arrayList2D[r].begin(), arrayList2D[r].end(), coutWidth, coutFillChar);
		}
	}

	template<class T>
	void outPut2DArray(T array2D[MAX_R][MAX_C], JCE::SizeType n, JCE::SizeType coutWidth = 2, char coutFillChar = '0'){
		for (JCE::SizeType r = 0; r < n; ++r){
			outPutIterable(array2D[r], array2D[r] + n, coutWidth, coutFillChar);
		}
	}

	template<class T>
	void outPut2DArrayTrangle(T array2D[MAX_R][MAX_C], JCE::SizeType n, JCE::SizeType coutWidth = 2, char coutFillChar = '0'){
		for (JCE::SizeType r = 0; r < n; ++r){
			outPutIterable(array2D[r], array2D[r] + r + 1, coutWidth, coutFillChar);
		}
	}

	template<class Iterator>
	JCE::SizeType rankStatistics(Iterator left, Iterator right){
		JCE::SizeType rank = 1;
		iterate(left + 1, right, [&](Iterator left){
			rank += *left == *(left - 1) ? 0 : 1;
		});
		return rank;
	}

	//ARRAY_TEMPLATE
	template<class T, class Iterator>
	T minValueStatistics(Iterator left, Iterator right, T MAX_VALUE){
		T minValue = MAX_VALUE;
		//iterate(left, right, [&](Iterator left){
		//	minValue = min(minValue, *left);
		//});
		while (left != right){
			minValue = min(minValue, *left);
			++left;
		}
		return minValue;
	}

	template<class T, class Iterator>
	T maxValueStatistics(Iterator left, Iterator right, T MIN_VALUE){
		T maxValue = MIN_VALUE;
		while (left != right){
			maxValue = max(maxValue, *left);
			++left;
		}
		return maxValue;
	}

	template<class T, class Iterator>
	T sumValueStatistics(Iterator left, Iterator right, T ZERO_VALUE){
		T sumValue = ZERO_VALUE;
		iterate(left, right, [&](Iterator left){
			sumValue += *left;
		});
		return sumValue;
	}

	template<class T, class Iterator>
	double avlValueStatistics(Iterator left, Iterator right, T ZERO_VALUE){
		return sumValueStatistics(left, right, ZERO_VALUE) / (double)(right - left);
	}

};

//Ч����չ
namespace Utility {
	//using namespace StandardExtend;
	/************����*****0*********45*******90*********135*******180********225*******270********315****/
	//int dir[8][2] = { { 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 }, { -1, 0 }, { -1, -1 }, { 0, -1 }, { 1, -1 } };
	const int Dir8[8][2] = { { 1, 0 }, { 1, -1 }, { 0, -1 }, { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } };
	const int Dir4[4][2] = { { 0, -1 /*��*/ }, { 0, 1 /*��*/ }, { -1, 0 /*��*/ }, { 1, 0 /*��*/ } };

	//�����˾��ȱȽϷ���˫���ȸ���
	class DSAUTILITYEXTENSION_API Double {
	public:
		Double() {}
		Double(int rhs) {
			value = rhs;
		}
		Double(double rhs) {
			value = rhs;
		}
		inline bool operator==(Double const &rhs) const {
			return fabs(this->value - rhs.value) < EPS;
		}
		inline bool operator!=(Double const &rhs) {
			return !(*this == rhs);
		}
		

		//cout<<
		friend std::ostream &operator<<(std::ostream &os, const Double &rhs) {
			os << rhs.value;
			return os;
		}

		static void setEPS(double EPS) {
			Double::EPS = EPS;
		}
		static double getEPS() {
			return EPS;
		}
	private:
		double value;
		//1e-6
		static double EPS;
	};

	//�Ǿۺ�  ����
	class DSAUTILITYEXTENSION_API PointDouble {
	public:
		double x, y;
		PointDouble() = default;
		PointDouble(double x, double y) {
			this->x = x;
			this->y = y;
		}
		PointDouble(std::istream &in) {
			//scanf("%lf%lf", &(this->x), &(this->y));
			in >> this->x >> this->y;
		}
		PointDouble O() {
			PointDouble ZERO = { 0.0, 0.0 };
			return ZERO;
		}

		//�ܴ���ʼ��'��Ծ��'�յ㷵��true
		static bool canJumpTo(PointDouble const &beganPoint, PointDouble const &endPoint, double jumpPower, double raduis) {
			//��� - �յ� - ��Ծ���� - ��뾶
			return dDistance(beganPoint, endPoint) <= raduis + jumpPower;
		}
		//������������
		static double dDistance(PointDouble const &p1, PointDouble const &p2) {
			return sqrt(dDistance2(p1, p2));
		}
		//�������������ƽ��
		static double dDistance2(PointDouble const &p1, PointDouble const &p2) {
			double dx = p2.x - p1.x;
			double dy = p2.y - p1.y;
			return (dx*dx + dy * dy);
		}
	};

	class DSAUTILITYEXTENSION_API Triangle {
	public:
		Triangle(){}
		Triangle(PointDouble const &p1, PointDouble const &p2, PointDouble const &p3){
			this->p1 = p1;
			this->p2 = p2;
			this->p3 = p3;
		}
		~Triangle(){}

		//������p1, p2, p3��ɵ������ε����
		static double dAreaOfTriangle(PointDouble const &p1, PointDouble const &p2, PointDouble const &p3) {
			double d12 = PointDouble::dDistance(p1, p2)
				, d13 = PointDouble::dDistance(p1, p3)
				, d23 = PointDouble::dDistance(p2, p3);
			//���׹�ʽ
			double dL = (d12 + d13 + d23) / 2.0;
			return sqrt(dL * (dL - d12) * (dL - d13) * (dL - d23));
		}
		//����pλ��p1, p2, p3��ɵ�����������ô����true ���򷵻�false
		static bool isInternalTriangle(PointDouble const &p1, PointDouble const &p2, PointDouble const &p3, PointDouble const &p) {
			double a12p = dAreaOfTriangle(p1, p2, p)
				, a13p = dAreaOfTriangle(p1, p3, p)
				, a23p = dAreaOfTriangle(p2, p3, p)
				, a123 = dAreaOfTriangle(p1, p2, p3);
			//���Ǹ���ָ�������������֮�������������������Χ�����ʱ �����������ڲ� ���ȹ��߿��ܻᱻ�� һ��1E-5����
			return (fabs(a123 - (a12p + a13p + a23p)) < EPS);
		}
		void mainForTriangle() {
			//freopen("input", "r", stdin);
			PointDouble p1, p2, p3, p;
			while (8 == scanf_s("%lf%lf%lf%lf%lf%lf%lf%lf"
				, &p1.x, &p1.y
				, &p2.x, &p2.y
				, &p3.x, &p3.y
				, &p.x, &p.y)) {
				puts(isInternalTriangle(p1, p2, p3, p) ? "Yes" : "No");
			}
		}
		//���Ϲ��ɶ��� ����true ��ϰ��HDU2899
		static bool isPythagoreanTheorem(int a, int b, int c) {
			return a * a == b * b + c * c || b * b == a * a + c * c || c * c == a * a + b * b;
		}
		//�ж��ɴ���Ĳ����Ƿ������������ ������ ����true ���򷵻�false
		static bool isTrangle(double a, double b, double c) {
			//�����α߳�����ʽ
			return a + b > c && a + c > b && b + c > a
				&& fabs(a - b) < c && fabs(a - c) < b && fabs(b - c) < a;
		}
		//�������Ԫ�صĸ���
		template<class T>
		static unsigned equalityElementCount(JCE::ArrayList<T> const &tempeList) {
			unsigned count = 0;
			for (int i = tempeList.size() - 1; i > -1; --i) {
				for (int j = i - 1; j > -1; --j) {
					count += (tempeList[i] == tempeList[j]) ? 1 : 0;
				}
			}
			return count;
		}

	private:
		PointDouble p1, p2, p3;
	};

	// ����
	class DSAUTILITYEXTENSION_API Fraction {
		//����
		double up = 0;
		//��ĸ
		double dw = 0;
	public:
		Fraction(int up, int dw) {
			this->up = up;
			this->dw = dw;
		}

		Fraction(double up, double dw) {
			this->up = up;
			this->dw = dw;
		}

		~Fraction() {
			//printf("������");
		}

		static Fraction ZERO() {
			return Fraction(0, 1);
		}

		static int gcd(int a, int b) {
			return a % b == 0 ? b : gcd(b, a % b);
		}

		double getUp() {
			return up;
		}

		void setUp(double up) {
			this->up = up;
		}

		double getDw() {
			return dw;
		}

		void setDw(double dw) {
			this->dw = dw;
		}

		void plash(Fraction *a) {
			this->up *= a->dw;
			this->up += a->up * this->dw;
			this->dw *= a->dw;
			//Fraction::~Fraction();
			delete a;
		}

		void toSimple() {
			// ������
			int Ga = gcd((int)this->up, (int)this->dw);
			this->up /= Ga;
			this->dw /= Ga;
		}

		double toDouble() {
			if (dw == 0)
				throw dw;
			return this->up / this->dw;
		}
	};


	//ansΪs1+s2(s1��s2�Ĳ���)(ֱ��ans.push_back(temp)����û��Ҫ��ʵ������s1��s2)
	//����s1��s2��������λ��, ansΪs1+s2(ֱ��push_back����) a0,a1,a2...an-1����λ��ָa[(n-1)/2] (��n����)����(n+1)/2����(a0Ϊ��һ����)
	template<class T>
	T median(JCE::ArrayList<T> ans) {
		std::sort(ans.begin(), ans.end());
		if (ans.size() % 2 == 0)
			return ans[ans.size() / 2 - 1];
		else
			return ans[ans.size() / 2];
	}
	//����(����ʵ�� �ױ�)
	template<class T>
	void swapUnstable(T &a, T &b){
		a += b;
		b = a - b;
		a = a - b;
	}
	//��תreverseRows�еĶ�ά����a ÿ��λ������[��leftSub,��rightSub)��Ԫ��
	template<class T>
	void reverse(JCE::ArrayList<JCE::ArrayList<T>> &a, Sub leftSub, Sub rightSub, JCE::SizeType reverseRows = 1) {
		for (int r = 0; r < reverseRows; r++) {
			for (int c = leftSub, i = 0; i < (rightSub - leftSub) / 2; i++, c++) {
				int temp = a[r][c];
				a[r][c] = a[r][rightSub - i - 1];
				a[r][rightSub - i - 1] = temp;
			}
		}
	}
	/*��R�� C�еĶ�ά����������kλ*/
	template<class T>
	void leftMoveK(JCE::ArrayList<JCE::ArrayList<T>> &a, Sub R, Sub C, JCE::SizeType k) {
		/*
		8 3
		1 2 3 4 5 6 7 8
		8 7 6 5 4/ 3 2 1
		4 5 6 7 8/ 1 2 3
		*/
		k %= C;
		reverse(a, 0, C, R);/*��ת����Ԫ��*/
		reverse(a, 0, C - k, R);/*��תǰC-k��Ԫ��*/
		reverse(a, C - k, C, R);/*��ת��k��Ԫ��*/
	}
	/*����kλ<==>����C-kλ leftMoveK(a, R, C, C-k); <==> rightMoveK(a, R, C, k);*/
	template<class T>
	void rightMoveK(JCE::ArrayList<JCE::ArrayList<T>> &a, int R, int C, int k) {
		/*
		8 3
		1 2 3 4 5 6 7 8
		8 7 6/ 5 4 3 2 1
		6 7 8/ 1 2 3 4 5
		*/
		k %= C;
		reverse(a, 0, C, R);
		reverse(a, 0, k, R);/*��תǰk��Ԫ��*/
		reverse(a, k, C, R);/*��ת��C-k��Ԫ��*/
	}
	// �����һ�����������������(longest continuous increment sub sequence)
	// ���س��� ����:���ڴ�������������е�����[leftResultSub, rightResultSub)
	template<class T>
	JCE::SizeType calcFirstLonConIncSubSeq(JCE::ArrayList<T> const &sequence, int &leftResultSub, int &rightResultSub) {
		int maxLen = 0, sequenceLen = sequence.size();
		//rightResultSub=-1: ʹ��������ǳ���ʱΪ0
		leftResultSub = 0, rightResultSub = -1;
		for (int l = 0, r = 0, len = 0;; ++r) {
			len = r - l + 1;
			maxLen = rightResultSub - leftResultSub + 1;
			//������������� (��һ��������:��ȡ��)
			if (len > maxLen) {
				leftResultSub = l, rightResultSub = r;
			}
			else {
				//do nothing
			}
			if (r >= sequenceLen - 1)
				break;
			//r < sequenceLen - 1 ��֤sequenceLen==1ʱ��ȷ
			if (sequence[r] < sequence[r + 1]) {
				//do nothing
			}
			//��ǰ���в��ǵ�������
			else {
				////���þֲ���������
				l = r + 1;
			}
		}
		++rightResultSub;
		return maxLen;
	}

	// uNum->resultNum �޷���תΪ�з���
	// (PS ʹ��assert��Ϊ�жϵ� ���assert��Ч��ô�ȼ���ǿ��ת��; �޷���ֵ)
	template<class UnT, class T>
	void toSignedNum(UnT uNum, T &resultNum) {
		//ȷ�����޷���
		assert(uNum - uNum - 1 > 0);
		resultNum = (T)uNum;
		//ȷ�����з���
		assert(resultNum - resultNum - 1 < 0);
		//ȷ��ת��������ȷ
		assert(resultNum > 0);
	}

	// ===== �� �� less more greater

	//qsort(a, ���򳤶�, һ��Ԫ�صĴ�С, intCmp);
	template<class Numeric>
	int lessQsortCmp(const void *lhs, const void *rhs) {
		return *((Numeric*)lhs) - *((Numeric*)rhs);
	}
	template<class Numeric>
	int moreQsortCmp(const void *lhs, const void *rhs) {
		return -lessQsortCmp<Numeric>(lhs, rhs);
	}
	template<class CmpAble>
	int structQsortCmp(const void *lhs, const void *rhs) {
		//������ֵ ��С��������
		int cp = ((CmpAble*)lhs)->value - ((CmpAble*)rhs)->value;
		if (cp == 0) {
			//���μ�ֵ �Ӵ�С����
			cp = ((CmpAble*)rhs)->value2 - ((CmpAble*)lhs)->value2;
		}
		return cp;
	}
	// sort(a, a+n, boolCmp)
	template<class CmpAble>
	bool lessSortCmp(CmpAble lhs, CmpAble rhs) {
		return lhs.value < rhs.value;
	}
	//ð������ ������a�е�n��Ԫ�ؽ��еĵ�K������
	template<class CmpAble>
	void BubbleSort(CmpAble a[], int n, int K, int cmp(const void *lhs, const void *rhs)) {
		CmpAble temp = 0;
		//K�������ʱ���ܼ�һ
		for (int i = 0; i < K; ++i) {
			//�˴������һ
			for (int j = 0; j < n - i - 1; ++j) {
				if (cmp(&a[j + 1], &a[j]) < 0) {
					temp = a[j + 1];
					a[j + 1] = a[j];
					a[j] = temp;
				}
			}
		}
	}
	//ѡ������
	template<class CmpAble>
	void SlectSort(CmpAble *left, CmpAble *right, int(*cmp)(void const *lhs, void const *rhs)) {
		// something bug here if left is a LinkedList::Iterator
		for (; left < right - 1; ++left) {
			for (CmpAble *j = left + 1; j < right; ++j) {
				if (cmp(j, left) < 0) {
					std::swap(*left, *j);
				}
			}
		}
	}
	//ϣ������
	template<class T>
	void shellSort(T a[], int n) {
		int si;
		int sedgewick[] = { 929, 505, 109, 41, 19, 5, 1, 0 };
		//�ҵ���һ��С������Ԫ�ظ���������
		for (si = 0; sedgewick[si] >= n; ++si);

		for (int d = sedgewick[si]; d > 0; d = sedgewick[++si]) {
			//��������
			for (int p = d, i; p < n; ++p) {
				T tmp = a[p];
				for (i = p; i >= d && a[i - d] > tmp; i -= d) {
					//i����[d, p], ��ǰ��"����"a[p]��ֵ����
					a[i] = a[i - d];
				}
				//a[i - d] <= tmpʱ ��Ϊǰ�涼�Ǿֲ������ [0, i-d]�����ڵ�ֵ <= a[p]һ������, (i, p]�ڵ�ֵ>a[p]һ������
				//����i����a[p]��λ�� ps��������Ҳ�� ֻ�ǻ�����������õĽ���
				a[i] = tmp;
			}
		}
	}
}

//��ѧ��չ
namespace MathExtend {
	//using namespace Utility;
	
	//����˷� productMatrix = productMatrix*originMatrix(����������ͬ)
	template<class T>
	void matrixMultiply(const Sub maxRowCol, JCE::ArrayList<JCE::ArrayList<T>> &originMatrix, JCE::ArrayList<JCE::ArrayList<T>> &productMatrix) {
		int r, c, i, tmp;
		JCE::ArrayList<JCE::ArrayList<T>> tempMatrix;
		//memcpy(tempMatrix, productMatrix, maxRowCol * 25 * sizeof(int));
		tempMatrix = originMatrix;
		for (r = 0; r < maxRowCol; r++) {
			for (c = 0; c < maxRowCol; c++) {
				//���б�����Ӧ��� �ۼӸ�tmp �ٸ��� productMatrix
				for (tmp = i = 0; i < maxRowCol; i++)
					tmp += originMatrix[r][i] * tempMatrix[i][c];
				productMatrix[r][c] = tmp;
			}
		}
	}

	// =======�� ��

	// ���ֲ���origin[n]��key ����key�ĵ�����, ��û�з���origin+size������ ע��:����ж����ͬԪ������������ص�Ԫ�ؿ�������������һ��
	// origin��������������������֧������[]����(����ʽ��������������) ����������[origin, origin+size)��Χ��Ԫ�ر���������
	// std�����ƶ�������ʵ��(std::binary_search) ��ֻ�� ���ȼ��� value ��Ԫ���Ƿ�����ڷ�Χ [first, last) ��
	// ���ƵĻ���:	A. count,find
	//				B.binary_search, lower_bound, upper_bound, equal_range
	template<class Iterator, class ValueType>
	Iterator dichotomy(Iterator origin, JCE::SizeType size, ValueType key) {
		int left = 0, right = size, mid = 0;
		while (left <= right) {
			mid = (right + left) / 2;
			if (origin[mid] < key) {
				left = mid + 1;
			}//��1֮ǰ��ȷ��mid���Ϸ�, ���ǿ��԰��������õ���һ�εļ�����, ������1��һ�ο��ܻ����¼���mid
			else if (origin[mid] > key) {
				//ͬ��
				right = mid - 1;
			}
			else {
				return origin + mid;
			}
		}
		return origin + size;
	}
	// ���ַ� ��һԪ����f(x)�� ����[left, right]�ڵĵ���С��x (eps: ����, �����ֵ���ⲿʵ��) TY(*fun)(TX x)
	template<class TY, class TX, class Fun>
	TX trichotomy(TX left, TX right, Fun fun, TX eps) {
		TX Ll, Rr;
		while (right - left > eps) {
			//����
			Ll = (2 * left + right) / 3;
			Rr = (2 * right + left) / 3;
			if (fun(Ll) > fun(Rr)) {
				left = Ll;
			}
			else {
				right = Rr;
			}
		}
		//����left ,right��һ������(PS: ��Ӧ����f(x)��ֵ ��Ϊfun���ܲ����û���Ҫ��fun �μ�����ע��)
		return left;
	}

	//���ش��ű� 64 bit ������λ ����� D[23] IntegerNum ��Ҫ֧�� + * ����
	//��С�ļ����������ǣ�D1 = 0��D2 = 1��D3=2��D4 = 9��D5 = 44��D6 = 265��D7 = 1854
	template<class IntegerNum>
	void buildIllArrangeList(JCE::ArrayList<IntegerNum> &D, JCE::SizeType MAX_SIZE = 23) {
		D.assign(MAX_SIZE, 0);
		D[0] = 0, D[1] = 0, D[2] = 1;
		for (JCE::SizeType n = 3; n < MAX_SIZE; ++n)
			D[n] = (D[n - 1] + D[n - 2]) * (n - 1);
	}
	//������bigInteger�ܱ���������true
	DSAUTILITYEXTENSION_API bool isDivisible(char *bigInteger, int MOD);
	//ɾ��resStr�е������Ӵ�delSubStr
	DSAUTILITYEXTENSION_API void eraseAllSubStr(char *resStr, char *delSubStr);
	//������
	DSAUTILITYEXTENSION_API int reverseOrderNumber(int number);
	//����radix��������number��digit���ֵĴ���
	DSAUTILITYEXTENSION_API int calcDigitCountInNumber(const int number, const int digit, int radix = 10);
	//����radix����������λ��
	DSAUTILITYEXTENSION_API int calcDigitTop(int number, int radix = 10);


	// ===== �� ѧ

	DSAUTILITYEXTENSION_API I64 quickFact(I64 a, I64 b, I64 mod);
	//�����ݻ���quickFact�� Ч���Ǽ��װ��1/5
	DSAUTILITYEXTENSION_API I64 quickPow_OLD(I64 C, I64 R, I64 k);
	//�����ݼ��װ� m^n % k ������ڼ���powЧ������ͨ���1/20 (��ͨ��Ч�ʸ�)
	DSAUTILITYEXTENSION_API I64 quickPow(I64 m, I64 n, I64 k);
	//��������ͨ�� m^n; ��std::powЧ�ʸ��иߵ����� std::powֵ�����
	DSAUTILITYEXTENSION_API I64 quickPow(I64 m, I64 n);
	//����pow(iNum, 2)�����mλ(ʵ����ò��ֻ�������1bit �������ﱻ�������޸���) ע�ⷵ��ֵ���ܰ�����λ��ֻ�����һλ��Ч
	DSAUTILITYEXTENSION_API long long powLastBit(int base, int m);
	//�����������
	DSAUTILITYEXTENSION_API double logR(double value, double base = 5);
	//�� ��
	DSAUTILITYEXTENSION_API double fact(int n);
	//int �޶��ڽ׳˱� (int 13!���� double��long long int 18!��)
	DSAUTILITYEXTENSION_API int* generateFactList(int maxN = 13);
	/*
	n < m && m != 0
		 fact(n)
	 ________________
	fact(n - m) * fact(m)
	*/
	//����� (ȡ��Ԫ�ز��ظ�����)
	//��n����ͬԪ���У���ȡm(m��n)��Ԫ�ز���һ�飬������n����ͬԪ����ȡ��m��Ԫ�ص�һ����ϣ�
	//ab == ba
	DSAUTILITYEXTENSION_API int C(int n, int m);
	//������ (ȡ��Ԫ�ز��ظ�����)
	//��n����ͬԪ���У���ȡm(m��n)��Ԫ�ذ���һ����˳���ų�һ�У�������n����ͬԪ����ȡ��m��Ԫ�ص�һ������
	//ab != ba
	DSAUTILITYEXTENSION_API int A(int n, int m);
	//����(����)�ж�
	DSAUTILITYEXTENSION_API int isPrime_OLD(int num);
	//����(����)�ж� sqrt��ƽ���Ż���(δʵ��)
	DSAUTILITYEXTENSION_API bool isPrime(int num);
	//�����Ӻ�(���������Լ����
	DSAUTILITYEXTENSION_API int factorSum(int x);
	//�����Ӻͱ�(Լ�� ����) (10^7һǧ��2.51s)(400w 10^6 900ms)
	DSAUTILITYEXTENSION_API void buildSieveFactorSumS(const int maxn, int a[]);
	//����(����) ɸѡ�� ������ɫ��(Sieve Eratosthenes)(0 1==-1, a[i]==0��ʾ����) maxN[2, 1e9) PS:maxN = 1e7ʱ����1s�� 1e9�ƺ��޷������ڴ�
	DSAUTILITYEXTENSION_API int* generateSievePrimeS(const int maxN = 2);
	//������Ŀ
	DSAUTILITYEXTENSION_API int factorCount(int x);
	//�������� (eps: ����)
	DSAUTILITYEXTENSION_API double sqrtImpl(double x, double eps = 1e-9);
	//�󷽲�
	DSAUTILITYEXTENSION_API double variance(float x[], int n);
	// ��������
	DSAUTILITYEXTENSION_API I64* catalanTable();
	//����lineNum����ʱ��󽻵���Ŀ
	DSAUTILITYEXTENSION_API int countMaxIntersection(int lineNum);


	//շת�����Լ��, ����(����)�ж�
	DSAUTILITYEXTENSION_API int gcd(int a, int b);
	/*
	��չ��ŷ������㷨��շת�������Extended Euclidean algorithm:
	����:
		��֪����a��b, �����a��b�����Լ����ͬʱ, ���ҵ����㱴���ʽax + by = gcd(a, b)������x��y;��Ҳ��������, ����һ���ܿ����Ǹ�����
	��a�Ǹ��������԰�����ת����|a|(-x) + by = gcd(|a|, b)��Ȼ���� x' = (-x)
		���ö��������Է��̵Ľ�*//*
	����ֵ      :��� Լ��,����,���� gcd(greatest common divisor) �� hcf(highest common factor)
	��֪����a, b:����Ǹ�����(���Ǹ�������Ҫ�����㷨)
	���ز���x, y:������ �� ģ��Ԫ...
	*//*
	�����ʽ(������)����:���������Լ���ı����������������������������ʾ
	����δ֪��x��y�ķ��� ax + by = m��������<==>m��d�ı�����(d����������a��b�����Լ��)
	�����ʽ�н�ʱ��Ȼ�������������⣬ÿ���x y����Ϊ��������������չŷ������㷨��á�
	Ӧ��:
	���ڲ�����������ax+by=c����c �� gcd(a,b)����������÷��̴���������
	���򲻴��� x , y �����⡣
	�����Ƿ����һ����k��ʹ��m��kȦ���n��һ��������⣺ k*m - n = 1;�Ƿ��н�
	1 % gcd(m,n) == 0
	*/
	DSAUTILITYEXTENSION_API int gcdEx_OLD(int a, int b, int &x, int &y);
	//��չŷ������㷨������ʽ��
	template<class IntegerNum>
	IntegerNum gcdEx(IntegerNum a, IntegerNum b, IntegerNum &x, IntegerNum &y) {
		if (b == 0) {
			x = 1; y = 0;
			return a;
		}
		IntegerNum g = gcdEx(b, a%b, y, x);
		y -= a / b * x;
		return g;
	}
	/*
	����һ(���涨��)��ax + by = gcd(a, b)����������
	���������gcd(a, b) = 1���򷽳�ax �� c (mod b)��[0, b-1]����Ψһ��.
	֤��:���ڷ��� ax + by = c
	<==> ax �� c (mod b)			==> �����̴����ؽ�x,��ôx + k*b���Ƿ��̵Ľ�
	����������gcd(a, b) = g���򷽳�ax �� c (mod b)��[0, b/g - 1]����Ψһ��.
	֤��:���ڷ��� a/g*x+b/g*y = c/g	(��x,y��������; g = gcd(a, b) ==> ��������Ǹ����� ==> �ұ�Ҳ�������� ==> ����x,y����������)
	<==> a/g*x �� c/g (mod b/g)	==> �����̴����ؽ�x,��ôx + k*(b/g)���Ƿ��̵Ľ�
	*/ //PKU1061-���ܵ�Լ��
	//�����Է���ax+by = c ����С�Ǹ�������x(ֻ�ܱ�֤x��������) �������ⲻ���ڷ���false
	template<class IntegerNum>
	bool linearEquationCondition1(IntegerNum a, IntegerNum &x, IntegerNum b, IntegerNum &y, IntegerNum c) {
		IntegerNum x0, y0,
			g = gcdEx(a, b, x0, y0);
		if (c%g != 0)
			return false;
		IntegerNum rx = b / g;
		x = x0 * c / g;
		x = (x%rx + rx) % rx;
		//while (x < 0 && rx > 0) x += rx; x %= rx;
		y = (c - a * x) / b;
		return true;
	}
	/*
	�������ڷ���ax+by = c
	ͨ��:	x = x0 + b*t
	y = y0 - a*t
	(��:x0, y0Ϊ���̵�һ���; ��:gcd(a, b) = 1; ��:tΪ��һ����.)

	��:	g = gcdEx(a, b, x0, y0);
	x0 = x0*c / g;
	y0 = y0*c / g;
	��:	x = x0 + b / g*t
	y = y0 - a / g*t
	����		|x|+|y| = |x0+b/g*t| + |y0-a/g*t|
	��֪		|x0+b/g*t|����������|y0-a/g*t|�����ݼ�.
	���涨	a>b
	��ô		ԭ����Ϊ�ȼ������İ�ͼ�� (��t����y0 - a/g*t == 0 ʱȡ��Сֵ)
	��		|x|+|y| ��t = y0*g/a ����(��)ȡ��Сֵ
	*/ //PKU2142-HDU1356-The Balance
	//�����Է���ax+by = c ʹ��|x|+|y|��С��һ���x,y
	DSAUTILITYEXTENSION_API void linearEquationCondition2(int a, int &x, int b, int &y, int c);
	/*
	modulo inverse
	����:����a��ģm��Ԫt, �����ڴ�ӡ���󲢽�������;
	a*modInv(a, m) % m = 1
	��Ϊat%m=1��Ҫ������2�Ļ���2*at��Ҫ������Ϊ3�Ļ���3*at
	����

	ͬ��� ��: ��һ������������������ ����ͬ��������ƶ�����[ͬ��]��
	ͬ���ϵ(��ɢ��ѧ):aRb <==> a��b��m��ʱ������ͬ������r, ��ʱ, ��m��ģ��, ��a��b(mod m); (modulo); ����:a��bģmͬ��
	ͬ���ϵ��һ��[�ȼ۹�ϵ]:�Է�,�Գ�,����

	ͬ����(congruence class��residue class):�ɶ���ģnͬ�������������ɵ�һ������; ����������֪��ģn����Ҳ�ɱ��Ϊ[a]��
	ͬ����Ĵ�����(representative):ͬ�����е����������ͬ�������һԪ��

	ģ��Ԫ: t �� a^(-1)(mod m) �� at �� 1(mod m); ��at��1ģmͬ��, ��t��a�Ķ�ͬ��m��[ģ��Ԫ], Ҳ����ģm��[���۵���];
	��ax + my = 1 <==> (1 �� 1) ax+my �� 1 �� ax(mod m) ==> ax �� 1; ���ݶ���, x����a����ģm��һ��ģ��Ԫ��
	*/
	DSAUTILITYEXTENSION_API int modInv(int a, int m);
	/*
	@see https://zh.wikipedia.org/wiki/%E4%B8%AD%E5%9B%BD%E5%89%A9%E4%BD%99%E5%AE%9A%E7%90%86
	�й�ʣ�ඨ��: ˵����һԪ����ͬ�෽�����н��׼���Լ���ⷽ��
	ͨ���Լ��㷨: ansNum = sum(remainderS[i]*M[i]*t[i]) + k*product(modS[i])
	(����: M[i]��ģ��������˵Ľ�� t[i]�������۵���(ģ��Ԫ), ��ģmP�������·�����ֻ��ansNumһ����)
	��������:
	���Ӷ���, ���ŵ��, �ﲻ֪��(xx��֮x����ģ��), ��������(������ģ�� ��һ�����Ե�������ͬ�������)
	*/
	// ��֪����modS(���m):									ģ����(Ҫ��modS��Ԫ����������, �޲������)
	// ��֪����remainderS(���a): ������(ǿ����)				remainderS[i] = ansNum%modS[i]
	// ����ֵ:		ͬʱ����%ģ���������Ԫ�ص�ֵ�ֱ�����������ӦԪ�� ����С������ansNum
	// ansNumģmodS[i]ͬ���������(������)	ansNum �� remainderS[i](mod modS[i])
	DSAUTILITYEXTENSION_API int chineseReminder(int *modS, int *remainderS, int sizeNum, int mP = 0);

	// ŷ������:���������������[1,n]����n ������ �ĸ��� �ĺ���
	DSAUTILITYEXTENSION_API int  Eular(int n);
	//��ʲ���ģ�ȡ���һ������ʤ
	DSAUTILITYEXTENSION_API int BaShen(int n, int min, int max);

	// ===== Backpack(����) DP(��̬�滮) ͳһĿ��: ��⽫��Щ��Ʒװ�뱳����ʹ��ֵ�ܺ����
	// ����������: capacity (���౸ѡ�ʻ� volume: ���, account: �˻�, amount: ����) <==> backPack.size+1
	// ����Ʒ��ֵ: value    (���౸ѡ�ʻ� weight: Ȩ��, ����, ����) PS ��ѡ�ʿ��ܺ�������, �������
	
	// ����һ��[��Ҽ����]�е���Ʒ���� => ��N����Ʒ��һ������Ϊcapacity�ı���backPack��ÿ����Ʒֻ��ȡ1�λ�ȡ(����Ϊ1)
	// �����i����Ʒ�ķѵķ����� C[i],�õ��ļ�ֵ�� W[i].
	template<class Numeric>
	void ZeroOneBackpackProcess(Numeric backPack[], Numeric capacity, Numeric currentCost, Numeric currentValue) {
		for (Numeric v = capacity; v >= currentCost; --v) {
			backPack[v] = max(backPack[v], backPack[v - currentCost] + currentValue);
		}
	}
	// (����㷨������) 01���������Ƿ� capacity�Ǳ���������  ����currentValue = 0; backPack[0] = 1;
	template<class Number>
	void ZOBackpackProcess(Number backPack[], Number capacity, Number currentCost, Number currentValue) {
		for (Number v = currentCost; v <= capacity; ++v) {
			backPack[v] += backPack[v - currentCost] + currentValue;
		}
	}
	// ����һ��[��ȫ����]�е���Ʒ���� ==> ��N����Ʒ��һ������Ϊcapacity�ı�����ÿ����Ʒ�������޼����á�
	template<class Number>
	void CompletePackProcess(Number backPack[], Number capacity, Number currentCost, Number currentValue) {
		for (Number v = currentCost; v <= capacity; ++v) {
			backPack[v] = max(backPack[v], backPack[v - currentCost] + currentValue);
		}
	}
	// ����һ��[���ر���]�е���Ʒ���� ==> O(logM) ��N����Ʒ��һ������Ϊcapacity�ı�����
	// ��i����Ʒ�����currentNum=M[i]�����ã�ÿ���ķѵĿռ���currentCost=C[i]����ֵ�� currentValue=W[i]��
	template<class Number>
	void MultiplyPackProcess(Number backPack[], Number capacity, Number currentCost, Number currentValue, Number currentNum) {
		if (currentCost*currentNum >= capacity) {
			CompletePackProcess(backPack, capacity, currentCost, currentValue);
		}
		else {
			Number k = 1;
			while (k < currentNum) {
				ZeroOneBackpackProcess(backPack, capacity, k*currentCost, k*currentValue);
				currentNum -= k;
				k <<= 1;
			}
			ZeroOneBackpackProcess(backPack, capacity, currentNum*currentCost, currentNum*currentValue);
		}
	}

	// =======���޵�(Confined) ===== �ݹ� ===== ̰�� ===== ����

	// ������͵�ʵ�����
	DSAUTILITYEXTENSION_API void MiLIGen(double u, double v1);
	//���digit��[leftSub, rightSub)��ȫ����  ���ֵ���
	template<class T, class Fun>
	void penetration(JCE::ArrayList<T> container, Sub leftSub, Sub rightSub, Fun visit) {
		if (leftSub == rightSub) {
			visit(container);
		}
		else {
			for (int i = leftSub; i < rightSub; ++i) {
				std::swap(container[leftSub], container[i]);
				penetration(container, leftSub + 1, rightSub, visit);
				std::swap(container[leftSub], container[i]);
			}
		}
	}
	//�������1-maxDigit(1, 9)��ȫ����  �ֵ���
	DSAUTILITYEXTENSION_API void pentration(int maxDigit);
	//�������1-maxDigit(1, 9)��ȫ����  ���ֵ���
	DSAUTILITYEXTENSION_API void pentration();
	//��ŵ���ݹ�ⷨ
	DSAUTILITYEXTENSION_API void hannoTowerMove(int n, char a, char b, char c);
	//��10���Ƶ�numberת��Ϊradix���Ƶ��ַ��� (�ݹ�ʵ��)
	DSAUTILITYEXTENSION_API std::string decToBin(int number, std::string &result, int radix = 2);
	// ����: �����Ӵ�s1��s2ƥ����ַ��� �Ƚϳ���len)
	DSAUTILITYEXTENSION_API int countMatchingChar(char *s1, char *s2, int len);
	// �ж�һ���ִ��Ƿ����(��ջʵ��)
	DSAUTILITYEXTENSION_API bool isPlalindrome(char const*str, int len);
	// �жϸ����ִ��Ƿ�ӵ��ƥ�������
	DSAUTILITYEXTENSION_API bool isMatchingParenthesis(char const*str, int len);
	// �жϸ�����ջ�����Ƿ�Ϸ�
	DSAUTILITYEXTENSION_API bool isValidityOfStack(char const*str, int len, int cap);
	//����n�� ���������
	DSAUTILITYEXTENSION_API void buildPtriangleTable(int tableBuffer[][StandardExtend::MAX_C], int n);
}

#endif
