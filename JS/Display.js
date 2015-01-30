
/*

    Display.js 

    Display tools & commands for Broadway.

*/

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */


function Display() 
{

    /*
		register a custom function to receive display notifications

		Example: 

			function myDisplayCallback( notification )
			{
				
			}

			function main()
			{
				
				display.setCallback("myDisplayCallback");
			}
    */
	this.setCallback = function( signature)
    {
    	GX_setCallback( signature);
    };

    /*
	Will return an array of possible configurations for the attached display.
	
	the configurations' are themselve arrays. For each config :
	[0] : whether the display config is native, or not.
	[1] : type 0 = SD , 1 = HD ( -1 = invalid but should not happen at this point)
	[2] : width
	[3] : height
	[4] : framerate
	[5] : aspect ratio

	Exemple:

	var videoModes = getDisplayConfigsList(); // feftch list

    var modesCount = videoModes.length; // num of possible configurations.

    print("got " + modesCount + " video modes");

    for (var i = 0 ; i<modesCount ; i++) // iteration over the array.
    {
    	var isNative = videoModes[i][0]; 
    	print("native = " + isNative );

    	var type = videoModes[i][1];
    	print("type = " + type);

    	var w = videoModes[i][2];
    	print("Width = " + w);

    	var h = videoModes[i][3];
    	print("Height = " + h);

    }

	*/

    this.getDisplayConfigsList = function()
	{
		return GX_getDisplayConfigs();
	};

	this.setDisplayConfig = function ( numConfig /* int */ )
	{
		return GX_setDisplayConfig( numConfig );
	};


	/* **** **** **** **** **** **** **** **** **** **** **** */
	/*
		Control over HDMI
	*/

	/*
		Will turn off the display.
	*/
	this.powerOff = function()
	{
		GX_powerOff();
	};

	/*
		Will turn on the display with previous mode stored or preferred mode.
	*/
	this.powerOn = function()
	{
		GX_powerOn();
	};

	this.isDisplayOn = function()
	{
		return GX_isDisplayOn();
	};

}

var display = new Display();



