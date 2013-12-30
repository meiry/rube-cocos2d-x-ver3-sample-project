//
//  ButtonRUBELayer.h
//  mtb
//
//  Created by chris on 10/04/13.
//
//  Extends RUBELayer to provide simple button functionality. You can make subclasses
//  of this class to set up menu screens in your app.
//
//  Fixtures in the scene with a custom string property called "selectorButton" will
//  be looked for to associate with functions in this class. The functions to associate
//  with should return void and take no parameters. You must register any functions
//  you want to use for this, in the constructor of the subclass. See the constructor
//  of the MenuScreenRUBELayer class for an example.
//
//  You can find more information in this video:
//  http://www.youtube.com/watch?v=JaHpmLN8DQA
//  The video covers the Objective-C version of this class, which is able to use a
//  second custom string property to automatically find and instantiate another layer
//  to be the main scene of the program, but that is not implemented here in the C++
//  version. But you can easily just call Director::getInstance()->replaceScene
//  to do this from the function that you associate the button with.
//

#ifndef BUTTON_RUBE_LAYER
#define BUTTON_RUBE_LAYER

#include "RUBELayer.h"

struct _buttonInfo {
    b2Fixture* fixture;
    std::string selectorName;
    std::string sceneName;
    cocos2d::SEL_CallFunc selector;
    //Class sceneClassType;
    std::string imageFile_normal;
    std::string imageFile_hover;
    int colorTint_normal[4];
    int colorTint_hover[4];
    bool highlighted;
    cocos2d::Sprite* sprite;
};

class ButtonRUBELayer : public RUBELayer
{
protected:
    std::string m_filename;
    std::vector<_buttonInfo> m_buttons;
    
    cocos2d::Touch* m_buttonTouch;
    _buttonInfo* m_touchedButton;
    
    
    std::map<std::string, cocos2d::SEL_CallFunc> m_selectorMap;
    
    cocos2d::SEL_CallFunc getSelectorByName(std::string name);
    void registerSelector(std::string name, cocos2d::SEL_CallFunc selector);

public:
    ButtonRUBELayer();
    virtual void onEnter();  
    virtual std::string getFilename();
    virtual cocos2d::CCPoint initialWorldOffset();
    virtual float initialWorldScale();
    
    virtual void afterLoadProcessing(b2dJson* json);
    
    virtual void draw();
    
    void setupButtonActions(b2dJson* json);    
    _buttonInfo* getButtonInfo(b2Fixture* fixture);
    
    void setButtonHighlighted(_buttonInfo* bi, bool tf);
    void doButtonTouch(cocos2d::Touch* touch);
    
    virtual void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    virtual void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    virtual void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    
    virtual bool allowPinchZoom();
    virtual bool allowButtonPresses();
    virtual bool actOnTouchDown();
    
    
};

#endif /* BUTTON_RUBE_LAYER */

