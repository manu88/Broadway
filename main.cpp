//
//  main.cpp
//  Broadway
//
//  Created by Manuel Deneu on 06/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include <iostream>



#include "BroadwayConfig.h"

#include "BroadwayController.h"


int main(int argc, const char * argv[])
{
    ScopedPlateformConfig p;
    
    Log::addLocalLogger();

    { // nested scope for Broadway to be deleted before cleanupLogger
        BroadwayController b( argv[1] );
    
        if ( !b.run() )
        {
            // error
        }
    }


      Log::cleanupLogger();


}

