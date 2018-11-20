#ifndef __GRAPHTEMPLATE_H
#define __GRAPHTEMPLATE_H

#include"../stdafx.h"
#include"../ExtendSpace.h"
#include"./GraphUtil.h"

/*

1.  若想实现当前算法下的不重复建图, 实现顶点删除是最理想的, 但删除顶点代价可能会比较高
	(只有邻接表 * 邻接表这样实现才可能高效删除顶点 但这又极大降低了边的获取效率)
2.  之前尝试的是凭借图模板, 在模板参数类中实现一个集合, 然后就可以很方便地更改一个顶点对应的权值, 
	但没想到之前的Graph中实际上没有使用顶点这个类型, 而仅仅是一个下标, 即, 就算'泛型'也只能针对权值
	(注意, 是边的权值, 跟单个顶点关系不大: 两个顶点决定一条边, 一条边具有一个权值), 
	另外, 如果想要由顶点实时计算边也应该能实现 但这样恐怕会降低效率
3. 结论: 要想高效地删除顶点, 可以利用已存在的visited方法即可, 这个数组原本只在dfs时用于回溯, 
	只需将其用于最短路径计算时的顶点有效性判断即可, 但这样引起了该属性的二义性(访问与否, 有效与否), 
	不过话说回来这个名字取的是有问题的, 就算是dfs一个顶点也可能访问多次,也就是说这样是合适的, 
	只是这样一来visited由之前实际上的dfs临时变量, 变成的一个属性, 所有的方法在操作前都需要考虑这一属性, 
	要想实现dfs也能通用的话, 貌似使用已有的visited也是最好的选择, 
	总之: 图建好后顶点的变动带来的影响比较大: 如果是现在这种存储边的图, 对于邻接表实现会遍历所有的边检查是否需要删除边, 
	对于邻接矩阵实现会删除一行, 一列, 内存移动也是比较大的, 如果采用这种是顶点无效的做法的话, 
	对内存(指已存在visited的情况下)和效率的影响都不大, 只有两处, 一是操作前的判断, 这里是O(1)的, 
	还有就是如果visited被弄乱了, 可能需要clear一下, 这里略微费点时间O(V), 但远小于删除顶点的开销: 
	邻接表图删除顶点(若是数组*链表实现)那么需要移动数组O(V), 还需检查所有的边O(E), 貌似上面也提到了, 
	更何况如果使用得当的话, 这个clear是完全可以避免的.

	注意, 一个原则: 图建好后顶点数不要轻易变化, 之后或许会考虑实现删除顶点, 
	使用顶点无效方法invalid(V) 只会使其不参与计算(最短路, dfs, bfs)而不会改变顶点的实际数量(可能需要实现一个获取有效顶点的方法)

	11-19 目前的实现: 无效指(visited[v] == true)
	使getEdgeList在顶点无效时返回边数为0的列表 (此举影响了已有的最短路径算法);
	dfs原本就使用了visited;
	bfs会在该点无效时跳过该点
*/


/*
邻接表图 需要V个头指针 + 2E个结点（每个结点至少2个域），则e小于V(V-1)/4时是省空间的
对于V个顶点e条边的邻接表图时间复杂度为O(V+e)
对于一个具有n个顶点e条边的无向图
它的邻接表表示有n个顶点表结点2e个边表结点
对于一个具有n个顶点e条边的有向图
它的邻接表表示有n个顶点表结点e个边表结点
如果图中边的数目远远小于n2称作稀疏图，这是用邻接表表示比用邻接矩阵表示节省空间;
如果图中边的数目接近于n2,对于无向图接近于n*(n-1)称作稠密图,考虑到邻接表中要附加链域，采用邻接矩阵表示法为宜。

这里我们定义e < V(V-1)/4时称作边相对少 而边的实际数量称作边的绝对数量(前者决策空间复杂度 后者决策时间复杂度)

顶点变动频繁父类需要改实现

搜索(dfs bfs):(建图的一般目的)
	边相对数量多:邻接矩阵图
	边相对数量少:
		边操作(增删改)
			频繁->邻接map图
			偶尔->邻接表图

tips: 如果某种关系下边特别多 可以尝试储存没有关系的
	又由于邻接表的搜索具有绝对优势 其实邻接矩阵可以不用考虑的
	于是只讨论邻接表
	边操作频繁但是其操作是 只增不减 或是 只改不删->map实现
	但反之仅当边数量绝对少->list实现
	(绝对少到底是多少这里难以给出直接的结论至少在4*4的全通坐标图的等价顶点图这个例子中 两者的表现相差大约在3ms左右)
	如果不知道使用场景那么使用map实现是折衷的选择(但是要注意没开优化的话调试奇慢 所以邻接矩阵图的存在价值就是调试)
*/
//顶点有向图抽象类 (边的储存由子类实现 点的储存以及点的访问性判断由接口类实现)  Adjacent: 邻接的; Adjoin: 邻接
//(GraphTemplate: 图表；曲线图)如果需要无向图 只使用无向插删即可
template<class WeightType>
class GraphTemplate {
public:
	const static int INF = MAX_INT32 / 2;

	//顶点的键类型
	typedef int VertexKey;
	//边的权值类型
	//typedef int WeightType;
	//顶点数据类型
	class VertexValue {
		//VertexKey ID;
		char data = '0';
	};
	//权边(没有出发点信息的带权值的边) 排序时按照权值排序 等权值者按目标点id排序
	struct IndexEdge {
		VertexKey targetID;//边的对象顶点ID
		WeightType weight;//不存在负值称长度 存在负值称权值
		IndexEdge() {
			weight = INF, targetID = -1;
		}
		IndexEdge(VertexKey targetID, WeightType weight) :weight(weight), targetID(targetID) {}
		//可用于: Dijkstra的优先队列优化 跳表的实现
		bool operator<(IndexEdge const &rhs)const {
			return weight == rhs.weight ? targetID < rhs.targetID
				: weight < rhs.weight;
		}
		//为了便于list的最短路径计算时的初始化
		operator int()const {
			return weight;
		}
	};
	/*有向边(边<=>直接关系)的定义  tips: ownerID targetID有(Relation)关系应该称作ownerID targetID连通*/
	struct Edge :public IndexEdge {
		VertexKey ownerID;//边的拥有者ID
		Edge(VertexKey ownerID, VertexKey targetID, WeightType weight)
			:IndexEdge(targetID, weight), ownerID(ownerID) {}
	};/*有向边<ownerID, targetID>*/

	//存在顶点数据的代码暂未编写vertexData未初始化
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
			//重新计算边数
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
	//插入 无向边
	void insertEdgeUndirected(Edge &e) {
		insertEdge(e);
		std::swap(e.ownerID, e.targetID);
		insertEdge(e);
	}
	//删除 无向边
	void deleteEdgeUndirected(VertexKey ownerID, VertexKey targetID) {
		deleteEdge(ownerID, targetID);
		deleteEdge(targetID, ownerID);
	}
	//更新式插入
	virtual void insertEdge(Edge const &e) = 0;//纯虚函数
	virtual void deleteEdge(VertexKey ownerID, VertexKey targetID) = 0;
	//删除顶点vID及其关联的所有边
	//virtual void deleteVertex(VertexKey vID) = 0;
	//若存在边将边的权值赋予参数 返回true; 否则参数的权值为无穷大 返回false
	virtual bool queryEdge(Edge &e) = 0;

	//(先序)深度优先搜索 返回从起始顶点startId到达终点destId的路径数
	virtual int dfs(VertexKey startId) = 0;

	//方法名: (层序)广度优先搜索 unweighted(无权图的单源最短路)
	//PS: 算法内部所有权都为1因此得出的距离实际上是步数  当然一个实际所有边的权都相等的图也可以使用有权图的方法
	//    dist中是起点到所有可到达结点的步数(origin-1step-2step) 并且是最优步数(等权图的最短径 2018-7-17)
	//    BFS是一种盲目搜寻法 并不使用经验法则演算法(启发式搜索) 但其是启发式最短路径算法的来源
	//效果1: 设置destId->快速计算是否能抵达某个顶点 同时计算出最优步数(或是等权图意义下的最短路径)
	//		优点: 不用修改有权图的实际权值
	//		判定方式: 使用 dist对起点 与 目标点的连通性进行判断
	//效果2:  判断整张图的连通性 返回值是入队的结点数目 在未设置destId时才有意义
	//		判定方式: 连通图的返回值应与结点数相等
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
			//每个可到达结点理论上入队且入队一次
			++cnt;
			if (origin == destId) {
				//若用户设置了目的点算法在计算出到目的点的路径时便结束 没设置时应保证其为无效值-1
				break;
			}
			//visited[origin] = true;//在运算结束前dist==-1表示未访问, 其替换了visited
			auto &edges = getEdgeList(origin);
			FOR_ALL_OBJECT(edges) {
				if (dist[element.targetID] == -1) {//若未被访问过
					dist[element.targetID] = dist[origin] + 1;//距离更新
					predecessor[element.targetID] = origin;//路径记录
					buffer.push(element.targetID);
				}
			}
			//visited[origin] = false;//bfs无法回溯
		}
		return cnt;
	}

	/*
	单源最短路径问题: 计算起始点到所有点的最短路径
	(最短路径估计返回数组, 路径返回数组, 起始原点)
	predecessor用于存储路径信息:
	predecessor[v] == infinitTemp 不存在从origin->v的最短路径
	previousID(前驱 tips: 不该称作前一个 前一个时间上意思强一些 前驱空间上意思更强)
	用户可用路径序列在外部自定义访问顶点数据(提供访问接口? 或是对用户开放?)

	负权边测试图:
	gg->insertEdge({ 0, 1, 2 });
	gg->insertEdge({ 0, 2, 3 });
	gg->insertEdge({ 2, 1, -2 });
	gg->insertEdge({ 2, 1, -2 });//加入这句是负权环测试图
	概念解释
	松弛操作: 是指对于每个顶点v∈V，都设置一个属性d[v]，用来描述从源点s到v的最短路径上权值的上界，
	称为最短路径估计（shortest-path estimate）。

	若计算成功(无负权环)返回true
	*/
	bool shortestPath(VertexKey startId, JCE::ArrayList<WeightType> &dist, JCE::ArrayList<VertexKey> &path) {
		if (negativeWeightCount == 0) {
			return Dijkstra(startId, dist, path);
		}
		else {
			return SPFA(startId, dist, path);
		}
	}
	/*多源最短路问题*/
	bool multisourceShortestPath(JCE::ArrayList<JCE::ArrayList<WeightType>> &dists, JCE::ArrayList<JCE::ArrayList<VertexKey>> &paths) {
		return FloydWarshall(dists, paths);
	}

	//设置目标顶点(ID) 设置一个不存在的顶点编号<==>未设置目的顶点
	void setDestVertex(VertexKey vertexID) {
		destId = vertexID;
	}
	template<class Fun>
	void setCustomVisit(Fun const &customVisit) {
		this->customVisit = customVisit;
	}
	//返回边数
	size_t getEdgeNum() const {
		return edgeNum;
	}
	//返回顶点数
	size_t getVertexNum() const {
		return vertexNum;
	}
	// 使顶点v无效
	void invalidVertex(VertexKey v) {
		visited[v] = true;
	}
	// 使顶点v有效
	void validVertex(VertexKey v) {
		visited[v] = false;
	}
protected:

	//在2017之下编译会通过 但实际上这样子是不能通过的(有对其进行修改) -> 旧版本的编译可以通过 高版本不一定(功能更加完善了)
	//typedef list<IndexEdge const> EdgesType;
	//typedef JCE::LinkedList<IndexEdge> EdgesType;
	using EdgesType = JCE::LinkedList<IndexEdge>;
	//typedef EdgesType::iterator EdgesTypeIterator;
	//typedef std::list<IndexEdge>::iterator EdgesTypeIterator;
	//顶点数据 顶点无数据可以不用出现 若顶点数量经常改动 可以直接用map<VertexValue>替代
	JCE::ArrayList<VertexValue> vertexData;
	//顶点数
	size_t vertexNum;
	//边数 V个顶点的无向完全图边数=V(V-1)/2
	int edgeNum;
	//负权边个数 (插入一条负权边++ 删除一条-- 当其为0时最短路径会调用dij计算 否则调用spfa)
	int negativeWeightCount = 0;

	JCE::ArrayList<bool> visited;
	VertexKey destId = -1;//bdfs目标点 初始不存在目标点(意味着: dfs不会结束 bfs得出的路径是到所有顶点的)
		//自定义访问
	void(*customVisit)(VertexKey) = [](VertexKey v) {
		//printf("访问结点%d\n", v);//测试用
	};
	/*
		0.每次从Q中delete_min是O(V)该元素v会被删除
		每次拓展更新与v有边的顶点的最短距离O(V) 若换为邻接表会是O(E)
		算法会在Q为空时停止 共需进行V次拓展操作
		时间复杂度O(V^2)
		1.若将q数组换为集合set
		v = popMin是O(1)
		思路1 =遍历剩下的元素w O(V-delCount) 平均(V/2)
		+查询是否有边<v, w>O(1)
		思路2 =遍历v的所有边e O(E)(E为图中所有顶点的边的平均数量)
		+查询是否还在集合中O(1)

		每次pop一个直到集合为空共V次
		前者总的时间是确定的 V*(V/2)
		于是当边的平均数量E < (V/2)时后一个策略更优一些
		思路1实现: 邻接矩阵边 set是可删除最小元素的退化BST(删除后不用调整), 树的遍历O(V)但常数>1 因此实际效果与O(V^2)差不多
		思路2实现: 邻接表边   set是最小堆
	*/
	/*
		优先队列(堆)优化版(A Star的无启发函数版)
		https://zh.wikipedia.org/wiki/%E6%88%B4%E5%85%8B%E6%96%AF%E7%89%B9%E6%8B%89%E7%AE%97%E6%B3%95
		目前已知的最快的 非负权有向图单源最短路径算法: 迪科斯彻算法
		dijkstra算法正确的前提是: 长的路径是由短的路径派生而来的。(因此不能计算负权)
	*/
	bool Dijkstra(VertexKey origin, JCE::ArrayList<WeightType> &dist, JCE::ArrayList<VertexKey> &predecessor) {
		//最短距离估计dist优先队列 每个顶点入队一次 松弛一次
		static std::priority_queue<IndexEdge> q;
		static VertexKey v = -1;
		dist.assign(vertexNum, INF);
		predecessor.assign(vertexNum, -1);
		dist[origin] = 0;//已知原点到本身的最短距离为0
		//最开始q=所有V(但实际上每次需要的只是最小值 因此只需要加入已知的非无穷最短距离估计估计的顶点就行了)
		q.push({ origin, dist[origin] });//{targetID, weight}

		//核心算法
		while (!q.empty()) {
			v = q.top().targetID; q.pop();
			auto &edges = getEdgeList(v);
			//对图中由v出发的每条边<v, w>的顶点w进行拓展松弛操作
			FOR_ALL_OBJECT(edges) {
				if (visited[element.targetID]) {
					// 跳过与无效顶点有关的边
					continue;
				}
				//松弛操作: 先松弛过的顶点不会被后松弛的优化 若还能只可能是负权边的情况
				if (dist[v] + element.weight < dist[element.targetID]) {
					if (element.weight < 0)
						return false;//错误:有负权边
					dist[element.targetID] = dist[v] + element.weight;//更新w的最短路径距离估计
					predecessor[element.targetID] = v;//更新w的最短路径前驱结点
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
		步数最优的Short Path Fast Agritem
		(队列优化的Bellman–Ford algorithm)
		SPFA算法在负边权图上可以完全取代Bellman-ford算法，另外在稀疏图中也表现良好。
		*/
	bool SPFA(VertexKey origin, JCE::ArrayList<WeightType> &dist, JCE::ArrayList<VertexKey> &predecessor) {
		static JCE::ArrayList<int> countEnQ;//记录各点入队次数，用于判断负权回路
		static JCE::ArrayList<bool> existInQ;//标志数组，判断是否在队列中
		static JCE::ArrayList<int> stepCount;//记录步数
		static JCE::LinkedList<VertexKey> q;//处理队列

		countEnQ.assign(vertexNum, 0);
		existInQ.assign(vertexNum, false);
		stepCount.assign(vertexNum, 0);
		dist.assign(vertexNum, INF);//初始化距离为无穷大
		predecessor.assign(vertexNum, -1);//初始化路径为未知

		countEnQ[origin] = 1;
		existInQ[origin] = true;
		//已知到原点最短距离为0 其余全为无穷大
		dist[origin] = 0;
		q.push_back(origin);

		while (!q.empty()) {
			auto v = q.front();
			q.pop_front();
			existInQ[v] = false;
			auto &edges = getEdgeList(v);
			FOR_ALL_OBJECT(edges) {
				//用发生优化的顶点v对其所有的边<v, w>对应的顶点w进行松弛操作
				if (element.weight + dist[v] <= dist[element.targetID]) {
					/* 出现更优的元素w: 距离优化 || 等距离的步数优化 */
					if (element.weight + dist[v] < dist[element.targetID] || stepCount[element.targetID] > stepCount[v] + 1) {

						dist[element.targetID] = element.weight + dist[v];
						predecessor[element.targetID] = v;//更新前驱结点
						stepCount[element.targetID] = stepCount[v] + 1;
						if (++countEnQ[element.targetID] == vertexNum) {
							return false;//负权回路
						}
						/*
						(有可能引起新的优化 <==> 最短路径估计值有所调整)的元素w
						将不在队列中的w入队
						*/
						if (!existInQ[element.targetID]) {
							//队列非空且优于队首(SLF)
							if (!q.empty() && dist[element.targetID] < dist[q.front()])
								q.push_front(element.targetID);//放在队首
							else
								q.push_back(element.targetID);//否则放在队尾
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
		可以正确处理有向图或负权（但不可存在负权回路）任意两点间的最短路径问题，同时也被用于计算有向图的传递闭包.
		时间复杂度O(V^3)空间复杂度为O(V^2)
		*/
	bool FloydWarshall(JCE::ArrayList<JCE::ArrayList<WeightType>> &dists, JCE::ArrayList<JCE::ArrayList<VertexKey>> &predecessors) {

		dists.resize(vertexNum);
		predecessors.resize(vertexNum);
		FOR(i, 0, vertexNum) {
			dists[i].assign(vertexNum, INF);
			predecessors[i].assign(vertexNum, -1);
			auto &edges = getEdgeList(i);
			FOR_ALL_OBJECT(edges) {
				//用权重初始化dist 没有边的初始化为INF
				dists[i][element.targetID] = element.weight;
				//初始化前驱 没有边的顶点前驱为-1
				predecessors[i][element.targetID] = i;
			}
			dists[i][i] = 0;//到自己的距离估计为0
			predecessors[i][i] = -1;//自己没有前驱
		}

		FOR(k, 0, vertexNum) {
			FOR(i, 0, vertexNum) {
				FOR(j, 0, vertexNum) {
					if (dists[i][k] + dists[k][j] < dists[i][j]) {
						if (i == j && dists[i][j] < 0) {
							//存在负值圈: 不能正确解决
							return false;
						}
						else {
							/*
							有负权时: -1 + MAX_INT32 < MAX_INT32
							若在最外层判断加上dists[i][j] < INF && 会导致最初距离估计为MAX_INT32的边无法松弛
							在这里加上加数是否存在无穷
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

		return true;//算法执行完毕
	}
private:
	//获取边listBuffer
	virtual EdgesType const &getEdgeList(size_t originNum) = 0;
	//子类构造的时候应该预留>2倍的空间
	virtual void resizeEdge(size_t newSize) = 0;
};

//邻接矩阵图
template<class WeightType>
class AdjacentMatrixGraphTemplate : public GraphTemplate<WeightType> {
	//模板中不能直接访问父类成员
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

	//邻接矩阵
	JCE::ArrayList<JCE::ArrayList<WeightType>> edgeData;
	//若ownerID与targetID之间存在直接关系(边)(即w是否v的邻接点) 返回true
	inline bool existEdge(VertexKey v, VertexKey w) {
		return edgeData[v][w] < INF;//不存在的边被初始化为INF
	}
	void resizeEdge(size_t newSize)override {
		edgeData.resize(newSize);
		FOR(i, 0, newSize) {
			edgeData[i].resize(newSize, INF);//重分配的内存被置为INF
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
			//已存在: 边数不增加 直接更新就行

			//确认更新对否负权边数量的影响
			if (edgeData[e.ownerID][e.targetID] < 0) {
				//减少
				negativeWeightCount -= e.weight < 0 ? 0 : 1;
			}
			else {
				//增加
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
		//只要有边就删除
		if (existEdge(ownerID, targetID)) {
			negativeWeightCount -= edgeData[ownerID][targetID] < 0 ? 1 : 0;
			edgeData[ownerID][targetID] = INF;
			--edgeNum;
		}
		else {
			//不存在;
		}
	}
	EdgesType const &getEdgeList(size_t originNum)override {
		//边读取缓冲
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
	多分支递归算法: past -> present -> next;
	present参数 -> 计算 -> 回溯 -> 递归
	顶点图的present与顶点的储存顺序有关)坐标图的present与位置有关是必须访问的 分支中虽能再次出现但难以决策
	这里的实现与回溯搜索的一般实现有些差异 回溯搜索不会visit起始顶点 但便于记忆与拓展
	(若非要使用可以考虑在DIR84中加一个{0, 0}或是使用do{}while();) 但通过DIR84"函数"少一次访问就没必要南辕北辙了

	图的dfs不等价于回溯搜索:
		1.图的访问条件是限定的(关系: 至少需要两个结点才能讨论) 因此传入的参数只能是{前驱, 现时, 后继}中包涵现时的两个
		 前驱与现时都与深度无关, 于是参数的含义也与depth无关
		 而由于初始结点是由用户外部传入的需要进行决策, 因此只能选择现时结点做参数
		2.在关系的讨论对象决定后首先要明确的是: 在结点a处理完毕前a不能再被a的后继访问, 因此顶点图需要做访问控制
		 在不考虑访问控制实现的前提下(一般用visited[])再明确: 当a的所有关系处理完毕时应当能被其它平行结点访问
		 于是访问控制的顺序必须是 置为访问->访问->置为未访问
		 (顺带一提: 回溯搜索只需要在达到要求时更新最优结果就行了, depth解决了一般的访问控制, 就算没有解决多半也不需要)
		3.在上述前提下 访问控制有两种实现: 处理现时结点; 处理后继结点
		 若选择后者 那么难以处理起始结点的状态(这将引起极大的不可逆错误)
	于是结论是: 这是顶点图 回溯dfs方法的最佳实现
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
				if (edgeData[presentId][nextId] < INF //存在关系
					&& !visited[nextId]) {
					//visited[nextId] = true;//普通回溯, 对后继结点进行访控
					count += dfs(nextId);
					//visited[nextId] = false;
				}
			}
			visited[presentId] = false;//dfs的回溯
			return count;
		}
	}
	template<class Fun>
	//拓扑排序实验
	int dfsTest(VertexKey presentId, Fun visit) {
		customVisit(presentId);

		if (presentId == destId) {
			return 1;
		}
		else {
			visited[presentId] = true;
			int count = 0;
			FOR(nextId, 0, vertexNum) {
				if (edgeData[presentId][nextId] < INF //存在关系
					&& !visited[nextId]) {
					//visited[nextId] = true;//对后继结点进行访控
					count += dfsTest(nextId, visit);
					//visited[nextId] = false;
				}
			}
			visit(presentId);
			visited[presentId] = false;//dfs的回溯
			return count;
		}
	}
	/* 邻接表存储 - 拓扑排序算法: 若图中不存在回路 则计算成功 返回true*/
	bool topologySort(JCE::ArrayList<VertexKey> &topOrderBuffer) {
		/* 对GraphTemplate进行拓扑排序,  topOrderBuffer[]顺序存储排序后的顶点下标 */
		topOrderBuffer.resize(vertexNum);
		JCE::ArrayList<VertexKey> indegree(vertexNum);
		//queue<VertexKey> q;
		//保证在同等排名下优先输出序号小的
		std::priority_queue<VertexKey, JCE::ArrayList<VertexKey>, JCE::greater<VertexKey>> q;
	}
};

//邻接表图(list实现)
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
	邻接表图(map实现)
	// [另一边的id][边的权值]
	typedef map<VertexKey, WeightType> EdgesType;
	//(map解决冲突的Hash表 map在list的基础上改善了边操作 但遍历稍慢)
	EdgesType edgeData[MaxVertexNum];
	一来像4*4的dfs搜索所有的路径这种没有什么实际应用场景
	二来这种实现是为了面对稠密图的list增删困难的情况
	但实际上此实现比邻矩费空间(至少4个域) 且与邻矩实现的稠密图中搜索效率相差不大(分析结果)
	于是就只有稀疏图中有点用 但插入删除省下的那点时间又与其遍历的时间劣势相抵了
	还有一点插入的时候边的数量难以统计
	*/
	//list遍历快于map
	//邻接表(链表解决冲突的Hash表)
	JCE::ArrayList<EdgesType> edgeData;
	//GraphTemplate<WeightType>::EdgesType::iterator 这么写是不行的 很迷
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
			//确认更新对否负权边数量的影响
			if (it->weight < 0) {
				//减少
				negativeWeightCount -= e.weight < 0 ? 0 : 1;
			}
			else {
				//增加
				negativeWeightCount += e.weight < 0 ? 1 : 0;
			}
			//已存在: 边数不增加 直接更新就行
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
		// 直接返回临时变量会出现奇怪的问题: 即使没有触发返回临时变量的情况, 返回的list size却为0
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
			//必存在关系
			FOR_ALL_OBJECT(edgeData[presentId]) {
				if (!visited[element.targetID]) {
					count += dfs(element.targetID);
				}
			}
			visited[presentId] = false;
			return count;
		}
	}

	/* 邻接表存储 - 拓扑排序算法: 若图中不存在回路 则计算成功 返回true*/
	bool topologySort(JCE::ArrayList<VertexKey> &topOrderBuffer) {
		/* 对GraphTemplate进行拓扑排序,  topOrderBuffer[]顺序存储排序后的顶点下标 */
		topOrderBuffer.resize(vertexNum);
		JCE::ArrayList<VertexKey> indegree(vertexNum);
		//queue<VertexKey> q;
		//保证在同等排名下优先输出序号小的
		std::priority_queue<VertexKey, JCE::ArrayList<VertexKey>, JCE::greater<VertexKey>> q;

		/* 遍历图，得到indegree[] edgeData.size()*/
		for (size_t v = 0; v < vertexNum; ++v) {
			//IndexEdge
			for (auto itW = edgeData[v].begin(); itW != edgeData[v].end(); ++itW) {
				++indegree[itW->targetID];/* 对有向边<v, itW->targetID>累计终点的入度 */
			}
		}

		/* 将所有入度为0的顶点入列 */
		for (size_t v = 0; v < vertexNum; ++v) {
			if (indegree[v] == 0) {
				q.push(v);
			}
		}

		/* 下面进入拓扑排序 */
		int cnt = -1;
		while (!q.empty()) {
			/* 弹出一个入度为0的顶点 加入结果序列 */
			VertexKey v = q.top();//q.front();
			q.pop();
			topOrderBuffer[++cnt] = v;
			/* 对v的每个邻接点itW->targetID(W->AdjV)*/
			for (auto itW = edgeData[v].begin(); itW != edgeData[v].end(); ++itW) {
				/* 若删除v使得itW->targetID入度为0 */
				if (--indegree[itW->targetID] == 0) {
					q.push(itW->targetID);/* 则该顶点入列 */
				}
			}
		}

		return cnt == edgeData.size();
	}
};

#endif
