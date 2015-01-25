/*

    Gpio.js 

    Gpio tools for Broadway.

*/

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

 
function Gpio() 
{

    this.setCallback = function( signature)
    {
    	GPIO_setCallback( signature);
    };

    this.setState = function( pin  , state)
    {
    	GPIO_setState( pin , state);
    };

    this.registerInput = function( pin , mode)
    {
    	GPIO_addInput( pin , mode );
    };

    this.removeInput = function( pin)
    {
        return GPIO_removeInput( pin );
    };

    
}



