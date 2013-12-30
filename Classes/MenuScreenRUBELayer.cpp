//  Author: Chris Campbell - www.iforce2d.net
//  -----------------------------------------
//
//  MenuScreenRUBELayer

#include "MenuScreenRUBELayer.h"
#include "ExamplesMenuLayer.h"

using namespace std;
using namespace cocos2d;

// In the constructor we need to set up the matching between the name of buttons and which
// function they will call (this must be done before init()). The first parameter of the
// registerSelector function is a string which will match the value of the "selectorButton"
// custom string property of the button fixture. The second parameter specifies which
// function of this class will be called when the button is pressed. The function should
// return void and take no parameters.
MenuScreenRUBELayer::MenuScreenRUBELayer()
{
    registerSelector("doSomething", callfunc_selector(MenuScreenRUBELayer::doSomething));
}

// Creates and returns an instance of this class. This can be handy for adding this menu
// screen as a layer on top of another layer. If you look in PlanetCuteRUBELayer.cpp
// there is a line in the 'scene' function that is commented out - try uncommenting it
// to see how this works.
MenuScreenRUBELayer* MenuScreenRUBELayer::create()
{
    MenuScreenRUBELayer* layer = new MenuScreenRUBELayer();
    layer->init();
	return layer;
}

// Standard Cocos2d method, simply returns a scene with an instance of this class as a child
Scene* MenuScreenRUBELayer::scene()
{
    Scene *scene = Scene::create();
    
    // add layer as a child to scene
    MenuScreenRUBELayer* layer = new MenuScreenRUBELayer();    
    layer->init();// do things that require virtual functions (can't do in constructor)
    scene->addChild(layer);
    layer->release();
    
    return scene;
}

// Override superclass to load different RUBE scene
string MenuScreenRUBELayer::getFilename()
{
    return "simplemenu.json";
}

// This function will be called when a fixture in the scene with a custom string property
// named "selectorButton" is pressed, and the value of that property is set to "doSomething".
// This requires that the function and the property value to match with is pre-registered,
// as done in the constructor of this class above.
void MenuScreenRUBELayer::doSomething()
{
    CCLOG("doSomething");
    Director::getInstance()->replaceScene( ExamplesMenuLayer::scene() );
}

