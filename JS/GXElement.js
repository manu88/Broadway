/*

    This Class defines basic operations on displayed elements (GXElement class).

    
    This is a base class. The following objects are constructed from this one:
    Image ( Image.js )
    Video ( Video.js )
    Path  ( Path.js  )  
    
    You don't have to import this header if you're using the these Objects, it will be automaticaly added.
*/

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */



function GXElement() 
{
 

    this.elementID = -1; // set by sublasses

    
    this.update = function()
    {
        GX_setNeedsDisplay( this.elementID );
    };

    /* **** **** **** **** **** **** **** **** **** **** **** */

    /* int */
    this.setLayer = function( layer  /*int */)
    {
        GX_setLayer( this.elementID , layer );
    };

    this.getLayer = function()
    {
        GX_getLayer( this.elementID );
    };


    /* **** **** **** **** **** **** **** **** **** **** **** */    

    /* 4 ints */
    this.setBounds = function( x , y , width , height  /*ints */)
    {
        GX_setBounds( this.elementID , x , y , width , height);
    };

    /* 2 ints */
    this.setPos = function( x , y  /*ints */ )
    {
        GX_setPos( this.elementID , x , y );
    };

    this.setSize = function( width , height /*ints */  )
    {
        GX_setSize( this.elementID , width , height );
    };    

    /* **** **** **** **** **** **** **** **** **** **** **** */

    /* bool */
    this.setVisible = function( visible  /*bool*/)
    {
        GX_setVisible( this.elementID , visible );
    };

    this.isVisible = function()
    {
        return GX_isVisible( this.elementID  );
    };

    this.flipVisibility = function() 
    { 
        this.setVisible( !this.isVisible() );
    };

    /* **** **** **** **** **** **** **** **** **** **** **** */    



}





