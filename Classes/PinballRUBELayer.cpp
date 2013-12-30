//  Author: Chris Campbell - www.iforce2d.net
//  -----------------------------------------
//
//  PinballRUBELayer
//
//  See header file for description.
//

#include "PinballRUBELayer.h"
#include "rubestuff/b2dJson.h"
#include "QueryCallbacks.h"

using namespace std;
USING_NS_CC;

// Standard Cocos2d method, simply returns a scene with an instance of this class as a child
Scene* PinballRUBELayer::scene()
{
    Scene *scene = Scene::create();
    
    // add layer as a child to scene
    PinballRUBELayer* layer = new PinballRUBELayer();
    layer->init();// do things that require virtual functions (can't do in constructor)
    scene->addChild(layer);
    layer->release();
    
    // only for this demo project, you can remove this in your own app
    scene->addChild(layer->setupMenuLayer());
    
    return scene;
}

void PinballRUBELayer::onEnter()
{
    Layer::onEnter();
	 
    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = CC_CALLBACK_2(PinballRUBELayer::onTouchesBegan, this);
   
    listener->onTouchesEnded = CC_CALLBACK_2(PinballRUBELayer::onTouchesEnded, this);
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this); 
}
// Override superclass to load different RUBE scene
string PinballRUBELayer::getFilename()
{
    return "pinball.json";
}


// Override superclass to set different starting offset
CCPoint PinballRUBELayer::initialWorldOffset()
{
    //place (0,0) of physics world at center of bottom edge of screen
    Size s = Director::getInstance()->getWinSize();
    return CCPointMake( s.width/2, 0 );
}


// Override superclass to set different starting scale
float PinballRUBELayer::initialWorldScale()
{
    Size s = Director::getInstance()->getWinSize();
    if ( s.height == 1136 || s.height == 568 ) // iPhone5 portrait
        // the pinball table was made for 4:3 displays so basing the scale on
        // the height on the 16:9 iPhone5 portrait orientation will result in the
        // sides of the table being off screen and not visible. For this case
        // we would rather base the scale on the width of the device to make
        // sure the whole table is visible
        return s.width / 32; // the table is 32 physics units wide
    else
        // the pinball table is 48 physics units high. The screen should be a little
        // higher than that to allow some room for the menu at the top. Subtract
        // 40 pixels from the current screen height before doing the usual division.
        return (s.height - 40) / 48;
}


// use some names for fixture tags to keep things understandable
enum {
    PFT_LEVEL1  = 1,    // level-switch sensor for lower level
    PFT_LEVEL2  = 2,    // level-switch sensor for upper level
    PFT_BALL    = 3,    // the ball
    PFT_DROP    = 4,    // the vertical drop at the end of those chute thingies in the upper level
    PFT_GUTTER  = 5,    // the gutter at the bottom of the table
    
    PFT_MAX
};
const char* fixtureNames[] = {
    "level1",
    "level2",
    "ball",
    "drop",
    "gutter"
};

// This is called after the Box2D world has been loaded, and while the b2dJson information
// is still available to do extra loading. Here is where we obtain the named items in the scene.
void PinballRUBELayer::afterLoadProcessing(b2dJson* json)
{
    RUBELayer::afterLoadProcessing(json); // loads world and images
    
    // find the ball fixture
    m_ballFixture = json->getFixtureByName("ball");
    
    // record the initial position of the ball body
    if ( b2Body* ballBody = json->getBodyByName("ball") )
        m_ballStartPosition = ballBody->GetPosition();
    
    // find flipper button fixtures
    m_leftFlipperButtonFixture = json->getFixtureByName("leftFlipperButton");
    m_rightFlipperButtonFixture = json->getFixtureByName("rightFlipperButton");
    
    // find flipper joints
    json->getJointsByName("flip_left", m_leftFlipperJoints);
    json->getJointsByName("flip_right", m_rightFlipperJoints);
    
    // find the imageInfo for the ball image
    m_ballSprite = NULL;
    for (set<RUBEImageInfo*>::iterator it = m_imageInfos.begin(); it != m_imageInfos.end(); ++it) {
        RUBEImageInfo* imgInfo = *it;
        if ( imgInfo->name == "ball" )
            m_ballSprite = imgInfo->sprite;
    }
    
    // look for fixtures that will be used in the collision callback, and
    // give them a 'tag' by setting their user data. Usually, the user data
    // would be a pointer to some type of object or structure with more
    // information (see the PlanetCute demo for an example) but for this
    // case we can simply set the user data to a plain old integer to denote
    // the type of each fixture.
    int numFixtureNames = sizeof(fixtureNames) / sizeof(char*);
    for (int i = 0; i < numFixtureNames; i++) {
        
        // get a list of fixtures with this name
        vector<b2Fixture*> switchFixtures;
        json->getFixturesByName(fixtureNames[i], switchFixtures);
        
        // set the 'tag' of all fixtures - note that this will give fixtures on
        // level1 and level2 tags of 1 and 2 respectively.
        for (int k = 0; k < switchFixtures.size(); k++)
            switchFixtures[k]->SetUserData((void*)(i+1));
    }
    
    // create a contact listener and let the Box2D world know about it
    m_contactListener = new PinballContactListener();
    m_world->SetContactListener( m_contactListener );
    
    // set the initial states for the contact listener info
    m_contactListener->m_needToSetLevel = 0;
    m_contactListener->m_needToHaltVelocity = false;
    m_contactListener->m_needToResetGame = false;
    
    // set initial flipper states to off
    m_leftFlipperTouch = NULL;
    m_rightFlipperTouch = NULL;
    
}


// This method should undo anything that was done by afterLoadProcessing, and make sure
// to call the superclass method so it can do the same
void PinballRUBELayer::clear()
{
    m_leftFlipperButtonFixture = NULL;
    m_rightFlipperButtonFixture = NULL;
    
    m_leftFlipperJoints.clear();
    m_rightFlipperJoints.clear();
    
    m_leftFlipperTouch = NULL;
    m_rightFlipperTouch = NULL;
	 
	//delete m_contactListener;
	 
    
    RUBELayer::clear();
}


// after every physics step, we need to check if the collision listener detected
// a collision that we should do something about. This info will be stored in the
// collision listener class itself
void PinballRUBELayer::update(float dt)
{
    //superclass will Step the physics world
    RUBELayer::update(dt);
    
    // adjust the motor speed of the flippers according to whether the player
    // is touching them or not. 
    float leftFlippersMotorSpeed = m_leftFlipperTouch ? 20 : -10;
    float rightFlippersMotorSpeed = m_rightFlipperTouch ? -20 : 10;
    for (int i = 0; i < m_leftFlipperJoints.size(); i++)
        ((b2RevoluteJoint*)m_leftFlipperJoints[i])->SetMotorSpeed( leftFlippersMotorSpeed );
    for (int i = 0; i < m_rightFlipperJoints.size(); i++)
        ((b2RevoluteJoint*)m_rightFlipperJoints[i])->SetMotorSpeed( rightFlippersMotorSpeed );
    
    // if the table needs to be reset, place the ball at the starting position
    if ( m_contactListener->m_needToResetGame )
        m_ballFixture->GetBody()->SetTransform(m_ballStartPosition, 0);    
    
    // if the ball should be halted, simply set its velocity to zero
    if ( m_contactListener->m_needToHaltVelocity )
        m_ballFixture->GetBody()->SetLinearVelocity(b2Vec2(0,0));
    
    // if the ball touched a level-switch sensor, update the filter mask bits
    // for the ball fixture according to the level it just entered
    if ( m_contactListener->m_needToSetLevel ) {

        // get the current filter
        b2Filter filter = m_ballFixture->GetFilterData();
        
        // update just the mask bits (the 4 here is to make sure the ball always
        // collides with the level-switch sensors
        filter.maskBits = 4 | m_contactListener->m_needToSetLevel;
        
        // set the updated filter in the fixture, and refilter (the refilter may
        // be necessary in cases where the ball is already touching something that
        // it could collide with but now can't, or vice versa
        m_ballFixture->SetFilterData(filter);
        m_ballFixture->Refilter();
    }
    
    // if the level was switched, we should also reorder the ball sprite to
    // make it appear to go under the upper level parts of the table
    if ( m_ballSprite ) {
        if ( m_contactListener->m_needToSetLevel == 1 )
            reorderChild(m_ballSprite, 15);
        if ( m_contactListener->m_needToSetLevel == 2 )
            reorderChild(m_ballSprite, 25);
    }

    // reset all the info in the collision listener
    m_contactListener->m_needToResetGame = false;
    m_contactListener->m_needToHaltVelocity = false;
    m_contactListener->m_needToSetLevel = 0;
}


void PinballRUBELayer::draw()
{
    //don't do debug draw
}


// check if the touch was on one of the flipper buttons
void PinballRUBELayer::onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, Event* event)
{
    // Calling the superclass handles the mouse joint dragging for the launcher,
    // but it also allows the user to grab the ball, and zoom and pan the scene.
    // For a real application you would want to implement a better method here.
    RUBELayer::onTouchesBegan(touches, event);
    
	for ( auto &touch: touches )
        {
        //cocos2d::Touch* touch = (cocos2d::Touch*)touch;
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
        
        // if the touched fixture was one of the flipper buttons, set that touch as
        // the current touch for that button
        if (callback.m_fixture == m_leftFlipperButtonFixture)
            m_leftFlipperTouch = touch;
        if (callback.m_fixture == m_rightFlipperButtonFixture)
            m_rightFlipperTouch = touch;
    }
}


// notice that we are not overriding ccTouchesMoved, so you can zoom and pan the layer :)


// when a touch ends, we need to check if it was a touch for one of the
// flipper buttons. If so, set the current touch for that button to nil
void PinballRUBELayer::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    // call the superclass to get the body-dragging behavior
    RUBELayer::onTouchesEnded(touches, event);
    
   for ( auto &touch: touches )
    {
       // Touch* touch = (Touch*)touch;
        if ( touch == m_leftFlipperTouch )
            m_leftFlipperTouch = NULL;
        if ( touch == m_rightFlipperTouch )
            m_rightFlipperTouch = NULL;
    }
}






//-------------- Contact listener class ------------------------

// This function will be called by Box2D during the Step function. We can
// use the info in the contact to see which two fixtures collided, but we
// cannot fiddle with the Box2D world inside the contact listener.
//
// Instead, we just record what needs to be done after the Step has finished,
// and the PinballRUBELayer class will deal with it (in the tick method).
//
void PinballContactListener::BeginContact(b2Contact* contact)
{
    // we know that fixture datas are just integers
    int tagA = (int)contact->GetFixtureA()->GetUserData();
    int tagB = (int)contact->GetFixtureB()->GetUserData();
    
    // both fixtures should have a tag otherwise we don't care about them
    if ( tagA == 0 || tagB == 0 )
        return;
    
    
    // the repetitive and error-prone nature of these comparisons leads me to
    // make a macro to keep it easier to read...
#define COMPARETAGS( tag1, tag2 ) \
    ( (tagA == tag1 && tagB == tag2) || (tagA == tag2 && tagB == tag1) )
    
    
    // check if the ball touched a level-switch sensor
    if ( COMPARETAGS(PFT_BALL, PFT_LEVEL1) )
        m_needToSetLevel = 1;
    if ( COMPARETAGS(PFT_BALL, PFT_LEVEL2) )
        m_needToSetLevel = 2;
    
    // check if the ball touched the vertical drop at the end of an upper-level chute
    if ( COMPARETAGS(PFT_BALL, PFT_DROP) ) {
        m_needToHaltVelocity = true;
        m_needToSetLevel = 1;
    }
    
    // check if the ball touched the gutter at the bottom of the table
    if ( COMPARETAGS(PFT_BALL, PFT_GUTTER) ) {
        m_needToHaltVelocity = true;
        m_needToResetGame = true;
        m_needToSetLevel = 2;
    }
}









