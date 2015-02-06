import("Image.js");

var img = new Image("broadway.jpg");


function main()
{
    img.setLayer(2);
    img.setBounds( 100 , 100 , 600 , 900);
    img.setVisible( true);
    
}