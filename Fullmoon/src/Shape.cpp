#include "Shape.h"
namespace EcologicEngine {
	int Shape::typeId() const {
		//type_info
		if (typeid(*this) == typeid(Ray)) {
			return 0;
		}
		else if (typeid(*this) == typeid(CircleShape)) {
			return 1;
		}
		else if (typeid(*this) == typeid(PolygonShape)) {
			return 2;
		}
		else {
			return -1;
		}
	}

}
