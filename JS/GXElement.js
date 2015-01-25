/*

    This Class defines basic operations on displayed elements (GXElement class).
    
    This is a base class. The following objects are constructed from this one:
    Image ( image.js )
    Video ( video.js )
    Path  ( path.js  )  

*/



function GXElement() 
{
 

    this.elementID = -1; // set by sublasses

    
    this.update = function()
    {
        GXElementSetNeedsDisplay( this.elementID );
    };

    /* **** **** **** **** **** **** **** **** **** **** **** */

    /* int */
    this.setLayer = function( layer )
    {
        GXElementSetLayer( this.elementID , layer );
    };

    this.getLayer = function()
    {
        GXElementGetLayer( this.elementID );
    };


    /* **** **** **** **** **** **** **** **** **** **** **** */    

    /* 4 ints */
    this.setBounds = function( x , y , width , height )
    {
        GXElementSetBounds( this.elementID , x , y , width , height);
    };

    /* 2 ints */
    this.setPos = function( x , y  )
    {
        GXElementSetPos( this.elementID , x , y );
    };

    this.setSize = function( width , height  )
    {
        GXElementSetSize( this.elementID , width , height );
    };    

    /* **** **** **** **** **** **** **** **** **** **** **** */

    /* bool */
    this.setVisible = function( visible )
    {
        GXElementSetVisible( this.elementID , visible );
    };

    this.isVisible = function()
    {
        return GXElementIsVisible( this.elementID  );
    };

    this.flipVisibility = function() 
    { 
        this.setVisible( !this.isVisible() );
    };

    /* **** **** **** **** **** **** **** **** **** **** **** */    



}





