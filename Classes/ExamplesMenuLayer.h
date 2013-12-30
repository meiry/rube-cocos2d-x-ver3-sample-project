//  Author: Chris Campbell - www.iforce2d.net
//  -----------------------------------------
//
//  only for this demo project, you can remove this in your own app
//

#ifndef EXAMPLES_MENU_LAYER
#define EXAMPLES_MENU_LAYER

#include "cocos2d.h"

class ExamplesMenuLayer : public cocos2d::Layer {
public:
    static cocos2d::Scene* scene();
    virtual bool init();
    
    void loadBasic(Object* sender);	 
    void loadImages(Object* sender);
    void loadDestroy(Object* sender);
    void loadPinball(Object* sender);
    void loadPlanetCute(Object* sender);
    void loadSimpleMenu(Object* sender);
    void loadUIControls(Object* sender); 
};

#endif /* EXAMPLES_MENU_LAYER */
