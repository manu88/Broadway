
import( "Osc.js" );
import( "WebServer.js");
import( "Time.js");
import( "Gpio.js");


var osc 	  = new Osc();
var webServer = new WebServer( 8080 );
var timer1 	  = new Timer();

var gp1  = new Gpio(); 

var count = 0;

function oscCallback( ip , port , address , vars)
{
	/*
	print("Got A osc message on port " + port +" from ip: " + ip+ " With address " + address  + "and " + vars.length + " args");

	for (var i = 0 ; i<vars.length ; i++)
    {
        print( vars[i] );
    }
	*/

	if ( address == "/resetTC")
		setRunningTC( 0,0,0,0);
	
}

function webCallback( ip , port , address , vars)
{
	
	if ( address == "/tc")
		return count++;
}


function timerCallback( timer)
{
	
 	if ( timer1.is( timer ) )
 	{
 		print( getRunningTC() );
 		
 	}
}

function gpioCallback( pin , state)
{
	print(" pin " + pin + " changed to " + state);
}


function main()
{

	
	setRunningTC( 0,0,0,0);

	osc.addListeningPort( 8000 );
	osc.setCallback("oscCallback");

	webServer.setCallback("webCallback");
    webServer.setRootDirectory("/Users/manueldeneu/Documents/projets/dev/Broadway/Broadway/HtmlFiles/");
	
    timer1.setCallback("timerCallback");

    timer1.setStartDelay( 0 , 0 , 2 , 0);
    timer1.setPeriod( 0 , 0 , 1 , 0);
    timer1.setPeriodic( true );
    timer1.start();

    gp1.setCallback( "gpioCallback");
    gp1.registerInput( 1 , 0);


    //timer1.stop();
	
}