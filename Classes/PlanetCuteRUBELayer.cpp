//  Author: Chris Campbell - www.iforce2d.net
//  -----------------------------------------
//
//  PlanetCuteRUBELayer
//
//  See header file for description.
//


#include "PlanetCuteRUBELayer.h"
#include "SimpleAudioEngine.h"
#include "UIControlsRUBELayer.h"

using namespace std;
using namespace cocos2d;
using namespace CocosDenshion;

// Standard Cocos2d method, simply returns a scene with an instance of this class as a child
Scene* PlanetCuteRUBELayer::scene()
{
    Scene *scene = Scene::create();
    
    // add layer as a child to scene
    PlanetCuteRUBELayer* layer = new PlanetCuteRUBELayer();        
    layer->init();// do things that require virtual functions (can't do in constructor)
    scene->addChild(layer);
    layer->release();
    
    // only for this demo project, you can remove this in your own app
    scene->addChild(layer->setupMenuLayer());
    
    // uncomment this to add a menu screen over the top of this layer
    //scene->addChild( UIControlsRUBELayer::create() );
        
    return scene;
}


// Override superclass to load different RUBE scene
string PlanetCuteRUBELayer::getFilename()
{
    return "planetcute.json";
}


// Override superclass to set different starting offset
CCPoint PlanetCuteRUBELayer::initialWorldOffset()
{
    //place (0,0) of physics world at center of bottom edge of screen
    Size s = Director::getInstance()->getWinSize();
    return CCPointMake( s.width/2, 0 );
}


// Override superclass to set different starting scale
float PlanetCuteRUBELayer::initialWorldScale()
{
    Size s = Director::getInstance()->getWinSize();
    return s.height / 8; //screen will be 8 physics units high
}


// This is called after the Box2D world has been loaded, and while the b2dJson information
// is still available to do extra loading. Here is where we obtain the named items in the scene.
void PlanetCuteRUBELayer::afterLoadProcessing(b2dJson* json)
{
    // call superclass method to load images etc
    RUBELayer::afterLoadProcessing(json);
    
    // preload the sound effects
    SimpleAudioEngine::sharedEngine()->preloadEffect("jump.wav");
    SimpleAudioEngine::sharedEngine()->preloadEffect("pickupgem.wav");
    SimpleAudioEngine::sharedEngine()->preloadEffect("pickupstar.wav");

    // find player body and foot sensor fixture
    m_playerBody = json->getBodyByName("player");
    m_footSensorFixture = json->getFixtureByName("footsensor");
    
    // find all fixtures in the scene named 'pickup' and loop over them
    std::vector<b2Fixture*> pickupFixtures;
    json->getFixturesByName("pickup", pickupFixtures);
    for (int i = 0; i < pickupFixtures.size(); i++) {
        b2Fixture* f = pickupFixtures[i];
        
        // For every pickup fixture, we create a FixtureUserData to set in
        // the user data.
        PlanetCuteFixtureUserData* fud = new PlanetCuteFixtureUserData;
        m_allPickups.insert(fud);
        f->SetUserData( fud );

        // set some basic properties of the FixtureUserData
        fud->fixtureType = FT_PICKUP;
        fud->body = f->GetBody();
        fud->originalPosition = f->GetBody()->GetPosition();

        // use the custom properties given to the fixture in the RUBE scene
        fud->pickupType = (_pickupType)json->getCustomInt(f, "pickuptype", PT_GEM);
        fud->bounceSpeedH = json->getCustomFloat(f, "horizontalbouncespeed");
        fud->bounceSpeedV = json->getCustomFloat(f, "verticalbouncespeed");
        fud->bounceWidth  = json->getCustomFloat(f, "bouncewidth");
        fud->bounceHeight = json->getCustomFloat(f, "bounceheight");
        
        // these "bounce deltas" are just a number given to sin when wobbling
        // the pickups. Each pickup has its own value to stop them from looking
        // like they are moving in unison.
        fud->bounceDeltaH = CCRANDOM_0_1() * M_PI_2;
        fud->bounceDeltaV = CCRANDOM_0_1() * M_PI_2;
    }
    
    // find the imageInfos for the text instruction images. Sprites 2 and 3 are
    // hidden initially
    m_instructionsSprite1 = NULL;
    m_instructionsSprite2 = NULL;
    m_instructionsSprite2 = NULL;
    for (set<RUBEImageInfo*>::iterator it = m_imageInfos.begin(); it != m_imageInfos.end(); ++it) {
        RUBEImageInfo* imgInfo = *it;
        if ( imgInfo->name == "instructions1" )
            m_instructionsSprite1 = imgInfo;
        if ( imgInfo->name == "instructions2" ) {
            m_instructionsSprite2 = imgInfo;
            m_instructionsSprite2->sprite->setOpacity(0); // hide
        }
        if ( imgInfo->name == "instructions3" ) {
            m_instructionsSprite3 = imgInfo;
            m_instructionsSprite3->sprite->setOpacity(0); // hide
        }
    }
    
    // Create a contact listener and let the Box2D world know about it.
    m_contactListener = new PlanetCuteContactListener();
    m_world->SetContactListener( m_contactListener );

    // Give the listener a reference to this class, to use in the callback
    m_contactListener->m_layer = this;

    // set the movement control touches to nil initially
    m_leftTouch = NULL;
    m_rightTouch = NULL;
    
    // initialize the values for ground detection
    m_numFootContacts = 0;
    m_jumpTimeout = 0;
    
    // camera will start at body position
    m_cameraCenter = m_playerBody->GetPosition();
}


// This method should undo anything that was done by afterLoadProcessing, and make sure
// to call the superclass method so it can do the same
void PlanetCuteRUBELayer::clear()
{
    SimpleAudioEngine::sharedEngine()->unloadEffect("jump.wav");
    SimpleAudioEngine::sharedEngine()->unloadEffect("pickupgem.wav");
    SimpleAudioEngine::sharedEngine()->unloadEffect("pickupstar.wav");
        
    m_playerBody = NULL;
    m_footSensorFixture = NULL;
    
    //delete m_contactListener;
    
    for (set<PlanetCuteFixtureUserData*>::iterator it = m_allPickups.begin(); it != m_allPickups.end(); ++it)
        delete *it;
    m_allPickups.clear();
        
    RUBELayer::clear();
}


// after every physics step, we need to check if the collision listener detected
// a collision that we should do something about. This info will be stored in the
// m_pickupsToProcess set
void PlanetCuteRUBELayer::update(float dt)
{
    // superclass will Step the physics world
    RUBELayer::update(dt);
    
    // loop over the list of pickups that were touched
    for (set<PlanetCuteFixtureUserData*>::iterator it = m_pickupsToProcess.begin(); it != m_pickupsToProcess.end(); ++it) {
        PlanetCuteFixtureUserData* fud = *it;
        
        // play sound effect
        if ( fud->pickupType == PT_GEM )
            SimpleAudioEngine::sharedEngine()->playEffect("pickupgem.wav", false);
        else if ( fud->pickupType == PT_STAR ) {
            SimpleAudioEngine::sharedEngine()->playEffect("pickupstar.wav", false);
            // if the player touched the star, we also want to show the "Well done!" sprite
            if ( m_instructionsSprite3 )
                m_instructionsSprite3->sprite->setOpacity(255);
        }
        
        // clean up all references to the pickup that was collected
        removeBodyFromWorld(fud->body);
        m_allPickups.erase(fud);
        delete fud;
        
        // you would also keep score here, in a real game
    }
    
    // after processing all the pickups in the loop above, we need to clear the list
    m_pickupsToProcess.clear();
    
    // make the pickups wobble around
    for (set<PlanetCuteFixtureUserData*>::iterator it = m_allPickups.begin(); it != m_allPickups.end(); ++it) {
        PlanetCuteFixtureUserData* fud = *it;
        fud->bounceDeltaH += fud->bounceSpeedH;
        fud->bounceDeltaV += fud->bounceSpeedV;
        b2Vec2 pos = fud->originalPosition + b2Vec2( fud->bounceWidth * sin(fud->bounceDeltaH), fud->bounceHeight * sin(fud->bounceDeltaV) );
        fud->body->SetTransform( pos, 0 );
    }
    
    // decrement the jump timer - if this gets to zero the player can jump again
    m_jumpTimeout--;
    
    // define the sideways force that the player can move with
    float maxSpeed = 4;
    float maxForce = 20;
    if ( m_numFootContacts < 1 )
        maxForce *= 0.25; // reduce sideways force when in the air
    
    // determine whether the player should moved left or right depending
    // on which touches are currently active
    b2Vec2 currentVelocity = m_playerBody->GetLinearVelocity();
    float force = 0;
    bool moving = false;
    if ( m_leftTouch && !m_rightTouch && currentVelocity.x > -maxSpeed ) {
        force = -maxForce;
        moving = true;
    }
    else if ( !m_leftTouch && m_rightTouch && currentVelocity.x < maxSpeed ) {
        force = maxForce;
        moving = true;
    }
    else if ( !m_leftTouch && !m_rightTouch ) {
        // if no touches are active, the player wants to stop. Apply a half-strength
        // force in the opposite direction to current movement, scaled down so that
        // it is zero when actually stopped. This has the unfortunate side-effect of
        // preventing the player from moving together with a moving platform that it
        // is standing on, but that's a problem for another day.
        force = -currentVelocity.x * maxForce * 0.5;
    }
    
    // apply whatever force we decided on above...
    m_playerBody->ApplyForce( b2Vec2(force,0), m_playerBody->GetWorldCenter(), true );
    
    // if we are moving, the user no longer needs to see the "Touch left/right to move" message,
    // so we remove it from the layer. We also show the "Touch both sides to jump" sprite now.
    if ( moving && m_instructionsSprite1 ) {
        removeImageFromWorld(m_instructionsSprite1);
        m_instructionsSprite1 = NULL;
        if ( m_instructionsSprite2 )
            m_instructionsSprite2->sprite->setOpacity(255);
    }
    
    // if all the conditions are met to allow a jump, use ApplyLinearImpulse to the player body.
    if ( m_leftTouch && m_rightTouch && m_numFootContacts > 0 && m_jumpTimeout <= 0 ) {
        m_playerBody->ApplyLinearImpulse( b2Vec2(0,5), m_playerBody->GetWorldCenter(), true );
        m_jumpTimeout = 15; // 1/4 second at 60 fps (prevents repeated jumps while the foot sensor is still touching the ground)
        SimpleAudioEngine::sharedEngine()->playEffect("jump.wav");
        if ( m_instructionsSprite2 ) {
            // the user no longer needs to see the message explaining how to jump, so remove it
            removeImageFromWorld(m_instructionsSprite2);
            m_instructionsSprite2 = NULL;
        }
    }

    // decide on a new point for the camera center. Look at where the player will be 2 seconds in the future
    // if they keep moving in the current direction, and move the camera a little bit toward that point.
    float cameraSmooth = 0.012;
    b2Vec2 playerPositionSoon = m_playerBody->GetPosition() + 2 * currentVelocity; //position 2 seconds from now
    m_cameraCenter = (1 - cameraSmooth) * m_cameraCenter + cameraSmooth * playerPositionSoon;
    
    // find out where the camera center is now, in pixels.
    b2Vec2 cameraCenterInPixels = getScale() * m_cameraCenter;
    
    // we want to put move the layer so that cameraCenterInPixels is at the center of the screen, so
    // the layer needs to move by the difference between the screen center and cameraCenterInPixels
    Size s = Director::getInstance()->getWinSize();
    CCPoint sceneOffset = CCPointMake(s.width/2 - cameraCenterInPixels.x, s.height/2 - cameraCenterInPixels.y);
    setPosition(sceneOffset);
}


// record when the user touches the left/right sides of the screen
void PlanetCuteRUBELayer::onTouchesBegan(const std::vector<Touch*>& touches, Event* event)
{
    Size s = Director::getInstance()->getWinSize();
    int middleOfScreen = s.width / 2;
    
    for ( auto &touch: touches )
        {
        //Touch* touch = (Touch*)touch;
        // if the touch location was on a side of the screen that we don't already
        // have a touch for, set that touch as the current touch for that side
        Point screenPos = touch->getLocationInView();
        if ( !m_leftTouch && screenPos.x < middleOfScreen )
            m_leftTouch = touch;
        if ( !m_rightTouch && screenPos.x > middleOfScreen )
            m_rightTouch = touch;
    }
}


void PlanetCuteRUBELayer::onTouchesMoved(const std::vector<Touch*>& touches, Event* event)
{
    // dont let the user pan and zoom the scene, or drag anything around :)
}


// when a touch ends, we need to check if it was one of the touches for the
// screen sides that we recorded in ccTouchesBegan. If so, set the current
// touch for that screen side to nil
void PlanetCuteRUBELayer::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    for ( auto &touch: touches )
        {
        //Touch* touch = (Touch*)touch;
        if ( touch == m_leftTouch )
            m_leftTouch = NULL;
        if ( touch == m_rightTouch )
            m_rightTouch = NULL;
    }
}





//-------------- Contact listener ------------------------

void PlanetCuteContactListener::BeginContact(b2Contact* contact)
{
    PlanetCuteRUBELayer* layer = (PlanetCuteRUBELayer*)m_layer;
    b2Fixture* fA = contact->GetFixtureA();
    b2Fixture* fB = contact->GetFixtureB();
    
    if ( fA == layer->m_footSensorFixture || fB == layer->m_footSensorFixture ) 
        layer->m_numFootContacts++;
    //CCLOG("Num foot contacts: %d", layer->m_numFootContacts);
    
    PlanetCuteFixtureUserData* fudA = (PlanetCuteFixtureUserData*)fA->GetUserData();
    PlanetCuteFixtureUserData* fudB = (PlanetCuteFixtureUserData*)fB->GetUserData();
    
    if ( fudA && fudA->fixtureType == FT_PICKUP && fB->GetBody() == layer->m_playerBody ) 
        layer->m_pickupsToProcess.insert(fudA);
    if ( fudB && fudB->fixtureType == FT_PICKUP && fA->GetBody() == layer->m_playerBody ) 
        layer->m_pickupsToProcess.insert(fudB);
}

void PlanetCuteContactListener::EndContact(b2Contact* contact)
{
    PlanetCuteRUBELayer* layer = (PlanetCuteRUBELayer*)m_layer;
    b2Fixture* fA = contact->GetFixtureA();
    b2Fixture* fB = contact->GetFixtureB();
    
    if ( fA == layer->m_footSensorFixture || fB == layer->m_footSensorFixture )
        layer->m_numFootContacts--;
    //CCLOG("Num foot contacts: %d", layer->m_numFootContacts);
}
















