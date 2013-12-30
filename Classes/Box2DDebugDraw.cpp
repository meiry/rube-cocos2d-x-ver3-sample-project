
#include "Box2DDebugDraw.h"

using namespace cocos2d;

Box2DDebugDraw::Box2DDebugDraw(float aRatio) : mRatio(aRatio)
{
}


void Box2DDebugDraw::DrawPolygon(const b2Vec2* aVertices, int32 aVertexCount, const b2Color& aColor)
{
    for (int i = 0; i < DEBUG_DRAW_MAX_VERTICES && i < aVertexCount; i++ )
        mVertices[i].setPoint( mRatio * aVertices[i].x, mRatio * aVertices[i].y );
    
    ccDrawColor4F(aColor.r, aColor.g, aColor.b, 1);
    ccDrawPoly((CCPoint*)aVertices, aVertexCount, true);
}


void Box2DDebugDraw::DrawSolidPolygon(const b2Vec2* aVertices, int32 aVertexCount, const b2Color& aColor)
{
    for (int i = 0; i < DEBUG_DRAW_MAX_VERTICES && i < aVertexCount; i++ )
        mVertices[i].setPoint( mRatio * aVertices[i].x, mRatio * aVertices[i].y );
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ccDrawSolidPoly(mVertices, aVertexCount, ccc4f(aColor.r, aColor.g, aColor.b, 0.5));
    
    ccDrawColor4F(aColor.r, aColor.g, aColor.b, 1);
    ccDrawPoly((CCPoint*)aVertices, aVertexCount, true);
}


void Box2DDebugDraw::DrawCircle(const b2Vec2& aCenter, float32 aRadius, const b2Color& aColor)
{
    ccDrawColor4F(aColor.r, aColor.g, aColor.b, 1);
    ccDrawCircle( CCPointMake(mRatio * aCenter.x, mRatio * aCenter.y), aRadius, 0, DEBUG_DRAW_CIRCLE_SEGMENTS, false);
}


void Box2DDebugDraw::DrawSolidCircle(const b2Vec2& aCenter, float32 aRadius, const b2Vec2& aAxis,
		const b2Color& aColor)
{
    const float coef = 2.0f * (float)M_PI / DEBUG_DRAW_CIRCLE_SEGMENTS;
    
    mVertices[0].setPoint( mRatio * aCenter.x, mRatio * aCenter.y );
    for(unsigned int i = 0; i <= DEBUG_DRAW_CIRCLE_SEGMENTS; i++) {
        float rads = i*coef;
        GLfloat j = aRadius * cosf(rads) + mRatio * aCenter.x;
        GLfloat k = aRadius * sinf(rads) + mRatio * aCenter.y;
        
        mVertices[i].setPoint( j, k );
    }
    ccDrawSolidPoly(mVertices, DEBUG_DRAW_CIRCLE_SEGMENTS, ccc4f(aColor.r, aColor.g, aColor.b, 0.5));
    
    ccDrawColor4F(aColor.r, aColor.g, aColor.b, 1);
    ccDrawCircle( CCPointMake(mRatio * aCenter.x, mRatio * aCenter.y), aRadius, 0, DEBUG_DRAW_CIRCLE_SEGMENTS, false);
    
    b2Vec2 p = aCenter + aRadius * aAxis;
    ccDrawLine(ccp(aCenter.x, aCenter.y), ccp(p.x, p.y));
}


void Box2DDebugDraw::DrawSegment(const b2Vec2& aP1, const b2Vec2& aP2, const b2Color& aColor)
{
    ccDrawColor4F(aColor.r, aColor.g, aColor.b, 1);
    ccDrawLine(ccp(aP1.x, aP1.y), ccp(aP2.x, aP2.y));
}


void Box2DDebugDraw::DrawTransform(const b2Transform& aXf)
{
	b2Vec2 p1 = aXf.p, p2;
	const float32 k_axisScale = 0.4f;

	p2 = p1 + k_axisScale * aXf.q.GetXAxis();
	DrawSegment(p1, p2, b2Color(1, 0, 0));

	p2 = p1 + k_axisScale * aXf.q.GetYAxis();
	DrawSegment(p1, p2, b2Color(0, 1, 0));
}


void Box2DDebugDraw::DrawPoint(const b2Vec2& aP, float32 aSize, const b2Color& aColor)
{
    ccDrawColor4F(aColor.r, aColor.g, aColor.b, 1);
	ccDrawPoint( CCPointMake(mRatio * aP.x, mRatio * aP.y) );
}


void Box2DDebugDraw::DrawString(int aX, int aY, const char* aString, ...)
{
}


void Box2DDebugDraw::DrawAABB(b2AABB* aAabb, const b2Color& aColor)
{
	mVertices[0].setPoint( aAabb->lowerBound.x * mRatio, aAabb->lowerBound.y * mRatio );
	mVertices[1].setPoint( aAabb->upperBound.x * mRatio, aAabb->lowerBound.y * mRatio );
	mVertices[2].setPoint( aAabb->upperBound.x * mRatio, aAabb->upperBound.y * mRatio );
	mVertices[3].setPoint( aAabb->lowerBound.x * mRatio, aAabb->upperBound.y * mRatio );
	ccDrawPoly((CCPoint*)mVertices, 8, true);
}

