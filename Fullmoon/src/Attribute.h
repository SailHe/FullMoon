#ifndef _Attribute_H
#include "stdafx.h"
#include "TimeClock.h"
#define _Attribute_H

using TargetLocation = Sprite;//��֤һ����target�ķ�ָ������
using TargetType = std::shared_ptr<TargetLocation const>;//(��֤һ������ʹ��->����)
//���԰� (�����ⲿ�ı��ֻ��EXP ��HP �������Գ�ʼ����ɺ� �Զ��仯)
//�������鿴������������ (��ʱ���Ƚ�����)
class Attribute{
	friend class Player;//���������޸�Ȩ��
	/*��������*/
	unsigned DNA;
	int weight, VIT, INT, STR, LUK, AGI;
	int fullHP, fullMP;/*����:����ָ����ֵhp���Լ���Ϊ��Ĵ��� ÿ�ν���0������1  ÿ��һ��ʱ���Զ�����*/
	//��������
	unsigned level, EXP;
	int visibility;/*�Ӿ� (�Ӿ���� �������ײ�¼����)*/
	int ATK, DEF, SP, HP, MP;
	Package bag;/*��������:id itemCnt*/
	//�ѷ����Ĺ�������
	int atkCount = 0;
	//��ʱ��
	TimeClock actionClock;//�ж���ʱ
	TimeClock moveClock;//�ƶ���ʱ
	TimeClock atkClock;//������ʱ
	TimeClock inertiaClock;//���Լ�ʱ
	TimeClock reactionClock;//��Ӧ��ʱ
	TimeClock bypassClock;//���м�ʱ

	size_t currentEcoregionsIndex;//��ǰ������̬

	std::shared_ptr<TargetLocation> targetBuffer;//���Ŀ�껺��
	TargetType target;//Ŀ���λ��
	//��ʼ��
	void initialize(){
		DNA = rand() & -1;
		weight = rand() % 200;//���� �ƻ����ڹ���
		fullHP = rand() % 100 + 100, fullMP = rand() % 100 + 100;
		ATK = rand() % 10 + 5, DEF = rand() % 10 + 5;
		VIT = rand() % 7 + 2;//����--΢��ħ��   ΢��ħ/���(def) Ѫ(hp)
		INT = rand() % 7 + 2;//����--ħ��(mp)   ħ��(matk) ħ��(mdef) �Ӿ�
		STR = rand() % 7 + 2;//����--�﹥(atk)  ���  ΢��ħ��
		LUK = rand() % 7 + 1;//����--����(cri)  ����(hit)  ����(dex)
		AGI = rand() % 3 + 2;//����--����v     ����v      ���� ������:��������ƫ�����仯����, ��ʼ�ڲ���ʱ
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
	//�򱳰����cnt�����Ϊid����Ʒ
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
	//�Ƴ��±�Ϊindex����Ʒcnt�� ���شӱ����Ƴ���ĸ���Ʒ��id
	size_t updateItemCount(size_t index, int cnt){
		auto it = bag.begin() + index;
		if (it == bag.end()){
			_DEBUG_ERROR("index����");
			return 0;
		}
		else{
			if ((*it).second + cnt < 0){
				_DEBUG_ERROR("��������");
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
	//��������1
	void lifetimeReduce(){
		//--fullHP;
		fullHP = 0;
	}
	//����HP: value < 0��damage ������recover ��HP���� ����false �����Զ���������
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
	//���ȼ���������true
	bool modifyEXP(int value){
		EXP += value;
		return lvUp();
	}
	//�ȼ���������true
	bool lvUp(){
		if (EXP >= 100){
			level += EXP / 100;
			return true;
		}
		return false;
	}
	//��������
	void update(){
		HP = fullHP, MP = fullMP;
		EXP %= 100;
		SP++;
		visibility = INT * 20 + 2 * GRID;
		ATK += (int)(STR * 0.5), DEF += (int)(VIT * 0.5);
		actionClock.SetDrtTime(2000 - AGI * 200);//�����ж���ʱ���(0.4, 2]s
		atkClock.SetDrtTime(2000 - AGI * 200);
		inertiaClock.SetDrtTime(150);/*����ʱ�� 0.15s*/
		moveClock.SetDrtTime(10);
		bypassClock.SetDrtTime(3000);
	}

	//����ѯ���Ƿ�ﵽ��ʱ���� ���ﵽ�Զ����� ������true ���򷵻�false
	bool actionTime(){
		return actionClock.IsTimeOut();
	}
	bool atkTime(){
		return atkClock.IsTimeOut();
	}
	//����ʱ��
	bool inertiaTime(){
		return inertiaClock.IsTimeOut();
	}
	//����ʱ��
	bool bypassTime(){
		return bypassClock.IsTimeOut();
	}
	//�ƶ�ʱ��
	bool moveTime(){
		return moveClock.IsTimeOut();
	}
	//��Ӧʱ��
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
