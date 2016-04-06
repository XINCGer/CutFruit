#ifndef __PRTRIANGULATOR_H__
#define __PRTRIANGULATOR_H__

#include "cocos2d.h"

class PRTriangulator : public cocos2d::CCObject {
public:
	virtual cocos2d::CCArray* triangulateVertices(cocos2d::CCArray* vertices){
		CC_UNUSED_PARAM(vertices);
		return NULL;
	}
};


#endif