#pragma once
#include "TreeObject.h"
#include "../ExtendSpace.h"

template<class Base>
/*ָ������ ������ʵ��HashTable дһ��hash()��������������  ���������ȽϷ���*/
/*������ͨ�ṹ��Ҫע��  �״η���ʱҪ���ⲿ����ĸ�������(�Զ�����)  ��ֵʱע�ⲻ�ı�ԭָ��*/
class PtrArray {
	JCE::ArrayList<Base*> table;
	unsigned size_ = 0;
public:
	PtrArray() {}
	//���Ԧ�(size)
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
		return size_;//vector ��size ����ʵ����ֻ������ָ����������
	}
	//����[]�����������ع����ָ����Ϊ��ֵ Խ���Զ������ڴ�
	Base*& operator[](unsigned index) {
		if (index >= size_) {
			size_ = index + 1;//ʵ��ʹ���ڴ�
			table.resize(index * 2 + 1);
			//_DEBUG_ERROR("PtrArray subscript out of range");
		}
		if (table[index] == NULL)table[index] = new Base();
		return table[index];
	}
	//�ַ��� hash����ʾ��
	static int strHash(char *key) {
		//���Ϊ182789
		return
			(key[0] - 'A') * 26 * 26 * 10 +
			(key[1] - 'A') * 26 * 10 +
			(key[2] - 'A') * 10 +
			(key[3] - '0');
	}
	//�����ֵ��ַ�����ʽhash���� 5λmax=9w
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

//���������
struct FamilyTreeNode {
	int bCnt = -1;//����ǰ�м����ո� ���������������һ��
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
	//���ص�һ���ɼ��ַ�ǰ�Ŀո���
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

//Ŀ¼(Catalog)�ṹ
struct CatalogNode {
	JCE::String name;
	//(set�ڵ�Ԫ���������)
	std::set<JCE::String> subFolder;//���ļ���������
	std::set<JCE::String> file;//�ļ�������
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
	//����������ȡһ�����ִ��ڶ���name��   �ļ���������true  �ļ�������false
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
	//������ļ����ڵ���������(����������, root���ǰ�Ŀո�����)
	void output(AvlTree<CatalogNode> *t, int blankCnt) {
		static CatalogNode tmp;
		print(blankCnt);
		//������ļ���
		for (auto i = subFolder.begin(); i != subFolder.end(); ++i) {
			tmp.name = *i;
			t->find(tmp)->Data.output(t, blankCnt + 2);
		}//����ļ�name
		for (auto i = file.begin(); i != file.end(); ++i) {
			for (int j = 0; j < blankCnt + 2; ++j)
				printf(" ");
			puts(&(*i)[0]);
		}
	}
};

struct Accounts {
	using myInt = int;
	myInt account;//unsigned �޷���ȫӳ��10λ�� ���г�ͻ
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
	//δ֪�ط�����Ĭ�Ϲ���
	Accounts() {}
	//������
	Accounts(myInt a, JCE::String &pa) {
		account = a, password = pa;
	}
	//������
	Accounts(myInt a) {
		account = a;
	}
	myInt hash()const {
		return account % 70000;//�ƺ���������������С���ʱЧ�����
	}
	/*
	map<JCE::String, JCE::String>
	0	sample ȫ��5�������Ϣ											����ȷ	2 ms	252KB
	1	����ȫ���������룬����ȫ��16λ									����ȷ	715 ms	19760KB
	2	N��Lָ���һ�룬��������ʺ������ȡ�����½硣���������ȡ�����½�	����ȷ	669 ms	10576KB

	HashAvlTable<Acounts> t = HashAvlTable<Acounts>(100000)
	0	sample ȫ��5�������Ϣ											����ȷ	7.6 ms	8012KB
	1	����ȫ���������룬����ȫ��16λ									����ȷ	842 ms	21340KB
	2	N��Lָ���һ�룬��������ʺ������ȡ�����½硣���������ȡ�����½�	����ȷ	601 ms	15432KB

	AvlTree<Acounts>
	0	sample ȫ��5�������Ϣ											����ȷ	2 ms	248KB
	1	����ȫ���������룬����ȫ��16λ									���г�ʱ	0 ms	0KB  > 1200ms
	2	N��Lָ���һ�룬��������ʺ������ȡ�����½硣���������ȡ�����½�	���г�ʱ	0 ms	0KB
	*/
};

struct Olympic {
	int id = -1, gold = 0, medal = 0, population = 0;
	//�漰double�Ƚ� �ظ�Ԫ��
	//double data_[5];//0ID 1���� 2���� 3�˾����� 4�˾�����(per capita)
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

//������score �����id
struct Student {
	char id[14];//����(13λ������)
	int score = 0;//�÷֣�Ϊ[0,100]�����ڵ�������
	int place = 0, rank = 1;//������ ����
	int localRank = 1;//�ڸÿ��������
	Student() {
		memset(id, 0, 14 * sizeof(char));
	}
	Student(Student const&rhs) {
		*this = rhs;
	}
	~Student() {}
	//������score �������ʱ���μ��Ӵ�С����
	//RE_ORDER�ɼ��Ըߵ��� id��С���� (����<ʱid�� > ����ʵ�����Ǵ�С ��������RE_ORDER ���С����)
	bool operator<(Student const &rhs)const {
		/*�ȼ�д��
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
	//���š����������������š��ڸÿ����������
	void outPut() {
		printf("%s %d %d %d\n", id, rank, place, localRank);
	}
};

