/*****************************************************************************/
/*

    This file defines the Image object. To use it, in your .js file:


    import("JS/image.js");
    
    
    ...
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

function Image( filename ) 
{
    this.prototype = new GXElement();

    this.filename = filename;

    this.elementID = GX_AddImage( filename );
}



