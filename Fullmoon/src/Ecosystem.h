#ifndef __Ecosystem_H
#define __Ecosystem_H//引擎
#include"Biology.h"


//生态系统 World
class Ecosystem{
public:
	//默认只有一个生态
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

	//加载一个生态数为size的生态
	void loadingEcoregions(Sprite const &totalWindow, size_t size){
		this->windowSize = { totalWindow.getWidth(), totalWindow.getHeight() };
		ecoregionsIndex = 0;
		for (int i = size; i > 0; --i){
			//实际每个生态有不同大小 199会出错
			ecoregions.push_back(new DisplayArea(totalWindow, 10));
		}
		if (player == nullptr){
			player.reset(new Player(ecoregions[ecoregionsIndex]));
		}
	}

	//运行生态系统 gameOver(手动结束 玩家死亡)返回false
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
		//判断地图的移动
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
	vector<DisplayArea *> ecoregions;//生态
	//玩家所在生态的编号
	size_t ecoregionsIndex = -1;
	Sprite gameWindowCentreSprite;
	shared_ptr<Player> player = nullptr;/*玩家实体*/
};


#endif