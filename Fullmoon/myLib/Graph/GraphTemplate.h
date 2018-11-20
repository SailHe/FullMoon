#ifndef __GRAPHTEMPLATE_H
#define __GRAPHTEMPLATE_H

#include"../stdafx.h"
#include"../ExtendSpace.h"
#include"./GraphUtil.h"

/*

1.  ����ʵ�ֵ�ǰ�㷨�µĲ��ظ���ͼ, ʵ�ֶ���ɾ�����������, ��ɾ��������ۿ��ܻ�Ƚϸ�
	(ֻ���ڽӱ� * �ڽӱ�����ʵ�ֲſ��ܸ�Чɾ������ �����ּ��󽵵��˱ߵĻ�ȡЧ��)
2.  ֮ǰ���Ե���ƾ��ͼģ��, ��ģ���������ʵ��һ������, Ȼ��Ϳ��Ժܷ���ظ���һ�������Ӧ��Ȩֵ, 
	��û�뵽֮ǰ��Graph��ʵ����û��ʹ�ö����������, ��������һ���±�, ��, ����'����'Ҳֻ�����Ȩֵ
	(ע��, �Ǳߵ�Ȩֵ, �����������ϵ����: �����������һ����, һ���߾���һ��Ȩֵ), 
	����, �����Ҫ�ɶ���ʵʱ�����ҲӦ����ʵ�� ���������»ή��Ч��
3. ����: Ҫ���Ч��ɾ������, ���������Ѵ��ڵ�visited��������, �������ԭ��ֻ��dfsʱ���ڻ���, 
	ֻ�轫���������·������ʱ�Ķ�����Ч���жϼ���, �����������˸����ԵĶ�����(�������, ��Ч���), 
	������˵�����������ȡ�����������, ������dfsһ������Ҳ���ܷ��ʶ��,Ҳ����˵�����Ǻ��ʵ�, 
	ֻ������һ��visited��֮ǰʵ���ϵ�dfs��ʱ����, ��ɵ�һ������, ���еķ����ڲ���ǰ����Ҫ������һ����, 
	Ҫ��ʵ��dfsҲ��ͨ�õĻ�, ò��ʹ�����е�visitedҲ����õ�ѡ��, 
	��֮: ͼ���ú󶥵�ı䶯������Ӱ��Ƚϴ�: ������������ִ洢�ߵ�ͼ, �����ڽӱ�ʵ�ֻ�������еı߼���Ƿ���Ҫɾ����, 
	�����ڽӾ���ʵ�ֻ�ɾ��һ��, һ��, �ڴ��ƶ�Ҳ�ǱȽϴ��, ������������Ƕ�����Ч�������Ļ�, 
	���ڴ�(ָ�Ѵ���visited�������)��Ч�ʵ�Ӱ�춼����, ֻ������, һ�ǲ���ǰ���ж�, ������O(1)��, 
	���о������visited��Ū����, ������Ҫclearһ��, ������΢�ѵ�ʱ��O(V), ��ԶС��ɾ������Ŀ���: 
	�ڽӱ�ͼɾ������(��������*����ʵ��)��ô��Ҫ�ƶ�����O(V), ���������еı�O(E), ò������Ҳ�ᵽ��, 
	���ο����ʹ�õõ��Ļ�, ���clear����ȫ���Ա����.

	ע��, һ��ԭ��: ͼ���ú󶥵�����Ҫ���ױ仯, ֮�����ῼ��ʵ��ɾ������, 
	ʹ�ö�����Ч����invalid(V) ֻ��ʹ�䲻�������(���·, dfs, bfs)������ı䶥���ʵ������(������Ҫʵ��һ����ȡ��Ч����ķ���)

	11-19 Ŀǰ��ʵ��: ��Чָ(visited[v] == true)
	ʹgetEdgeList�ڶ�����Чʱ���ر���Ϊ0���б� (�˾�Ӱ�������е����·���㷨);
	dfsԭ����ʹ����visited;
	bfs���ڸõ���Чʱ�����õ�
*/


/*
�ڽӱ�ͼ ��ҪV��ͷָ�� + 2E����㣨ÿ���������2���򣩣���eС��V(V-1)/4ʱ��ʡ�ռ��
����V������e���ߵ��ڽӱ�ͼʱ�临�Ӷ�ΪO(V+e)
����һ������n������e���ߵ�����ͼ
�����ڽӱ��ʾ��n���������2e���߱���
����һ������n������e���ߵ�����ͼ
�����ڽӱ��ʾ��n���������e���߱���
���ͼ�бߵ���ĿԶԶС��n2����ϡ��ͼ���������ڽӱ��ʾ�����ڽӾ����ʾ��ʡ�ռ�;
���ͼ�бߵ���Ŀ�ӽ���n2,��������ͼ�ӽ���n*(n-1)��������ͼ,���ǵ��ڽӱ���Ҫ�������򣬲����ڽӾ����ʾ��Ϊ�ˡ�

�������Ƕ���e < V(V-1)/4ʱ����������� ���ߵ�ʵ�����������ߵľ�������(ǰ�߾��߿ռ临�Ӷ� ���߾���ʱ�临�Ӷ�)

����䶯Ƶ��������Ҫ��ʵ��

����(dfs bfs):(��ͼ��һ��Ŀ��)
	�����������:�ڽӾ���ͼ
	�����������:
		�߲���(��ɾ��)
			Ƶ��->�ڽ�mapͼ
			ż��->�ڽӱ�ͼ

tips: ���ĳ�ֹ�ϵ�±��ر�� ���Գ��Դ���û�й�ϵ��
	�������ڽӱ���������о������� ��ʵ�ڽӾ�����Բ��ÿ��ǵ�
	����ֻ�����ڽӱ�
	�߲���Ƶ������������� ֻ������ ���� ֻ�Ĳ�ɾ->mapʵ��
	����֮����������������->listʵ��
	(�����ٵ����Ƕ����������Ը���ֱ�ӵĽ���������4*4��ȫͨ����ͼ�ĵȼ۶���ͼ��������� ���ߵı�������Լ��3ms����)
	�����֪��ʹ�ó�����ôʹ��mapʵ�������Ե�ѡ��(����Ҫע��û���Ż��Ļ��������� �����ڽӾ���ͼ�Ĵ��ڼ�ֵ���ǵ���)
*/
//��������ͼ������ (�ߵĴ���������ʵ�� ��Ĵ����Լ���ķ������ж��ɽӿ���ʵ��)  Adjacent: �ڽӵ�; Adjoin: �ڽ�
//(GraphTemplate: ͼ������ͼ)�����Ҫ����ͼ ֻʹ�������ɾ����
template<class WeightType>
class GraphTemplate {
public:
	const static int INF = MAX_INT32 / 2;

	//����ļ�����
	typedef int VertexKey;
	//�ߵ�Ȩֵ����
	//typedef int WeightType;
	//������������
	class VertexValue {
		//VertexKey ID;
		char data = '0';
	};
	//Ȩ��(û�г�������Ϣ�Ĵ�Ȩֵ�ı�) ����ʱ����Ȩֵ���� ��Ȩֵ�߰�Ŀ���id����
	struct IndexEdge {
		VertexKey targetID;//�ߵĶ��󶥵�ID
		WeightType weight;//�����ڸ�ֵ�Ƴ��� ���ڸ�ֵ��Ȩֵ
		IndexEdge() {
			weight = INF, targetID = -1;
		}
		IndexEdge(VertexKey targetID, WeightType weight) :weight(weight), targetID(targetID) {}
		//������: Dijkstra�����ȶ����Ż� �����ʵ��
		bool operator<(IndexEdge const &rhs)const {
			return weight == rhs.weight ? targetID < rhs.targetID
				: weight < rhs.weight;
		}
		//Ϊ�˱���list�����·������ʱ�ĳ�ʼ��
		operator int()const {
			return weight;
		}
	};
	/*�����(��<=>ֱ�ӹ�ϵ)�Ķ���  tips: ownerID targetID��(Relation)��ϵӦ�ó���ownerID targetID��ͨ*/
	struct Edge :public IndexEdge {
		VertexKey ownerID;//�ߵ�ӵ����ID
		Edge(VertexKey ownerID, VertexKey targetID, WeightType weight)
			:IndexEdge(targetID, weight), ownerID(ownerID) {}
	};/*�����<ownerID, targetID>*/

	//���ڶ������ݵĴ�����δ��дvertexDataδ��ʼ��
	GraphTemplate(int vertexNum)
		:vertexNum(vertexNum), visited(vertexNum, false) {
		edgeNum = 0;
	}
	virtual ~GraphTemplate() {
	}

	void resize(size_t newSize) {
		resizeEdge(newSize);
		vertexData.resize(newSize);
		visited.resize(newSize, false);
		//destId = -1;
		if (newSize < vertexNum) {
			//���¼������
			edgeNum = 0;
			negativeWeightCount = 0;
			FOR(i, 0, newSize) {
				auto listEdges = getEdgeList(i);
				edgeNum += listEdges.size();
				FOR_ALL_OBJECT(listEdges) {
					if (element.weight < 0) {
						++negativeWeightCount;
					}
				}
			}
		}
		else {
			//Do nothing
		}
		vertexNum = newSize;//resizeVertex
	}
	//���� �����
	void insertEdgeUndirected(Edge &e) {
		insertEdge(e);
		std::swap(e.ownerID, e.targetID);
		insertEdge(e);
	}
	//ɾ�� �����
	void deleteEdgeUndirected(VertexKey ownerID, VertexKey targetID) {
		deleteEdge(ownerID, targetID);
		deleteEdge(targetID, ownerID);
	}
	//����ʽ����
	virtual void insertEdge(Edge const &e) = 0;//���麯��
	virtual void deleteEdge(VertexKey ownerID, VertexKey targetID) = 0;
	//ɾ������vID������������б�
	//virtual void deleteVertex(VertexKey vID) = 0;
	//�����ڱ߽��ߵ�Ȩֵ������� ����true; ���������ȨֵΪ����� ����false
	virtual bool queryEdge(Edge &e) = 0;

	//(����)����������� ���ش���ʼ����startId�����յ�destId��·����
	virtual int dfs(VertexKey startId) = 0;

	//������: (����)����������� unweighted(��Ȩͼ�ĵ�Դ���·)
	//PS: �㷨�ڲ�����Ȩ��Ϊ1��˵ó��ľ���ʵ�����ǲ���  ��Ȼһ��ʵ�����бߵ�Ȩ����ȵ�ͼҲ����ʹ����Ȩͼ�ķ���
	//    dist������㵽���пɵ�����Ĳ���(origin-1step-2step) ���������Ų���(��Ȩͼ����̾� 2018-7-17)
	//    BFS��һ��äĿ��Ѱ�� ����ʹ�þ��鷨�����㷨(����ʽ����) ����������ʽ���·���㷨����Դ
	//Ч��1: ����destId->���ټ����Ƿ��ִܵ�ĳ������ ͬʱ��������Ų���(���ǵ�Ȩͼ�����µ����·��)
	//		�ŵ�: �����޸���Ȩͼ��ʵ��Ȩֵ
	//		�ж���ʽ: ʹ�� dist����� �� Ŀ������ͨ�Խ����ж�
	//Ч��2:  �ж�����ͼ����ͨ�� ����ֵ����ӵĽ����Ŀ ��δ����destIdʱ��������
	//		�ж���ʽ: ��ͨͼ�ķ���ֵӦ���������
	int bfs(VertexKey origin, JCE::ArrayList<WeightType> &dist, JCE::ArrayList<VertexKey> &predecessor) {
		static std::queue<VertexKey> buffer;
		dist.assign(vertexNum, -1);
		predecessor.assign(vertexNum, -1);
		dist[origin] = 0;
		buffer.push(origin);
		int cnt = 0;

		while (!buffer.empty()) {
			origin = buffer.front();
			buffer.pop();
			customVisit(origin);
			//ÿ���ɵ�������������������һ��
			++cnt;
			if (origin == destId) {
				//���û�������Ŀ�ĵ��㷨�ڼ������Ŀ�ĵ��·��ʱ����� û����ʱӦ��֤��Ϊ��Чֵ-1
				break;
			}
			//visited[origin] = true;//���������ǰdist==-1��ʾδ����, ���滻��visited
			auto &edges = getEdgeList(origin);
			FOR_ALL_OBJECT(edges) {
				if (dist[element.targetID] == -1) {//��δ�����ʹ�
					dist[element.targetID] = dist[origin] + 1;//�������
					predecessor[element.targetID] = origin;//·����¼
					buffer.push(element.targetID);
				}
			}
			//visited[origin] = false;//bfs�޷�����
		}
		return cnt;
	}

	/*
	��Դ���·������: ������ʼ�㵽���е�����·��
	(���·�����Ʒ�������, ·����������, ��ʼԭ��)
	predecessor���ڴ洢·����Ϣ:
	predecessor[v] == infinitTemp �����ڴ�origin->v�����·��
	previousID(ǰ�� tips: ���ó���ǰһ�� ǰһ��ʱ������˼ǿһЩ ǰ���ռ�����˼��ǿ)
	�û�����·���������ⲿ�Զ�����ʶ�������(�ṩ���ʽӿ�? ���Ƕ��û�����?)

	��Ȩ�߲���ͼ:
	gg->insertEdge({ 0, 1, 2 });
	gg->insertEdge({ 0, 2, 3 });
	gg->insertEdge({ 2, 1, -2 });
	gg->insertEdge({ 2, 1, -2 });//��������Ǹ�Ȩ������ͼ
	�������
	�ɳڲ���: ��ָ����ÿ������v��V��������һ������d[v]������������Դ��s��v�����·����Ȩֵ���Ͻ磬
	��Ϊ���·�����ƣ�shortest-path estimate����

	������ɹ�(�޸�Ȩ��)����true
	*/
	bool shortestPath(VertexKey startId, JCE::ArrayList<WeightType> &dist, JCE::ArrayList<VertexKey> &path) {
		if (negativeWeightCount == 0) {
			return Dijkstra(startId, dist, path);
		}
		else {
			return SPFA(startId, dist, path);
		}
	}
	/*��Դ���·����*/
	bool multisourceShortestPath(JCE::ArrayList<JCE::ArrayList<WeightType>> &dists, JCE::ArrayList<JCE::ArrayList<VertexKey>> &paths) {
		return FloydWarshall(dists, paths);
	}

	//����Ŀ�궥��(ID) ����һ�������ڵĶ�����<==>δ����Ŀ�Ķ���
	void setDestVertex(VertexKey vertexID) {
		destId = vertexID;
	}
	template<class Fun>
	void setCustomVisit(Fun const &customVisit) {
		this->customVisit = customVisit;
	}
	//���ر���
	size_t getEdgeNum() const {
		return edgeNum;
	}
	//���ض�����
	size_t getVertexNum() const {
		return vertexNum;
	}
	// ʹ����v��Ч
	void invalidVertex(VertexKey v) {
		visited[v] = true;
	}
	// ʹ����v��Ч
	void validVertex(VertexKey v) {
		visited[v] = false;
	}
protected:

	//��2017֮�±����ͨ�� ��ʵ�����������ǲ���ͨ����(�ж�������޸�) -> �ɰ汾�ı������ͨ�� �߰汾��һ��(���ܸ���������)
	//typedef list<IndexEdge const> EdgesType;
	//typedef JCE::LinkedList<IndexEdge> EdgesType;
	using EdgesType = JCE::LinkedList<IndexEdge>;
	//typedef EdgesType::iterator EdgesTypeIterator;
	//typedef std::list<IndexEdge>::iterator EdgesTypeIterator;
	//�������� ���������ݿ��Բ��ó��� ���������������Ķ� ����ֱ����map<VertexValue>���
	JCE::ArrayList<VertexValue> vertexData;
	//������
	size_t vertexNum;
	//���� V�������������ȫͼ����=V(V-1)/2
	int edgeNum;
	//��Ȩ�߸��� (����һ����Ȩ��++ ɾ��һ��-- ����Ϊ0ʱ���·�������dij���� �������spfa)
	int negativeWeightCount = 0;

	JCE::ArrayList<bool> visited;
	VertexKey destId = -1;//bdfsĿ��� ��ʼ������Ŀ���(��ζ��: dfs������� bfs�ó���·���ǵ����ж����)
		//�Զ������
	void(*customVisit)(VertexKey) = [](VertexKey v) {
		//printf("���ʽ��%d\n", v);//������
	};
	/*
		0.ÿ�δ�Q��delete_min��O(V)��Ԫ��v�ᱻɾ��
		ÿ����չ������v�бߵĶ������̾���O(V) ����Ϊ�ڽӱ����O(E)
		�㷨����QΪ��ʱֹͣ �������V����չ����
		ʱ�临�Ӷ�O(V^2)
		1.����q���黻Ϊ����set
		v = popMin��O(1)
		˼·1 =����ʣ�µ�Ԫ��w O(V-delCount) ƽ��(V/2)
		+��ѯ�Ƿ��б�<v, w>O(1)
		˼·2 =����v�����б�e O(E)(EΪͼ�����ж���ıߵ�ƽ������)
		+��ѯ�Ƿ��ڼ�����O(1)

		ÿ��popһ��ֱ������Ϊ�չ�V��
		ǰ���ܵ�ʱ����ȷ���� V*(V/2)
		���ǵ��ߵ�ƽ������E < (V/2)ʱ��һ�����Ը���һЩ
		˼·1ʵ��: �ڽӾ���� set�ǿ�ɾ����СԪ�ص��˻�BST(ɾ�����õ���), ���ı���O(V)������>1 ���ʵ��Ч����O(V^2)���
		˼·2ʵ��: �ڽӱ��   set����С��
	*/
	/*
		���ȶ���(��)�Ż���(A Star��������������)
		https://zh.wikipedia.org/wiki/%E6%88%B4%E5%85%8B%E6%96%AF%E7%89%B9%E6%8B%89%E7%AE%97%E6%B3%95
		Ŀǰ��֪������ �Ǹ�Ȩ����ͼ��Դ���·���㷨: �Ͽ�˹���㷨
		dijkstra�㷨��ȷ��ǰ����: ����·�����ɶ̵�·�����������ġ�(��˲��ܼ��㸺Ȩ)
	*/
	bool Dijkstra(VertexKey origin, JCE::ArrayList<WeightType> &dist, JCE::ArrayList<VertexKey> &predecessor) {
		//��̾������dist���ȶ��� ÿ���������һ�� �ɳ�һ��
		static std::priority_queue<IndexEdge> q;
		static VertexKey v = -1;
		dist.assign(vertexNum, INF);
		predecessor.assign(vertexNum, -1);
		dist[origin] = 0;//��֪ԭ�㵽�������̾���Ϊ0
		//�ʼq=����V(��ʵ����ÿ����Ҫ��ֻ����Сֵ ���ֻ��Ҫ������֪�ķ�������̾�����ƹ��ƵĶ��������)
		q.push({ origin, dist[origin] });//{targetID, weight}

		//�����㷨
		while (!q.empty()) {
			v = q.top().targetID; q.pop();
			auto &edges = getEdgeList(v);
			//��ͼ����v������ÿ����<v, w>�Ķ���w������չ�ɳڲ���
			FOR_ALL_OBJECT(edges) {
				if (visited[element.targetID]) {
					// ��������Ч�����йصı�
					continue;
				}
				//�ɳڲ���: ���ɳڹ��Ķ��㲻�ᱻ���ɳڵ��Ż� ������ֻ�����Ǹ�Ȩ�ߵ����
				if (dist[v] + element.weight < dist[element.targetID]) {
					if (element.weight < 0)
						return false;//����:�и�Ȩ��
					dist[element.targetID] = dist[v] + element.weight;//����w�����·���������
					predecessor[element.targetID] = v;//����w�����·��ǰ�����
					q.emplace(element.targetID, dist[element.targetID]);
				}
			}
			/*//mi zhi
			if (v == this->destId){
				while (!q.empty()){
					q.pop();
				}
			}*/
		}
		return true;
	}
	/*
		https://zh.wikipedia.org/wiki/%E8%B4%9D%E5%B0%94%E6%9B%BC-%E7%A6%8F%E7%89%B9%E7%AE%97%E6%B3%95#%E9%98%9F%E5%88%97%E4%BC%98%E5%8C%96
		�������ŵ�Short Path Fast Agritem
		(�����Ż���Bellman�CFord algorithm)
		SPFA�㷨�ڸ���Ȩͼ�Ͽ�����ȫȡ��Bellman-ford�㷨��������ϡ��ͼ��Ҳ�������á�
		*/
	bool SPFA(VertexKey origin, JCE::ArrayList<WeightType> &dist, JCE::ArrayList<VertexKey> &predecessor) {
		static JCE::ArrayList<int> countEnQ;//��¼������Ӵ����������жϸ�Ȩ��·
		static JCE::ArrayList<bool> existInQ;//��־���飬�ж��Ƿ��ڶ�����
		static JCE::ArrayList<int> stepCount;//��¼����
		static JCE::LinkedList<VertexKey> q;//�������

		countEnQ.assign(vertexNum, 0);
		existInQ.assign(vertexNum, false);
		stepCount.assign(vertexNum, 0);
		dist.assign(vertexNum, INF);//��ʼ������Ϊ�����
		predecessor.assign(vertexNum, -1);//��ʼ��·��Ϊδ֪

		countEnQ[origin] = 1;
		existInQ[origin] = true;
		//��֪��ԭ����̾���Ϊ0 ����ȫΪ�����
		dist[origin] = 0;
		q.push_back(origin);

		while (!q.empty()) {
			auto v = q.front();
			q.pop_front();
			existInQ[v] = false;
			auto &edges = getEdgeList(v);
			FOR_ALL_OBJECT(edges) {
				//�÷����Ż��Ķ���v�������еı�<v, w>��Ӧ�Ķ���w�����ɳڲ���
				if (element.weight + dist[v] <= dist[element.targetID]) {
					/* ���ָ��ŵ�Ԫ��w: �����Ż� || �Ⱦ���Ĳ����Ż� */
					if (element.weight + dist[v] < dist[element.targetID] || stepCount[element.targetID] > stepCount[v] + 1) {

						dist[element.targetID] = element.weight + dist[v];
						predecessor[element.targetID] = v;//����ǰ�����
						stepCount[element.targetID] = stepCount[v] + 1;
						if (++countEnQ[element.targetID] == vertexNum) {
							return false;//��Ȩ��·
						}
						/*
						(�п��������µ��Ż� <==> ���·������ֵ��������)��Ԫ��w
						�����ڶ����е�w���
						*/
						if (!existInQ[element.targetID]) {
							//���зǿ������ڶ���(SLF)
							if (!q.empty() && dist[element.targetID] < dist[q.front()])
								q.push_front(element.targetID);//���ڶ���
							else
								q.push_back(element.targetID);//������ڶ�β
							existInQ[element.targetID] = true;
						}
					}
				}
			}
		}
		return true;
	}
	/*
		https://zh.wikipedia.org/zh-hans/Floyd-Warshall%E7%AE%97%E6%B3%95
		������ȷ��������ͼ��Ȩ�������ɴ��ڸ�Ȩ��·���������������·�����⣬ͬʱҲ�����ڼ�������ͼ�Ĵ��ݱհ�.
		ʱ�临�Ӷ�O(V^3)�ռ临�Ӷ�ΪO(V^2)
		*/
	bool FloydWarshall(JCE::ArrayList<JCE::ArrayList<WeightType>> &dists, JCE::ArrayList<JCE::ArrayList<VertexKey>> &predecessors) {

		dists.resize(vertexNum);
		predecessors.resize(vertexNum);
		FOR(i, 0, vertexNum) {
			dists[i].assign(vertexNum, INF);
			predecessors[i].assign(vertexNum, -1);
			auto &edges = getEdgeList(i);
			FOR_ALL_OBJECT(edges) {
				//��Ȩ�س�ʼ��dist û�бߵĳ�ʼ��ΪINF
				dists[i][element.targetID] = element.weight;
				//��ʼ��ǰ�� û�бߵĶ���ǰ��Ϊ-1
				predecessors[i][element.targetID] = i;
			}
			dists[i][i] = 0;//���Լ��ľ������Ϊ0
			predecessors[i][i] = -1;//�Լ�û��ǰ��
		}

		FOR(k, 0, vertexNum) {
			FOR(i, 0, vertexNum) {
				FOR(j, 0, vertexNum) {
					if (dists[i][k] + dists[k][j] < dists[i][j]) {
						if (i == j && dists[i][j] < 0) {
							//���ڸ�ֵȦ: ������ȷ���
							return false;
						}
						else {
							/*
							�и�Ȩʱ: -1 + MAX_INT32 < MAX_INT32
							����������жϼ���dists[i][j] < INF && �ᵼ������������ΪMAX_INT32�ı��޷��ɳ�
							��������ϼ����Ƿ��������
							*/
							if (dists[i][k] != INF && dists[k][j] != INF) {
								dists[i][j] = dists[i][k] + dists[k][j];
								predecessors[i][j] = k;
							}
							else {
								//MAX_INT32 + MAX_INT32 = MAX_INT32;Do nothing
							}

						}
					}
				}
			}
		}

		return true;//�㷨ִ�����
	}
private:
	//��ȡ��listBuffer
	virtual EdgesType const &getEdgeList(size_t originNum) = 0;
	//���๹���ʱ��Ӧ��Ԥ��>2���Ŀռ�
	virtual void resizeEdge(size_t newSize) = 0;
};

//�ڽӾ���ͼ
template<class WeightType>
class AdjacentMatrixGraphTemplate : public GraphTemplate<WeightType> {
	//ģ���в���ֱ�ӷ��ʸ����Ա
	using GraphTemplate<WeightType>::VertexKey;
	using GraphTemplate<WeightType>::INF;
	using GraphTemplate<WeightType>::Edge;
	using GraphTemplate<WeightType>::negativeWeightCount;
	using GraphTemplate<WeightType>::edgeNum;
	using GraphTemplate<WeightType>::EdgesType;
	using GraphTemplate<WeightType>::vertexNum;
	using GraphTemplate<WeightType>::destId;
	using GraphTemplate<WeightType>::customVisit;
	using GraphTemplate<WeightType>::visited;

	//�ڽӾ���
	JCE::ArrayList<JCE::ArrayList<WeightType>> edgeData;
	//��ownerID��targetID֮�����ֱ�ӹ�ϵ(��)(��w�Ƿ�v���ڽӵ�) ����true
	inline bool existEdge(VertexKey v, VertexKey w) {
		return edgeData[v][w] < INF;//�����ڵı߱���ʼ��ΪINF
	}
	void resizeEdge(size_t newSize)override {
		edgeData.resize(newSize);
		FOR(i, 0, newSize) {
			edgeData[i].resize(newSize, INF);//�ط�����ڴ汻��ΪINF
		}
	}
public:
	AdjacentMatrixGraphTemplate(int vertexNum)
		:GraphTemplate<WeightType>(vertexNum) {
		edgeData.reserve(2 * vertexNum + 50);
		edgeData.resize(vertexNum);
		FOR(i, 0, vertexNum) {
			edgeData[i].reserve(2 * vertexNum + 50);
			edgeData[i].assign(vertexNum, INF);
		}
	}
	virtual ~AdjacentMatrixGraphTemplate() {
	}
	bool queryEdge(Edge &e)override {
		if (existEdge(e.ownerID, e.targetID)) {
			e.weight = edgeData[e.ownerID][e.targetID];
			return true;
		}
		else {
			e.weight = INF;
			return false;
		}
	}
	void insertEdge(Edge const &e)override {
		if (existEdge(e.ownerID, e.targetID)) {
			//�Ѵ���: ���������� ֱ�Ӹ��¾���

			//ȷ�ϸ��¶Է�Ȩ��������Ӱ��
			if (edgeData[e.ownerID][e.targetID] < 0) {
				//����
				negativeWeightCount -= e.weight < 0 ? 0 : 1;
			}
			else {
				//����
				negativeWeightCount += e.weight < 0 ? 1 : 0;
			}
		}
		else {
			negativeWeightCount += e.weight < 0 ? 1 : 0;
			++edgeNum;
		}
		edgeData[e.ownerID][e.targetID] = e.weight;
	}
	void deleteEdge(VertexKey ownerID, VertexKey targetID)override {
		//ֻҪ�б߾�ɾ��
		if (existEdge(ownerID, targetID)) {
			negativeWeightCount -= edgeData[ownerID][targetID] < 0 ? 1 : 0;
			edgeData[ownerID][targetID] = INF;
			--edgeNum;
		}
		else {
			//������;
		}
	}
	EdgesType const &getEdgeList(size_t originNum)override {
		//�߶�ȡ����
		static EdgesType edgesBuffer;
		edgesBuffer.clear();
		if (visited[originNum]) {
			//do nothing
		}
		else {
			FOR(i, 0, vertexNum) {
				if (existEdge(originNum, i)) {
					edgesBuffer.emplace_back(i, edgeData[originNum][i]);
				}
			}
		}
		return edgesBuffer;
	}

	/*
	���֧�ݹ��㷨: past -> present -> next;
	present���� -> ���� -> ���� -> �ݹ�
	����ͼ��present�붥��Ĵ���˳���й�)����ͼ��present��λ���й��Ǳ�����ʵ� ��֧�������ٴγ��ֵ����Ծ���
	�����ʵ�������������һ��ʵ����Щ���� ������������visit��ʼ���� �����ڼ�������չ
	(����Ҫʹ�ÿ��Կ�����DIR84�м�һ��{0, 0}����ʹ��do{}while();) ��ͨ��DIR84"����"��һ�η��ʾ�û��Ҫ��ԯ������

	ͼ��dfs���ȼ��ڻ�������:
		1.ͼ�ķ����������޶���(��ϵ: ������Ҫ��������������) ��˴���Ĳ���ֻ����{ǰ��, ��ʱ, ���}�а�����ʱ������
		 ǰ������ʱ��������޹�, ���ǲ����ĺ���Ҳ��depth�޹�
		 �����ڳ�ʼ��������û��ⲿ�������Ҫ���о���, ���ֻ��ѡ����ʱ���������
		2.�ڹ�ϵ�����۶������������Ҫ��ȷ����: �ڽ��a�������ǰa�����ٱ�a�ĺ�̷���, ��˶���ͼ��Ҫ�����ʿ���
		 �ڲ����Ƿ��ʿ���ʵ�ֵ�ǰ����(һ����visited[])����ȷ: ��a�����й�ϵ�������ʱӦ���ܱ�����ƽ�н�����
		 ���Ƿ��ʿ��Ƶ�˳������� ��Ϊ����->����->��Ϊδ����
		 (˳��һ��: ��������ֻ��Ҫ�ڴﵽҪ��ʱ�������Ž��������, depth�����һ��ķ��ʿ���, ����û�н�����Ҳ����Ҫ)
		3.������ǰ���� ���ʿ���������ʵ��: ������ʱ���; �����̽��
		 ��ѡ����� ��ô���Դ�����ʼ����״̬(�⽫���𼫴�Ĳ��������)
	���ǽ�����: ���Ƕ���ͼ ����dfs���������ʵ��
	*/
	int dfs(VertexKey presentId)override {
		customVisit(presentId);

		if (presentId == destId) {
			return 1;
		}
		else {
			visited[presentId] = true;
			int count = 0;
			FOR(nextId, 0, vertexNum) {
				if (edgeData[presentId][nextId] < INF //���ڹ�ϵ
					&& !visited[nextId]) {
					//visited[nextId] = true;//��ͨ����, �Ժ�̽����зÿ�
					count += dfs(nextId);
					//visited[nextId] = false;
				}
			}
			visited[presentId] = false;//dfs�Ļ���
			return count;
		}
	}
	template<class Fun>
	//��������ʵ��
	int dfsTest(VertexKey presentId, Fun visit) {
		customVisit(presentId);

		if (presentId == destId) {
			return 1;
		}
		else {
			visited[presentId] = true;
			int count = 0;
			FOR(nextId, 0, vertexNum) {
				if (edgeData[presentId][nextId] < INF //���ڹ�ϵ
					&& !visited[nextId]) {
					//visited[nextId] = true;//�Ժ�̽����зÿ�
					count += dfsTest(nextId, visit);
					//visited[nextId] = false;
				}
			}
			visit(presentId);
			visited[presentId] = false;//dfs�Ļ���
			return count;
		}
	}
	/* �ڽӱ�洢 - ���������㷨: ��ͼ�в����ڻ�· �����ɹ� ����true*/
	bool topologySort(JCE::ArrayList<VertexKey> &topOrderBuffer) {
		/* ��GraphTemplate������������,  topOrderBuffer[]˳��洢�����Ķ����±� */
		topOrderBuffer.resize(vertexNum);
		JCE::ArrayList<VertexKey> indegree(vertexNum);
		//queue<VertexKey> q;
		//��֤��ͬ������������������С��
		std::priority_queue<VertexKey, JCE::ArrayList<VertexKey>, JCE::greater<VertexKey>> q;
	}
};

//�ڽӱ�ͼ(listʵ��)
template<class WeightType>
class AdjacentListGraphTemplate : public GraphTemplate<WeightType> {
	using GraphTemplate<WeightType>::VertexKey;
	using GraphTemplate<WeightType>::INF;
	using GraphTemplate<WeightType>::Edge;
	using GraphTemplate<WeightType>::negativeWeightCount;
	using GraphTemplate<WeightType>::edgeNum;
	using GraphTemplate<WeightType>::EdgesType;
	using GraphTemplate<WeightType>::vertexNum;
	using GraphTemplate<WeightType>::destId;
	using GraphTemplate<WeightType>::customVisit;
	using GraphTemplate<WeightType>::visited;

	using GraphTemplate<WeightType>::IndexEdge;
	/*
	�ڽӱ�ͼ(mapʵ��)
	// [��һ�ߵ�id][�ߵ�Ȩֵ]
	typedef map<VertexKey, WeightType> EdgesType;
	//(map�����ͻ��Hash�� map��list�Ļ����ϸ����˱߲��� ����������)
	EdgesType edgeData[MaxVertexNum];
	һ����4*4��dfs�������е�·������û��ʲôʵ��Ӧ�ó���
	��������ʵ����Ϊ����Գ���ͼ��list��ɾ���ѵ����
	��ʵ���ϴ�ʵ�ֱ��ھطѿռ�(����4����) �����ھ�ʵ�ֵĳ���ͼ������Ч������(�������)
	���Ǿ�ֻ��ϡ��ͼ���е��� ������ɾ��ʡ�µ��ǵ�ʱ�������������ʱ�����������
	����һ������ʱ��ߵ���������ͳ��
	*/
	//list��������map
	//�ڽӱ�(��������ͻ��Hash��)
	JCE::ArrayList<EdgesType> edgeData;
	//GraphTemplate<WeightType>::EdgesType::iterator ��ôд�ǲ��е� ����
	auto &listFind(EdgesType &edgssList, VertexKey keyID) {
		static auto it = edgssList.begin();
		it = find_if(edgssList.begin(), edgssList.end(), [&](IndexEdge const &i) {
			return keyID == i.targetID;
		});
		return it;
	}
	void resizeEdge(size_t newSize) {
		edgeData.resize(newSize);
	}
public:
	AdjacentListGraphTemplate(int vertexNum) :GraphTemplate<WeightType>(vertexNum) {
		edgeData.reserve(2 * vertexNum + 50);
		edgeData.resize(vertexNum);
	}
	virtual ~AdjacentListGraphTemplate() {
	}

	bool queryEdge(Edge &e)override {
		auto it = listFind(edgeData[e.ownerID], e.targetID);
		if (it == edgeData[e.ownerID].end()) {
			e.weight = INF;
			return false;
		}
		else {
			e.weight = it->weight;
			return true;
		}
	}
	void insertEdge(Edge const &e)override {
		auto it = listFind(edgeData[e.ownerID], e.targetID);
		if (it == edgeData[e.ownerID].end()) {
			edgeData[e.ownerID].push_back(e);
			++edgeNum;
			negativeWeightCount += e.weight < 0 ? 1 : 0;
		}
		else {
			//ȷ�ϸ��¶Է�Ȩ��������Ӱ��
			if (it->weight < 0) {
				//����
				negativeWeightCount -= e.weight < 0 ? 0 : 1;
			}
			else {
				//����
				negativeWeightCount += e.weight < 0 ? 1 : 0;
			}
			//�Ѵ���: ���������� ֱ�Ӹ��¾���
			it->weight = e.weight;
		}
	}
	void deleteEdge(VertexKey ownerID, VertexKey targetID)override {
		auto it = listFind(edgeData[ownerID], targetID);
		if (it == edgeData[ownerID].end()) {
			//do nothig
		}
		else {
			negativeWeightCount -= it->weight < 0 ? 1 : 0;
			edgeData[ownerID].erase(it);
			--edgeNum;
		}
	}
	//void deleteVertex(VertexKey vID) override {}

	EdgesType const &getEdgeList(size_t originNum)override {
		// ֱ�ӷ�����ʱ�����������ֵ�����: ��ʹû�д���������ʱ���������, ���ص�list sizeȴΪ0
		static EdgesType EMPTY_LIST;
		return visited[originNum] ? EMPTY_LIST : edgeData[originNum];
	}
	int dfs(VertexKey presentId)override {
		customVisit(presentId);

		if (presentId == destId) {
			return 1;
		}
		else {
			visited[presentId] = true;
			int count = 0;
			//�ش��ڹ�ϵ
			FOR_ALL_OBJECT(edgeData[presentId]) {
				if (!visited[element.targetID]) {
					count += dfs(element.targetID);
				}
			}
			visited[presentId] = false;
			return count;
		}
	}

	/* �ڽӱ�洢 - ���������㷨: ��ͼ�в����ڻ�· �����ɹ� ����true*/
	bool topologySort(JCE::ArrayList<VertexKey> &topOrderBuffer) {
		/* ��GraphTemplate������������,  topOrderBuffer[]˳��洢�����Ķ����±� */
		topOrderBuffer.resize(vertexNum);
		JCE::ArrayList<VertexKey> indegree(vertexNum);
		//queue<VertexKey> q;
		//��֤��ͬ������������������С��
		std::priority_queue<VertexKey, JCE::ArrayList<VertexKey>, JCE::greater<VertexKey>> q;

		/* ����ͼ���õ�indegree[] edgeData.size()*/
		for (size_t v = 0; v < vertexNum; ++v) {
			//IndexEdge
			for (auto itW = edgeData[v].begin(); itW != edgeData[v].end(); ++itW) {
				++indegree[itW->targetID];/* �������<v, itW->targetID>�ۼ��յ����� */
			}
		}

		/* ���������Ϊ0�Ķ������� */
		for (size_t v = 0; v < vertexNum; ++v) {
			if (indegree[v] == 0) {
				q.push(v);
			}
		}

		/* ��������������� */
		int cnt = -1;
		while (!q.empty()) {
			/* ����һ�����Ϊ0�Ķ��� ���������� */
			VertexKey v = q.top();//q.front();
			q.pop();
			topOrderBuffer[++cnt] = v;
			/* ��v��ÿ���ڽӵ�itW->targetID(W->AdjV)*/
			for (auto itW = edgeData[v].begin(); itW != edgeData[v].end(); ++itW) {
				/* ��ɾ��vʹ��itW->targetID���Ϊ0 */
				if (--indegree[itW->targetID] == 0) {
					q.push(itW->targetID);/* ��ö������� */
				}
			}
		}

		return cnt == edgeData.size();
	}
};

#endif
