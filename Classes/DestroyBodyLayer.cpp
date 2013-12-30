//  Author: Chris Campbell - www.iforce2d.net
//  -----------------------------------------
//
//  DestroyBodyLayer
//
//  See header file for description.
//

#include "DestroyBodyLayer.h"
#include "QueryCallbacks.h"

using namespace std;
using namespace cocos2d;


// Standard Cocos2d method, simply returns a scene with an instance of this class as a child
Scene* DestroyBodyLayer::scene()
{
    Scene *scene = Scene::create();
    
    // add layer as a child to scene
    DestroyBodyLayer* layer = new DestroyBodyLayer();
    layer->init();// do things that require virtual functions (can't do in constructor)
    scene->addChild(layer);
    layer->release();
    
    // only for this demo project, you can remove this in your own app
    scene->addChild(layer->setupMenuLayer());
    
    return scene;
}


// Override this to find the body that was touched and remove it.
void DestroyBodyLayer::onTouchesBegan(const std::vector<Touch*>& touches, Event* event)
{   
    Touch *touch = (Touch*)touches[0];
    Point screenPos = touch->getLocationInView();
    b2Vec2 worldPos = screenToWorld(screenPos);
    
    // Make a small box around the touched point to query for overlapping fixtures
    b2AABB aabb;
    b2Vec2 d(0.001f, 0.001f);
    aabb.lowerBound = worldPos - d;
    aabb.upperBound = worldPos + d;
    
    // Query the world for overlapping fixtures (the TouchDownQueryCallback simply
    // looks for any fixture that contains the touched point)
    TouchDownQueryCallback callback(worldPos);
    m_world->QueryAABB(&callback, aabb);
    
    // Check if we found something, and it was a dynamic body (could also destroy static
    // bodies but we want to keep the pinch-zoom and pan from the superclass, and it's
    // hard not to touch the ground body in this scene)
    if (callback.m_fixture && callback.m_fixture->GetBody()->GetType() == b2_dynamicBody)
    {
        b2Body* touchedBody = callback.m_fixture->GetBody();
        removeBodyFromWorld(touchedBody);
    }
}


