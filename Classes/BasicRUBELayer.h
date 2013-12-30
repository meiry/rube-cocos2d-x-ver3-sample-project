//  Author: Chris Campbell - www.iforce2d.net
//  -----------------------------------------
//
//  BasicRUBELayer
//
//  This class extends CCLayer to load in a RUBE scene file on init.
//  It uses the debug draw display to show the scene, and  does not
//  load any images. The touch methods are used to zoom, pan, and to
//  create a mouse joint to drag dynamic bodies around.
//
//  This class is mostly here to keep the first example simple, and
//  concentrate on getting positions and scales correct before any
//  images are involved. In most cases you would subclass RUBELayer
//  to make a more useful layer.
//
//  The scene file to load, and the initial position and scale are
//  given by methods which should be overridden in subclasses.
//
//  The position of the layer is set with setPosition(CCPoint)
//  and specifies the location on screen where 0,0 in the physics world
//  will be located, in pixels. Hence you can check anytime where the
//  (0,0) point of the physics world is with getPosition()
//
//  The scale of the layer is the number of pixels for one physics unit.
//  Eg. if the screen is 320 pixels high and you want it to be 10 units
//  high in the physics world, the scale would be 32. You can set this
//  with [self setScale:(float)] and check it with [self scale].
//
//  This class provides the screenToWorld and worldToScreen methods
//  which are invaluable when converting locations between screen and
//  physics world coordinates.
//

#include "cocos2d.h"
#include <Box2D/Box2D.h>
#include "Box2DDebugDraw.h"

#ifndef BASIC_RUBE_LAYER
#define BASIC_RUBE_LAYER

class b2dJson;

class BasicRUBELayer : public cocos2d::Layer
{
protected:
    b2World* m_world;                       // the physics world
	Box2DDebugDraw* m_debugDraw;            // used to draw debug data
    b2MouseJoint* m_mouseJoint;             // used when dragging bodies around
    b2Body* m_mouseJointGroundBody;         // the other body for the mouse joint (static, no fixtures)
    cocos2d::Touch* m_mouseJointTouch;    // keep track of which touch started the mouse joint

    cocos2d::Menu* m_menuLayer;           // only for this demo project, you can remove this in your own app
        
public:
    BasicRUBELayer();
    virtual ~BasicRUBELayer();
    
    static cocos2d::Scene* scene();                           // returns a Scene that contains the HelloWorld as the only child
    virtual bool init();                                        // virtual functions cannot be used in the constructor, but we want to allow some customization from subclasses
	virtual void onEnter();  
    
    virtual cocos2d::Layer* setupMenuLayer();                 // only for this demo project, you can remove this in your own app
    void goBack(Object* sender);                                              // only for this demo project, you can remove this in your own app
    void updateAfterOrientationChange();                        // only for this demo project (repositions the menu), you can remove this in your own app
    
    virtual std::string getFilename();                          // override this in subclasses to specify which .json file to load
    virtual cocos2d::CCPoint initialWorldOffset();              // override this in subclasses to set the inital view position
    virtual float initialWorldScale();                          // override this in subclasses to set the initial view scale
    
    virtual void loadWorld(Object* sender);                                   // attempts to load the world from the .json file given by getFilename
    virtual void afterLoadProcessing(b2dJson* json);            // override this in a subclass to do something else after loading the world (before discarding the JSON info)
    virtual void clear();                                       // undoes everything done by loadWorld and afterLoadProcessing, so that they can be safely called again

    virtual b2Vec2 screenToWorld(cocos2d::Point screenPos);   // converts a position in screen pixels to a location in the physics world
    virtual cocos2d::Point worldToScreen(b2Vec2 worldPos);    // converts a location in the physics world to a position in screen pixels

    virtual void update(float dt);                              // standard Cocos2d layer method
    virtual void draw();                                        // standard Cocos2d layer method
    
    virtual void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    virtual void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    virtual void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    virtual void onTouchesCancelled(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);

    b2Fixture* getTouchedFixture(cocos2d::Touch* touch);      // returns the first fixture found under the touch location
    
    virtual bool allowPinchZoom();                              // return false from this function to prevent pinch zoom and pan
};

#endif /* BASIC_RUBE_LAYER */


