// ui_main.cpp
//

#include "quakedef.h"
#include "ui_main.h"

idNewHud			*newUI;

idNewHud::idNewHud() {
	downButton = new idUIButton( "newui/Black_Arrow_Down.tga", 30, vid.height - 100, 60, 60 );
	upButton = new idUIButton( "newui/Black_Arrow_Up.tga", 30, vid.height - 200, 60, 60 );

	leftButton = new idUIButton( "newui/Black_Arrow_Left.tga", vid.width - 210, vid.height - 150, 60, 60 );
	rightButton = new idUIButton( "newui/Black_Arrow_Right.tga", vid.width - 90, vid.height - 150, 60, 60 );

	fireButton =  new idUIButton( "newui/FireButton.tga", vid.width - 150, vid.height - 210, 60, 60 );
	//lel = new idUIButton("newui/Black_Arrow_Up.tga", vid.width - 210, vid.width - 50, 60, 60);
}

void idNewHud::Draw( void ) {
	upButton->Draw();
	downButton->Draw();

	leftButton->Draw();
	rightButton->Draw();

	fireButton->Draw();
	//lel->Draw();
}

void idNewHud::OnPressEvent( int x, int y ) {
	//int _x = (vid.height - y) / 2;
	//int _y = vid.width + ((vid.height - x) / 2.0);
	//int _x = floor(y / vid.height * 100.0f * vid.width / 100.0f);
	int _x = floor(((float)y / (float)vid.height * 100.0f) * ((float)vid.width / 100.0f));
	_x = vid.width - _x;
	//Sys_Printf("(%i / %i * 100) * (%i / 100)", y, vid.height, vid.width);
	//Sys_Printf("%f", (y / vid.height * 100) * (vid.width / 100));
	//int _y = int(50);
	int _y = floor(((float)x / (float)vid.width * 100.0f) * ((float)vid.height / 100.0f));
	//lel = new idUIButton("newui/Black_Arrow_Up.tga", _x, _y, 60, 60);
	

	if(upButton->IsPointInsideButton( _x, _y )) {
		Key_Event( K_UPARROW, true );
		
	}
	else {
		Key_Event( K_UPARROW, false );
	}

	if(downButton->IsPointInsideButton( _x, _y )) {
		Key_Event( K_DOWNARROW, true );
		
	}
	else {
		Key_Event( K_DOWNARROW, false );
	}

	if(leftButton->IsPointInsideButton( _x, _y )) {
		Key_Event( K_LEFTARROW, true );
		
	}
	else {
		Key_Event( K_LEFTARROW, false );
	}

	if(rightButton->IsPointInsideButton( _x, _y )) {
		Key_Event( K_RIGHTARROW, true );
		
	}
	else {
		Key_Event( K_RIGHTARROW, false );
	}

	if(fireButton->IsPointInsideButton( _x, _y )) {
		Key_Event( K_CTRL, true );
		
	}
	else {
		Key_Event( K_CTRL, false );
	}
}

void idNewHud::OnPressRelease( int x, int y ) {
	//int _x = (vid.height - y) / 2;
	//int _y = vid.width + ((vid.height - x) / 2.0);

	int _x = floor(((float)y / (float)vid.height * 100.0f) * ((float)vid.width / 100.0f));
	_x = vid.width - _x;
	int _y = floor(((float)x / (float)vid.width * 100.0f) * ((float)vid.height / 100.0f));

	//if(!upButton->IsPointInsideButton( _x, _y )) 
	{
		Key_Event( K_UPARROW, false );
	}


	//if(!downButton->IsPointInsideButton( _x, _y )) 
	{
		Key_Event( K_DOWNARROW, false );
		
	}

	//if(!leftButton->IsPointInsideButton( _x, _y )) 
	{
		Key_Event( K_LEFTARROW, false );
	}


	//if(!rightButton->IsPointInsideButton( _x, _y )) 
	{
		Key_Event( K_RIGHTARROW, false );
		
	}

	{ 
		Key_Event( K_CTRL, false );
	}

}