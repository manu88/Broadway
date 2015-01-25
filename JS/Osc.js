/*

    Osc.js 

    Osc tools for Broadway.

*/

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

 
function Osc() 
{
    

    this.addListeningPort = function( inPort )
    {
		return OSC_addPort( inPort );  
    };

    this.send = function( ip , port , address, datas )
    {
		return OSC_send( ip , port , address, datas );
    };

    /*
		register a custom function to receive incomming osc messages:

		Example: 
			var osc = new Osc();

			function myOscCallback( ip , port , address , vars)
			{
				// ... do something with the incomming message ...
			}

			function main()
			{
				osc.addListeningPort( 8000 );
				osc.addListeningPort( 9000 );
				osc.setCallback("myOscCallback");
			}
    */

    this.setCallback = function( signature)
    {
    	OSC_setCallback( signature);
    };

    
}



