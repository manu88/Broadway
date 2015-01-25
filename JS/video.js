/*****************************************************************************/
/*

    This file defines the Video object. To use it, in your .js file:


    import("JS/video.js");
    
    var video = new Video("file.mov");
    a.start();
    ...
*/

// inherits from GXElement to support basic composing operations 
import("JS/GXElement.js"); 

function Video( filename ) 
{
    this.prototype = new GXElement();

    this.filename = filename;
    this.elementID = VideoAdd( filename );

    this.start = function() 
    { 
        VideoStart( this.elementID );
    };

    this.setLooped = function( loop)
    {
        VideoSetLooped( this.elementID , loop);
    };

    this.seekTo = function( hh , mm , ss )
    {
        VideoSeekTo( this.elementID , hh , mm , ss );
    };


    this.stop = function() 
    { 
        VideoStop( this.elementID );
    };    

    this.pause = function() 
    { 
        VideoPause( this.elementID );
    };

    this.isStarted = function() 
    { 
        return VideoIsStarted( this.elementID );
    };

    this.isPaused = function() 
    { 
        return VideoIsPaused( this.elementID );
    };  

    this.getTC = function() 
    { 
        return VideoGetTime( this.elementID );
    };  

    this.setVolume = function( vol )
    {
        VideoSetVolume( this.elementID , vol);
    };



}



