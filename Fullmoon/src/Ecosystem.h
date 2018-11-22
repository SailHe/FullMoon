#ifndef __Ecosystem_H
#define __Ecosystem_H
#include"Biology.h"

namespace EcologicEngine {

	using BiologyManager = std::shared_ptr<Biology>;
	// EcologicSystem(��̬ϵͳ)
	class Ecosystem {
	public:
		Ecosystem() {
			gameWindowCentreSprite = RenderManager::cameraSprite;
			gameWindowCentreSprite.enlargement(0.50);
			gameWindowCentreSprite.setCentre(RenderManager::cameraSprite.getCentre());
		}

		~Ecosystem() {
			for (size_t i = 0; i < ecoregions.size(); ++i) {
				delete ecoregions[i];
			}
		}

		// ����һ����̬������Ϊsize����̬ϵͳ Ĭ��ֻ��һ����̬����
		void loading(Sprite const &totalWindow, size_t size) {
			this->windowSize = { totalWindow.getWidth(), totalWindow.getHeight() };
			ecoregionsIndex = 0;
			for (int i = size; i > 0; --i) {
				//ʵ��ÿ����̬�в�ͬ��С 199�����
				ecoregions.push_back(new Atlas(totalWindow));
			}
			// ������Ϊsize
			// ����������Ⱥ
			createColony(10);
			registration();
			//�������
			if (player == nullptr) {
				player.reset(new Player(ecoregions[ecoregionsIndex]));
			}
		}

		// ������̬ϵͳ gameOver(�ֶ����� �������)����false
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
			WindowSprite temp;
			temp.drawRectangle(gameWindowCentreSprite.getRect());
			temp.drawRectangle(RenderManager::displaySprite.getRect());
			if (gameWindowCentreSprite.contains(player->getSprite())) {
				//donthing
			}
			else {

			}
			//�жϵ�ͼ���ƶ�
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
		// չʾ����̬(adj. ��̬�ģ���̬ѧ��)
		void shawEcologic() {
			/*orderly����: ����ԭ��Զ�Ķ�����draw �Գ���θ�*/
			auto it = member.begin();
			while (it != member.end()) {
				auto &now = it->second;
				//���ж�������Atlas*���ж�<���> ��Ʒ���� ��һ���ͼ�ı�� -δ��д
				if (!now->action()) {
					//�ö������� ������������@TODO
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
				// ������̬����ecoregions[ecoregionsIndex]
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
			if (member.size() != ecoregions[ecoregionsIndex]->eventSize()) {
				_DEBUG_ERROR("��ʼ������: ע����ײ�¼�ǰ �¼������������������!(ȫλ���¼�, �����������λ��)");
			}
			//ע��Ѳ���¼�
			FOR_ALL_OBJECT(member) {
				element.second->registration();
			}
			//ע�ᴫ���¼�
			/*FOR(it, transmissionList.begin(), transmissionList.end()) {
				ecoregions[ecoregionsIndex]->sendImpactEvent(*it);
			}*/
		}

		// ����̬�����г�Ա <body��Ϣ, ʵ��(��̬��Ҫ��ָ��)> ����ջ����������ڴ����� new���ڶ�������
		std::map<int, BiologyManager> member;
		GP Size windowSize;
		// Ecoregion(��̬����)
		ArrayList<Atlas *> ecoregions;
		// ���������̬�ı��
		size_t ecoregionsIndex = -1;
		Sprite gameWindowCentreSprite;
		// ���ʵ��
		std::shared_ptr<Player> player = nullptr;
	};

}

#endif