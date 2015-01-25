/*****************************************************************************/
/*

    This file defines the Image object. To use it, in your .js file:


    import("JS/image.js");
    
    
    ...
*/

import("JS/GXElement.js");

function Image( filename ) 
{
    this.prototype = new GXElement();

    this.filename = filename;

    this.elementID = ImageAdd( filename );
}



