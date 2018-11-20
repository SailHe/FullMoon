#ifndef _Attribute_H
#include "stdafx.h"
#include "TimeClock.h"
#define _Attribute_H

using TargetLocation = Sprite;//保证一定是target的非指针类型
using TargetType = std::shared_ptr<TargetLocation const>;//(保证一定可以使用->访问)
//属性包 (允许外部改变的只有EXP 和HP 其余属性初始化完成后 自动变化)
//允许外界查看所有推理属性 (计时器比较特殊)
class Attribute{
	friend class Player;//玩家类具有修改权限
	/*基本属性*/
	unsigned DNA;
	int weight, VIT, INT, STR, LUK, AGI;
	int fullHP, fullMP;/*寿命:这里指健康值hp可以减少为零的次数 每次降至0寿命减1  每过一段时间自动减少*/
	//推理属性
	unsigned level, EXP;
	int visibility;/*视距 (视距过大 易造成碰撞事件溢出)*/
	int ATK, DEF, SP, HP, MP;
	Package bag;/*背包袋子:id itemCnt*/
	//已发出的攻击个数
	int atkCount = 0;
	//计时器
	TimeClock actionClock;//行动计时
	TimeClock moveClock;//移动计时
	TimeClock atkClock;//攻击计时
	TimeClock inertiaClock;//惯性计时
	TimeClock reactionClock;//反应计时
	TimeClock bypassClock;//绕行计时

	size_t currentEcoregionsIndex;//当前所属生态

	std::shared_ptr<TargetLocation> targetBuffer;//随机目标缓冲
	TargetType target;//目标的位置
	//初始化
	void initialize(){
		DNA = rand() & -1;
		weight = rand() % 200;//质量 计划用于惯性
		fullHP = rand() % 100 + 100, fullMP = rand() % 100 + 100;
		ATK = rand() % 10 + 5, DEF = rand() % 10 + 5;
		VIT = rand() % 7 + 2;//耐力--微增魔限   微增魔/物防(def) 血(hp)
		INT = rand() % 7 + 2;//智力--魔限(mp)   魔攻(matk) 魔防(mdef) 视距
		STR = rand() % 7 + 2;//力量--物攻(atk)  物防  微增魔攻
		LUK = rand() % 7 + 1;//运气--暴击(cri)  命中(hit)  闪避(dex)
		AGI = rand() % 3 + 2;//敏捷--行走v     攻速v      闪避 已用于:人物坐标偏增量变化基数, 初始内部计时
		EXP = 0, level = 1;
		SP = 0;//status point
		reactionClock.SetDrtTime(3000);//3s
	}
public:
	Attribute(TargetLocation *&&targetBuffer){
		this->targetBuffer.reset(targetBuffer);
		currentEcoregionsIndex = 0;
		initialize();
		update();
	}
	~Attribute(){}
	//向背包添加cnt个编号为id的物品
	void addItem(size_t id, int cnt){
		auto it = find_if(PARAMETER_ALL_OBJECT(bag), [id](const std::pair <int, int> &a){
			return a.first == id;
		});
		if (it == bag.end()){
			bag.push_back(std::make_pair(id, cnt));
		}
		else{
			(*it).second += cnt;
		}
	}
	//移除下标为index的物品cnt个 返回从背包移除后的该物品的id
	size_t updateItemCount(size_t index, int cnt){
		auto it = bag.begin() + index;
		if (it == bag.end()){
			_DEBUG_ERROR("index错误");
			return 0;
		}
		else{
			if ((*it).second + cnt < 0){
				_DEBUG_ERROR("数量错误");
				return 0;
			}
			(*it).second += cnt;
			size_t id = it->first;
			if ((*it).second == 0){
				bag.erase(it);
			}
			return id;
		}
	}
	Package &getBag(){
		return bag;
	}
	int getAtkCount()const{
		return atkCount;
	}
	//--atkCount
	void atkCountSub(){
		--atkCount;
	}
	//寿命减少1
	void lifetimeReduce(){
		//--fullHP;
		fullHP = 0;
	}
	//修正HP: value < 0是damage 否则是recover 若HP归零 返回false 并会自动计算寿命
	bool modifyHP(int value){
		HP += value;
		if (HP <= 0)
			return false;
		else{
			if (HP > fullHP){
				HP = fullHP;
			}
		}
		return true;
	}
	//若等级提升返回true
	bool modifyEXP(int value){
		EXP += value;
		return lvUp();
	}
	//等级提升返回true
	bool lvUp(){
		if (EXP >= 100){
			level += EXP / 100;
			return true;
		}
		return false;
	}
	//更新属性
	void update(){
		HP = fullHP, MP = fullMP;
		EXP %= 100;
		SP++;
		visibility = INT * 20 + 2 * GRID;
		ATK += (int)(STR * 0.5), DEF += (int)(VIT * 0.5);
		actionClock.SetDrtTime(2000 - AGI * 200);//设置行动计时间隔(0.4, 2]s
		atkClock.SetDrtTime(2000 - AGI * 200);
		inertiaClock.SetDrtTime(150);/*惯性时间 0.15s*/
		moveClock.SetDrtTime(10);
		bypassClock.SetDrtTime(3000);
	}

	//允许询问是否达到计时周期 若达到自动清零 并返回true 否则返回false
	bool actionTime(){
		return actionClock.IsTimeOut();
	}
	bool atkTime(){
		return atkClock.IsTimeOut();
	}
	//惯性时间
	bool inertiaTime(){
		return inertiaClock.IsTimeOut();
	}
	//绕行时间
	bool bypassTime(){
		return bypassClock.IsTimeOut();
	}
	//移动时间
	bool moveTime(){
		return moveClock.IsTimeOut();
	}
	//反应时间
	bool reactionTime(){
		return reactionClock.IsTimeOut();
	}

	size_t getCurrentEcoregionsIndex() const {
		return currentEcoregionsIndex;
	}
	void setCurrentEcoregionsIndex(size_t index){
		currentEcoregionsIndex = index;
	}
	unsigned getLevel()const{
		return level;
	}
	unsigned getEXP()const{
		return EXP;
	}
	int getVisibility()const{
		return visibility;
	}
	int getATK()const{
		return ATK;
	}
	int getDEF()const{
		return DEF;
	}
	int getSP()const{
		return SP;
	}
	int getHP()const{
		return HP;
	}
	int getMP()const{
		return MP;
	}
	int getFullHP()const{
		return fullHP;
	}
	int getFullMP()const{
		return fullMP;
	}

	void randomTarget(){
		targetBuffer->randomLocation();
		setTarget(targetBuffer);
	}
	TargetType const &getTarget(){
		return this->target;
	}
	void setTarget(TargetType const &target){
		this->target = target;
	}
	void setTarget(TargetLocation const &target){
		*targetBuffer = target;
		this->target = targetBuffer;
	}
};

#endif
