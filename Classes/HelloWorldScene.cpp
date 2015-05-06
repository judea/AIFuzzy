#include "HelloWorldScene.h"

#include "fuzzy/FuzzyModule.h"
#include "fuzzy/FuzzyVariable.h"
#include "fuzzy/FuzzySet.h"
#include "fuzzy/FuzzyOperators.h"


USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);
    
    
    // Fuzzy
    FuzzyModule fm;
    
    // 距離
    FuzzyVariable& DistToTarget = fm.CreateFLV("DistToTarget");
    FzSet Target_Close = DistToTarget.AddLeftShoulderSet("Target_Close", 0, 25, 150);
    FzSet Target_Medium = DistToTarget.AddTriangularSet("Target_Medium", 25, 50, 300);
    FzSet Target_Far = DistToTarget.AddRightShoulderSet("Target_Far", 150, 300, 500);
    
    // 望ましさ
    FuzzyVariable& Desirability = fm.CreateFLV("Desirability");
    FzSet Undesirable = Desirability.AddLeftShoulderSet("Undesirable", 0, 30, 50);
    FzSet Desirable = Desirability.AddTriangularSet("Desirable", 30, 50, 70);
    FzSet VeryDesirable = Desirability.AddRightShoulderSet("VeryDesirable", 50, 70, 100);
    
    // 弾数
    FuzzyVariable& AmmoStatus = fm.CreateFLV("AmmoStatus");
    FzSet Ammo_Low = AmmoStatus.AddLeftShoulderSet("Ammo_Low", 0, 10, 10);
    FzSet Ammo_Okay = AmmoStatus.AddTriangularSet("Ammo_Okay", 0, 10, 30);
    FzSet Ammo_Loads = AmmoStatus.AddRightShoulderSet("Ammo_Loads", 10, 30, 40);
    
    fm.AddRule(FzAND(Target_Close, Ammo_Loads), Undesirable);
    fm.AddRule(FzAND(Target_Close, Ammo_Okay), Undesirable);
    fm.AddRule(FzAND(Target_Close, Ammo_Low), Undesirable);
    fm.AddRule(FzAND(Target_Medium, Ammo_Loads), VeryDesirable);
    fm.AddRule(FzAND(Target_Medium, Ammo_Okay), VeryDesirable);
    fm.AddRule(FzAND(Target_Medium, Ammo_Low), Desirable);
    fm.AddRule(FzAND(Target_Far, Ammo_Loads), Desirable);
    fm.AddRule(FzAND(Target_Far, Ammo_Okay
), Desirable);
    fm.AddRule(FzAND(Target_Far, Ammo_Low), Desirable);
    
    //
    log("Desirability : %lf", CalculateDesirability(fm, 200, 8));
    
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

double HelloWorld::CalculateDesirability(FuzzyModule& fm, double dist, double ammo)
{
    fm.Fuzzify("DistToTarget", dist);
    fm.Fuzzify("AmmoStatus", ammo);
    
    return fm.DeFuzzify("Desirability", FuzzyModule::centroid);
}

