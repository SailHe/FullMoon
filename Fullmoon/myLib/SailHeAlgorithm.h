#ifndef __MYALGORITHM_H
#define __MYALGORITHM_H
//包涵了几乎所有文件
//#include<bits/stdc++.h>
/*
__int64 I64 (use long long in acm oj)
ps: ACM OJ应该是开了编译器优化的
Rows = r;Cols  = c;//报表行列数
1s 一般可以执行 10 ^ 7 次操作
DP:动态规划
D:\LaberDate\

Online Judge在线评测系统(例如pta, hdu等等)


字体测试
XxKkIiLl 10 oO{}{}
Pp
lllll
11111
*/
#include <setjmp.h>
#include <assert.h>
#include <time.h>
#include <cstring>//memset//
#include <cstdio>//scanf//
#include<iostream>
#include<algorithm>
#include<cmath>
#include<iomanip>
#include<set>
#include<string>
#include<queue>
#include<list>
#include<map>
#include<functional>
#include<stack>
using namespace std;

#define PIL 3.14159265358979//14位π
#define MileByF(feet) ((feet)/5280.0)//5280 feet in a mile.
#define FootByI(inches) ((inches)/12.0)//12 inches in a foot.
#define HourByM(minutes) ((minutes)/60.0)//60 minutes in an hour.
#define MinuteByS(seconds) ((seconds)/60.0)//60 seconds in a minute.
#define FurlongByM(meters) ((meters)/201.168)//201.168 meters in a furlong(mile).
#define GET_ARRAY_LEN(array, length) { length =  sizeof(array) / sizeof(array[0]); }//求数组长度
#define SUM(a,b) (a)+(b)
#define MAX(a,b) (a)>(b)?(a):(b)
#define MIN(a,b) (a)<(b)?(a):(b)
#define SWAP(a, b) {int temp;temp = (a);(a) = (b);(b) = temp;}
#define PX Dir4[n][0]
#define	PY Dir4[n][1]
#define ALL(object) object.begin(), object.end()
#define FOR(i, bound, limit) for (int i = bound; i < limit; ++i)
#define infinity 2147483647
#define MAX_IN32 2147483647
#define MIN_IN32 (-MAX_IN32-1)
#define MAXN 1000000+2
const double EPSINON = 1e-15;
const double EPS = 1e-9;
const double PI = 3.14159265;//八位π
const double PIA = acos(-1.0);
const int DIR4x[] = { -1, 0, 1, 0 };
const int DIR4y[] = { 0, 1, 0, -1 };
extern const int Dir8[8][2];
extern const int Dir4[4][2];
typedef __int64 I64;

#endif
