//  Author: Chris Campbell - www.iforce2d.net
//  -----------------------------------------
//
//  UIControlsRUBELayer
//

#include "UIControlsRUBELayer.h"
#include "ExamplesMenuLayer.h"
#include "rubestuff/b2dJson.h"

using namespace std;
using namespace cocos2d;

// In the constructor we need to set up the matching between the name of buttons and which
// function they will call (this must be done before init()). The first parameter of the
// registerSelector function is a string which will match the value of the "selectorButton"
// custom string property of the button fixture. The second parameter specifies which
// function of this class will be called when the button is pressed. The function should
// return void and take no parameters.
UIControlsRUBELayer::UIControlsRUBELayer()
{
    registerSelector("goBack", callfunc_selector(UIControlsRUBELayer::goBack));
    registerSelector("toggleDrawer", callfunc_selector(UIControlsRUBELayer::toggleDrawer));
}


// Creates and returns an instance of this class. This can be handy for adding this menu
// screen as a layer on top of another layer. If you look in PlanetCuteRUBELayer.cpp
// there is a line in the 'scene' function that is commented out - try uncommenting it
// to see how this works.
UIControlsRUBELayer* UIControlsRUBELayer::create()
{
    UIControlsRUBELayer* layer = new UIControlsRUBELayer();
    layer->init();
    return layer;
}


// Standard Cocos2d method, simply returns a scene with an instance of this class as a child
Scene* UIControlsRUBELayer::scene()
{
    Scene *scene = Scene::create();
    
    // add layer as a child to scene
    UIControlsRUBELayer* layer = new UIControlsRUBELayer();    
    layer->init();// do things that require virtual functions (can't do in constructor)
    scene->addChild(layer);
    layer->release();
    
    return scene;
}


// Override superclass to load different RUBE scene
string UIControlsRUBELayer::getFilename()
{
    return "uicontrols.json";
}


// Override superclass so we can locate the named joints for the dial, slider etc
void UIControlsRUBELayer::afterLoadProcessing(b2dJson* json)
{
    ButtonRUBELayer::afterLoadProcessing(json);
    
    // find the UI control items in the scene
    m_dialJoint = (b2RevoluteJoint*)json->getJointByName("dial");
    m_sliderJoint = (b2PrismaticJoint*)json->getJointByName("slider");
    m_drawerBody = json->getBodyByName("drawer");
    
    m_drawerShowing = false;
}


void UIControlsRUBELayer::update(float dt)
{
    ButtonRUBELayer::update(dt);
    
    // Log movements of the dial
    if ( m_dialJoint ) {
        float currentDialValue = m_dialJoint->GetJointAngle();
        if ( currentDialValue != m_lastDialValue ) {
            m_lastDialValue = currentDialValue;
            CCLOG("Dial moved to %f degrees", CC_RADIANS_TO_DEGREES(m_lastDialValue));
        }
    }
    
    // Log movements of the slider
    if ( m_sliderJoint ) {
        float currentSliderValue = m_sliderJoint->GetJointTranslation();
        if ( currentSliderValue != m_lastSliderValue ) {
            m_lastSliderValue = currentSliderValue;
            CCLOG("Slider moved to %f", m_lastSliderValue);
        }
    }
    
    // Update the position of the drawer
    if ( m_drawerBody ) {
        if ( m_drawerShowing && m_drawerBody->GetPosition().y < 2 ) 
            m_drawerBody->SetLinearVelocity( b2Vec2(0,4) );
        else if ( !m_drawerShowing && m_drawerBody->GetPosition().y > 0 )
            m_drawerBody->SetLinearVelocity( b2Vec2(0,-4) );
        else
            m_drawerBody->SetLinearVelocity( b2Vec2(0,0) );
    }
}


// This function will be called when a fixture in the scene with a custom string property
// named "selectorButton" is pressed, and the value of that property is set to "goBack".
// This requires that the function and the property value to match with is pre-registered,
// as done in the constructor of this class above.
void UIControlsRUBELayer::goBack()
{
    Director::getInstance()->replaceScene( ExamplesMenuLayer::scene() );
}


// This function will be called when a fixture in the scene with a custom string property
// named "selectorButton" is pressed, and the value of that property is set to "toggleDrawer".
// This requires that the function and the property value to match with is pre-registered,
// as done in the constructor of this class above.
void UIControlsRUBELayer::toggleDrawer()
{
    m_drawerShowing = !m_drawerShowing;
}

