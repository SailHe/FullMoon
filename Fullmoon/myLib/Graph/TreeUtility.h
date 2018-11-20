#pragma once
#include "TreeObject.h"
#include "../ExtendSpace.h"

template<class Base>
/*指针数组 可用来实现HashTable 写一个hash()方法做索引即可  管理容器比较方便*/
/*管理普通结构体要注意  首次访问时要将外部构造的给它管理(自动析构)  赋值时注意不改变原指针*/
class PtrArray {
	JCE::ArrayList<Base*> table;
	unsigned size_ = 0;
public:
	PtrArray() {}
	//线性Θ(size)
	PtrArray(int siz_) {
		size_ = siz_;
		table.resize(size_);
	}
	~PtrArray() {
		for (JCE::SizeType i = 0; i < table.size(); ++i) {
			delete table[i];
		}
	}
	int size() {
		return size_;//vector 的size 计算实际上只是两个指针的相减操作
	}
	//重载[]操作符，返回管理的指针作为左值 越界自动申请内存
	Base*& operator[](unsigned index) {
		if (index >= size_) {
			size_ = index + 1;//实际使用内存
			table.resize(index * 2 + 1);
			//_DEBUG_ERROR("PtrArray subscript out of range");
		}
		if (table[index] == NULL)table[index] = new Base();
		return table[index];
	}
	//字符串 hash策略示范
	static int strHash(char *key) {
		//最大为182789
		return
			(key[0] - 'A') * 26 * 26 * 10 +
			(key[1] - 'A') * 26 * 10 +
			(key[2] - 'A') * 10 +
			(key[3] - '0');
	}
	//纯数字的字符串形式hash策略 5位max=9w
	int numStrHash(char *key) {
		int sum =
			(key[0] - '0') * 10000 +
			(key[1] - '0') * 1000 +
			(key[2] - '0') * 100 +
			(key[3] - '0') * 10 +
			(key[4] - '0') * 1;
		return sum % 30000;
	}
};

//家谱树结点
struct FamilyTreeNode {
	int bCnt = -1;//输入前有几个空格 即代表的是属于哪一级
	JCE::String father;
	JCE::String name;
	bool operator>(FamilyTreeNode const &rhs) {
		return name > rhs.name;
	}
	bool operator==(FamilyTreeNode const &rhs) {
		return name == rhs.name;
	}
	bool operator<=(FamilyTreeNode const &rhs) {
		return name <= rhs.name;
	}
	bool operator>=(FamilyTreeNode const &rhs) {
		return name >= rhs.name;
	}
	bool operator<(FamilyTreeNode const &rhs) {
		return name < rhs.name;
	}
	FamilyTreeNode() {}
	FamilyTreeNode(JCE::String name_, int cnt) {
		name = name_;
		bCnt = cnt;
	}
	FamilyTreeNode(JCE::String name_) {
		name = name_;
	}
	~FamilyTreeNode() {}
	//返回第一个可见字符前的空格数
	static int blnakCount() {
		int count = 0;
		char c;
		while ((c = getchar()) == ' ' || c == '\n') {
			if (c == ' ')
				++count;
		}
		ungetc(c, stdin);
		return count;
	}
};

//目录(Catalog)结构
struct CatalogNode {
	JCE::String name;
	//(set内的元素是有序的)
	std::set<JCE::String> subFolder;//子文件夹名集合
	std::set<JCE::String> file;//文件名集合
	CatalogNode() {}
	CatalogNode(JCE::String name_) {
		name = name_;
	}
	~CatalogNode() {}
	bool operator==(CatalogNode const &rhs)const {
		return name == rhs.name;
	}
	bool operator!=(CatalogNode const &rhs)const {
		return !(*this == rhs);
	}
	bool operator<(CatalogNode const &rhs)const {
		return name < rhs.name;
	}
	bool operator>(CatalogNode const &rhs)const {
		return name > rhs.name;
	}
	bool operator<=(CatalogNode const &rhs)const {
		return !(*this > rhs);
	}
	bool operator>=(CatalogNode const &rhs)const {
		return !(*this < rhs);
	}
	//从输入流获取一个名字存在对象name内   文件夹名返回true  文件名返回false
	bool getAName() {
		name.clear();
		char c;
		while ((c = getchar()) != '\n' && c != '\\') {
			name += c;
		}
		return c == '\\';
	}
	void print(int blankCnt) {
		for (int i = 0; i < blankCnt; ++i)
			printf(" ");
		//cout << name << endl;
		puts(&(name)[0]);
	}
	//输出该文件夹内的所有内容(所属数据树, root输出前的空格缩进)
	void output(AvlTree<CatalogNode> *t, int blankCnt) {
		static CatalogNode tmp;
		print(blankCnt);
		//输出子文件夹
		for (auto i = subFolder.begin(); i != subFolder.end(); ++i) {
			tmp.name = *i;
			t->find(tmp)->Data.output(t, blankCnt + 2);
		}//输出文件name
		for (auto i = file.begin(); i != file.end(); ++i) {
			for (int j = 0; j < blankCnt + 2; ++j)
				printf(" ");
			puts(&(*i)[0]);
		}
	}
};

struct Accounts {
	using myInt = int;
	myInt account;//unsigned 无法完全映射10位数 会有冲突
	JCE::String password;
	bool operator<(Accounts const &rhs)const {
		return account < rhs.account;
	}
	bool operator>(Accounts const &rhs)const {
		return account > rhs.account;
	}
	bool operator==(Accounts const &rhs)const {
		return account == rhs.account;
	}
	//未知地方必须默认构造
	Accounts() {}
	//插入用
	Accounts(myInt a, JCE::String &pa) {
		account = a, password = pa;
	}
	//查找用
	Accounts(myInt a) {
		account = a;
	}
	myInt hash()const {
		return account % 70000;//似乎与输入数据量大小差不多时效率最佳
	}
	/*
	map<JCE::String, JCE::String>
	0	sample 全部5种输出信息											答案正确	2 ms	252KB
	1	最大表，全部是新申请，密码全部16位									答案正确	715 ms	19760KB
	2	N和L指令各一半，随机交错。帐号随机，取到上下界。密码随机，取到上下界	答案正确	669 ms	10576KB

	HashAvlTable<Acounts> t = HashAvlTable<Acounts>(100000)
	0	sample 全部5种输出信息											答案正确	7.6 ms	8012KB
	1	最大表，全部是新申请，密码全部16位									答案正确	842 ms	21340KB
	2	N和L指令各一半，随机交错。帐号随机，取到上下界。密码随机，取到上下界	答案正确	601 ms	15432KB

	AvlTree<Acounts>
	0	sample 全部5种输出信息											答案正确	2 ms	248KB
	1	最大表，全部是新申请，密码全部16位									运行超时	0 ms	0KB  > 1200ms
	2	N和L指令各一半，随机交错。帐号随机，取到上下界。密码随机，取到上下界	运行超时	0 ms	0KB
	*/
};

struct Olympic {
	int id = -1, gold = 0, medal = 0, population = 0;
	//涉及double比较 重复元素
	//double data_[5];//0ID 1金牌 2奖牌 3人均金牌 4人均奖牌(per capita)
	Olympic() {}
	~Olympic() {}
	bool operator<(Olympic const &rhs)const {
		return id < rhs.id;
	}
	bool operator>(Olympic const &rhs)const {
		return id > rhs.id;
	}
	bool operator==(Olympic const &rhs)const {
		return id == rhs.id;
	}
};

//主键是score 其次是id
struct Student {
	char id[14];//考号(13位整数字)
	int score = 0;//得分（为[0,100]区间内的整数）
	int place = 0, rank = 1;//考点编号 排名
	int localRank = 1;//在该考点的排名
	Student() {
		memset(id, 0, 14 * sizeof(char));
	}
	Student(Student const&rhs) {
		*this = rhs;
	}
	~Student() {}
	//主键是score 当其相等时按次键从大到小排序
	//RE_ORDER成绩丛高到低 id从小到大 (重载<时id是 > 所以实际上是大到小 但这里是RE_ORDER 变回小到大)
	bool operator<(Student const &rhs)const {
		/*等价写法
		if (score == rhs.score)
		return strcmp(id, rhs.id) > 0;
		return score < rhs.score;
		*/
		return score < rhs.score ||
			score == rhs.score && strcmp(id, rhs.id) > 0;
	}
	bool operator>(Student const &rhs)const {
		return score > rhs.score ||
			score == rhs.score && strcmp(id, rhs.id) < 0;
	}
	bool operator==(Student const &rhs)const {
		return score == rhs.score && strcmp(id, rhs.id) == 0;
	}
	//考号、最终排名、考点编号、在该考点的排名。
	void outPut() {
		printf("%s %d %d %d\n", id, rank, place, localRank);
	}
};

