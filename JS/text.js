
import("JS/GXElement.js");


function Text()
{
    this.prototype = new GXElement();

    this.elementID = TextAdd();

    /* string */
    this.setText = function( text)
    {
        TextSetText( this.elementID , text);
    };

    this.setCarret = function ( x , y)
    {
        TextSetCarret( this.elementID , x , y);
    };

    this.setFontSize = function ( size )
    {
        TextSetFontSize( this.elementID , size );
    };

    this.setFont = function( fontname )
    {
        
        TextSetFont( this.elementID , fontname );
    };

    this.setColor = function( r , b , g , a)
    {
        TextSetColor( this.elementID , r , g , b , a);
    };
}
