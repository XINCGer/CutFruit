#include "HelloWorldScene.h"

#define PTM_RATIO 32.0
#define PI 3.14159265358
using namespace cocos2d;
using namespace CocosDenshion;

CCScene* HelloWorld::scene()
{
	CCScene * scene = NULL;
	do
	{
		// 'scene' is an autorelease object
		scene = CCScene::create();
		CC_BREAK_IF(! scene);
		
		// 'layer' is an autorelease object
		HelloWorld *layer = HelloWorld::create();
		CC_BREAK_IF(! layer);

		// add layer as a child to scene
		scene->addChild(layer);
	} while (0);

	// return the scene
	return scene;
}

HelloWorld::~HelloWorld()
{
	delete world;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	bool bRet = false;
	do 
	{
		CC_BREAK_IF(! CCLayer::init());
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		angle = 0;
		gaptime = 2.5f;
		scoreTotal = 0;
		deathNum = 0;

		//create bg
		CCSprite *bg = CCSprite::create("bg.png");
		ratio = winSize.height/bg->getContentSize().height;
		bg->setAnchorPoint(ccp(0, 0));
		bg->setPosition(ccp(0, 0));
		bg->setScaleY(ratio);
		this->addChild(bg, -2);

		//init partice system
		CCParticleSystemQuad* sunPollen = CCParticleSystemQuad::particleWithFile("sun_pollen.plist");
		this->addChild(sunPollen);

		particle[0] = CCParticleSystemQuad::particleWithFile("banana_splurt.plist");
		particle[1] = CCParticleSystemQuad::particleWithFile("grapes_splurt.plist");
		particle[2] = CCParticleSystemQuad::particleWithFile("pineapple_splurt.plist");
		particle[3] = CCParticleSystemQuad::particleWithFile("strawberry_splurt.plist");
		particle[4] = CCParticleSystemQuad::particleWithFile("watermelon_splurt.plist");
		particle[5] = CCParticleSystemQuad::particleWithFile("explosion.plist");
		for(int i=0; i<6; i++)
		{
			particle[i]->stopSystem();
			this->addChild(particle[i], 5);
		}

		//define the X
		for(int i=0; i<3; i++)
		{
			CCSprite* unfillX = CCSprite::create("x_unfilled.png");
			fillX[i] = CCSprite::create("x_filled.png");			
			unfillX->setPosition( ccp(winSize.width*0.8 + i*35, winSize.height*0.9) );
			fillX[i]->setPosition( ccp(winSize.width*0.8 + i*35, winSize.height*0.9) );
			this->addChild(unfillX, 9);
			this->addChild(fillX[i], 10);
			fillX[i]->setVisible(false);
		}
		
		//score image
		CCSprite* score = CCSprite::create("fruit_cut.png");
		score->setPosition(ccp(winSize.width/20, winSize.height*0.9));
		this->addChild(score, 10);

		//init labelTTF
		label = CCLabelTTF::create("0", "Arial", 24);
		label->setColor(ccYELLOW);
		label->setPosition(ccp(winSize.width/20+35, winSize.height*0.9));
		this->addChild(label, 9);

		//define the world
		b2Vec2 gravity;
		gravity.Set(0.0f, -40.0f);
		world = new b2World(gravity);
		world->SetAllowSleeping(true);
		world->SetContinuousPhysics(true);

		b2BodyDef groundBodyDef;
		groundBodyDef.position.Set(0, 0);

		b2Body *groundBody = world->CreateBody(&groundBodyDef);
		b2EdgeShape groundBox;

		SimpleAudioEngine::sharedEngine()->preloadEffect("swoosh.mp3");
		SimpleAudioEngine::sharedEngine()->preloadEffect("toss_consecutive.mp3");
		SimpleAudioEngine::sharedEngine()->preloadEffect("toss_simultaneous.mp3");
		SimpleAudioEngine::sharedEngine()->preloadEffect("toss_bomb.mp3");
		SimpleAudioEngine::sharedEngine()->preloadEffect("lose_life.mp3");
		SimpleAudioEngine::sharedEngine()->preloadEffect("squash.mp3");
		SimpleAudioEngine::sharedEngine()->playBackgroundMusic("nature_bgm.mp3");

		setTouchEnabled(true);
		scheduleUpdate();

		bRet = true;
	} while (0);

	return bRet;
}

void HelloWorld::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)  
{  
	CCSetIterator it = pTouches->begin();
	CCTouch* touch = (CCTouch*)*it;
	CCPoint beginPoint = touch->locationInView();
	beginPoint = CCDirector::sharedDirector()->convertToGL(beginPoint);

	pointList.push_back(beginPoint);
} 

void HelloWorld::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)  
{
	CCTouch* touch = (CCTouch*)pTouches->anyObject();
	pt = touch->locationInView();
	pt = CCDirector::sharedDirector()->convertToGL(pt);

	CCSetIterator it = pTouches->begin();

	CCPoint nextPoint = touch->locationInView();
	nextPoint = CCDirector::sharedDirector()->convertToGL(nextPoint);

	CCPoint preMovePoint = touch->previousLocationInView();
	preMovePoint = CCDirector::sharedDirector()->convertToGL(preMovePoint);

	float distance = ccpDistance(nextPoint, preMovePoint);
	if (distance > 1)
	{
		int d = (int)distance;
		for (int i = 0; i < d; i++ )
		{
			float distanceX = nextPoint.x - preMovePoint.x;
			float distanceY = nextPoint.y - preMovePoint.y;

			float percent = i / distance;
			CCPoint newPoint;
			newPoint.x = preMovePoint.x + (distanceX * percent);
			newPoint.y = preMovePoint.y + (distanceY * percent); 

			pointList.push_back(newPoint);
		}
	}
}

void HelloWorld::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
	pt = ccp(-1000, -1000);

	pointList.clear();
}

void HelloWorld::update(float dt)
{
	int32 velocityIterations = 8;  
	int32 positionIterations = 8; 
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	angle += 2;

	//delete old fruit
	for(std::vector<CCSprite*>::iterator iter = fruites.begin(); iter != fruites.end(); )
	{
		if( (*iter)->getPositionY() < -100)
		{
			if((*iter)->getTag() != kPiece && (*iter)->getTag() != kBomb)
			{
				deathNum += 1;
				SimpleAudioEngine::sharedEngine()->playEffect("toss_consecutive.mp3");
			}
			this->removeChild(*iter, true);	
			iter = fruites.erase(iter);
		}
		else
		{
			iter++;
		}
	}

	//check the fruit
	b2Body* b = world->GetBodyList();
	if(pt.y != -1000)
	while(b)
	{
		b2Body* p = b;
		b = b->GetNext();
		if(p->GetUserData() != NULL)
		{
			CCSprite *sprite = (CCSprite*)(p->GetUserData());
			if(sprite->getTag() == kPiece)
				continue;	
			CCRect rect = sprite->boundingBox();
			if(rect.CCRectContainsPoint(rect, pt))
			{
				float fruitAngle = sprite->getRotation();
				CCPoint fruitPosition = sprite->getPosition();
				CCSprite* piece1 = CCSprite::create();
				CCSprite* piece2 = CCSprite::create();
				switch(sprite->getTag())
				{
				case 0:
					piece1 = CCSprite::create("banana1.png");
					piece2 = CCSprite::create("banana2.png");
					particle[0]->setPosition( fruitPosition );
					particle[0]->resetSystem();
					break;
				case 1:
					piece1 = CCSprite::create("grapes1.png");
					piece2 = CCSprite::create("grapes2.png");
					particle[1]->setPosition( fruitPosition );
					particle[1]->resetSystem();
					break;
				case 2:
					piece1 = CCSprite::create("pineapple1.png");
					piece2 = CCSprite::create("pineapple2.png");
					particle[2]->setPosition( fruitPosition );
					particle[2]->resetSystem();
					break;
				case 3:
					piece1 = CCSprite::create("strawberry1.png");
					piece2 = CCSprite::create("strawberry2.png");
					particle[3]->setPosition( fruitPosition );
					particle[3]->resetSystem();
					break;
				case 4:
					piece1 = CCSprite::create("watermelon1.png");
					piece2 = CCSprite::create("watermelon2.png");
					particle[4]->setPosition( fruitPosition );
					particle[4]->resetSystem();
					break;
				case 5:
					piece1 = CCSprite::create("bomb1.png");
					piece2 = CCSprite::create("bomb2.png");
					particle[5]->setPosition( fruitPosition );
					particle[5]->resetSystem();
					break;
				}

				piece1->setTag(kPiece);
				piece2->setTag(kPiece);
				float angle = p->GetAngle();
				this->addChild(piece1);
				this->addChild(piece2);
				piece1->setRotation(fruitAngle);
				piece2->setRotation(fruitAngle);
				piece1->setScale(ratio);
				piece2->setScale(ratio);
				piece1->setPosition(fruitPosition);
				piece2->setPosition(fruitPosition);
				fruites.push_back(piece1);
				fruites.push_back(piece2);
				createBody(piece1, true, true);
				createBody(piece2, false, false);
				if(sprite->getTag() == 5)
				{
					SimpleAudioEngine::sharedEngine()->playEffect("toss_bomb.mp3");
					deathNum += 1;
				}
				else
					SimpleAudioEngine::sharedEngine()->playEffect("squash.mp3");					
				world->DestroyBody(p);
				this->removeChild(sprite, true);

				scoreTotal += 1;
			}
		}	
	}

	//delete old body
	while(b)
	{
		b2Body* p = b;
		b = b->GetNext();
		if(p->GetPosition().y < -50.0f)
		{
			world->DestroyBody(p);
		}
	}

	// Instruct the world to perform a single step of simulation.  
	world->Step(dt, velocityIterations, positionIterations);  
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())  
	{  
		CCSprite *myActor = (CCSprite*)b->GetUserData();  
		if (myActor)  
		{  
			//Synchronize the AtlasSprites position and rotation with the corresponding body  
			myActor->setPosition(CCPointMake( b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO)); 	
			if(myActor->getTag() != kPiece)
				myActor->setRotation(angle);
		}  
	}

	static float countTime = 0;
	countTime += dt;
	if(countTime > gaptime)
	{
		countTime = 0;
		int many = (rand()%7 + 5)/5;
		for(int i=0; i<many; i++)
		{
			int fruitNum = rand()%102;
			switch(fruitNum/20)
			{
			case 0:
				fruit = CCSprite::create("banana.png");
				fruit->setTag(kBanana);
				break;
			case 1:
				fruit = CCSprite::create("grapes.png");
				fruit->setTag(kGrapes);
				break;
			case 2:
				fruit = CCSprite::create("pineapple.png");
				fruit->setTag(kPineapple);
				break;
			case 3:
				fruit = CCSprite::create("strawberry.png");
				fruit->setTag(kStrawberry);
				break;
			case 4:
				fruit = CCSprite::create("watermelon.png");
				fruit->setTag(kWatermelon);
				break;
			case 5:
				fruit = CCSprite::create("bomb.png");
				fruit->setTag(kBomb);
				break;
			}


			fruit->setScale(ratio);
			this->addChild(fruit);

			float randWidth = (float)(rand()%int(winSize.width));
			fruit->setPosition(ccp(randWidth, -50-rand()%50));
			b2CircleShape bananaShape;
			bananaShape.m_radius = ratio * fruit->getContentSize().width/PTM_RATIO/2;
			b2FixtureDef bananaFixtureDef;
			bananaFixtureDef.shape = &bananaShape;
			bananaFixtureDef.isSensor = true;
 			bananaFixtureDef.density = 1;
			b2BodyDef bananaBodyDef;
			bananaBodyDef.type = b2_dynamicBody;
			bananaBodyDef.position.Set(fruit->getPositionX()/PTM_RATIO, fruit->getPositionY()/PTM_RATIO);
			b2Body *bananaBody = world->CreateBody(&bananaBodyDef);
			bananaBody->SetUserData(fruit);
			bananaBody->CreateFixture(&bananaFixtureDef);
			bananaBody->ApplyLinearImpulse(b2Vec2((winSize.width/2 - randWidth)/PTM_RATIO*5, 80.0f + rand()%10), bananaBody->GetWorldCenter() );
			bananaBody->SetLinearDamping(0.2f);

			fruit->retain();
			fruites.push_back(fruit);
		}	
	}

	//space 5 second, change speed
	static float speedChange;
	speedChange += dt;
	if(speedChange > 4.0f && gaptime >= 0.3)
	{
		gaptime *= 0.9f;
		if(gaptime <= 0.3f)
			gaptime = 0.3f;
		speedChange = 0.0f;
	}

	switch(deathNum)
	{
	case 1:
		fillX[0]->setVisible(true);
		break;
	case 2:
		fillX[0]->setVisible(true);
		fillX[1]->setVisible(true);
		break;
	case 3:
		fillX[0]->setVisible(true);
		fillX[1]->setVisible(true);
		fillX[2]->setVisible(true);
		break;
	}
	if(deathNum >= 3)
	{
		CCMenuItemFont* gameOver = CCMenuItemFont::itemWithString("RESTART", this, menu_selector( HelloWorld::restart) );
		gameOver->setFontSize(24);
		gameOver->setColor(ccRED);
		menu = CCMenu::create(gameOver, NULL);
		menu->setPosition( ccp(winSize.width/2 -15, winSize.height/2) );
		this->addChild(menu, 6);
		this->unscheduleUpdate();
	}
}

void HelloWorld::draw()
{ 
	drawLine();
	char str[10];
	sprintf(str, "%d", scoreTotal);
	label->setString(str);
}

void HelloWorld::createBody(CCSprite* sprite, float fruitAngle, bool left)
{
	b2CircleShape shape;
	shape.m_radius = ratio * sprite->getContentSize().width/PTM_RATIO/2;
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.isSensor = true;
	fixtureDef.density = 1.0f;
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.gravityScale = 1.0f;
	bodyDef.position.Set(sprite->getPositionX()/PTM_RATIO, sprite->getPositionY()/PTM_RATIO);
	b2Body* body = world->CreateBody(&bodyDef);
	body->SetUserData(sprite);
	body->CreateFixture(&fixtureDef);
	if(left)
	{
		body->ApplyLinearImpulse(b2Vec2(cos(fruitAngle)*10.0f, sin(fruitAngle)*10.0f), body->GetWorldCenter() );
	}
	else
	{
		body->ApplyLinearImpulse(b2Vec2(-cos(fruitAngle)*10.0f, -sin(fruitAngle)*10.0f), body->GetWorldCenter() );
	}
	
	body->SetLinearDamping(0.2f);
	sprite->retain();
}

void HelloWorld::drawLine()
{
	int tickSubCount = 20;
	int pointListKeepCount = 500;

	for (int i=0; i<tickSubCount ; i++) 
	{
		if (pointList.size() >5)
		{
			pointList.pop_front();
		}
		else 
		{
			break;
		}
	}
	while (pointList.size() > pointListKeepCount) 
	{
		pointList.pop_front();
	}

	float max_lineWidth = 3;
	float min_lineWidth = 1;
	int   alpha_min = 10;
	int   alpha_max =  200;

	int  R = rand()%255;
	int  G = rand()%255;
	int  B = rand()%255;

	int pointListCount = pointList.size();
	list <CCPoint>::iterator it = pointList.begin();
	float pointIndex = 0;
	for(;it!=pointList.end();it++)
	{
		int distanceToMiddle = fabs(pointIndex-pointListCount/2);
		float percent = 1.0-(float)distanceToMiddle/(float)(pointListCount/2.0);
		float lintWidth = min_lineWidth + max_lineWidth*percent;
		int alpha = alpha_min +alpha_max*percent;

		glColor4ub(R,G,B,alpha );
		glPointSize(lintWidth);
		ccDrawPoint( *it);

		pointIndex++;
	}
}

void HelloWorld::restart(CCObject* pSender)
{
	CCDirector::sharedDirector()->replaceScene( CCTransitionFade::create(1.0f, HelloWorld::scene() ));
}