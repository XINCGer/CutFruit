#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#include "Box2D/Box2D.h"

#include "SimpleAudioEngine.h"

#include <list>

using namespace cocos2d;

typedef enum {
	kBanana = 0,
	kGrapes = 1,
	kPineapple = 2,
	kStrawberry = 3,
	kWatermelon = 4,
	kBomb = 5,
	kPiece = 6,
} SpriteKind;

class HelloWorld : public cocos2d::CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
	~HelloWorld();

    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
	virtual void draw();

	virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);
	void clearSlices();

	void drawLine();
	void update(float dt);
	void createBody(CCSprite* sprite, float angle, bool left);
	void restart(CCObject* pSender);

    LAYER_CREATE_FUNC(HelloWorld);

public:
	b2Body *groundBody;
	b2World *world;
	CCSpriteBatchNode *batch;
	CCSprite* fruit;
	CCSprite* fillX[3];
	CCPoint center, currentPoint;
	CCLabelTTF* label;
	CCMenu* menu;
	CCPoint pt;
	CCParticleSystemQuad* particle[6];
	int scoreTotal;
	int deathNum;
	float gaptime;
	float ratio;
	float angle;
	list<CCPoint> pointList;
	std::vector<CCSprite *> fruites;
};

#endif  // __HELLOWORLD_SCENE_H__