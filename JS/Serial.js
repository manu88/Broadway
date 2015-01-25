/*

    Serial.js 

    Serial communications tools for Broadway.

*/

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

 
function Serial() // port = "/dev/tty*" something 
{
    this.port = undefined;

    this.setCallback = function( signature)
    {
    	SERIAL_setCallback( signature);
    };

    this.open = function( port, baudRate)
    {
        if ( SERIAL_openPort( port , baudRate) )
            this.port = port;
    };

    this.close = function()
    {
        if ( SERIAL_closePort( this.port ) )
            this.port = undefined; 
    };

    // wil return a list will all ports available at runtime
    this.getAllPorts = function()
    {
        return SERIAL_getAllPorts(); 
    };


    
}



