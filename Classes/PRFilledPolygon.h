#ifndef __PRFILLED_POLYGON_H__
#define __PRFILLED_POLYGON_H__

#include "cocos2d.h"
#include "PRTriangulator.h"

class PRFilledPolygon : public cocos2d::CCNode {
public:
	~PRFilledPolygon();

	bool initWithMember(cocos2d::CCArray* polygonPoints, cocos2d::CCTexture2D* fillTexture);
	bool initWithMember(cocos2d::CCArray* polygonPoints, cocos2d::CCTexture2D* fillTexture, PRTriangulator* polygonTriangulator);


	static PRFilledPolygon* filledPolygon(cocos2d::CCArray* polygonPoints, cocos2d::CCTexture2D* fillTexture);
	static PRFilledPolygon* filledPolygon(cocos2d::CCArray* polygonPoints, cocos2d::CCTexture2D* fillTexture, PRTriangulator* polygonTriangulator);

	void setPoints(cocos2d::CCArray* points);
	void setTriangulator(PRTriangulator* var);
	void setTexture(cocos2d::CCTexture2D* var);
	PRTriangulator* getTriangulator();
	cocos2d::CCTexture2D* getTexture();



private:
	int areaTrianglePointCount;

	cocos2d::CCTexture2D *texture;
	cocos2d::ccBlendFunc blendFunc;

	cocos2d::CCPoint *areaTrianglePoints;
	cocos2d::CCPoint *textureCoordinates;

	PRTriangulator* triangulator;

	void calculateTextureCoordinates();
	void draw();
	void updateBlendFunc();
};


#endif