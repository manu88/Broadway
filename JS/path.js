/*****************************************************************************/
/*

    This file defines the Path object. To use it, in your .js file:


    import("JS/path.js");
    
*/

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

import("GXElement.js");

var ARC_OPEN  = 0; // default
var ARC_CHORD = 1;
var ARC_PIE   = 2;

var Cap_Butt   = 0; // default
var Cap_Round  = 1;
var Cap_Square = 2;


var Join_Miter = 0; // default
var Join_Round = 1;
var Join_Bevel = 2;


function Path() 
{
    this.prototype = new GXElement();
    this.elementID = GX_AddPath();

    this.clear = function()
    {
        PathClear( this.elementID );
    };

    this.update = function()
    {
        PathUpdate( this.elementID );
    };

    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */

    this.addSvgFile = function ( svgFile )
    {
        PathAddSvg( this.elementID , svgFile);
    };

    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */

    this.fill = function()
    {
        PathFill( this.elementID );
    };

    this.stroke = function()
    {
        PathStroke( this.elementID );
    };

    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */

    this.setStrokeColor = function(r , g , b ,a)
    {
        PathSetStrokeColor( this.elementID , r , g , b ,a);
    };

    this.setStrokeWidth = function ( width )
    {
        PathSetStrokeWidth( this.elementID , width );

    };

    this.setCapStyle = function( style )
    {
        PathSetCapStyle( this.elementID , style );
    };

    this.setJoinStyle = function( style )
    {
        PathSetJoinStyle( this.elementID , style );
    };

    this.setFillColor = function(r , g , b ,a)
    {
        PathSetFillColor( this.elementID , r , g , b ,a);
    };

    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */

    this.lineTo = function(x , y)
    {
        PathLineTo( this.elementID , x , y);
    };

    this.moveTo = function(x , y)
    {
        PathMoveTo( this.elementID , x , y);
    };

    this.cubicTo = function(cp0x , cp0y , cp1x , cp1y , x , y)
    {
        PathCubicTo( this.elementID , cp0x , cp0y , cp1x , cp1y , x , y);
    };

    this.quadTo = function(cpx , cpy ,  x , y)
    {
        PathQuadTo( this.elementID , cpx , cpy ,  x , y);
    };

    this.close = function()
    {
        PathClose( this.elementID );
    };    

    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */

    this.addRect = function(x , y , width , height ) 
    { 
        PathAddRect( this.elementID , x , y , width , height , );
    };

    this.addEllipse = function(x , y , width , height , stroke , fill) 
    { 
        PathAddEllipse( this.elementID , x , y , width , height );
    };

    this.addRoundedRect = function(x , y , width , height, arcW , arcH , stroke , fill) 
    { 
        PathAddRoundedRect( this.elementID , x , y , width , height , arcW , arcH );
    };

    this.addLine = function(x0 , y0 , x1 , y1) 
    { 
        PathAddLine( this.elementID , x0 , y0 , x1 , y1 );
    };

    this.addArc = function( x , y , w , h , start , extent , type)
    {
        PathAddArc( this.elementID , x , y , w , h , start , extent , type );
    };



}



