//  Author: Chris Campbell - www.iforce2d.net
//  -----------------------------------------
//
//  UIControlsRUBELayer
//

#ifndef UI_CONTROLS_RUBE_LAYER
#define UI_CONTROLS_RUBE_LAYER

#include "ButtonRUBELayer.h"

class UIControlsRUBELayer : public ButtonRUBELayer
{
private:
    bool m_drawerShowing;
    
    float m_lastDialValue;
    float m_lastSliderValue;
    
    b2RevoluteJoint* m_dialJoint;
    b2PrismaticJoint* m_sliderJoint;
    b2Body* m_drawerBody;
    
public:
    UIControlsRUBELayer();
    
    static UIControlsRUBELayer* create();
    
    static cocos2d::Scene* scene();
    virtual std::string getFilename();
    
    virtual void afterLoadProcessing(b2dJson* json);
    
    virtual void update(float dt);
    
    void goBack();
    void toggleDrawer();
};

#endif /* UI_CONTROLS_RUBE_LAYER */
