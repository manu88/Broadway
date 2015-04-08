//
//  BroadwayController.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 06/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//
#include <unistd.h>
#include "BroadwayController.h"


BroadwayController::BroadwayController( const std::string fileConfig ) :

ApplicationBase ( fileConfig ),

_shouldReset ( false ),

_delayAtInit( Timecode(0) ), // default : no delay

/* Modules  */

_net       ( nullptr ),
_web       ( nullptr ),
_interface ( nullptr ),
_display   ( nullptr ),

_scene        (nullptr),
_splashScreen (nullptr),
_circleComp   (nullptr),

/* Callbacks signatures */

_netCallback    ( "" ),
_webCallback    ( "" ),
_timerCallback  ( "" ),
_gpioCallback   ( "" ),
_serialCallback ( "" ),
_displayCallback( "" )

{
    _coreModulesLoaded.addModule( JAVA_INTERPRETER );
    
    _jsMachine.setDelegate( this );
    
    _scheduler.setDelegate( this );
}

BroadwayController::~BroadwayController()
{
    unloadAllModules();

    Log::log("Broadway succesfully returned\n");
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool BroadwayController::loadConfigFile()
{
    
    reloadDatabase();
    

    if ( getDatabase().itemExists("LogFile"))
        Log::addFileLogger( getDatabase().getValueForItemName( "LogFile" )->getString() );
    
    if ( getDatabase().itemExists( NAME_ITEM_SCRIPTFILE ))
        _currentScriptFile = getDatabase().getValueForItemName( NAME_ITEM_SCRIPTFILE) ->getString() ;
    
    
    if ( getDatabase().itemExists( NAME_ITEM_INIT_DELAY ))
        _delayAtInit = Timecode(0 ,0 , getDatabase().getValueForItemName( NAME_ITEM_INIT_DELAY )->getInt() );
    
    
    /* User's files path */
    
    if ( getDatabase().itemExists( NAME_ITEM_USERPATHS ))
    {

        for ( const auto path : getDatabase().getValueForItemNameAsVector( NAME_ITEM_USERPATHS ) )
        {
            if ( FileSystem::isFolder( path.getString() ) )
                _userSearchPaths.push_back( FileSystem::correctPathIfNeeded( path.getString() ) );
            else
                Log::log("Warning = '%s' is not a correct path for user paths " , path.getString().c_str()  );
        }

    }
    
    /* Modules list*/
    
    if ( getDatabase().itemExists( NAME_ITEM_NET) )
        _coreModulesLoaded.setModule( NETWORK , getDatabase().getValueForItemName( NAME_ITEM_NET )->getBool() );
    
    if ( getDatabase().itemExists( NAME_ITEM_WEB ) )
        _coreModulesLoaded.setModule( WEB_SERVER , getDatabase().getValueForItemName( NAME_ITEM_WEB )->getBool() );
    
    if ( getDatabase().itemExists( NAME_ITEM_GPIO ) )
        _coreModulesLoaded.setModule( GPIO , getDatabase().getValueForItemName( NAME_ITEM_GPIO )->getBool() );
    
    if ( getDatabase().itemExists( NAME_ITEM_DISPLAY ) )
        _coreModulesLoaded.setModule( GRAPHICS , getDatabase().getValueForItemName( NAME_ITEM_DISPLAY )->getBool() );
    
    
    
    summarizeConfig();
    


    
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void BroadwayController::summarizeConfig()
{
    
    Log::log("Module Web server : %s " , _coreModulesLoaded.checkModule( WEB_SERVER )? "YES" : "NO" );
    Log::log("Module Network    : %s"  , _coreModulesLoaded.checkModule( NETWORK )? "YES" : "NO");
    Log::log("Module GPIO       : %s"  , _coreModulesLoaded.checkModule( GPIO )? "YES" : "NO");
    Log::log("Module DISPLAY    : %s"  , _coreModulesLoaded.checkModule( GRAPHICS )? "YES" : "NO");
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool BroadwayController::prepareForConfigAndReload()
{
    
    _jsMachine.reset();
    
    if (!initializeApp() )
    {
        Log::log("Error : unable to initialize Application!");
    }
        
    
    loadConfigFile();
    
    registerFunctions();
    
    loadAllModules();
    
    if ( _delayAtInit.isValid() )
    {
        Thread::sleepFor( _delayAtInit );
    }
    
    hideSplash();
    
    if ( _jsMachine.parseScriptFile( _currentScriptFile ) )
        _jsMachine.evaluateAsString("main();");
    
    _scheduler.setRunningTC( Scheduler::getCurrentTC() );
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void BroadwayController::registerFunctions()
{
    
    _jsMachine.registerFunctionWithSignature("test(str)");
    
    // Broadways functions
    
    _jsMachine.registerFunctionWithSignature("quit()");
    
    _jsMachine.registerFunctionWithSignature("getConfigValueForName( name )");
    _jsMachine.registerFunctionWithSignature("reloadMachine()");
    _jsMachine.registerFunctionWithSignature("import( file )");
    
    _jsMachine.registerFunctionWithSignature("getScriptText()");
    _jsMachine.registerFunctionWithSignature("remplaceScriptText( text )");
    _jsMachine.registerFunctionWithSignature("getFileText( file )");
    
    _jsMachine.registerFunctionWithSignature("getFileFromUserList( file )");
    _jsMachine.registerFunctionWithSignature("getFolderFromUserList( folder )");
    
    // timers & scheduler
    _jsMachine.registerFunctionWithSignature("TIMER_setCallback( signature)");
    _jsMachine.registerFunctionWithSignature("TIMER_add( hhS , mmS, ssS, msS , hhP , mmP , ssP , msP , oneShot)");
    _jsMachine.registerFunctionWithSignature("TIMER_remove( timerId )");
    
    _jsMachine.registerFunctionWithSignature("TIMER_getCurrentTimeInMs()");
    _jsMachine.registerFunctionWithSignature("TIMER_getCurrentTimeOfTheDay()");
    
    _jsMachine.registerFunctionWithSignature("TIMER_setRunningTC(hh,mm,ss,ms)");
    _jsMachine.registerFunctionWithSignature("TIMER_getRunningTC()");
    
    
    // net related JS methods
    if ( _coreModulesLoaded.checkModule( NETWORK ))
    {
        _jsMachine.registerFunctionWithSignature("OSC_send( ip , port , addressPattern, datas)");
        _jsMachine.registerFunctionWithSignature("OSC_addPort( port)");
        _jsMachine.registerFunctionWithSignature("OSC_setCallback( signature)");
    }
    
    if ( _coreModulesLoaded.checkModule( WEB_SERVER ))
    {
        _jsMachine.registerFunctionWithSignature("WEB_startServer( port )");
        _jsMachine.registerFunctionWithSignature("WEB_setCallback( signature)");
        _jsMachine.registerFunctionWithSignature("WEB_setRootDirectory( filePath )");
        
        _jsMachine.registerFunctionWithSignature("WEB_decodeURI( text)");
        
    }
    
    if ( _coreModulesLoaded.checkModule( GPIO ))
    {
        _jsMachine.registerFunctionWithSignature("GPIO_setCallback( signature)");
        _jsMachine.registerFunctionWithSignature("GPIO_setState( pin , state)");
        _jsMachine.registerFunctionWithSignature("GPIO_addInput( pin , mode )");
        _jsMachine.registerFunctionWithSignature("GPIO_removeInput( pin )");
        
        _jsMachine.registerFunctionWithSignature("SERIAL_getAllPorts()");
        _jsMachine.registerFunctionWithSignature("SERIAL_openPort( port , baudRate)");
        _jsMachine.registerFunctionWithSignature("SERIAL_closePort( port )");
        
    }
    
    if ( _coreModulesLoaded.checkModule(GRAPHICS ) )
    {
        /* Display config */
        _jsMachine.registerFunctionWithSignature("GX_getDisplayConfigs()");
        _jsMachine.registerFunctionWithSignature("GX_setCallback( signature)");
        _jsMachine.registerFunctionWithSignature("GX_setDisplayConfig( numConfig )");
        
        _jsMachine.registerFunctionWithSignature("GX_powerOn()");
        _jsMachine.registerFunctionWithSignature("GX_powerOff()");
        _jsMachine.registerFunctionWithSignature("GX_isDisplayOn()");
        
        /* Scene & elements */
        
        // GXElement
        _jsMachine.registerFunctionWithSignature("GX_setNeedsDisplay( id )");
        
        _jsMachine.registerFunctionWithSignature("GX_setLayer( id , layer )");
        _jsMachine.registerFunctionWithSignature("GX_getLayer( id )");
        
        _jsMachine.registerFunctionWithSignature("GX_setBounds( id , x , y , w , h )");
        _jsMachine.registerFunctionWithSignature("GX_setPos( id , x , y )");
        _jsMachine.registerFunctionWithSignature("GX_setSize( id , w , h )");
        
        _jsMachine.registerFunctionWithSignature("GX_setVisible( id , visible)");
        _jsMachine.registerFunctionWithSignature("GX_isVisible( id )");
        
        _jsMachine.registerFunctionWithSignature("GX_AddImage( filename)");
        _jsMachine.registerFunctionWithSignature("GX_AddPath()");        
        
    }
    
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
void BroadwayController::signalReset()
{
    _shouldReset = true;
    fflush(stdin);
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void BroadwayController::reset()
{
    unloadAllModules();
    
    Controllers::waitForAllControllersToFinish();
    
    _shouldReset = false;
    
    prepareForConfigAndReload();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool BroadwayController::loadAllModules()
{
     _scheduler.start();

    if ( _coreModulesLoaded.checkModule( GRAPHICS ))
        addDisplayModule();
    
    if ( _coreModulesLoaded.checkModule( NETWORK ))
        addNetModule();
    
    if ( _coreModulesLoaded.checkModule( WEB_SERVER ))
        addWebModule();
    
    if ( _coreModulesLoaded.checkModule( GPIO ))
        addInterfaceModule();

    Controllers::waitForAllControllersToBeReady();
        
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool BroadwayController::unloadAllModules()
{
    _scheduler.stop();
    _scheduler.removeAllEvents();
    
    if ( _coreModulesLoaded.checkModule( NETWORK ))
        removeNetModule();
    
    if ( _coreModulesLoaded.checkModule( WEB_SERVER ))
        removeWebModule();
    
    if ( _coreModulesLoaded.checkModule( GPIO ))
        removeInterfaceModule();
    
    if ( _coreModulesLoaded.checkModule( GRAPHICS ))
        removeDisplayModule();
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool BroadwayController::loadController( AbstractController *controller )
{

    assert( controller );

    controller->start();
    
    Controllers::waitForAllControllersToBeReady();
    
    return true;
}

bool BroadwayController::unloadController( AbstractController *controller)
{
    assert( controller );
    
    controller->stop();
    
    Controllers::waitForAllControllersToFinish();

    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool BroadwayController::addNetModule()
{
    if (_net != nullptr)
        return true;
    
    _net = new NetworkController();
    _net->setDelegate( this );
    
    return loadController( _net );
    
}

bool BroadwayController::removeNetModule()
{
    unloadController( _net );
    
    delete _net;
    
    _net = nullptr;
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool BroadwayController::addWebModule()
{
    if (_web != nullptr)
        return true;
    
    _web = new WebServer();
    
    _web->setDelegate( this );

    return true; // started later
}

bool BroadwayController::removeWebModule()
{
    unloadController( _web );
    
    delete _web;
    
    _web = nullptr;
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool BroadwayController::addInterfaceModule()
{
    if (_interface != nullptr)
        return true;
    
    _interface = new InterfaceController();
    
    _interface->setDelegate( this );
    
    return loadController( _interface );
}

bool BroadwayController::removeInterfaceModule()
{
    unloadController( _interface );
    
    delete _interface;
    
    _interface = nullptr;
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool BroadwayController::addDisplayModule()
{
    if ( _display != nullptr )
        return true;
    
    _display = new DisplayController();
    
    _display->setDelegate( this );
    
    _display->start();
    
    
    Log::log("\n wait for display to be ready...");
    while ( _display->isReady() == false)
    {
    
    }

    _scene  = new GXScene();

    _display->setDisplayedElement( _scene );
    
    showSplash();
    /**/
    
    /**/
    
    return true; // loadController( _display );
    
}

bool BroadwayController::removeDisplayModule()
{
    unloadController( _display );
    
    delete _display;
    
    if ( _scene )
        delete _scene;
    
    _scene = nullptr;
    
    _display = nullptr;
    
    return true;
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void BroadwayController::showSplash()
{
    if ( !_coreModulesLoaded.checkModule( GRAPHICS ))
        return;
    
    if ( _circleComp == nullptr )
        _circleComp = new CircleWaitComponent();
    
    _circleComp->setLayer(1);
    _circleComp->setBounds( _scene->getBounds() );
    
    if (_splashScreen == nullptr )
        _splashScreen = new GXImage( getDataValue(NAME_ITEM_SPLASHSCREENIMG, "splash.jpg")->getString() );
    // getDatabase().getValueForItemName( NAME_ITEM_SPLASHSCREENIMG )->getString() );

    _splashScreen->setLayer(0);
    
    
    _scene->addElement( _splashScreen );
    _scene->addElement( _circleComp);
    
    _splashScreen->setNeedsDisplay();
    
    _circleComp->startContinuousRendering();
    
    _scene->setNeedsDisplay();
    
    _display->update();
    
    _splashScreen->setNeedsDisplay();
}

void BroadwayController::hideSplash()
{
    if ( !_coreModulesLoaded.checkModule( GRAPHICS ))
        return;
    
    _circleComp->stopContinuousRendering();

    _scene->removeElement( _circleComp );
    _scene->removeElement( _splashScreen );
    
    _circleComp   = nullptr;
    _splashScreen = nullptr;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool BroadwayController::run()
{
    prepareForConfigAndReload();



 
    bool withLiveParser = true;
    
    while ( shouldQuit() == false )
    {
        if (_shouldReset)
            reset();
        
        if ( withLiveParser )
        {
            Log::log("> ");
            char buffer[2048];
            fgets ( buffer, sizeof( buffer ), stdin );
            _jsMachine.executeBuffer( buffer );
        }
        else
        {
            usleep(4000);
        }
        
    }

    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool BroadwayController::stop()
{

    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void BroadwayController::functionCalled( const Selector *selector )
{
    if      ( StringOperations::beginWith(selector->signature, "GX_") && gxFunctionCalled( selector ) )
        return;
    
    else if ( StringOperations::beginWith(selector->signature, "OSC_") && netFunctionCalled( selector ) )
        return;
    
    else if ( StringOperations::beginWith(selector->signature, "WEB_") && webFunctionCalled( selector ) )
        return;
    
    else if ( StringOperations::beginWith(selector->signature, "TIMER_") && timeFunctionCalled( selector ) )
        return;
    
    else if ( StringOperations::beginWith(selector->signature, "GPIO_") && gpioFunctionCalled( selector ) )
        return;
    
    else if ( StringOperations::beginWith(selector->signature, "SERIAL_") && serialFunctionCalled( selector ) )
        return;
    
    
    else if (selector->identifier == "test")
    {

        
    }
    // no -no -nooooo !! an registered function with no actual implementation was called !

    else if ( !broadwayFunctionCalled( selector ))
        assert( false);
   
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

inline bool BroadwayController::broadwayFunctionCalled( const Selector *selector)
{
    CScriptVar *vars = selector->variables;
    
    if ( selector->identifier == "getConfigValueForName")
    {
        const std::string item = vars->getParameter("name")->getString();
        
        if ( getDatabase().itemExists( item ) )
        {
            const std::string value = getDatabase().getValueForItemName(item)->getString();
            vars->setReturnVar(new CScriptVar ( value ));
        }
        else
        {
            vars->setReturnVar(new CScriptVar () );
            Log::log("Value '%s' does not exists in config file '%s'" , item.c_str() , getDataFile().c_str() );
        }
        
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "reloadMachine")
    {
        signalReset();

        
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "quit")
    {
        sendQuitSignal();
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "import")
    {
        const std::string headerFile = FileSystem::locateFileFromFoldersList(  vars->getParameter("file")->getString() ,
                                                                               _userSearchPaths
                                                                             );

        if (!_jsMachine.importScriptFile( headerFile ))
            Log::log("Error while importing file '%s' " , headerFile.c_str() );

        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "getScriptText")
    {
        vars->setReturnVar( new CScriptVar ( FileSystem::getFileText( _currentScriptFile ) ) );
        
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "remplaceScriptText")
    {
        const std::string text = vars->getParameter("text")->getString();
        
        FileSystem::setFileText(_currentScriptFile, text);
        
        
        return true;
    }

    /* **** **** **** **** */
    
    else if ( selector->identifier == "getFileText")
    {
        const std::string file = vars->getParameter("file")->getString();
        
        if ( FileSystem::fileExists( file ) )
            vars->setReturnVar( new CScriptVar( FileSystem::getFileText( file) ));
        
        else
            vars->setReturnVar( new CScriptVar() );
        
        return true;
    }

    /* **** **** **** **** */
    
    else if ( selector->identifier == "getFileFromUserList")
    {
        const std::string file = FileSystem::locateFileFromFoldersList( vars->getParameter("file")->getString() , _userSearchPaths);
        
        
        if ( !file.empty() )
            vars->setReturnVar( new CScriptVar( file ));
        else
            vars->setReturnVar( new CScriptVar() );
        
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "getFolderFromUserList")
    {
        const std::string folder = FileSystem::locateFolderFromFoldersList( vars->getParameter("folder")->getString() , _userSearchPaths);
        
        
        if ( !folder.empty() )
            vars->setReturnVar( new CScriptVar( folder ));
        else
            vars->setReturnVar( new CScriptVar() );
        
        return true;
    }
    //
    

    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

inline bool BroadwayController::gxFunctionCalled( const Selector *selector)
{
    CScriptVar *vars = selector->variables;
    
    if ( selector->identifier == "GX_getDisplayConfigs")
    {
        
        CScriptVar *array = new CScriptVar();
        array->setArray();
        
        int count = 0;
        for (const DisplayInformations &mode : _display->getAvailableVideoMode() )
        {
            CScriptVar *tok = new CScriptVar();
            tok->setArray();
            
            tok->setArrayIndex(0, new CScriptVar( mode.native ) );
            tok->setArrayIndex(1, new CScriptVar( ( int ) mode.type ) );
            tok->setArrayIndex(2, new CScriptVar( ( int ) mode.size.width ) );
            tok->setArrayIndex(3, new CScriptVar( ( int ) mode.size.height ) );
            tok->setArrayIndex(4, new CScriptVar( mode.framerate ) );
            tok->setArrayIndex(5, new CScriptVar( mode.aspectRatio ) );
            
            array->setArrayIndex( count, tok);
            
            count++;
        }
        
        vars->setReturnVar( array );
        
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "GX_setCallback" )
    {
        _displayCallback = vars->getParameter("signature")->getString();
    
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "GX_setDisplayConfig" )
    {
        const int index = vars->getParameter("numConfig")->getInt();

        bool ret = false;
        
        if ( index < ( int ) _display->getAvailableVideoMode().size() )
        {
            const DisplayInformations &info = _display->getAvailableVideoMode().at( index );
            
            ret = _display->setVideoModeTo( info );
            
        }
        
        vars->setReturnVar( new CScriptVar( ret ));

        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "GX_powerOn")
    {
        
        _display->powerOn();
        
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "GX_powerOff")
    {
        _display->powerOff();
        
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "GX_isDisplayOn")
    {
        vars->setReturnVar( new CScriptVar ( _display->isDisplayOn() ) );
        
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "GX_setNeedsDisplay")
    {
        const int id = vars->getParameter("id")->getInt();
        
        GXElement* element =  _scene->getElementByID( id);
 
        if ( element )
        {
            element->setNeedsDisplay();
        }
        
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "GX_setLayer")
    {
        const int id = vars->getParameter("id")->getInt();
        
        GXElement* element =  _scene->getElementByID( id);
        
        if ( element )
            element->setLayer( vars->getParameter("layer")->getInt() );

        
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "GX_getLayer")
    {
        const int id = vars->getParameter("id")->getInt();
        
        GXElement* element =  _scene->getElementByID( id);
        
        if ( element )
            vars->setReturnVar( new CScriptVar( element->getLayer() ));
        
        return true;
    }


    /* **** **** **** **** */
    
    else if ( selector->identifier == "GX_setBounds")
    {
        const int id = vars->getParameter("id")->getInt();

        GXElement* element =  _scene->getElementByID( id);
        
        if ( element )
        {
            const int x = vars->getParameter("x")->getInt();
            const int y = vars->getParameter("y")->getInt();
            const int w = vars->getParameter("w")->getInt();
            const int h = vars->getParameter("h")->getInt();

            element->setBounds(x, y, w, h);
        }
        
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "GX_setPos")
    {
        const int id = vars->getParameter("id")->getInt();
        
        GXElement* element =  _scene->getElementByID( id);
        
        if ( element )
        {
            const int x = vars->getParameter("x")->getInt();
            const int y = vars->getParameter("y")->getInt();
            
            element->moveTo( x, y);
        }
        
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "GX_setSize")
    {
        const int id = vars->getParameter("id")->getInt();
        
        GXElement* element =  _scene->getElementByID( id);
        
        if ( element )
        {
            const int w = vars->getParameter("w")->getInt();
            const int h = vars->getParameter("h")->getInt();
            
            element->setSize(w, h);
        }
        
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "GX_setVisible")
    {
        const int id = vars->getParameter("id")->getInt();
        
        GXElement* element =  _scene->getElementByID( id);
        
        if ( element )
        {
            const bool vis = vars->getParameter("visible")->getBool();

            element->setVisible( vis );
        }
        
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "GX_isVisible")
    {
        const int id = vars->getParameter("id")->getInt();
        
        GXElement* element =  _scene->getElementByID( id);
        
        if ( element )
        {
            vars->setReturnVar( new CScriptVar( element->isVisible() ) );
        }
        
        return true;
    }
    

    
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "GX_AddImage")
    {
        const std::string file = vars->getParameter("filename")->getString();
        
        GXImage *img = new GXImage( file );
        
        _scene->addElement( img );
        
        vars->setReturnVar( new CScriptVar( img->getElementId() ) );
        
        return true;
    }

    /* **** **** **** **** */
    
    else if ( selector->identifier == "GX_AddPath")
    {
        GXPaintJS *path = new GXPaintJS();
        
        _scene->addElement( path );
        
        vars->setReturnVar( new CScriptVar( path->getElementId() ) );
        
        return true;
    }

    
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

inline bool BroadwayController::netFunctionCalled( const Selector *selector)
{
    
    CScriptVar *vars = selector->variables;
    
    if ( selector->identifier == "OSC_addPort" )
    {
        const int port = vars->getParameter("port")->getInt();
        const bool ret = _net->addPort( port );
        
        vars->setReturnVar( new CScriptVar( ret ) );
        
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "OSC_send" )
    {
        // OSC_send( ip , port , addressPattern, datas)
        const std::string ip = vars->getParameter("ip")->getString();
        const int port = vars->getParameter("port")->getInt();
        
        const std::string address = vars->getParameter("addressPattern")->getString();
        
        const bool ret = _net->sendOSC( ip,
                                       port,
                                       address ,
                                       *JSMachine::getArgumentsFromJSArray( vars->getParameter("datas") )
                                       );
        
        
        vars->setReturnVar( new CScriptVar( ret ) );
        
        return true;        
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "OSC_setCallback" )
    {
        const std::string signature = vars->getParameter("signature")->getString();
        
        _netCallback = signature;
        
        return true;
    }
    
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

inline bool BroadwayController::webFunctionCalled( const Selector *selector)
{
    CScriptVar *vars = selector->variables;
    
    if ( selector->identifier == "WEB_startServer" )
    {
        const int port = vars->getParameter("port")->getInt();
        _web->setPort(port);        
        
        const bool ret =  loadController( _web );
        
        vars->setReturnVar( new CScriptVar(ret));
        
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "WEB_setCallback" )
    {
        const std::string signature = vars->getParameter("signature")->getString();
        
        _webCallback = signature;
        
        return true;
    }

    /* **** **** **** **** */
    
    else if ( selector->identifier == "WEB_setRootDirectory" )
    {
        const std::string filePath = vars->getParameter("filePath")->getString();
        
        const bool ret = _web->setWorkingDirectory( filePath );
        
        vars->setReturnVar( new CScriptVar(ret));
        
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "WEB_decodeURI" )
    {
        const std::string text = WebServer::getDecodedUrl( vars->getParameter("text")->getString() );
        
        vars->setReturnVar( new CScriptVar( text ));
        
        return true;
    }

    
    
    return false;
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

inline bool BroadwayController::timeFunctionCalled( const Selector *selector)
{
    CScriptVar *vars = selector->variables;
    if ( selector->identifier == "TIMER_add")
    {
//        ( hhS , mmS, ssS, msS , hhP , mmP , ssP , msP , oneShot)
        const Timecode start( vars->getParameter("hhS")->getInt() ,
                              vars->getParameter("mmS")->getInt() ,
                              vars->getParameter("ssS")->getInt() ,
                              vars->getParameter("msS")->getInt()
                             );
        
        const Timecode period( vars->getParameter("hhP")->getInt() ,
                               vars->getParameter("mmP")->getInt() ,
                               vars->getParameter("ssP")->getInt() ,
                               vars->getParameter("msP")->getInt()
                             );
        
        const bool oneshot = vars->getParameter("oneShot")->getBool();
        
        const int ret = _scheduler.registerTimedEvent( start, period, oneshot);
        
        vars->setReturnVar( new  CScriptVar ( ret ) );

        return true;
    }

    /* **** **** **** **** */    
    
    else if ( selector->identifier == "TIMER_setCallback" )
    {

        _timerCallback = vars->getParameter("signature")->getString();
        
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "TIMER_remove" )
    {
        
        const int timerIdToRemove = vars->getParameter("timerId")->getInt();
        
        if ( !_scheduler.removeEvent( timerIdToRemove ) )
        {
            Log::log("Unable to remove timer with id %i" , timerIdToRemove);
        }

        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "TIMER_getCurrentTimeInMs" )
    {
        
        vars->setReturnVar( new CScriptVar( Scheduler::getTimeInMs() ));
        
        
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "TIMER_getCurrentTimeOfTheDay" )
    {
        
        vars->setReturnVar( new CScriptVar( Scheduler::getCurrentTC().getString() ));
        
        
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "TIMER_setRunningTC" )
    {
        
        const Timecode time( vars->getParameter("hh")->getInt() ,
                               vars->getParameter("mm")->getInt() ,
                               vars->getParameter("ss")->getInt() ,
                               vars->getParameter("ms")->getInt()
                              );

        _scheduler.setRunningTC( time );
        
        return true;
    }
    //
    /* **** **** **** **** */
    
    else if ( selector->identifier == "TIMER_getRunningTC" )
    {
        
        vars->setReturnVar( new CScriptVar ( (double) _scheduler.getRunningTC().getInMs() ) );

        
        return true;
    }
    
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

inline bool BroadwayController::gpioFunctionCalled( const Selector *selector)
{
    CScriptVar *vars = selector->variables;
    
    if ( selector->identifier == "GPIO_setCallback")
    {
        _gpioCallback = vars->getParameter("signature")->getString();
        
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "GPIO_setState")
    {
    //    ( pin , state)
        const int pin = vars->getParameter("pin")->getInt();
        const GpioState state = static_cast<GpioState>( vars->getParameter("state")->getInt() );
        
        _interface->sendGpo(pin, state);
        
        return true;
    }
    
    /* **** **** **** **** */
    
    else if ( selector->identifier == "GPIO_addInput")
    {
        //    ( pin , state)
        const int pin = vars->getParameter("pin")->getInt();
        const GPioInputType mode = static_cast<GPioInputType>( vars->getParameter("mode")->getInt() );
            
        _interface->addGpioInput( pin ,  mode);
        
        return true;
    }

    /* **** **** **** **** */
    
    else if ( selector->identifier == "GPIO_removeInput")
    {
        const int pin = vars->getParameter("pin")->getInt();
        
        const bool ret = _interface->removeGpioInput( pin );
        
        vars->setReturnVar( new CScriptVar( ret) );
        
        return true;
    }
    

    
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

inline bool BroadwayController::serialFunctionCalled( const Selector *selector)
{
    CScriptVar *vars = selector->variables;
    
    if ( selector->identifier == "SERIAL_getAllPorts")
    {
        vars->setReturnVar( JSMachine:: getJSArrayFromVector< std::string> ( SerialEvent::getSerialDevicesList()));
        
        return true;
    }
    
    /* **** **** **** **** */    
    
    else if ( selector->identifier == "SERIAL_openPort" )
    {
        const std::string port = vars->getParameter("port")->getString();
        const SerialSpeed speed = static_cast<SerialSpeed>( vars->getParameter("baudRate")->getInt() );
        
        SerialEvent *serial = nullptr;
        
        if ( (serial = _interface->addSerial( port )) != nullptr)
            serial->setSpeed( speed );
        else
            Log::log("Error : unable to open port '%s'",port.c_str() );
        
        vars->setReturnVar( new CScriptVar( serial != nullptr ));
        
        return true;
    }
    
    /* **** **** **** **** */    
    
    else if ( selector->identifier == "SERIAL_closePort" )
    {
        const std::string port = vars->getParameter("port")->getString();
        
        SerialEvent *serial = nullptr;
        
        if ( (serial = _interface->getSerialEventByPort(port) ) != nullptr)
            serial->closePort();

        else
            Log::log("Error : unable to open port '%s'",port.c_str() );
        
        vars->setReturnVar( new CScriptVar( serial != nullptr ));
        
        return true;
        
    }
    
    /* **** **** **** **** */    
    
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


void BroadwayController::oscReceived( const std::string &ipAddress ,
                                      const int port,
                                      const std::string & addressPattern,
                                      const ArgumentsArray &arguments)
{

    if ( _netCallback != "")
    {
        std::ostringstream stream;
        stream
        <<  _netCallback <<"("
        << "\"" << ipAddress <<"\" ,"
        << port << ","
        << "\"" << addressPattern <<"\" ,"
        << JSMachine::getArgumentsAsJSArrayString( arguments )
        << " );";
        
        _jsMachine.evaluateAsString( stream.str().c_str() );
    }
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

std::string BroadwayController::getRequest( const std::string &ipAddress ,
                       const int port,
                       const std::string &addressPattern,
                       const Database &arguments)
{


    if ( _webCallback != "")
    {
        std::ostringstream stream;
        stream
        <<  _webCallback <<"("
        << "\"" << ipAddress <<"\" ,"
        << port << ","
        << "\"" << addressPattern <<"\" ,"
        << JSMachine::getDatabaseAsJSArrayString( arguments )
        << " );";
        
        
        return _jsMachine.evaluateAsString( stream.str().c_str() );
    }
    
    else
        return "";

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void BroadwayController::scheduledEventReceived( Event &event)
{

/*

    if (test )
        _interface->sendGpo(1, high);
    else
        _interface->sendGpo(1, low);
    
    test = !test;
*/
    /*
    if (event)
    std::ostringstream stream;
    stream
    <<  _timerCallback <<"("
    <<  event.timerId
    << " );";
    
    
    _jsMachine.evaluateAsString( stream.str().c_str() );
     */
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void BroadwayController::inputChanged( const InterfaceEvent *event )
{
    if (event->isGpioEvent())
    {
        const GpioEvent* gpi = dynamic_cast<const GpioEvent*>( event );
        
        std::ostringstream stream;
        stream
        <<  _gpioCallback <<"("
        << gpi->getPin()
        << " , "
        << gpi->getState()
        << " );";
        
        _jsMachine.evaluateAsString( stream.str().c_str() );
        
    }

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void BroadwayController::displayDidChange( DisplayNotification notification )
{
//    Log::log("display notification");
    
    std::ostringstream stream;
    stream
    <<  _displayCallback
    <<"("
    << notification
    << " );";
    
    _jsMachine.evaluateAsString( stream.str().c_str() );
    

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */




