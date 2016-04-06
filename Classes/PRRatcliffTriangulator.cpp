#include "PRRatcliffTriangulator.h"
#include "triangulate.h"

using namespace cocos2d;

CCArray* PRRatcliffTriangulator::triangulateVertices(cocos2d::CCArray* vertices){
	Vector2dVector* inputPointsForTriangulation = new Vector2dVector;
	CCObject* temp;
	CCARRAY_FOREACH(vertices, temp){
		CCString* ps = (CCString*)temp;
		CCPoint point = CCPointFromString(ps->getCString());
		inputPointsForTriangulation->push_back(Vector2d(point.x, point.y));
	}

	Vector2dVector triangulatedPoints;

	Triangulate::Process(*inputPointsForTriangulation, triangulatedPoints);
	delete inputPointsForTriangulation;

	int triangulatedPointCount = triangulatedPoints.size();
	CCArray* triangulatedPointsArray = CCArray::arrayWithCapacity(triangulatedPointCount);
	for(int i = 0; i < triangulatedPointCount; i++){
		CCString* triangulatedPointValue = CCString::stringWithFormat("{%f, %f}", triangulatedPoints[i].GetX(), triangulatedPoints[i].GetY());
		triangulatedPointsArray->addObject(triangulatedPointValue);
	}
	return triangulatedPointsArray;
}
