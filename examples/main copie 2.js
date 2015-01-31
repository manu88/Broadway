
import("Display.js");

function myDisplayCallback( notification )
{
	print(" got a display Notification " + notification);

}

function main()
{
    print("GX tests");

    display.powerOn();

    display.setCallback("myDisplayCallback");

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