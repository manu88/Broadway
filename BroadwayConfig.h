//
//  Config.h
//  Broadway_core
//
//  Created by Manuel Deneu on 06/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef Broadway_core_Config_h
#define Broadway_core_Config_h


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

#ifndef USE_WEB_SERVER 
    #define USE_WEB_SERVER
#endif

#ifndef USE_JAVA_INTERPRETER
    #define USE_JAVA_INTERPRETER
#endif

#ifndef USE_NETWORK
    #define USE_NETWORK
#endif

#ifndef USE_GPIO
    #define USE_GPIO
#endif

#ifndef USE_GRAPHICS
    #define USE_GRAPHICS
#endif
/*
#ifndef USE_GRAPHICS_HELPERS
    #define USE_GRAPHICS_HELPERS
#endif

#ifndef USE_GRAPHICS_EXTRA
    #define USE_GRAPHICS_EXTRA
#endif

#ifndef USE_XML_PARSER
    #define USE_XML_PARSER
#endif
*/
#include "../Core/Core.h"

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*
    The following list defines tokens, flags litterals used in Broadway, e.g items names for config files, etc.

 */

#define NAME_ITEM_SCRIPTFILE "ScriptFile"

#define NAME_ITEM_USERPATHS "UserSearchPaths"

#define NAME_ITEM_NET     "Network"
#define NAME_ITEM_WEB     "Webserver"
#define NAME_ITEM_GPIO    "Gpio"
#define NAME_ITEM_DISPLAY "Display"

#define NAME_ITEM_INIT_DELAY      "InitDelayInSec"
#define NAME_ITEM_SPLASHSCREENIMG "SplashScreenImg"

#define BLANK_CONFIG "##################################################################################################################### \n\
# Mandatory Part \n\
\n\
ScriptFile =  \n\
\n\
\n\
UserSearchPaths  = \n\
#UserSearchPaths += /Users/manueldeneu/Documents/projets/dev/Broadway/Broadway/ \n\
\n\
#################### \n\
# if set to 1, these modules will be dynamicaly loaded with the machine. \n\
Network   = 0 \n\
Webserver = 0 \n\
Gpio      = 0 \n\
Display   = 0 \n\
\n\
\n\
#################### \n\
# Start-up \n\
# Add a delay ( in seconds ) a startup after modules init, so everyting can be initialized properly ( eg. wifi connections ).\n\
InitDelayInSec = 10 \n\
\n\
# if a delay is set, an image can be displayed during the initialisation. \n\
# if not defined or null, the splash screen is discarded. \n\
SplashScreenImg = broadway.jpg \n\
\n\
#####################################################################################################################\n\
#####################################################################################################################\n\
#	User's part \n\
"


#endif
