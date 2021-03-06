//
//  BroadwayController.h
//  Broadway_core
//
//  Created by Manuel Deneu on 06/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__BroadwayController__
#define __Broadway_core__BroadwayController__

#include <iostream>

#include "BroadwayConfig.h"

#include "../Core/GXExtra/GraphicsComponents.h"

class BroadwayController : public ApplicationBase,
                           public JSMachineDelegate,
                           public NetworkControllerDelegate,
                           public WebServerDelegate,
                           public SchedulerDelegate,
                           public InterfaceControllerDelegate,
                           public DisplayControllerDelegate
{

public:
    BroadwayController( const std::string fileConfig );
    ~BroadwayController();
    
    /*
        Cleanup environment and restart
     */
    bool prepareForConfigAndReload();
    
    // async method
    void signalReset();
    
    void reset();
    
    bool run();
    bool stop();
    
    void summarizeConfig();
    
    // only relevant is GX module is on :-)
    void showSplash();
    void hideSplash();
    
private:
    
    /* Machine modules */
    
    bool loadConfigFile();
    
    void registerFunctions();

    bool loadAllModules();
    bool unloadAllModules();

    //the following are only called by loadAllModules / unloadAllModules!
    bool addNetModule();
    bool removeNetModule();
    
    bool addWebModule();
    bool removeWebModule();
    
    bool addInterfaceModule();
    bool removeInterfaceModule();
    
    bool addDisplayModule();
    bool removeDisplayModule();
    
    // called by prepareForConfigAndReload() only
    bool loadController( AbstractController *controller );
    bool unloadController( AbstractController *controller );
    
    /* Callbacks */

    void functionCalled( const Selector *selector );
        inline bool broadwayFunctionCalled( const Selector *selector);
        inline bool gxFunctionCalled( const Selector *selector);
        inline bool netFunctionCalled( const Selector *selector);
        inline bool webFunctionCalled( const Selector *selector);
        inline bool timeFunctionCalled( const Selector *selector);
        inline bool gpioFunctionCalled( const Selector *selector);
        inline bool serialFunctionCalled( const Selector *selector);
    
    void oscReceived(const std::string &ipAddress , const int port,const std::string & addressPattern, const ArgumentsArray &arguments);
    
    std::string getRequest( const std::string &ipAddress ,
                           const int port,
                           const std::string &addressPattern,
                           const Database &arguments);


    void scheduledEventReceived( Event &event);
    void inputChanged( const InterfaceEvent *event );
    
    void displayDidChange( DisplayNotification notification );
    

    bool _shouldReset;
    
    /* User Attributes */
    std::string              _currentScriptFile;
    std::vector<std::string> _userSearchPaths;
    
    Timecode                 _delayAtInit;
    
    
    JSMachine               _jsMachine;
    Scheduler               _scheduler;    
    
    CoreModules             _coreModulesLoaded;
    
    // additionnal modules
    NetworkController   *_net;
    WebServer           *_web;
    InterfaceController *_interface;
    DisplayController   *_display;
    
    GXScene *_scene;
    
    GXImage *_splashScreen;
    CircleWaitComponent *_circleComp;
    
    /* JS callback signatures */
    std::string _netCallback;
    std::string _webCallback;
    std::string _timerCallback;
    std::string _gpioCallback;
    std::string _serialCallback;
    std::string _displayCallback;

    
    
};

#endif /* defined(__Broadway_core__BroadwayController__) */
