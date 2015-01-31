import("Time.js");
import("Osc.js");

var timer1 = new Timer();

var num = 0;

var outPort = 4597;
var outIP = "127.0.0.1";

function timerCallback( timerID)
        {
            if ( timer1.is ( timerID ) )
            {
                //print("timer1");
                osc.send( outIP , outPort , "/clock", num++ );
            }

        }

function main()
{
    print("Timer tests");

    timer1.setCallback("timerCallback");

    timer1.setPeriod( 0 /* hh*/,0 /* min*/, 0 /*secs*/, 500 /* ms */);
    timer1.start();

    

}