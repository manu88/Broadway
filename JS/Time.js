/*

    Time.js 

    Time tools for Broadway.

*/

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */




/* **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** */
/*
    Time operations according to the system clock
*/

function getCurrentTimeInMs()
{
    return TIMER_getCurrentTimeInMs();
}

// will return the current time in a formatted string "hh:mm:ss:mss"
function getCurrentTimeOfTheDay()
{
    return TIMER_getCurrentTimeOfTheDay();
}

/* **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** */
/*
    Time operations according to internal timecode.
    You can run a clock, reset it and use external sync messages 
    whith the following functions
*/

function setRunningTC( hh , mm , ss , ms)
{
    TIMER_setRunningTC( hh , mm , ss ,ms );  
}

function getRunningTC()
{
    return TIMER_getRunningTC();
}

/* **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** */
/*
    Timer class
*/


function Timer() 
{
    /* 
        This is a 'static' method, common to all Timer instances !
        the callback has to be somthing like this

        var myTimer1 = new Timer();
        var myTimer2 = new Timer();
        // ... init timers, set period etc.

        function timerCallback( timerID)
        {
            if ( myTimer1.is ( timerID ) )
            {
                myTimer1 has been fired
            }
            else if ( myTimer2.is ( timerID ) )
            {
                myTimer2 has been fired
            }
        }

        function main()
        {
            myTimer1.setCallback("timerCallback");
            // 
        }
    */
    
    this.setCallback = function( signature)
    {
        TIMER_setCallback( signature);
    };
    

    this.timerId = -1;
    
    // start Time
    this.hhS = 0;
    this.mmS = 0;
    this.ssS = 0;
    this.msS = 0;

    // period
    this.hhP = 0;
    this.mmP = 0;
    this.ssP = 0;
    this.msP = 0;

    this.oneShot = false;

    this.running = false;
    
    /* **** **** **** **** **** **** **** **** **** */

    this.is = function ( id )
    {
        return this.timerId == id;
    };


 
    this.setStartDelay = function( hh , mm , ss , ms )
    {
        this.hhS = hh;
        this.mmS = mm;
        this.ssS = ss;
        this.msS = ms;
    };

    this.setPeriod = function( hh , mm , ss , ms )
    {
        this.hhP = hh;
        this.mmP = mm;
        this.ssP = ss;
        this.msP = ms;
    };

    this.setPeriodic = function( periodic )
    {
        this.oneShot = !periodic;
    };

    /* **** **** **** **** **** **** **** **** **** */

    this.isRunning = function()
    {
        return this.running;
    };

    this.start = function()
    {
        if (!this.running)
        {
            this.timerId =  TIMER_add( this.hhS , this.mmS , this.ssS, this.msS ,
                                       this.hhP , this.mmS , this.ssP, this.msP , 
                                       this.oneShot );

            this.running = true;
        }

    };

    this.restart = function()
    {
        this.stop();
        this.start();
    };

    this.stop = function()
    {
        if ( this.running )
        {
            TIMER_remove( this.timerId );
            this.running = false;
        }
    };
}




