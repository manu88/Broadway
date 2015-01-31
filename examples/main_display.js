import("WebServer.js");
import("Osc.js");


var webServer = new WebServer( 8080);

var osc = new Osc();

var num = 0;

function oscCallback( ip , port , address , vars)
{
    if ( address == "/test")
    {
    	num++;
       osc.send("127.0.0.1", 9000 , "/back", [0 , "lol", 123.2 , num] );
	}
    else if ( address == "/quit")
       quit();
 
}

function webCallback( ip , port , address , vars)
{
	
	var pairsNum = vars.length;

	if ( address == "/get_script")
	{
		return getScriptText();
	}

	else if ( address == "/editJS" )
	{
		var text = vars[0][1];
		remplaceScriptText( decodeURI( text ) );
		return "edited" ;	
	}
		

	else if ( address == "/reload")
	{
		reloadMachine();
		return "reload";
	}

	else if ( address == "/quit")
	{
		quit();
	}

	else if ( address == "/log")
	{
		return getFileText( getConfigValueForName("LogFile") );
	}

}

function main()
{
       print("hello");
	webServer.setCallback( "webCallback" );

	webServer.setRootDirectory( getFolderFromUserList("HtmlFiles/") );

	osc.setCallback( "oscCallback" );
	osc.addListeningPort( 8000 );

}