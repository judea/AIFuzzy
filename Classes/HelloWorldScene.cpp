#include "HelloWorldScene.h"

#include "fuzzy/FuzzyModule.h"
#include "fuzzy/FuzzyVariable.h"
#include "fuzzy/FuzzySet.h"
#include "fuzzy/FuzzyOperators.h"


USING_NS_CC;

static double LIMIT_SEC = 60;

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
    
    
    // 経過時間
    FuzzyVariable& ElapsedTime = fm.CreateFLV("ElapsedTime");
    FzSet Time_Early = ElapsedTime.AddLeftShoulderSet("Time_Early", 0, 15, 30);
    FzSet Time_Medium = ElapsedTime.AddTriangularSet("Time_Medium", 15, 30, 50);
    FzSet Time_Late = ElapsedTime.AddRightShoulderSet("Time_Late", 30, 50, LIMIT_SEC);
    
    // 関心度
    FuzzyVariable& Interest = fm.CreateFLV("Interest");
    FzSet Interest_Low = Interest.AddLeftShoulderSet("Interest_Low", 0, 10, 40);
    FzSet Interest_Medium = Interest.AddTriangularSet("Interest_Medium", 30, 50, 70);
    FzSet Interest_High = Interest.AddRightShoulderSet("Interest_High", 60, 80, 100);
    
    // 入札意欲
    FuzzyVariable& Desirability = fm.CreateFLV("Desirability");
    FzSet Undesirable = Desirability.AddLeftShoulderSet("Undesirable", 0, 30, 50);
    FzSet Desirable = Desirability.AddTriangularSet("Desirable", 30, 50, 70);
    FzSet VeryDesirable = Desirability.AddRightShoulderSet("VeryDesirable", 50, 70, 100);
    

    // ファジールール
    fm.AddRule(FzAND(Time_Early, Interest_Low), Undesirable);
    fm.AddRule(FzAND(Time_Early, Interest_Medium), VeryDesirable);
    fm.AddRule(FzAND(Time_Early, Interest_High), VeryDesirable);
    fm.AddRule(FzAND(Time_Medium, Interest_Low), Undesirable);
    fm.AddRule(FzAND(Time_Medium, Interest_Medium), Desirable);
    fm.AddRule(FzAND(Time_Medium, Interest_High), Desirable);
    fm.AddRule(FzAND(Time_Late, Interest_Low), Undesirable);
    fm.AddRule(FzAND(Time_Late, Interest_Medium
), Undesirable);
    fm.AddRule(FzAND(Time_Late, Interest_High), VeryDesirable);
    
    
    this->scheduleUpdate();
    
    return true;
}

void HelloWorld::update(float delta){
    
    elapsedSec += delta;
    
    double interest = 40;
    
    if ((double)elapsedSec <= LIMIT_SEC)
    {
        log("Desirability(%lf) : %lf", (double)elapsedSec, CalculateDesirability(fm, (double)elapsedSec, interest));
    }
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

double HelloWorld::CalculateDesirability(FuzzyModule& fm, double sec, double interest)
{
    fm.Fuzzify("ElapsedTime", sec);
    fm.Fuzzify("Interest", interest);
    
    return fm.DeFuzzify("Desirability", FuzzyModule::centroid);
}

