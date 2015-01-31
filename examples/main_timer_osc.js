import("Time.js");
import("Osc.js");

var timer1 = new Timer();

var num = 0;

function timerCallback( timerID)
        {
            if ( timer1.is ( timerID ) )
            {
                //print("timer1");
                osc.send( "127.0.0.1" , 4597 , "/clock", num++ );
            }

        }

function main()
{
    print("Timer tests");

    timer1.setCallback("timerCallback");

    timer1.setPeriod( 0 /* hh*/,0 /* min*/, 0 /*secs*/, 500 /* ms */);
    timer1.start();

    

}