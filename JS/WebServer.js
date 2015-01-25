/*

    Webserver.js 

    Embedded web server for Broadway.

*/

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

function decodeURI( text )
{
    return WEB_decodeURI( text );
}

function WebServer( portNum ) 
{
    
	this.port = WEB_startServer( portNum );

	/*

        ex : webCallback( ip , port , address , vars)

        vars will contain an array of arrays, eg for this url :
        /testUrl?box1=hello&box2=&textfield=mysupertextfield
        -> var= [ ["box1" , "hello"] , ["box2" , "" ] ,["textfield" , "mysupertextfield" ]   ];

        ! Values are still percent-formated! You have to call decodeUri() to get a properly decoded string

    */
    this.setCallback = function( signature)
    {
    	WEB_setCallback( signature);
    };

    this.setRootDirectory = function( filePath )
    {
    	return WEB_setRootDirectory( filePath );
    };

    
}



