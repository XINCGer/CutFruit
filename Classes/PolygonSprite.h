#ifndef __POLYGON_SPRITE_H__
#define __POLYGON_SPRITE_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "PRFilledPolygon.h"

#define PTM_RATIO 32

typedef enum _State
{
	kStateIdle = 0,
	kStateTossed
} State;

typedef enum _Type
{
	kTypeWatermelon = 0,
	kTypeStrawberry,
	kTypePineapple,
	kTypeGrapes,
	kTypeBanana,
	kTypeBomb
} Type;

class PolygonSprite : public PRFilledPolygon {
public:
	bool initWithTextureBodyOriginal(cocos2d::CCTexture2D* texture, b2Body* body, bool original);
	//bool initWithFileBodyOriginal(cocos2d::CCString* fileName, b2Body* body, bool original);
	bool initWithFileBodyOriginal(const char* fileName, b2Body* body, bool original);
	bool initWithWorld(b2World* world);

	//static PolygonSprite* spriteWithFile(cocos2d::CCString* fileName, b2Body* body, bool original);
	static PolygonSprite* spriteWithFile(const char* fileName, b2Body* body, bool original);
	static PolygonSprite* spriteWithTexture(cocos2d::CCTexture2D* texture, b2Body* body, bool original);
	static PolygonSprite* spriteWithWorld(b2World* world);

	b2Body* createBodyForWorld(b2World* world, b2Vec2 position, float rotation,
		b2Vec2* vertices, int32 count, float density, float friction, float restitution);

	void activateCollisions();
	void deactivateCollisions();

	void setPosition(const cocos2d::CCPoint& var);


	CC_SYNTHESIZE(b2Body*, _body, Body);
	CC_SYNTHESIZE(bool, _original, Original);
	CC_SYNTHESIZE(b2Vec2, _centroid, Centroid);

	CC_SYNTHESIZE(bool, _sliceEntered, SliceEntered);
	CC_SYNTHESIZE(bool, _sliceExited, SliceExited);
	CC_SYNTHESIZE_PASS_BY_REF(b2Vec2, _entryPoint, EntryPoint);
	CC_SYNTHESIZE_PASS_BY_REF(b2Vec2, _exitPoint, ExitPoint);
	CC_SYNTHESIZE(double, _sliceEntryTime, SliceEntryTime);

	CC_SYNTHESIZE(State, _state, State);
	CC_SYNTHESIZE(Type, _type, Type);

	CC_SYNTHESIZE(cocos2d::CCParticleSystemQuad*, _splurt, Splurt);

private:
	cocos2d::CCAffineTransform nodeToParentTransform();

	//bool _sliceEntered;
	//bool _sliceExited;
	//b2Vec2 _entryPoint;
	//b2Vec2 _exitPoint;
	//double _sliceEntryTime;

};

#endif