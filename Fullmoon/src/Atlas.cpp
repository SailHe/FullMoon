#include "Atlas.h"

namespace EcologicEngine {

	AnimationManager Atlas::maper;
	// ��Ļ�Ӵ��ڵ�ͼ�е�λ��
	int Atlas::leftTopX = 0;
	int Atlas::oldLeftTopX = 0;
	int Atlas::leftTopY = 0;
	Sprite RenderManager::displayArea;
	Sprite RenderManager::cameraArea;

	bool Atlas::run() {
		this->drawDropItem();
		int eventCnt = this->refresh();
		return true;
	}

}
