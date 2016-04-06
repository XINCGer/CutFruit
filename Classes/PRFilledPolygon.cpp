#include "PRFilledPolygon.h"
#include "PRRatcliffTriangulator.h"

using namespace cocos2d;

PRFilledPolygon::~PRFilledPolygon(){
	delete[] areaTrianglePoints;
	delete[] textureCoordinates;

	CC_SAFE_RELEASE_NULL(texture);
	CC_SAFE_RELEASE_NULL(triangulator);
}

PRFilledPolygon* PRFilledPolygon::filledPolygon(cocos2d::CCArray* polygonPoints, cocos2d::CCTexture2D* fillTexture){
	PRFilledPolygon* pr = new PRFilledPolygon;
	if(pr && pr->initWithMember(polygonPoints, fillTexture)){
		pr->autorelease();
		return pr;
	}
	CC_SAFE_DELETE(pr);
	return NULL;
}

PRFilledPolygon* PRFilledPolygon::filledPolygon(cocos2d::CCArray* polygonPoints, cocos2d::CCTexture2D* fillTexture, PRTriangulator* polygonTriangulator){
	PRFilledPolygon* pr = new PRFilledPolygon;
	if(pr && pr->initWithMember(polygonPoints, fillTexture, polygonTriangulator)){
		pr->autorelease();
		return pr;
	}
	CC_SAFE_DELETE(pr);
	return NULL;
}

bool PRFilledPolygon::initWithMember(cocos2d::CCArray* polygonPoints, cocos2d::CCTexture2D* fillTexture){
	bool bRet = false;
	do 
	{
		PRRatcliffTriangulator* prrt = new PRRatcliffTriangulator;
		CC_BREAK_IF(!prrt);
		prrt->autorelease();

		CC_BREAK_IF(! initWithMember(polygonPoints, fillTexture, prrt));

		bRet = true;
	} while (0);
	return bRet;
}

bool PRFilledPolygon::initWithMember(cocos2d::CCArray* polygonPoints, cocos2d::CCTexture2D* fillTexture, PRTriangulator* polygonTriangulator){
	bool bRet = false;
	do 
	{
		areaTrianglePoints = NULL;
		textureCoordinates = NULL;
		texture = NULL;
		triangulator = NULL;

		this->setShaderProgram(CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTexture));

		this->setTriangulator(polygonTriangulator);
		
		this->setPoints(polygonPoints);
		this->setTexture(fillTexture);
		

		bRet = true;
	} while (0);
	return bRet;
}

void PRFilledPolygon::setPoints(cocos2d::CCArray* points){
	if(areaTrianglePoints){
		delete[] areaTrianglePoints;
	}
	if(textureCoordinates){
		delete[] textureCoordinates;
	}

	CCArray* triangulatedPoints = triangulator->triangulateVertices(points);

	areaTrianglePointCount = triangulatedPoints->count();
	areaTrianglePoints = new CCPoint[areaTrianglePointCount];
	textureCoordinates = new CCPoint[areaTrianglePointCount];

	for(int i = 0; i < areaTrianglePointCount; i++){
		CCString* temp = (CCString*)triangulatedPoints->objectAtIndex(i); 

		areaTrianglePoints[i] = CCPointFromString(temp->getCString());
	}

	//this->calculateTextureCoordinates();
}

void PRFilledPolygon::calculateTextureCoordinates(){
	for(int i = 0; i < areaTrianglePointCount; i++){
		textureCoordinates[i] = ccpMult(areaTrianglePoints[i], 1.0f / texture->getPixelsWide() * CC_CONTENT_SCALE_FACTOR());
		textureCoordinates[i].y = 1 - textureCoordinates[i].y;
	}
}

void PRFilledPolygon::draw(){
	CC_NODE_DRAW_SETUP();

	ccGLBindTexture2D(texture->getName());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	ccGLBlendFunc(blendFunc.src, blendFunc.dst);

	ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position | kCCVertexAttribFlag_TexCoords );

	glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, areaTrianglePoints);
	glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, textureCoordinates);

	glDrawArrays(GL_TRIANGLES, 0, areaTrianglePointCount);
}

void PRFilledPolygon::updateBlendFunc(){
	// it's possible to have an untextured sprite
	if( !texture || ! texture->hasPremultipliedAlpha() ) {
		blendFunc.src = GL_SRC_ALPHA;
		blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
		
	} else {
		blendFunc.src = CC_BLEND_SRC;
		blendFunc.dst = CC_BLEND_DST;
	}
}

void PRFilledPolygon::setTexture(cocos2d::CCTexture2D* var){
	if(texture != var){
		CC_SAFE_RELEASE_NULL(texture);
		CC_SAFE_RETAIN(var);
		texture = var;

		ccTexParams texParams = { GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT };
		texture->setTexParameters(&texParams);

		this->updateBlendFunc();
		this->calculateTextureCoordinates();
	}
}

CCTexture2D* PRFilledPolygon::getTexture(){
	return texture;
}

void PRFilledPolygon::setTriangulator(PRTriangulator* var){
	if(triangulator != var){
		CC_SAFE_RELEASE_NULL(triangulator);
		CC_SAFE_RETAIN(var);
		triangulator = var;
	}
}

PRTriangulator* PRFilledPolygon::getTriangulator(){
	return triangulator;
}