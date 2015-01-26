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

_shouldQuit  ( false ),
_shouldReset ( false ),

_fileConfig ( fileConfig ),

/* Modules  */

_net       ( nullptr ),
_web       ( nullptr ),
_interface ( nullptr ),
_display   ( nullptr ),

/* Callbacks signatures */

_netCallback    ( "" ),
_webCallback    ( "" ),
_timerCallback  ( "" ),
_gpioCallback   ( "" ),
_serialCallback ( "" )

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

bool BroadwayController::loadConfigFile( const std::string fileConfig )
{
    if ( !FileSystem::fileExists( fileConfig ) )
        return false;

    
    _config.clear();
    _userSearchPaths.clear();
    
    if ( _config.parseFile( fileConfig , '=' ) )
    {
        if ( _config.itemExists("LogFile"))
            Log::addFileLogger( _config.getValueForItemName<std::string>( "LogFile" ) );
        
        if ( _config.itemExists( NAME_ITEM_SCRIPTFILE ))
            _currentScriptFile = _config.getValueForItemName<std::string>( NAME_ITEM_SCRIPTFILE );
        
        
        /* User's files path */
        
        if ( _config.itemExists( NAME_ITEM_USERPATHS ))
        {

            for ( auto path : _config.getValueForItemNameAsVector( NAME_ITEM_USERPATHS ) )
            {
                if (FileSystem::isFolder(path))
                    _userSearchPaths.push_back( FileSystem::correctPathIfNeeded( path ) );
                else
                    Log::log("Warning = '%s' is not a correct path for user paths " , path.c_str()  );
            }

        }
        
        /* Modules list*/
        
        if ( _config.itemExists( NAME_ITEM_NET) )
            _coreModulesLoaded.setModule( NETWORK , _config.getValueForItemNameAsBool( NAME_ITEM_NET ) );
        
        if ( _config.itemExists( NAME_ITEM_WEB ) )
            _coreModulesLoaded.setModule( WEB_SERVER , _config.getValueForItemNameAsBool( NAME_ITEM_WEB ) );
        
        if ( _config.itemExists( NAME_ITEM_GPIO ) )
            _coreModulesLoaded.setModule( GPIO , _config.getValueForItemNameAsBool( NAME_ITEM_GPIO ) );
        
        if ( _config.itemExists( NAME_ITEM_DISPLAY ) )
            _coreModulesLoaded.setModule( GRAPHICS , _config.getValueForItemNameAsBool( NAME_ITEM_DISPLAY ) );
        
        
        
        summarizeConfig();
        
    }
    
    else
    {
        Log::log(" Error, can't open '%s' for config parse" , fileConfig.c_str() );
        return false;
    }
    
    
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
    
    loadConfigFile( _fileConfig );
    
    registerFunctions();
    
    loadAllModules();
    
    
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
    
    if ( _coreModulesLoaded.checkModule( NETWORK ))
        addNetModule();
    
    if ( _coreModulesLoaded.checkModule( WEB_SERVER ))
        addWebModule();
    
    if ( _coreModulesLoaded.checkModule( GPIO ))
        addInterfaceModule();
    
    if ( _coreModulesLoaded.checkModule( GRAPHICS ))
        addDisplayModule();
    
    
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
    
    return loadController( _display );
    
}

bool BroadwayController::removeDisplayModule()
{
    unloadController( _display );
    
    delete _display;
    
    _display = nullptr;
    
    return true;
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool BroadwayController::run()
{
    prepareForConfigAndReload();

    
    _scene  = new GXScene();
    _display->setDisplayedElement( _scene );
    
    CircleWaitComponent *comp = new CircleWaitComponent();
    
//    comp->setTransparency(false);
    comp->setLayer(1);
    
    _img = new GXImage("broadway.jpg");
    _img->setLayer(0);
    
    _scene->addElement( _img );
    _scene->addElement( comp);
    


    _img->setNeedsDisplay();
    
    comp->startContinuousRendering();
    _scene->setNeedsDisplay();
    
    
    
    _display->update();
    
    bool withLiveParser = true;
    
    while ( _shouldQuit == false )
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
    if      ( StringOperations::beginWith(selector->signature, "OSC_") && netFunctionCalled( selector ) )
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
        const std::string ctn = WebServer::getDecodedUrl(selector->variables->getParameter("str")->getString() );
        printf("\n ret = '%s'",ctn.c_str());
        
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
        
        if ( _config.itemExists( item ) )
        {
            const std::string value = _config.getValueForItemName<std::string>(item);
            vars->setReturnVar(new CScriptVar ( value ));
        }
        else
        {
            vars->setReturnVar(new CScriptVar () );
            Log::log("Value '%s' does not exists in config file '%s'" , item.c_str() , _fileConfig.c_str() );
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
        _shouldQuit = true;
        
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
        
        vars->setReturnVar( new CScriptVar ( _scheduler.getRunningTC().getInMs() ) );

        
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
                                            const Database<std::string> &arguments)
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

void BroadwayController::scheduledEventReceived( TimedEvent &event)
{

/*

    if (test )
        _interface->sendGpo(1, high);
    else
        _interface->sendGpo(1, low);
    
    test = !test;
*/    
    std::ostringstream stream;
    stream
    <<  _timerCallback <<"("
    <<  event.timerId
    << " );";
    
    
    _jsMachine.evaluateAsString( stream.str().c_str() );
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
        << gpi->pin
        << " , "
        << gpi->state
        << " );";
        
        _jsMachine.evaluateAsString( stream.str().c_str() );
        

        
    }
    

}





