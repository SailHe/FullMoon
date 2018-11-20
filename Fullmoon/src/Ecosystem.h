#ifndef __Ecosystem_H
#define __Ecosystem_H//����
#include"Biology.h"


//��̬ϵͳ World
class Ecosystem{
public:
	//Ĭ��ֻ��һ����̬
	Ecosystem(){
		gameWindowCentreSprite = RenderManager::cameraSprite;
		gameWindowCentreSprite.enlargement(0.50);
		gameWindowCentreSprite.setCentre(RenderManager::cameraSprite.getCentre());
	}

	~Ecosystem(){
		for (size_t i = 0; i < ecoregions.size(); ++i){
			delete ecoregions[i];
		}
	}

	//����һ����̬��Ϊsize����̬
	void loadingEcoregions(Sprite const &totalWindow, size_t size){
		this->windowSize = { totalWindow.getWidth(), totalWindow.getHeight() };
		ecoregionsIndex = 0;
		for (int i = size; i > 0; --i){
			//ʵ��ÿ����̬�в�ͬ��С 199�����
			ecoregions.push_back(new DisplayArea(totalWindow, 10));
		}
		if (player == nullptr){
			player.reset(new Player(ecoregions[ecoregionsIndex]));
		}
	}

	//������̬ϵͳ gameOver(�ֶ����� �������)����false
	bool running(Bitmap *plat){
		size_t indexBuffer = player->getAttribute().getCurrentEcoregionsIndex();
		if (indexBuffer != ecoregionsIndex){
			if (0 <= indexBuffer && indexBuffer < ecoregions.size()){
				ecoregionsIndex = indexBuffer;
				player->setLocation(ecoregions[ecoregionsIndex]->loadPlat());
				player->setBelongs(ecoregions[ecoregionsIndex]);
				ecoregions[ecoregionsIndex]->mosaicTransition(plat);
			}
		}
		WindowSprite temp;
		temp.drawRectangle(gameWindowCentreSprite.getRect());
		temp.drawRectangle(RenderManager::displaySprite.getRect());
		if (gameWindowCentreSprite.contains(player->getSprite())){
			//donthing
		}
		else{

		}
		//�жϵ�ͼ���ƶ�
		//ecoregions[ecoregionsIndex]->renderDisplayMap();
		ecoregions[ecoregionsIndex]->draw(plat);
		ecoregions[ecoregionsIndex]->shawEcologic();
		return player->action();
	}

	void mouseMasege(MOUSEMSG const &C){
		player->setMouseCmd(C);
	}

private:
	GP Size windowSize;
	vector<DisplayArea *> ecoregions;//��̬
	//���������̬�ı��
	size_t ecoregionsIndex = -1;
	Sprite gameWindowCentreSprite;
	shared_ptr<Player> player = nullptr;/*���ʵ��*/
};


#endif