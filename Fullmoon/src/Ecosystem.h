#ifndef __Ecosystem_H
#define __Ecosystem_H
#include"Biology.h"

namespace EcologicEngine {

	using BiologyManager = std::shared_ptr<Biology>;
	// EcologicSystem(生态系统)
	class Ecosystem {
	public:
		Ecosystem() {
			gameWindowCentreSprite = RenderManager::cameraArea;
			gameWindowCentreSprite.enlargement(0.50);
			gameWindowCentreSprite.setCentre(RenderManager::cameraArea.getCentre());
		}

		~Ecosystem() {
			for (size_t i = 0; i < ecoregions.size(); ++i) {
				delete ecoregions[i];
			}
		}

		// 加载一个生态区域数为size的生态系统 默认只有一个生态区域
		void loading(Sprite const &totalWindow, size_t size) {
			this->windowSize = { totalWindow.getWidth(), totalWindow.getHeight() };
			ecoregionsIndex = 0;
			for (int i = size; i > 0; --i) {
				//实际每个生态有不同大小 199会出错
				ecoregions.push_back(new Atlas(totalWindow));
			}
			// 生物数为size
			// 创建的生物群
			createColony(0);
			//创建玩家
			if (player == nullptr) {
				player.reset(new Player(ecoregions[ecoregionsIndex]));
				// 用于初始化 RenderManager::cameraArea
				player->shaw();
			}
			registration();
		}

		// 运行生态系统 gameOver(手动结束 玩家死亡)返回false
		bool running(Bitmap *plat) {
			size_t indexBuffer = player->getAttribute().getCurrentEcoregionsIndex();
			if (indexBuffer != ecoregionsIndex) {
				if (0 <= indexBuffer && indexBuffer < ecoregions.size()) {
					ecoregionsIndex = indexBuffer;
					player->setLocation(ecoregions[ecoregionsIndex]->loadPlat());
					player->setBelongs(ecoregions[ecoregionsIndex]);
					ecoregions[ecoregionsIndex]->mosaicTransition(plat);
				}
			}
			// WindowSprite temp;
			// temp.drawRectangle(gameWindowCentreSprite.getRect());
			// temp.drawRectangle(RenderManager::displayArea.getRect());
			if (gameWindowCentreSprite.contains(player->getSprite())) {
				// do nothing
			}
			else {

			}
			Logger::writeLine(player->getSprite().getLocation(), _T("玩家位置"));
			// 判断地图的移动
			//ecoregions[ecoregionsIndex]->renderDisplayMap();
			ecoregions[ecoregionsIndex]->draw(plat);
			shawEcologic();
			ecoregions[ecoregionsIndex]->run();
			return player->action();
		}

		void mouseMasege(MOUSEMSG const &C) {
			player->setMouseCmd(C);
		}

private:
		// 展示此生态(adj. 生态的；生态学的)
		void shawEcologic() {
			/*orderly有序化: 距离原点远的对象先draw 显出层次感*/
			auto it = member.begin();
			while (it != member.end()) {
				auto &now = it->second;
				//所有对象都在其Atlas*内行动<领地> 物品掉落 下一块地图的编号 -未编写
				if (!now->action()) {
					//该对象死亡 销毁死亡对象@TODO
					auto temp = it->second;
					it = member.erase(it);
					//delete temp;
					temp = nullptr;
				}
				else {
					++it;
				}
			}
		}

		void createColony(size_t bioCount) {
			BiologyManager temp = nullptr;
			FOR(i, 0, bioCount) {
				// 属于生态区域ecoregions[ecoregionsIndex]
				temp.reset(new People(ecoregions[ecoregionsIndex]));
				member.insert({ temp->getID(), temp });
			}
			FOR(i, 0, bioCount) {
				temp.reset(new Monster(ecoregions[ecoregionsIndex]));
				member.insert({ temp->getID(), temp });
			}
			ecoregions[ecoregionsIndex]->refresh();
		}

		void registration() {
			// 非玩家 + 玩家
			if (member.size() + 1 != ecoregions[ecoregionsIndex]->eventSize()) {
				_DEBUG_ERROR("初始化错误: 注册碰撞事件前 事件总数与生物总数相等!(全位置事件, 否则不利于随机位置)");
			}
			// 注册巡逻事件
			FOR_ALL_OBJECT(member) {
				element.second->registration();
			}
			// 注册传送事件
			ecoregions[ecoregionsIndex]->registration();
		}

		// 该生态的所有成员 <body信息, 实体(多态需要用指针)> 若在栈上申请过多内存会溢出 new是在堆上申请
		std::map<int, BiologyManager> member;
		GP Size windowSize;
		// Ecoregion(生态区域)
		ArrayList<Atlas *> ecoregions;
		// 玩家所在生态的编号
		size_t ecoregionsIndex = -1;
		Sprite gameWindowCentreSprite;
		// 玩家实体
		std::shared_ptr<Player> player = nullptr;
	};

}

#endif