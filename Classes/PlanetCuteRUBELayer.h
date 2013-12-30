//  Author: Chris Campbell - www.iforce2d.net
//  -----------------------------------------
//
//  PlanetCuteRUBELayer
//
//  A simple platform game to demonstrate the following:
//   - detecting when a character is on the ground
//   - basic movement control
//   - basic camera following
//   - detecting collision with pickups
//   - simple sound effects
//   - removing bodies and images from world
//   - obtaining named items from RUBE scene
//   - using RUBE custom properties
//
//  The character can move and jump, and collect pickups. There
//  are two types of pickup which each play a different sound.
//
//  The pickups in the game have their category set from a custom
//  property given to them in RUBE, and are wobbled around according to
//  some custom properties as well.
//

#ifndef PLANET_CUTE_RUBE_LAYER
#define PLANET_CUTE_RUBE_LAYER

#include "RUBELayer.h"
#include "rubestuff/b2dJson.h"

// broad categories to organize fixtures into
enum _fixtureType {
    FT_PLAYER,
    FT_PICKUP
};

// more detailed sub-categories for pickups
enum _pickupType {
    PT_GEM,
    PT_STAR
};

//
//  PlanetCuteFixtureUserData
//
//  For this example we want to have more information associated
//  with items in the scene than a simple integer tag. This class
//  holds all the info about a fixture, and will be set in the
//  fixture user data for relevant fixtures.
//
//  A lot of the information held in this class actually relates
//  to bodies, so it could just as easily been placed in the user
//  data for bodies instead, especially for this example because
//  we're mostly dealing with the gems, which all have their own
//  body anyway.
//
//  On the other hand, when we have things to collide with that
//  do not have a body each (eg. many fixtures attached to the
//  ground body) then we will need to keep user data in fixtures
//  to differentiate them.
//
//  A body can be obtained from a fixture with GetBody(), so for
//  this demo we will just place all user data in fixtures, and
//  use GetBody() where necessary to use the body itself.
//

struct PlanetCuteFixtureUserData
{
    _fixtureType fixtureType;
    _pickupType pickupType;
    b2Body* body;
        
    b2Vec2 originalPosition;
    float bounceDeltaH;
    float bounceDeltaV;
    float bounceSpeedH;
    float bounceSpeedV;
    float bounceWidth;
    float bounceHeight;
};

//--------------------------------------

// The BeginContact function in this class will be called by Box2D during
// the Step function. We can use the info in the contact to see which two
// fixtures collided, but we cannot fiddle with the Box2D world inside the
// contact listener because it gets grumpy if fiddled with while busy.
//
// Instead, this class holds a reference to the main layer, which it will
// use to update some variables to let the layer know something needs to be
// done after the physics Step has completed (in the tick method).
class PlanetCuteContactListener : public b2ContactListener
{
public:
	virtual void BeginContact(b2Contact* contact);      // called by Box2D during the Step function when two fixtures begin touching
	virtual void EndContact(b2Contact* contact);        // called by Box2D during the Step function when two fixtures finish touching
    
    class PlanetCuteRUBELayer* m_layer;
};

//-----------------------

class PlanetCuteRUBELayer : public RUBELayer
{
protected:
    int m_jumpTimeout;                                  // decremented every tick, and set to a positive value when jumping. It's ok to jump if
                                                        //     this is <= 0 (this is to prevent repeated jumps while the foot sensor is still touching the ground)
    
    PlanetCuteContactListener* m_contactListener;       // lets us know when two fixtures touched
    
    cocos2d::Touch* m_leftTouch;                      // used to keep track of when the user touched the left side of the screen (if this is nil, the left side is not touched)
    cocos2d::Touch* m_rightTouch;                     // used to keep track of when the user touched the right side of the screen (if this is nil, the right side is not touched)
    
    b2Vec2 m_cameraCenter;                              // the current position of the camera in world coordinates. This is used to smoothly move the camera to a point a little
                                                        //       ahead of the player body, to help the user see what's coming in that direction.
    
    RUBEImageInfo* m_instructionsSprite1;               // a sprite that says "Touch left/right half of screen to move"
    RUBEImageInfo* m_instructionsSprite2;               // a sprite that says "Touch both sides together to jump"
    RUBEImageInfo* m_instructionsSprite3;               // a sprite that says "Well done!"
                                                        // These sprite references are held so that we can show/hide/remove them
    
public:
    b2Body* m_playerBody;                                       // duh...
    b2Fixture* m_footSensorFixture;                             // a small sensor fixture attached to the bottom of the player body to detect when it's standing on something
    int m_numFootContacts;                                      // the current number of other fixtures touching the foot sensor. If this is > 0 the character is standing on something
    
    std::set<PlanetCuteFixtureUserData*> m_allPickups;          // a array containing every pickup in the scene. This is used to loop through every tick and make them wobble around.
    std::set<PlanetCuteFixtureUserData*> m_pickupsToProcess;    // The contact listener will put some pickups in this set when the player touches them. After the Step has finished, the
                                                                //       layer will look in this list and process any pickups (remove them from world, play sound, count score etc).
                                                                //       This is made a set instead of an array because a set prevents duplicate objects from being added. Sometimes
                                                                //       a pickup can collide with more than one other fixture in the same time step (eg. the player and the foot sensor)
                                                                //       and looping through an array with duplicates would result in removing the same body from the scene twice -> crash.
    
public:
    static cocos2d::Scene* scene();                       // returns a scene that contains this as the only child
    
    virtual std::string getFilename();                      // overrides base class
    virtual cocos2d::Point initialWorldOffset();          // overrides base class
    virtual float initialWorldScale();                      // overrides base class
    
    virtual void afterLoadProcessing(b2dJson* json);        // overrides base class
    virtual void clear();                                   // overrides base class
    
    virtual void update(float dt);                          // standard Cocos2d function
    
    virtual void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    virtual void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    virtual void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
};

#endif /*PLANET_CUTE_RUBE_LAYER*/
