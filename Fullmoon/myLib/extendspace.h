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
#define PI_14 3.14159265358979//14位π
#define MileByF(feet) ((feet)/5280.0)//5280 feet in a mile.
#define FootByI(inches) ((inches)/12.0)//12 inches in a foot.
#define HourByM(minutes) ((minutes)/60.0)//60 minutes in an hour.
#define MinuteByS(seconds) ((seconds)/60.0)//60 seconds in a minute.
#define FurlongByM(meters) ((meters)/201.168)//201.168 meters in a furlong(mile).
#define CALC_ARRAY_LEN(array, length) { length =  sizeof(array) / sizeof(array[0]); }//求数组长度

const double EPSINON = 1e-15;
const double EPS = 1e-9;
const double EPS_DOUBLE = 1e-6;
const double PI = 3.14159265;//八位π
const double PIA = acos(-1.0);//蓝桥杯可用

typedef int Sub;//下标类型
typedef Sub RowsSub;//行标类型
typedef Sub ColsSub;//列标类型
typedef __int64 I64;


#define ARRAY_TEMPLATE template<class T, class Iterator>

//标准扩展: 里面的许多标准输入输出方法模板(iterate系列)只建议用作测试 不建议实际使用
namespace StandardExtend {
	const int MAX_R = 1024;
	const int MAX_C = 1024;
	//using namespace std;
	//using namespace JCE;

	//变为小写字符
	DSAUTILITYEXTENSION_API char toLowerAlph(char c);
	DSAUTILITYEXTENSION_API int toIntNum(char alphOrCharNum);
	//10ToA(Alph)
	DSAUTILITYEXTENSION_API char toUppercaseAscllChar(int num);
	//'A' == 'a' == '0' ... 'J' == 'j' == '9' 以此类推
	DSAUTILITYEXTENSION_API bool isAa0Equal(char a, char b);

	// @see https://my.oschina.net/wangsifangyuan/blog/1499715
	// 输出格式化信息到输出框
	DSAUTILITYEXTENSION_API void outputDebugFormat(const char *strOutputString, ...);

	// =====DATE
	//计算与参数的时间差（单位：Minite）
	DSAUTILITYEXTENSION_API double calcDifftime(time_t startTime);
	//计算与参数的时钟差（单位：s）
	DSAUTILITYEXTENSION_API double calcDiffClock(clock_t startClock);
	//用于测试Function 会自动输出Function的执行时钟(单位：s)
	template<class Function>
	void testAndDiffClock(Function fun, JCE::String const &name = "") {
		std::clock_t startClock = clock();
		fun();
		std::cout << name + "执行时间：" << calcDiffClock(startClock) << std::endl;
	}

	template<class T>
	bool testAndOut(JCE::String const &name, T realValue, T expectValue) {
		assert(realValue == expectValue);
		std::cout << name + " 实际值: " << realValue << "; 期望值: " << expectValue << std::endl;
		return true;
	}

	//闰年判断
	DSAUTILITYEXTENSION_API int isIntercalary(int year);
	//计算参数(月, 日)是参数(年)中的第几天 即: 将 年-月-日 转换为 天数
	//传入 (year, month, day 当前年, 月, 日)
	//返回天数
	DSAUTILITYEXTENSION_API int calcNumberOfDays(int year, int month, int day);
	//返回t1与t2间的分数时间差min, 并将h，min结果存于参数指针中
	//t格式805 (表示08:05)
	DSAUTILITYEXTENSION_API int timeNumDiffrence(int t1, int t2, int *t_h, int *t_min);

	//清空stdin输入流直至遇到end字符
	DSAUTILITYEXTENSION_API void refreshStdin(char end);
	// 可以替换为: destContainer.resize(origin.end() - origin.begin());
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
	bool inRange(T left, T value, T right) {
		return left <= value && value < right;
	}

	template<class Iterator, class Fun>
	void iterate(Iterator left, Iterator right, Fun visit) {
		while (left != right) {
			visit(left);
			++left;
		}
	}

	template<class Iterator>
	//coutFillChar表示填充字符 相当于printf的%02d 中的'0' coutWidth表示其中的2
	void outPutIterable(Iterator left, Iterator right, JCE::SizeType coutWidth = 0, char coutFillChar = '\0', char intervalCahr = ' ') {
		std::cout << std::setfill(coutFillChar);
		int count = -1;
		iterate(left, right, [&](Iterator left) {
			char c = (++count == 0 ? '\0' : intervalCahr);
			std::cout << c << std::setw(coutWidth) << *left;
		});
		puts("");
	}

	template<class Iterator>
	//lineWrap 隔多少行换行 小于0 表示输出结束换行
	void outPutIterable(Iterator left, Iterator right, JCE::SizeType coutWidth, char coutFillChar, JCE::SizeType lineWrap) {
		std::cout << std::setfill(coutFillChar);
		if (left == right) {
			return;
		}
		//设置left对齐
		std::cout << std::setiosflags(std::ios::left);
		std::cout << std::setw(coutWidth) << *left;
		++left;
		int c = 1;
		lineWrap = lineWrap < 0 ? right - left : lineWrap;
		iterate(left, right, [&](Iterator left) {
			++c;
			if (c % lineWrap == 0) {
				//最后一个
				//std::cout << " " << std::setw(coutWidth) << *left << std::endl;
				std::cout << std::setw(coutWidth) << *left << std::endl;
			}
			else if (c % lineWrap == 1) {
				//第一个
				std::cout << std::setw(coutWidth) << *left;
			}
			else {
				//中间的
				//std::cout << " " << std::setw(coutWidth) << *left;
				std::cout << std::setw(coutWidth) << *left;
			}
		});
		//最后一次没有换行这里换行
		if (c % lineWrap != 0) {
			puts("");
		}
		//取消对齐方式
		std::cout << std::resetiosflags(std::ios::left);
	}

	template<class Iterator, class Fun>
	//lineWrapFun 返回是否换行  bool(* lineWrapFun)(Iterator left, Iterator right)
	void outPutIterable(Iterator left, Iterator right, JCE::SizeType coutWidth, char coutFillChar, Fun lineWrapFun) {
		std::cout << std::setfill(coutFillChar);
		//设置left对齐
		std::cout << std::setiosflags(std::ios::left);
		int c = 0;
		bool isPastWrap = false;
		//lineWrapFun = nullptr == lineWrapFun ? [&](){return right - left; } : lineWrapFun;
		iterate(left, right, [&](Iterator left) {
			++c;
			isPastWrap = lineWrapFun(left);
			if (1 == c) {//第一个
				std::cout << std::setw(coutWidth) << *left;
			}
			else {
				//中间&最后
				std::cout << std::setw(coutWidth) << *left;
			}
			if (isPastWrap) {//最后一个
				std::cout << std::setw(coutWidth) << std::endl;
				c = 0;
			}
		});
		//最后一次没有换行这里换行
		if (!isPastWrap) {
			puts("");
		}
		//取消对齐方式
		std::cout << std::resetiosflags(std::ios::left);
	}


	template<class T>
	void outPut2DArrayList(JCE::ArrayList<JCE::ArrayList<T>> const &arrayList2D, char coutFillChar = '0', JCE::SizeType coutWidth = 2) {
		JCE::SizeType rows = arrayList2D.size();
		for (JCE::SizeType r = 0; r < rows; ++r) {
			outPutIterable(arrayList2D[r].begin(), arrayList2D[r].end(), coutWidth, coutFillChar);
		}
	}

	template<class T>
	void outPut2DArray(T array2D[MAX_R][MAX_C], JCE::SizeType n, JCE::SizeType coutWidth = 2, char coutFillChar = '0') {
		for (JCE::SizeType r = 0; r < n; ++r) {
			outPutIterable(array2D[r], array2D[r] + n, coutWidth, coutFillChar);
		}
	}

	template<class T>
	void outPut2DArrayTrangle(T array2D[MAX_R][MAX_C], JCE::SizeType n, JCE::SizeType coutWidth = 2, char coutFillChar = '0') {
		for (JCE::SizeType r = 0; r < n; ++r) {
			outPutIterable(array2D[r], array2D[r] + r + 1, coutWidth, coutFillChar);
		}
	}

	template<class Iterator>
	JCE::SizeType rankStatistics(Iterator left, Iterator right) {
		JCE::SizeType rank = 1;
		iterate(left + 1, right, [&](Iterator left) {
			rank += *left == *(left - 1) ? 0 : 1;
		});
		return rank;
	}

	//ARRAY_TEMPLATE
	template<class T, class Iterator>
	T minValueStatistics(Iterator left, Iterator right, T MAX_VALUE) {
		T minValue = MAX_VALUE;
		//iterate(left, right, [&](Iterator left){
		//	minValue = min(minValue, *left);
		//});
		while (left != right) {
			minValue = min(minValue, *left);
			++left;
		}
		return minValue;
	}

	template<class T, class Iterator>
	T maxValueStatistics(Iterator left, Iterator right, T MIN_VALUE) {
		T maxValue = MIN_VALUE;
		while (left != right) {
			maxValue = max(maxValue, *left);
			++left;
		}
		return maxValue;
	}

	template<class T, class Iterator>
	T sumValueStatistics(Iterator left, Iterator right, T ZERO_VALUE) {
		T sumValue = ZERO_VALUE;
		iterate(left, right, [&](Iterator left) {
			sumValue += *left;
		});
		return sumValue;
	}

	template<class T, class Iterator>
	double avlValueStatistics(Iterator left, Iterator right, T ZERO_VALUE) {
		return sumValueStatistics(left, right, ZERO_VALUE) / (double)(right - left);
	}

};

//效用扩展
namespace Utility {
	//using namespace StandardExtend;
	/************逆向*****0*********45*******90*********135*******180********225*******270********315****/
	//int dir[8][2] = { { 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 }, { -1, 0 }, { -1, -1 }, { 0, -1 }, { 1, -1 } };
	const int Dir8[8][2] = { { 1, 0 }, { 1, -1 }, { 0, -1 }, { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } };
	const int Dir4[4][2] = { { 0, -1 /*左*/ }, { 0, 1 /*右*/ }, { -1, 0 /*上*/ }, { 1, 0 /*下*/ } };

	//重载了精度比较符的双精度浮点
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

	//非聚合  点类
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

		//能从起始点'跳跃至'终点返回true
		static bool canJumpTo(PointDouble const &beganPoint, PointDouble const &endPoint, double jumpPower, double raduis) {
			//起点 - 终点 - 跳跃能力 - 活动半径
			return dDistance(beganPoint, endPoint) <= raduis + jumpPower;
		}
		//返回两点间距离
		static double dDistance(PointDouble const &p1, PointDouble const &p2) {
			return sqrt(dDistance2(p1, p2));
		}
		//返回两点间距离的平方
		static double dDistance2(PointDouble const &p1, PointDouble const &p2) {
			double dx = p2.x - p1.x;
			double dy = p2.y - p1.y;
			return (dx*dx + dy * dy);
		}
	};

	class DSAUTILITYEXTENSION_API Triangle {
	public:
		Triangle() {}
		Triangle(PointDouble const &p1, PointDouble const &p2, PointDouble const &p3) {
			this->p1 = p1;
			this->p2 = p2;
			this->p3 = p3;
		}
		~Triangle() {}

		//返回由p1, p2, p3组成的三角形的面积
		static double dAreaOfTriangle(PointDouble const &p1, PointDouble const &p2, PointDouble const &p3) {
			double d12 = PointDouble::dDistance(p1, p2)
				, d13 = PointDouble::dDistance(p1, p3)
				, d23 = PointDouble::dDistance(p2, p3);
			//海伦公式
			double dL = (d12 + d13 + d23) / 2.0;
			return sqrt(dL * (dL - d12) * (dL - d13) * (dL - d23));
		}
		//若点p位于p1, p2, p3组成的三角形内那么返回true 否则返回false
		static bool isInternalTriangle(PointDouble const &p1, PointDouble const &p2, PointDouble const &p3, PointDouble const &p) {
			double a12p = dAreaOfTriangle(p1, p2, p)
				, a13p = dAreaOfTriangle(p1, p3, p)
				, a23p = dAreaOfTriangle(p2, p3, p)
				, a123 = dAreaOfTriangle(p1, p2, p3);
			//被那个点分割出的所有面积块之和与三角形总面积在误差范围内相等时 则在三角形内部 精度过高可能会被坑 一般1E-5即可
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
		//符合勾股定理 返回true 练习题HDU2899
		static bool isPythagoreanTheorem(int a, int b, int c) {
			return a * a == b * b + c * c || b * b == a * a + c * c || c * c == a * a + b * b;
		}
		//判断由传入的参数是否能组成三角形 若可以 返回true 否则返回false
		static bool isTrangle(double a, double b, double c) {
			//三角形边长不等式
			return a + b > c && a + c > b && b + c > a
				&& fabs(a - b) < c && fabs(a - c) < b && fabs(b - c) < a;
		}
		//返回相等元素的个数
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

	// 分数
	class DSAUTILITYEXTENSION_API Fraction {
		//分子
		double up = 0;
		//分母
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
			//printf("已析构");
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
			// 公倍数
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


	//ans为s1+s2(s1与s2的并集)(直接ans.push_back(temp)即可没必要建实际生物s1与s2)
	//返回s1与s2并集的中位数, ans为s1+s2(直接push_back即可) a0,a1,a2...an-1的中位数指a[(n-1)/2] (共n个数)即第(n+1)/2个数(a0为第一个数)
	template<class T>
	T median(JCE::ArrayList<T> ans) {
		std::sort(ans.begin(), ans.end());
		if (ans.size() % 2 == 0)
			return ans[ans.size() / 2 - 1];
		else
			return ans[ans.size() / 2];
	}
	//交换(引用实现 易爆)
	template<class T>
	void swapUnstable(T &a, T &b) {
		a += b;
		b = a - b;
		a = a - b;
	}
	//反转reverseRows行的二维数组a 每行位于区间[列leftSub,列rightSub)的元素
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
	/*将R行 C列的二维数组向左移k位*/
	template<class T>
	void leftMoveK(JCE::ArrayList<JCE::ArrayList<T>> &a, Sub R, Sub C, JCE::SizeType k) {
		/*
		8 3
		1 2 3 4 5 6 7 8
		8 7 6 5 4/ 3 2 1
		4 5 6 7 8/ 1 2 3
		*/
		k %= C;
		reverse(a, 0, C, R);/*反转所有元素*/
		reverse(a, 0, C - k, R);/*反转前C-k个元素*/
		reverse(a, C - k, C, R);/*反转后k个元素*/
	}
	/*右移k位<==>左移C-k位 leftMoveK(a, R, C, C-k); <==> rightMoveK(a, R, C, k);*/
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
		reverse(a, 0, k, R);/*反转前k个元素*/
		reverse(a, k, C, R);/*反转后C-k个元素*/
	}
	// 计算第一个最长连续递增子序列(longest continuous increment sub sequence)
	// 返回长度 参数:用于储存最长连续子序列的区间[leftResultSub, rightResultSub)
	template<class T>
	JCE::SizeType calcFirstLonConIncSubSeq(JCE::ArrayList<T> const &sequence, int &leftResultSub, int &rightResultSub) {
		int maxLen = 0, sequenceLen = sequence.size();
		//rightResultSub=-1: 使计算其初是长度时为0
		leftResultSub = 0, rightResultSub = -1;
		for (int l = 0, r = 0, len = 0;; ++r) {
			len = r - l + 1;
			maxLen = rightResultSub - leftResultSub + 1;
			//更新最长递增子列 (第一个子序列:不取等)
			if (len > maxLen) {
				leftResultSub = l, rightResultSub = r;
			}
			else {
				//do nothing
			}
			if (r >= sequenceLen - 1)
				break;
			//r < sequenceLen - 1 保证sequenceLen==1时正确
			if (sequence[r] < sequence[r + 1]) {
				//do nothing
			}
			//当前子列不是递增子列
			else {
				////重置局部递增子列
				l = r + 1;
			}
		}
		++rightResultSub;
		return maxLen;
	}

	// uNum->resultNum 无符号转为有符号
	// (PS 使用assert作为判断的 如果assert无效那么等价于强制转换; 无返回值)
	template<class UnT, class T>
	void toSignedNum(UnT uNum, T &resultNum) {
		//确认是无符号
		assert(uNum - uNum - 1 > 0);
		resultNum = (T)uNum;
		//确认是有符号
		assert(resultNum - resultNum - 1 < 0);
		//确认转换后结果正确
		assert(resultNum > 0);
	}

	// ===== 排 序 less more greater

	//qsort(a, 排序长度, 一个元素的大小, intCmp);
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
		//按主价值 从小到大排序
		int cp = ((CmpAble*)lhs)->value - ((CmpAble*)rhs)->value;
		if (cp == 0) {
			//按次价值 从大到小排序
			cp = ((CmpAble*)rhs)->value2 - ((CmpAble*)lhs)->value2;
		}
		return cp;
	}
	// sort(a, a+n, boolCmp)
	template<class CmpAble>
	bool lessSortCmp(CmpAble lhs, CmpAble rhs) {
		return lhs.value < rhs.value;
	}
	//冒泡排序 对数组a中的n个元素进行的第K次排序
	template<class CmpAble>
	void BubbleSort(CmpAble a[], int n, int K, int cmp(const void *lhs, const void *rhs)) {
		CmpAble temp = 0;
		//K代表遍数时不能减一
		for (int i = 0; i < K; ++i) {
			//此处必须减一
			for (int j = 0; j < n - i - 1; ++j) {
				if (cmp(&a[j + 1], &a[j]) < 0) {
					temp = a[j + 1];
					a[j + 1] = a[j];
					a[j] = temp;
				}
			}
		}
	}
	//选择排序
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
	//希尔排序
	template<class T>
	void shellSort(T a[], int n) {
		int si;
		int sedgewick[] = { 929, 505, 109, 41, 19, 5, 1, 0 };
		//找到第一个小于排序元素个数的增量
		for (si = 0; sedgewick[si] >= n; ++si);

		for (int d = sedgewick[si]; d > 0; d = sedgewick[++si]) {
			//插入排序
			for (int p = d, i; p < n; ++p) {
				T tmp = a[p];
				for (i = p; i >= d && a[i - d] > tmp; i -= d) {
					//i属于[d, p], 将前面"大于"a[p]的值后移
					a[i] = a[i - d];
				}
				//a[i - d] <= tmp时 因为前面都是局部有序的 [0, i-d]区间内的值 <= a[p]一定成立, (i, p]内的值>a[p]一定成立
				//于是i就是a[p]的位置 ps这句放里面也行 只是会多做几次无用的交换
				a[i] = tmp;
			}
		}
	}
}

//数学扩展
namespace MathExtend {
	//using namespace Utility;

	//矩阵乘法 productMatrix = productMatrix*originMatrix(两矩阵不能相同)
	template<class T>
	void matrixMultiply(const Sub maxRowCol, JCE::ArrayList<JCE::ArrayList<T>> &originMatrix, JCE::ArrayList<JCE::ArrayList<T>> &productMatrix) {
		int r, c, i, tmp;
		JCE::ArrayList<JCE::ArrayList<T>> tempMatrix;
		//memcpy(tempMatrix, productMatrix, maxRowCol * 25 * sizeof(int));
		tempMatrix = originMatrix;
		for (r = 0; r < maxRowCol; r++) {
			for (c = 0; c < maxRowCol; c++) {
				//行列遍历对应相乘 累加给tmp 再赋给 productMatrix
				for (tmp = i = 0; i < maxRowCol; i++)
					tmp += originMatrix[r][i] * tempMatrix[i][c];
				productMatrix[r][c] = tmp;
			}
		}
	}

	// =======查 找

	// 二分查找origin[n]中key 返回key的迭代器, 若没有返回origin+size迭代器 注意:如果有多个相同元素这个方法返回的元素可能是其中任意一个
	// origin迭代器所处的容器必须支持索引[]操作(序列式容器，或者数组) 并且容器内[origin, origin+size)范围内元素必须已排序
	// std有类似二分搜索实现(std::binary_search) 但只是 检查等价于 value 的元素是否出现于范围 [first, last) 中
	// 类似的还有:	A. count,find
	//				B.binary_search, lower_bound, upper_bound, equal_range
	template<class Iterator, class ValueType>
	Iterator dichotomy(Iterator origin, JCE::SizeType size, ValueType key) {
		int left = 0, right = size, mid = 0;
		while (left <= right) {
			mid = (right + left) / 2;
			if (origin[mid] < key) {
				left = mid + 1;
			}//加1之前若确认mid不合法, 于是可以把这个结果用到下一次的计算中, 若不加1下一次可能会重新计算mid
			else if (origin[mid] > key) {
				//同上
				right = mid - 1;
			}
			else {
				return origin + mid;
			}
		}
		return origin + size;
	}
	// 三分法 求一元函数f(x)在 区间[left, right]内的的最小解x (eps: 精度, 求最大值可外部实现) TY(*fun)(TX x)
	template<class TY, class TX, class Fun>
	TX trichotomy(TX left, TX right, Fun fun, TX eps) {
		TX Ll, Rr;
		while (right - left > eps) {
			//三分
			Ll = (2 * left + right) / 3;
			Rr = (2 * right + left) / 3;
			if (fun(Ll) > fun(Rr)) {
				left = Ll;
			}
			else {
				right = Rr;
			}
		}
		//返回left ,right任一个即可(PS: 不应返回f(x)的值 因为fun可能不是用户需要的fun 参见顶部注解)
		return left;
	}

	//返回错排表 64 bit 二进制位 最多算 D[23] IntegerNum 需要支持 + * 运算
	//最小的几个错排数是：D1 = 0，D2 = 1，D3=2，D4 = 9，D5 = 44，D6 = 265，D7 = 1854
	template<class IntegerNum>
	void buildIllArrangeList(JCE::ArrayList<IntegerNum> &D, JCE::SizeType MAX_SIZE = 23) {
		D.assign(MAX_SIZE, 0);
		D[0] = 0, D[1] = 0, D[2] = 1;
		for (JCE::SizeType n = 3; n < MAX_SIZE; ++n)
			D[n] = (D[n - 1] + D[n - 2]) * (n - 1);
	}
	//若大数bigInteger能被整除返回true
	DSAUTILITYEXTENSION_API bool isDivisible(char *bigInteger, int MOD);
	//删除resStr中的所有子串delSubStr
	DSAUTILITYEXTENSION_API void eraseAllSubStr(char *resStr, char *delSubStr);
	//逆序数
	DSAUTILITYEXTENSION_API int reverseOrderNumber(int number);
	//计算radix进制整数number中digit出现的次数
	DSAUTILITYEXTENSION_API int calcDigitCountInNumber(const int number, const int digit, int radix = 10);
	//计算radix进制整数的位数
	DSAUTILITYEXTENSION_API int calcDigitTop(int number, int radix = 10);


	// ===== 数 学

	DSAUTILITYEXTENSION_API I64 quickFact(I64 a, I64 b, I64 mod);
	//快速幂基于quickFact版 效率是简易版的1/5
	DSAUTILITYEXTENSION_API I64 quickPow_OLD(I64 C, I64 R, I64 k);
	//快速幂简易版 m^n % k 如果用于计算pow效率是普通版的1/20 (普通版效率高)
	DSAUTILITYEXTENSION_API I64 quickPow(I64 m, I64 n, I64 k);
	//快速幂普通版 m^n; 与std::pow效率各有高低相差不大 std::pow值域更大
	DSAUTILITYEXTENSION_API I64 quickPow(I64 m, I64 n);
	//返回pow(iNum, 2)的最后m位(实际上貌似只能算最后1bit 可能哪里被无意中修改了) 注意返回值可能包涵多位但只有最后一位有效
	DSAUTILITYEXTENSION_API long long powLastBit(int base, int m);
	//任意底数对数
	DSAUTILITYEXTENSION_API double logR(double value, double base = 5);
	//阶 乘
	DSAUTILITYEXTENSION_API double fact(int n);
	//int 限度内阶乘表 (int 13!爆； double和long long int 18!爆)
	DSAUTILITYEXTENSION_API int* generateFactList(int maxN = 13);
	/*
	n < m && m != 0
		 fact(n)
	 ________________
	fact(n - m) * fact(m)
	*/
	//组合数 (取出元素不重复出现)
	//从n个不同元素中，任取m(m≤n)个元素并成一组，叫做从n个不同元素中取出m个元素的一个组合；
	//ab == ba
	DSAUTILITYEXTENSION_API int C(int n, int m);
	//排列数 (取出元素不重复出现)
	//从n个不同元素中，任取m(m≤n)个元素按照一定的顺序排成一列，叫做从n个不同元素中取出m个元素的一个排列
	//ab != ba
	DSAUTILITYEXTENSION_API int A(int n, int m);
	//素数(质数)判断
	DSAUTILITYEXTENSION_API int isPrime_OLD(int num);
	//素数(质数)判断 sqrt的平方优化版(未实测)
	DSAUTILITYEXTENSION_API bool isPrime(int num);
	//真因子和(除本身外的约数）
	DSAUTILITYEXTENSION_API int factorSum(int x);
	//真因子和表(约数 因数) (10^7一千万2.51s)(400w 10^6 900ms)
	DSAUTILITYEXTENSION_API void buildSieveFactorSumS(const int maxn, int a[]);
	//素数(质数) 筛选法 埃拉托色尼(Sieve Eratosthenes)(0 1==-1, a[i]==0表示素数) maxN[2, 1e9) PS:maxN = 1e7时超过1s了 1e9似乎无法分配内存
	DSAUTILITYEXTENSION_API int* generateSievePrimeS(const int maxN = 2);
	//因子数目
	DSAUTILITYEXTENSION_API int factorCount(int x);
	//开方函数 (eps: 精度)
	DSAUTILITYEXTENSION_API double sqrtImpl(double x, double eps = 1e-9);
	//求方差
	DSAUTILITYEXTENSION_API double variance(float x[], int n);
	// 卡塔兰数
	DSAUTILITYEXTENSION_API I64* catalanTable();
	//返回lineNum条边时最大交点数目
	DSAUTILITYEXTENSION_API int countMaxIntersection(int lineNum);


	//辗转相除求公约数, 互素(互质)判定
	DSAUTILITYEXTENSION_API int gcd(int a, int b);
	/*
	扩展的欧几里得算法（辗转相除法）Extended Euclidean algorithm:
	功能:
		已知整数a、b, 在求得a、b的最大公约数的同时, 可找到满足贝祖等式ax + by = gcd(a, b)的整数x、y;（也称裴蜀数, 其中一个很可能是负数）
	若a是负数，可以把问题转化成|a|(-x) + by = gcd(|a|, b)，然后令 x' = (-x)
		利用定理求线性方程的解*//*
	返回值      :最大公 约数,因数,因子 gcd(greatest common divisor) 或 hcf(highest common factor)
	已知参数a, b:任意非负整数(若是负数可能要更新算法)
	返回参数x, y:贝祖数 或 模逆元...
	*//*
	贝祖等式(裴蜀定理)大意:两数的最大公约数的倍数可以用两数的整数倍相加来表示
	关于未知数x和y的方程 ax + by = m有整数解<==>m是d的倍数。(d是任意整数a、b的最大公约数)
	裴蜀等式有解时必然有无穷多个整数解，每组解x y都称为裴蜀数，可用扩展欧几里得算法求得。
	应用:
	对于不定整数方程ax+by=c，若c 是 gcd(a,b)的整数倍则该方程存在整数解
	否则不存在 x , y 整数解。
	讨论是否存在一整数k，使得m在k圈后比n大一；就是求解： k*m - n = 1;是否有解
	1 % gcd(m,n) == 0
	*/
	DSAUTILITYEXTENSION_API int gcdEx_OLD(int a, int b, int &x, int &y);
	//扩展欧几里得算法精简正式版
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
	定理一(贝祖定理)：ax + by = gcd(a, b)必有整数解
	定理二：若gcd(a, b) = 1，则方程ax ≡ c (mod b)在[0, b-1]上有唯一解.
	证明:对于方程 ax + by = c
	<==> ax ≡ c (mod b)			==> 若方程存在特解x,那么x + k*b还是方程的解
	定理三：若gcd(a, b) = g，则方程ax ≡ c (mod b)在[0, b/g - 1]上有唯一解.
	证明:对于方程 a/g*x+b/g*y = c/g	(若x,y是整数解; g = gcd(a, b) ==> 方程左边是个整数 ==> 右边也该是整数 ==> 否则x,y不是整数解)
	<==> a/g*x ≡ c/g (mod b/g)	==> 若方程存在特解x,那么x + k*(b/g)还是方程的解
	*/ //PKU1061-青蛙的约会
	//求线性方程ax+by = c 的最小非负整数解x(只能保证x满足条件) 若整数解不存在返回false
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
	定理：对于方程ax+by = c
	通解:	x = x0 + b*t
	y = y0 - a*t
	(①:x0, y0为方程的一组解; ②:gcd(a, b) = 1; ③:t为任一整数.)

	①:	g = gcdEx(a, b, x0, y0);
	x0 = x0*c / g;
	y0 = y0*c / g;
	②:	x = x0 + b / g*t
	y = y0 - a / g*t
	于是		|x|+|y| = |x0+b/g*t| + |y0-a/g*t|
	易知		|x0+b/g*t|单调递增，|y0-a/g*t|单调递减.
	若规定	a>b
	那么		原函数为先减后增的凹图像 (当t满足y0 - a/g*t == 0 时取最小值)
	即		|x|+|y| 在t = y0*g/a 附近(③)取最小值
	*/ //PKU2142-HDU1356-The Balance
	//求线性方程ax+by = c 使得|x|+|y|最小的一组解x,y
	DSAUTILITYEXTENSION_API void linearEquationCondition2(int a, int &x, int b, int &y, int c);
	/*
	modulo inverse
	功能:返回a的模m逆元t, 不存在打印错误并结束程序;
	a*modInv(a, m) % m = 1
	因为at%m=1：要求余数2的话，2*at，要求余数为3的话，3*at
	定义

	同余号 ≡: 若一个正整数除两个整数 得相同余数，则称二整数[同余]。
	同余关系(离散数学):aRb <==> a与b用m除时具有相同的余数r, 此时, 称m是模数, 记a≡b(mod m); (modulo); 读作:a与b模m同余
	同余关系是一个[等价关系]:自反,对称,传递

	同余类(congruence class或residue class):由对于模n同余的所有整数组成的一个集合; 若从上下文知道模n，则也可标记为[a]。
	同余类的代表数(representative):同余类中的拿来代表该同余类的任一元素

	模逆元: t ≡ a^(-1)(mod m) 或 at ≡ 1(mod m); 若at与1模m同余, 则t是a的对同余m的[模逆元], 也叫做模m的[数论倒数];
	若ax + my = 1 <==> (1 ≡ 1) ax+my ≡ 1 ≡ ax(mod m) ==> ax ≡ 1; 根据定义, x即是a关于模m的一个模逆元。
	*/
	DSAUTILITYEXTENSION_API int modInv(int a, int m);
	/*
	@see https://zh.wikipedia.org/wiki/%E4%B8%AD%E5%9B%BD%E5%89%A9%E4%BD%99%E5%AE%9A%E7%90%86
	中国剩余定理: 说明了一元线性同余方程组有解的准则以及求解方法
	通解以及算法: ansNum = sum(remainderS[i]*M[i]*t[i]) + k*product(modS[i])
	(其中: M[i]是模数两两相乘的结果 t[i]是其数论倒数(模逆元), 在模mP的意义下方程组只有ansNum一个解)
	经典问题:
	孙子定理, 韩信点兵, 物不知数(xx数之x就是模数), 生理周期(周期是模数 上一次外显的日期是同余代表数)
	*/
	// 已知参数modS(简称m):									模数组(要求modS的元素两两互质, 无参数检查)
	// 已知参数remainderS(简称a): 余数组(强参数)				remainderS[i] = ansNum%modS[i]
	// 返回值:		同时满足%模数组的所有元素的值分别等于余数组对应元素 的最小正整数ansNum
	// ansNum模modS[i]同余代表数组(弱参数)	ansNum ≡ remainderS[i](mod modS[i])
	DSAUTILITYEXTENSION_API int chineseReminder(int *modS, int *remainderS, int sizeNum, int mP = 0);

	// 欧拉函数:在数论中用于求解[1,n]中与n 互质数 的个数 的函数
	DSAUTILITYEXTENSION_API int  Eular(int n);
	//巴什博弈：取最后一个的人胜
	DSAUTILITYEXTENSION_API int BaShen(int n, int min, int max);

	// ===== Backpack(背包) DP(动态规划) 统一目的: 求解将哪些物品装入背包可使价值总和最大。
	// 背包总容量: capacity (其余备选词汇 volume: 体积, account: 账户, amount: 总数) <==> backPack.size+1
	// 单物品价值: value    (其余备选词汇 weight: 权重, 重量, 负重) PS 备选词可能含有歧义, 或易误解

	// 处理一件[零壹背包]中的物品过程 => 有N种物品和一个容量为capacity的背包backPack。每种物品只能取1次或不取(数量为1)
	// 放入第i件物品耗费的费用是 C[i],得到的价值是 W[i].
	template<class Numeric>
	void ZeroOneBackpackProcess(Numeric backPack[], Numeric capacity, Numeric currentCost, Numeric currentValue) {
		for (Numeric v = capacity; v >= currentCost; --v) {
			backPack[v] = max(backPack[v], backPack[v - currentCost] + currentValue);
		}
	}
	// (这个算法有问题) 01背包数量记法 capacity是背包总容量  参数currentValue = 0; backPack[0] = 1;
	template<class Number>
	void ZOBackpackProcess(Number backPack[], Number capacity, Number currentCost, Number currentValue) {
		for (Number v = currentCost; v <= capacity; ++v) {
			backPack[v] += backPack[v - currentCost] + currentValue;
		}
	}
	// 处理一件[完全背包]中的物品过程 ==> 有N种物品和一个容量为capacity的背包，每种物品都有无限件可用。
	template<class Number>
	void CompletePackProcess(Number backPack[], Number capacity, Number currentCost, Number currentValue) {
		for (Number v = currentCost; v <= capacity; ++v) {
			backPack[v] = max(backPack[v], backPack[v - currentCost] + currentValue);
		}
	}
	// 处理一件[多重背包]中的物品过程 ==> O(logM) 有N种物品和一个容量为capacity的背包。
	// 第i种物品最多有currentNum=M[i]件可用，每件耗费的空间是currentCost=C[i]，价值是 currentValue=W[i]。
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

	// =======受限的(Confined) ===== 递归 ===== 贪心 ===== 分治

	// 密里根油滴实验程序
	DSAUTILITYEXTENSION_API void MiLIGen(double u, double v1);
	//输出digit中[leftSub, rightSub)的全排列  非字典序
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
	//输出数字1-maxDigit(1, 9)的全排列  字典序
	DSAUTILITYEXTENSION_API void pentration(int maxDigit);
	//输出数字1-maxDigit(1, 9)的全排列  非字典序
	DSAUTILITYEXTENSION_API void pentration();
	//汉诺塔递归解法
	DSAUTILITYEXTENSION_API void hannoTowerMove(int n, char a, char b, char c);
	//将10进制的number转换为radix进制的字符串 (递归实现)
	DSAUTILITYEXTENSION_API std::string decToBin(int number, std::string &result, int radix = 2);
	// 计数: 返回子串s1与s2匹配的字符数 比较长度len)
	DSAUTILITYEXTENSION_API int countMatchingChar(char *s1, char *s2, int len);
	// 判断一个字串是否回文(堆栈实现)
	DSAUTILITYEXTENSION_API bool isPlalindrome(char const*str, int len);
	// 判断给定字串是否拥有匹配的括号
	DSAUTILITYEXTENSION_API bool isMatchingParenthesis(char const*str, int len);
	// 判断给定堆栈操作是否合法
	DSAUTILITYEXTENSION_API bool isValidityOfStack(char const*str, int len, int cap);
	//生成n行 的杨辉三角
	DSAUTILITYEXTENSION_API void buildPtriangleTable(int tableBuffer[][StandardExtend::MAX_C], int n);
}

#endif
