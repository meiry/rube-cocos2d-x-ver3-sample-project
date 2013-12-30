//  Author: Chris Campbell - www.iforce2d.net
//  -----------------------------------------
//
//  only for this demo project, you can remove this in your own app
//

#include "ExamplesMenuLayer.h"

#include "BasicRUBELayer.h"
#include "RUBELayer.h"
#include "DestroyBodyLayer.h"
#include "PinballRUBELayer.h"
#include "PlanetCuteRUBELayer.h"
#include "MenuScreenRUBELayer.h"
#include "UIControlsRUBELayer.h"

using namespace cocos2d;

Scene* ExamplesMenuLayer::scene()
{
    Scene *scene = Scene::create();
    
    // add layer as a child to scene
    Layer* layer = new ExamplesMenuLayer();
    layer->init();
    scene->addChild(layer);
    layer->release();
    
    return scene;
}

bool ExamplesMenuLayer::init()
{
	if ( Layer::init() ) {
        
        //setTouchEnabled( true );
         


      auto* basicItem = MenuItemFont::create("Basic load", CC_CALLBACK_1(ExamplesMenuLayer::loadBasic, this));
        
	  auto* imagesItem = MenuItemFont::create("Images load",  CC_CALLBACK_1(ExamplesMenuLayer::loadImages, this));
        auto* destroyItem = MenuItemFont::create("Destroy bodies",CC_CALLBACK_1(ExamplesMenuLayer::loadDestroy, this));
        auto* pinballItem = MenuItemFont::create("Pinball demo", CC_CALLBACK_1(ExamplesMenuLayer::loadPinball, this));
        auto* planetCuteItem = MenuItemFont::create("PlanetCute demo", CC_CALLBACK_1(ExamplesMenuLayer::loadPlanetCute, this));
        auto* simpleMenuItem = MenuItemFont::create("Simple menu demo",CC_CALLBACK_1(ExamplesMenuLayer::loadSimpleMenu, this));
        auto* uiControlsItem = MenuItemFont::create("UI controls demo",CC_CALLBACK_1(ExamplesMenuLayer::loadUIControls, this));
        
        Menu* menu = Menu::create(basicItem,imagesItem,destroyItem,pinballItem,planetCuteItem,simpleMenuItem,uiControlsItem,NULL);
        menu->alignItemsVertically();
        addChild(menu); 
        
	}
	return true;
}
//
void ExamplesMenuLayer::loadBasic(Object* sender)
{
	 Director::getInstance()->replaceScene( BasicRUBELayer::scene() );
}

void ExamplesMenuLayer::loadImages(Object* sender)
{
    Director::getInstance()->replaceScene( RUBELayer::scene() );
}

void ExamplesMenuLayer::loadDestroy(Object* sender)
{
    Director::getInstance()->replaceScene( DestroyBodyLayer::scene() );
}

void ExamplesMenuLayer::loadPinball(Object* sender)
{
    Director::getInstance()->replaceScene( PinballRUBELayer::scene() );
}

void ExamplesMenuLayer::loadPlanetCute(Object* sender)
{
    Director::getInstance()->replaceScene( PlanetCuteRUBELayer::scene() );
}

void ExamplesMenuLayer::loadSimpleMenu(Object* sender)
{
    Director::getInstance()->replaceScene( MenuScreenRUBELayer::scene() );
}

void ExamplesMenuLayer::loadUIControls(Object* sender)
{
    Director::getInstance()->replaceScene( UIControlsRUBELayer::scene() );
}



