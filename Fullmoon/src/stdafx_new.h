#ifndef __STDAFX_H
#define __STDAFX_H
/*StandardExtend Application Framework Extensions����׼Ӧ�ó����ܵ���չ��*/
/*���������ļ���Ҫ��ͷ�ļ�(��ʱ�䰴����ĸ����)*/
#include <assert.h>//����
#include <windows.h>//QueryPerformanceFrequency//
#include <conio.h>//getch//
#include <time.h>
#include<cstring>//memset//
#include<cstdio>//scanf//
#include <fstream>
#include <iostream>
#include<algorithm>
#include<cmath>
#include<iomanip> //cout���ֲ�����
#include<set>
#include<string>
#include<stack>
#include<queue>
#include<list>
#include<map>
#include<functional> //greater<int>
#include<memory> //����ָ��:shared_ptr<>; unique_ptr<>;---P400
#include <unordered_set>
#include <unordered_map>

/*
explicit(��ʽ/���/��ȷ),ֻ�����ڴ����������Ĺ��캯��ǰ�����߱�����������ʹ�øù��캯��������ʽ����ת����
���ȷʵ��ת��������������ʽ������ת����ʽ�����С��Ӷ������˿��ܳ��ֵ�����Ͷ���ĵ��ÿ�����

<�������ģʽ> ��ģʽʹ�� C++ �����д�ó��Ķ����š�
:������һ�����뵥Ԫ��Translation Unit���ж��壨define�����������͡������ȣ�����һ�����뵥Ԫ�������ǡ�
��������Compiler�����������з��뵥Ԫ��
��������Linker����������������ָ�� extern ���ŵ����ã�
�Ӷ����ɵ�һ��ִ���ļ���
extern(������) Ϊ�˷����������뵥Ԫ������һ�����ļ����еı�������󣬶���ͨ���ͣ��������������ࡢ�ṹ���ࣩ
export(����/����/���) �ڶ���ģ��������ģ�庯��ʱ��extern����
*/

//���ڰ�ȫ�滻
#define scanf scanf_s
#define gets gets_s

//���Կ��� �رպ�ɼ���һЩ����Ҫ���ж� �������Ч��
#define DEBUG false

//����һ����������  �����: ֧�ֵ�����������
#define PARAMETER_ALL_OBJECT(container) container.begin(), container.end()

//limit(�޶ȣ�����)��һ�����޼�: > limitʱ�˳�
//�޽�for
#define FOR_LIMIT(i, lowerBound, limit) for (auto i = lowerBound; i < limit; ++i)

//�޷����޽�for
#define FOR_LIMIT_UNSIGNED(i, lowerBound, limit) for (size_t i = lowerBound; i < limit; ++i)

//it���û�ָ����һ������, ���������½�(lowerBound)����, ������Χ(bound)�ڱ��뱣֤�������ֹ��(terminator: �ս���)
//��ֹfor �����: ָ��������, ����1, ��2 (��12��֧��!=����) ֻ�����ڱ���(����std::vector)
#define FOR(it, lowerBound, terminator) for (auto it = lowerBound; it != terminator; ++it)

//for����������Ԫ�� �����: ֧�ֵ�����������  (�����������end()�Ż���FOR)
#define FOR_ALL_OBJECT(container) for (auto &element : container)

//ĩβ�޿ո�Ĵ�ӡ(cout)
#define DISPLAY(A, lowerBound, terminator) {FOR(i, lowerBound, terminator){printf(i == lowerBound ? "" : " ");cout << A[i] << endl;}}

//�����ô�ӡ
#define DE_PRINTF(s){if(DEBUG)printf("---%s\n", s);}
#ifndef  _DEBUG_ERROR
//_STL_VERIFY(false, s);
#define _DEBUG_ERROR(s){_ASSERT_EXPR(false, s); }
#endif //  _DEBUG_ERROR

//˫�ֽ��޷�������(16λ)
#define MAX_INT16 65535
#define MIN_INT16 (-MAX_INT16-1)
//4�ֽ�32λ�з������� 2^31-1 = 0x7FFFFFFF; Լ2e9(10bit)
#define MAX_INT32 2147483647
#define MIN_INT32 (-MAX_INT32-1)
//8�ֽ�64λ�з������� 2^63-1 std::to_string(pow(2, 63)); Լ9e18(19bit)
#define MAX_INT64 9223372036854775807
#define MIN_INT64 (-MAX_INT64-1)

#define infinity MAX_INT32
//using namespace std;
//using namespace std::placeholders;

//_ASSERTE(_CrtCheckMemory());//�ڴ���

//java������������չ�ռ�
namespace JavaContainerExtend {
	//�ɱ�����
	template<typename T> using Varrays = std::vector<T>;
	//�����
	template<typename T> using ArrayList = std::vector<T>;
	//�ڽӱ�
	template<typename T> using LinkedList = std::list<T>;
	//�ַ���
	using String = std::string;

	using SizeType = size_t;

	template<typename T> using stack = std::stack<T>;
	template<typename T> using queue = std::queue<T>;
	template<typename T> using priority_queue = std::priority_queue<T>;
	template<typename T> using greater = std::greater<T>;
	template<typename T1, typename T2> using pair = std::pair<T1, T2>;
}

namespace JCE = JavaContainerExtend;

#endif
