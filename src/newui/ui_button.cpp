// ui_button.cpp
//

#include "quakedef.h"
#include "ui_main.h"

idUIButton::idUIButton( const char *path, int x, int y, int width, int height ) {
	image = R_LoadPic32( (char *)path );
	_x = x;
	_y = y;
	_width = width;
	_height = height;
}

void idUIButton::Draw( void ) {
	Draw_Image( _x, _y, _width, _height, image );
}

bool idUIButton::IsPointInsideButton( int x, int y ) {
	char kb;
	if(x < _x || x > _x + _width) {

		return false;
	}

	if(y < _y || y > _y + _height) {
		return false;
	}

	return true;
}