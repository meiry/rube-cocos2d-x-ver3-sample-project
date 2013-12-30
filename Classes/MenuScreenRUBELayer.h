//  Author: Chris Campbell - www.iforce2d.net
//  -----------------------------------------
//
//  MenuScreenRUBELayer
//
//  Example of a simple menu screen with buttons made using fixtures
//  in the scene.
//
//  You can find more information in this video:
//  http://www.youtube.com/watch?v=JaHpmLN8DQA
//

#ifndef MENU_SCREEN_RUBE_LAYER
#define MENU_SCREEN_RUBE_LAYER

#include "ButtonRUBELayer.h"

class MenuScreenRUBELayer : public ButtonRUBELayer
{
public:
    MenuScreenRUBELayer();
    
    static MenuScreenRUBELayer* create();
    
    static cocos2d::Scene* scene();
    virtual std::string getFilename();
    
    void doSomething();                     // this will be called when the button is pressed. See also the constructor of this class where this is registered.
};

#endif /* MENU_SCREEN_RUBE_LAYER */
