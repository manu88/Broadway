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

bool generateBlankConfig()
{
    FILE *file = NULL;
    
    if ( (file = fopen("blank_config.txt", "w")))
    {
        fprintf(file, BLANK_CONFIG);
        
        fclose( file );
        return true;
        
    }
    return true;
}


int main(int argc, const char * argv[])
{
    if ( argc < 1)
    {
        printf("\n Usage : Broadway configFile.txt");
        printf("\n To generate a blank config file : Broadway -g");
        return 0;
    }
    
    if ( strcmp( argv[1] ,"-g") == 0 )
    {
        generateBlankConfig();
        printf("\n Blank config file generated : blank_config.txt \n");
        return 0;
    }
    
    
    
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

    return 0;
}

