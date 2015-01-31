
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


/* **** **** **** **** **** **** */
/*
	Notifications sent to display's callback ( see bellow.)
	Simply use bit mask op to test a flag.

	Example:

	function myDisplayCallback( notification )
	{
	if ( notification & ( HDMI_UNPLUGGED) )
		print("HDMI detached" );

	else if ( notification & ( HDMI_ATTACHED ) )
		print(" HDMI attached ");
	}
*/

var HDMI_UNPLUGGED           = (1 << 0); // <HDMI cable is detached
var HDMI_ATTACHED  			 = (1 << 1); // <HDMI cable is attached but not powered on
var HDMI_DVI                 = (1 << 2); // <HDMI is on but in DVI mode (no audio)
var HDMI_HDMI                = (1 << 3); // <HDMI is on and HDMI mode is active
var HDMI_HDCP_UNAUTH         = (1 << 4); // <HDCP authentication is broken (e.g. Ri mismatched) or not active
var HDMI_HDCP_AUTH           = (1 << 5); // <HDCP is active
var HDMI_HDCP_KEY_DOWNLOAD   = (1 << 6); // <HDCP key download successful/fail
var HDMI_HDCP_SRM_DOWNLOAD   = (1 << 7); // <HDCP revocation list download successful/fail
var HDMI_CHANGING_MODE       = (1 << 8); // <HDMI is starting to change mode, clock has not yet been set

var SDTV_UNPLUGGED           = 1 << 16; //  <SDTV cable unplugged, subject to platform support
var SDTV_ATTACHED            = 1 << 17; //  <SDTV cable is plugged in
var SDTV_NTSC                = 1 << 18; //  <SDTV is in NTSC mode
var SDTV_PAL                 = 1 << 19; //  <SDTV is in PAL mode
var SDTV_CP_INACTIVE         = 1 << 20; //  <Copy protection disabled
var SDTV_CP_ACTIVE           = 1 << 21; //   <Copy protection enabled

var DISPLAY_ACTIVE           = 1 << 30;


/* **** **** **** **** **** **** */
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



