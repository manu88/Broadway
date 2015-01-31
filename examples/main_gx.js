
import("Display.js");
import("Time.js");

function myDisplayCallback( notification )
{
	if ( notification & ( HDMI_UNPLUGGED) )
		print("HDMI_UNPLUGGED" );

	else if ( notification & ( HDMI_ATTACHED ) )
		print(" HDMI_ATTACHED ");

	else if ( notification & ( DISPLAY_ACTIVE ) )
		print(" Display active ");



}

function main()
{
    print("GX tests");

    display.setCallback("myDisplayCallback");

    display.powerOn();



    var videoModes = display.getDisplayConfigsList();

    var modesCount = videoModes.length;

    print("got " + modesCount + " video modes");

    for (var i = 0 ; i<modesCount ; i++)
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

    var ret = display.setDisplayConfig( 10 );

    print("try set video mode ret = " + ret);


	

}