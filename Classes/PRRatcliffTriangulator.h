#ifndef __PR_RATCLIFF_TRIANGULATOR_H__
#define __PR_RATCLIFF_TRIANGULATOR_H__

#include "cocos2d.h"

#include "PRTriangulator.h"

class PRRatcliffTriangulator : public PRTriangulator {
public:
	virtual cocos2d::CCArray* triangulateVertices(cocos2d::CCArray* vertices);
};


#endif